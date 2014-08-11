
#include <somnus/channel.hpp>

#include <thread>
#include <chrono>

void addFiber(std::shared_ptr<somnus::Yarn> yarn, int i)
{
    yarn->run([=]()
    {
        printf("Running fiber %d\n", i);
    });
}

int main(int argc, char* argv[])
{
    auto yarn = std::make_shared<somnus::Yarn>();

    // Here is a long running fiber (8 seconds total).
    yarn->run([&]() {

        printf("Sleep 1\n");
        std::this_thread::sleep_for(std::chrono::seconds(2));
    
        addFiber(yarn, 1);
        addFiber(yarn, 2);

        somnus::defer();

        printf("Sleep 2\n");
        std::this_thread::sleep_for(std::chrono::seconds(2));
        
        addFiber(yarn, 3);
        addFiber(yarn, 4);
        
        // somnus::defer();

        printf("Sleep 3\n");
        std::this_thread::sleep_for(std::chrono::seconds(2));
        
        addFiber(yarn, 5);
        addFiber(yarn, 6);

        somnus::defer();
        
        printf("Sleep 4\n");
        std::this_thread::sleep_for(std::chrono::seconds(2));

    });

    std::this_thread::sleep_for(std::chrono::seconds(10));
    
    return 0;
}
