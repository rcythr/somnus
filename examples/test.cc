
#include <somnus/is_shared_ptr.hpp>
#include <cstdio>

using namespace somnus;

int main(int argc, char* argv[])
{
    printf("%d %d\n", is_shared_ptr<int>::value, is_shared_ptr<std::shared_ptr<int>>::value);
    return 0;
}
