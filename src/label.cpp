#include "label.h"

const char *label_to_cstr(Label label)
{
    switch (label)
    {
    case LABEL_NONE:
        return "none";
    case LABEL_WASH:
        return "wash";
    case LABEL_SAN:
        return "san";
    default:
        assert(0 && "label_to_cstr: unreachable");
    }
}