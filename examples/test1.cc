
#include <somnus/actor.hpp>

#include <chrono>
#include <thread>

#include <cstdio>

int main(int argc, char* argv[])
{
    auto actor = std::make_shared<somnus::Actor>();

    std::shared_ptr<somnus::Task> task = nullptr;

    actor->run([&] () 
    {
        printf("PART 1\n");

        task = somnus::this_task();
        somnus::yield();

        printf("PART 2\n");
    });

    std::this_thread::sleep_for(std::chrono::seconds(1));

    actor->run(task);

    std::this_thread::sleep_for(std::chrono::seconds(1));

    return 0;
}
