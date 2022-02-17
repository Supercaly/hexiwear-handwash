#ifndef _LABEL_H_
#define _LABEL_H_

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
        return "unknown label";
    }
}

#endif // _LABEL_H_