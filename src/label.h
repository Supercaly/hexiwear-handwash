#ifndef _LABEL_H_
#define _LABEL_H_

#include "assert.h"

enum Label
{
    LABEL_NONE,
    LABEL_WASH,
    LABEL_SAN,
};

const char *label_to_cstr(Label label);

#endif // _LABEL_H_