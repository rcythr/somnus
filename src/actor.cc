
#include "actor.hpp"

#include <boost/coroutine/coroutine.hpp>

using namespace somnus;

typedef boost::coroutines::coroutine<void()> coro;

thread_local std::shared_ptr<Actor> _actor_context = nullptr;
thread_local std::shared_ptr<Task> _task_context = nullptr;

namespace somnus
{
    struct Task 
    {
        Task(std::function<void()> fn)
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

        void* _yield_context;
    };

    Actor::Actor(ThreadMode mode)
    {
        if(mode == ThreadMode::spawn)
        {
            _t = std::thread(std::bind(&Actor::start, this));
        }
    }

    void Actor::start()
    {
        set_this_actor(shared_from_this());

        while(true)
        {
            std::shared_ptr<Task> task;
            
            {
                std::unique_lock<std::mutex> ul(_m);
                while(_waiting.empty())
                {
                    _cv.wait(ul);
                }

                task = _waiting.front();
                _waiting.pop();
            }
           
            set_this_task(task);
            task->run();
        }
    }

    void Actor::run(std::function<void()> fn)
    {
        run(std::make_shared<Task>(fn));
    }

    void Actor::run(std::shared_ptr<Task> task)
    {
        {
            std::unique_lock<std::mutex> ul(_m);
            _waiting.push(task);
        }
        _cv.notify_one();
    }

    void Actor::defer()
    {
        std::shared_ptr<Task> task = this_task();
        bool shouldDefer = false;

        {
            std::unique_lock<std::mutex> ul(_m);
            if(!_waiting.empty())
            {
                shouldDefer = true;
                _waiting.push(task);
            }
        }

        if(shouldDefer)
        {
            (*(task->_coro_context))();
        }
    }

    std::shared_ptr<Actor> this_actor() { return _actor_context; }
    std::shared_ptr<Task> this_task() { return _task_context; }
    void set_this_actor(std::shared_ptr<Actor> actor) { _actor_context = actor; }
    void set_this_task(std::shared_ptr<Task> task) { _task_context = task; }

    void* yield()
    {
        (*(_task_context->_coro_context))();
        return _task_context->_yield_context;
    }
    
    void defer()
    {
        _actor_context->defer();
    }

    void set_yield_data(std::shared_ptr<Task> t, void* data)
    {
        t->_yield_context = data;
    }

}
