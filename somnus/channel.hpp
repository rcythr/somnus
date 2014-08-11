#pragma once

#include <somnus/yarn.hpp>

#include <condition_variable>
#include <mutex>
#include <queue>
#include <tuple>

namespace somnus
{
    template<typename T, class Enable = void>
    struct Channel;

    // Channel for pointer types.
    template<typename T>
    struct Channel<T, typename std::enable_if<std::is_pointer<T>::value>::type>
    {
        T pop()
        {
            _m.lock();
            if(!_queue.empty())
            {
                void* data = _queue.front();
                _queue.pop();

                _m.unlock();

                return static_cast<T>(data);
            }
            else
            {
                std::shared_ptr<Yarn> yarn = this_yarn();
                std::shared_ptr<Fiber> fiber = this_fiber();

                _waiting_fibers.push(std::tie(yarn, fiber));

                _m.unlock();

                return static_cast<T>(yield());
            }
        }

        void push(T t)
        {
            _m.lock();

            if(!_waiting_fibers.empty())
            {
                auto yarn_fiber = _waiting_fibers.front();
                _waiting_fibers.pop();

                _m.unlock();
                
                auto yarn = std::get<0>(yarn_fiber);
                auto fiber = std::get<1>(yarn_fiber);

                set_yield_data(fiber, t);
                yarn->run(fiber);
                return;
            }
            else
            {
                _queue.push(t);
                _m.unlock();
                return;
            }
        }
    private:
        std::mutex _m;
        std::queue<void*> _queue;
        std::queue<std::tuple<std::shared_ptr<Yarn>, std::shared_ptr<Fiber>>> _waiting_fibers;
    };

    // Channel for non-pointer types.
    template<typename T>
    struct Channel<T, typename std::enable_if<!std::is_pointer<T>::value>::type>
    {

        T pop()
        {
            T t;

            _m.lock();
            if(!_queue.empty())
            {
                void* data = _queue.front();
                _queue.pop();

                _m.unlock();

                T* ptr = static_cast<T*>(data);
                t = *ptr;
                delete ptr;
            }
            else
            {
                std::shared_ptr<Yarn> yarn = this_yarn();
                std::shared_ptr<Fiber> fiber = this_fiber();

                _waiting_fibers.push(std::tie(yarn, fiber));

                _m.unlock();

                T* ptr = static_cast<T*>(yield());
                t = *ptr;
                delete ptr;
            }

            return t;
        }

        void push(T in)
        {
            T* t = new T();
            *t = in;

            _m.lock();

            if(!_waiting_fibers.empty())
            {
                auto yarn_fiber = _waiting_fibers.front();
                _waiting_fibers.pop();

                _m.unlock();
                
                auto yarn = std::get<0>(yarn_fiber);
                auto fiber = std::get<1>(yarn_fiber);

                set_yield_data(fiber, t);
                yarn->run(fiber);
                return;
            }
            else
            {
                _queue.push(t);
                _m.unlock();
                return;
            }
        }
    private:
        std::mutex _m;
        std::queue<void*> _queue;
        std::queue<std::tuple<std::shared_ptr<Yarn>, std::shared_ptr<Fiber>>> _waiting_fibers;
    };

}
