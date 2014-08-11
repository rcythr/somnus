
#include <somnus/channel.hpp>

int main(int argc, char* argv[])
{
    somnus::Channel<int> channel;

    auto producer = std::make_shared<somnus::Actor>();
    auto consumer = std::make_shared<somnus::Actor>();

    consumer->run([&] ()
    {
        for(int i=0; i < 200; ++i)
        {
            printf("%d\n", channel.pop());
        }
    });

    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    producer->run([&] () 
    {
        for(int i = 0; i < 200; ++i)
        {
            channel.push(i);
        }
    });

    std::this_thread::sleep_for(std::chrono::seconds(1));

    return 0;
}
