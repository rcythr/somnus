#pragma once

#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

namespace somnus
{
    struct Fiber;

    struct Rope : std::enable_shared_from_this<Rope>
    {
        enum class ThreadMode
        {
            spawn,
            defer
        };

        Rope(ThreadMode mode = ThreadMode::spawn);
        Rope(const Rope& other) = delete;
        Rope(Rope&& other) = delete;

        void start();

        void run(std::function<void()> fn);
        
        void run(std::shared_ptr<Fiber> fiber);

        void defer();

    private:
        std::mutex _m;
        std::condition_variable _cv;
        std::queue<std::shared_ptr<Fiber>> _waiting;

        std::thread _t;
    };

    std::shared_ptr<Rope> this_rope();
    void set_this_rope(std::shared_ptr<Rope> rope);

    void set_this_fiber(std::shared_ptr<Fiber> fiber);
    std::shared_ptr<Fiber> this_fiber();

    std::shared_ptr<void> yield();
    
    void defer();

    void switch_to_rope(std::shared_ptr<Rope> rope);

    void set_yield_data(std::shared_ptr<Fiber> t, void* data);

    void set_yield_data(std::shared_ptr<Fiber> t, std::shared_ptr<void> data);
}
