#pragma once

#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

namespace somnus
{
    /**
     * 
     */
    struct Fiber;

    struct Yarn : std::enable_shared_from_this<Yarn>
    {
        enum class ThreadMode
        {
            spawn,
            defer
        };

        Yarn(ThreadMode mode = ThreadMode::spawn);
        Yarn(const Yarn& other) = delete;
        Yarn(Yarn&& other) = delete;

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

    std::shared_ptr<Yarn> this_yarn();
    void set_this_yarn(std::shared_ptr<Yarn> yarn);

    void set_this_fiber(std::shared_ptr<Fiber> fiber);
    std::shared_ptr<Fiber> this_fiber();

    void* yield();
    
    void defer();

    void set_yield_data(std::shared_ptr<Fiber> t, void* data);
}
