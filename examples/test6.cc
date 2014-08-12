
#include <somnus/rope.hpp>

/**
 * This example shows an example of spontaneously switching from one rope to another rope via 
 *    a single call to switch_to_rope.
 *
 * Output:
 *      Rope1 = [PtrA], Rope2 = [PtrB]
 *      Current Rope = [PtrA]
 *      Current Rope = [PtrB]
 *      [ ~2 second pause ]
 */
int main(int argc, char* argv[])
{
    auto rope1 = std::make_shared<somnus::Rope>();
    auto rope2 = std::make_shared<somnus::Rope>();

    printf("Rope1 = %p, Rope2 = %p\n", (void*)rope1.get(), (void*)rope2.get());

    rope1->run([&]()
    {
        printf("Current Rope = %p\n", (void*)somnus::this_rope().get());
        somnus::switch_to_rope(rope2);
        printf("Current Rope = %p\n", (void*)somnus::this_rope().get());
    });

    std::this_thread::sleep_for(std::chrono::seconds(2));

    return 0;
}
