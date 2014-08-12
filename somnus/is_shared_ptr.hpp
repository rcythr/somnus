
#include <memory>

namespace somnus
{
    template<typename T>
    struct is_shared_ptr
    {
        static const bool value = false;
    };

    template<typename T>
    struct is_shared_ptr<std::shared_ptr<T>>
    {
        static const bool value = true;
    };
}
