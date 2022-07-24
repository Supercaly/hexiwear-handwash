#ifndef LABEL_H_
#define LABEL_H_

enum class Label
{
    NONE,
    WASH,
    SAN
};

static inline Label label_from_double(double l)
{
    switch ((int)l)
    {
    case 1:
        return Label::WASH;
    case 2:
        return Label::SAN;
    default:
        return Label::NONE;
    }
}

static inline const char *label_to_cstr(Label label)
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

#endif // LABEL_H_