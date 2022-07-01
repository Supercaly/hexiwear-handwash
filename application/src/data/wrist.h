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

#endif // WRIST_H_