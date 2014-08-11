#pragma once

#include "actor.hpp"

#include <condition_variable>
#include <mutex>
#include <queue>
#include <tuple>

namespace somnus
{
    template<typename T>
    struct Channel
    {

        T pop()
        {
            T t;

            _m.lock();
            if(!_queue.empty())
            {
                printf("Pop from queue\n");
                void* data = _queue.front();
                _queue.pop();

                _m.unlock();

                T* ptr = static_cast<T*>(data);
                t = *ptr;
                delete ptr;
            }
            else
            {
                printf("Deferred pop\n");
                std::shared_ptr<Actor> actor = this_actor();
                std::shared_ptr<Task> task = this_task();

                _waiting_tasks.push(std::tie(actor, task));

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

            if(!_waiting_tasks.empty())
            {
                printf("Push to waiting task.\n");
                auto actor_task = _waiting_tasks.front();
                _waiting_tasks.pop();

                _m.unlock();
                
                auto actor = std::get<0>(actor_task);
                auto task = std::get<1>(actor_task);

                set_yield_data(task, t);
                actor->run(task);
                return;
            }
            else
            {
                printf("Push to queue.\n");
                _queue.push(t);
                _m.unlock();
                return;
            }
        }

    private:

        std::mutex _m;
        std::queue<void*> _queue;
        std::queue<std::tuple<std::shared_ptr<Actor>, std::shared_ptr<Task>>> _waiting_tasks;
    };

}
