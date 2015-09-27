#include "keys.h"

bool operator==(const RichKey& lhs, const RichKey& rhs)
{
    return lhs.mods == rhs.mods
        && lhs.key == rhs.key
        && lhs.flags == rhs.flags;
}
