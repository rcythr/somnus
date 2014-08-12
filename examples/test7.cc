
#include <somnus/rope.hpp>
#include <somnus/tether.hpp>

struct MockType
{
    MockType(std::shared_ptr<somnus::Rope> rope)
        : tether(rope)
    {
    }

    void foo()
    {
        somnus::ScopedTether st(tether);
        printf("Foo Inner Rope: %p\n", (void*)somnus::this_rope().get());
    }

    void bar()
    {
        tether();
        printf("Bar Inner Rope: %p\n", (void*)somnus::this_rope().get());
    }

    somnus::Tether tether;
};

/**
 */
int main(int argc, char* argv[])
{
    auto rope1 = std::make_shared<somnus::Rope>();
    auto rope2 = std::make_shared<somnus::Rope>();
    MockType mock(rope2);

    rope1->run([&]()
    {
        printf("Pre-Foo Rope: %p\n", (void*)somnus::this_rope().get());
        mock.foo();
        printf("Post-Foo Rope: %p\n\n", (void*)somnus::this_rope().get());
        
        printf("Pre-Bar Rope: %p\n", (void*)somnus::this_rope().get());
        mock.bar();
        printf("Post-Bar Rope: %p\n", (void*)somnus::this_rope().get());
    });

    std::this_thread::sleep_for(std::chrono::seconds(3));

    return 0;
}
