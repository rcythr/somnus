#pragma once

#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

namespace somnus
{
    struct Task;

    enum class ThreadMode
    {
        spawn,
        defer
    };

    struct Actor : std::enable_shared_from_this<Actor>
    {
        Actor(ThreadMode mode = ThreadMode::spawn);
        Actor(const Actor& other) = delete;
        Actor(Actor&& other) = delete;

        void start();

        void run(std::function<void()> fn);
        void run(std::shared_ptr<Task> task);

        void defer();

    private:
        std::mutex _m;
        std::condition_variable _cv;
        std::queue<std::shared_ptr<Task>> _waiting;

        std::thread _t;
    };

    std::shared_ptr<Actor> this_actor();
    void set_this_actor(std::shared_ptr<Actor> actor);

    void set_this_task(std::shared_ptr<Task> task);
    std::shared_ptr<Task> this_task();

    void* yield();
    
    void defer();

    void set_yield_data(std::shared_ptr<Task> t, void* data);
}
