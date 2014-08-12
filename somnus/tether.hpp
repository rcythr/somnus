#pragma once

#include <somnus/rope.hpp>

namespace somnus
{

    struct Tether
    {
        Tether(std::shared_ptr<Rope> rope)
            : parent_rope(rope)
        {
        }

        void operator()()
        {
            if(parent_rope != this_rope())
            {
                switch_to_rope(parent_rope);
            }
        }

        std::shared_ptr<Rope> parent_rope;
    };

    struct ScopedTether
    {
        ScopedTether(Tether& t)
            : other_rope(this_rope())
        {
            if(t.parent_rope != other_rope)
            {
                switch_to_rope(t.parent_rope);
            }
        }

        ~ScopedTether()
        {
            if(other_rope != this_rope())
            {
                switch_to_rope(other_rope);
            }
        }

        std::shared_ptr<Rope> other_rope;
    };
}
