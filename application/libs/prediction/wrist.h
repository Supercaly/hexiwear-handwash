#ifndef WRIST_H_
#define WRIST_H_

enum Wrist
{
    LEFT = 0,
    RIGHT = 1
};

static inline float wrist_to_float(Wrist w)
{
    return w ? 0.0 : 1.0;
}

static inline char *wrist_to_string(Wrist w)
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