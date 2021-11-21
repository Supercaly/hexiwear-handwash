#ifndef _LABEL_H_
#define _LABEL_H_

#include "assert.h"

enum class Label
{
    NONE,
    WASH,
    SAN
};

static const char *label_to_cstr(Label label)
{
    switch (label)
    {
    case Label::NONE:
        return "none";
    case Label::WASH:
        return "wash";
    case Label::SAN:
        return "san";
    default:
        assert(0 && "label_to_cstr: unreachable");
    }
}

#endif // _LABEL_H_