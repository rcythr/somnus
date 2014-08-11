
#include <somnus/yarn.hpp>

#include <chrono>
#include <thread>

#include <cstdio>

int main(int argc, char* argv[])
{
    auto yarn = std::make_shared<somnus::Yarn>();

    std::shared_ptr<somnus::Fiber> fiber = nullptr;

    yarn->run([&] () 
    {
        printf("PART 1\n");

        fiber = somnus::this_fiber();
        somnus::yield();

        printf("PART 2\n");
    });

    std::this_thread::sleep_for(std::chrono::seconds(1));

    yarn->run(fiber);

    std::this_thread::sleep_for(std::chrono::seconds(1));

    return 0;
}
