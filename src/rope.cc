
#include <somnus/rope.hpp>

#include <stdexcept>
#include <boost/coroutine/coroutine.hpp>

using namespace somnus;

typedef boost::coroutines::coroutine<void()> coro;

thread_local std::shared_ptr<Rope> _rope_context = nullptr;
thread_local std::shared_ptr<Fiber> _fiber_context = nullptr;

namespace somnus
{
    struct Fiber 
    {
        Fiber(std::function<void()> fn)
            : _fn(fn)
            , _coroutine(nullptr)
            , _coro_context(nullptr)
            , _yield_context(nullptr)
        {
        }

        void run()
        {
            if(_coroutine == nullptr)
            {
                _coroutine = std::make_shared<coro>([this] (coro::caller_type& context)
                {
                    _coro_context = &context;
                    _fn();
                });
            }
            else
            {
                (*_coroutine)();
            }
        }

        std::function<void()> _fn;

        std::shared_ptr<coro> _coroutine;
        coro::caller_type* _coro_context;

        std::shared_ptr<void> _yield_context;
    };

    Rope::Rope(ThreadMode mode)
    {
        if(mode == ThreadMode::spawn)
        {
            _t = std::thread(std::bind(&Rope::start, this));
        }
    }

    void Rope::start()
    {
        set_this_rope(shared_from_this());

        while(true)
        {
            std::shared_ptr<Fiber> fiber;
            
            {
                std::unique_lock<std::mutex> ul(_m);
                while(_waiting.empty())
                {
                    _cv.wait(ul);
                }

                fiber = _waiting.front();
                _waiting.pop();
            }
           
            set_this_fiber(fiber);
            fiber->run();
        }
    }

    void Rope::run(std::function<void()> fn)
    {
        run(std::make_shared<Fiber>(fn));
    }

    void Rope::run(std::shared_ptr<Fiber> fiber)
    {
        {
            std::unique_lock<std::mutex> ul(_m);
            _waiting.push(fiber);
        }
        _cv.notify_one();
    }

    void Rope::defer()
    {
        std::shared_ptr<Fiber> fiber = this_fiber();
        bool shouldDefer = false;

        {
            std::unique_lock<std::mutex> ul(_m);
            if(!_waiting.empty())
            {
                shouldDefer = true;
                _waiting.push(fiber);
            }
        }

        if(shouldDefer)
        {
            (*(fiber->_coro_context))();
        }
    }

    std::shared_ptr<Rope> this_rope() 
    { 
        if(_rope_context == nullptr)
        {
            throw std::runtime_error("Call to this_rope() made outside of fiber.");
        }

        return _rope_context; 
    }

    std::shared_ptr<Fiber> this_fiber() 
    { 
        if(_fiber_context == nullptr)
        {
            throw std::runtime_error("Call to this_fiber() made outside of fiber.");
        }

        return _fiber_context; 
    }

    void set_this_rope(std::shared_ptr<Rope> rope) 
    { 
        _rope_context = rope; 
    }

    void set_this_fiber(std::shared_ptr<Fiber> fiber) 
    { 
        _fiber_context = fiber; 
    }

    std::shared_ptr<void> yield()
    {
        if(_fiber_context == nullptr)
        {
            throw std::runtime_error("Call to yield() made outside of fiber.");
        }

        (*(_fiber_context->_coro_context))();
        return _fiber_context->_yield_context;
    }
    
    void defer()
    {
        if(_rope_context == nullptr)
        {
            throw std::runtime_error("Call to defer() made outside of fiber.");
        }

        _rope_context->defer();
    }

    void set_yield_data(std::shared_ptr<Fiber> t, void* data)
    {
        t->_yield_context = std::shared_ptr<void>((void*)data, [](void* data) {/* Lifetime managed elsewhere. Don't delete it. */});
    }
    
    void set_yield_data(std::shared_ptr<Fiber> t, std::shared_ptr<void> data)
    {
        t->_yield_context = data;
    }

}
