#ifndef WRIST_H_
#define WRIST_H_

enum Wrist
{
    LEFT = 0,
    RIGHT = 1
};

static inline float wrist_to_float(Wrist w)
{
    return w == Wrist::LEFT ? 0.0 : 1.0;
}

static inline const char *wrist_to_cstr(Wrist w)
{
    switch (w)
    {
    case Wrist::LEFT:
        return "left";
    case Wrist::RIGHT:
        return "right";
    default:
        return "unknown wrist";
    }
}

#endif // WRIST_H_