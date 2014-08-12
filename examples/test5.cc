
#include <somnus/channel.hpp>

#include <thread>
#include <chrono>

void addFiber(std::shared_ptr<somnus::Rope> rope, int i)
{
    rope->run([=]()
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
    auto rope = std::make_shared<somnus::Rope>();

    // Here is a long running fiber (8 seconds total).
    rope->run([&]() {

        printf("Sleep 1\n");
        std::this_thread::sleep_for(std::chrono::seconds(2));
    
        addFiber(rope, 1);
        addFiber(rope, 2);

        somnus::defer();

        printf("Sleep 2\n");
        std::this_thread::sleep_for(std::chrono::seconds(2));
        
        addFiber(rope, 3);
        addFiber(rope, 4);
        
        // somnus::defer();

        printf("Sleep 3\n");
        std::this_thread::sleep_for(std::chrono::seconds(2));
        
        addFiber(rope, 5);
        addFiber(rope, 6);

        somnus::defer();
        
        printf("Sleep 4\n");
        std::this_thread::sleep_for(std::chrono::seconds(2));

    });

    std::this_thread::sleep_for(std::chrono::seconds(10));
    
    return 0;
}
