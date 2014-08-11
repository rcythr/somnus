
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

/**
 * This example shows the usage of the defer() function. defer() allows a fiber to 
 *      yield to other fibers which are waiting to execute.
 *
 * Output:
 *      Sleep 1
 *      [ ~2 second pause ]
 *      Running fiber 1
 *      Running fiber 2
 *      Sleep 2
 *      [ ~2 second pause ]
 *      Sleep 3
 *      [ ~2 second pause ]
 *      Running fiber 3
 *      Running fiber 4
 *      Running fiber 5
 *      Running fiber 6
 *      Sleep 4
 *      [ ~4 second pause]
 */
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
