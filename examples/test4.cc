
#include <somnus/channel.hpp>

#include <thread>
#include <chrono>

void addTask(std::shared_ptr<somnus::Actor> actor, int i)
{
    actor->run([=]()
    {
        printf("Running task %d\n", i);
    });
}

int main(int argc, char* argv[])
{
    auto actor = std::make_shared<somnus::Actor>();

    // Here is a long running task (8 seconds total).
    actor->run([&]() {

        printf("Sleep 1\n");
        std::this_thread::sleep_for(std::chrono::seconds(2));
    
        addTask(actor, 1);
        addTask(actor, 2);

        somnus::defer();

        printf("Sleep 2\n");
        std::this_thread::sleep_for(std::chrono::seconds(2));
        
        addTask(actor, 3);
        addTask(actor, 4);
        
        // somnus::defer();

        printf("Sleep 3\n");
        std::this_thread::sleep_for(std::chrono::seconds(2));
        
        addTask(actor, 5);
        addTask(actor, 6);

        somnus::defer();
        
        printf("Sleep 4\n");
        std::this_thread::sleep_for(std::chrono::seconds(2));

    });

    std::this_thread::sleep_for(std::chrono::seconds(10));
    
    return 0;
}
