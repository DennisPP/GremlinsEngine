#include "TouchControl.h"

#include <math.h>

#include <cstddef>

TTouchControl::TTouchControl()
{
    pointer = NULL;
}


bool TTouchControl::GetDistance() const
{
    float dx = x - xo;
    float dy = y - yo;
    return sqrtf(dx*dx+dy*dy);
}
