
#include "actor.hpp"
#include "channel.hpp"

int main(int argc, char* argv[])
{
    somnus::Channel<int*> channel;

    auto producer = std::make_shared<somnus::Actor>();
    auto consumer = std::make_shared<somnus::Actor>();

    consumer->run([&] ()
    {
        for(int i=0; i < 200; ++i)
        {
            int* p = channel.pop();
            printf("%d\n", *p);
            delete p;
        }
    });

    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    producer->run([&] () 
    {
        for(int i = 0; i < 200; ++i)
        {
            int* p = new int;
            *p = i;
            channel.push(p);
        }
    });

    std::this_thread::sleep_for(std::chrono::seconds(1));

    return 0;
}
