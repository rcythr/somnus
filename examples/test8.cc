
#include <somnus/event_dispatch.hpp>

int main(int argc, char* argv[])
{
    somnus::EventDispatch m("../examples/test8.cfg");

    int val = 100;
    auto val2 = std::make_shared<int>();
    *val2 = 200;

    m.register_event<int*>(1, 3, [] (int* a) { printf("Pointer Type: %p %d\n", (void*)somnus::this_rope().get(), *a); });
    m.register_event<std::shared_ptr<int>>(2, 4, [] (std::shared_ptr<int> a) { printf("SharedPtr Type: %p %d\n", (void*)somnus::this_rope().get(), *a); });
    m.register_event<int>(3, 5, [] (int& a) { printf("Value Type: %p %d\n", (void*)somnus::this_rope().get(), a); });

    m.dispatch(3, &val);
    m.dispatch(4, val2);
    m.dispatch(5, val);

    std::this_thread::sleep_for(std::chrono::seconds(2));

    return 0;
}
