#ifndef WATCHY_BadForEye_H
#define WATCHY_BadForEye_H

#include <Watchy.h>
#include "BadForEye.h"

class BadForEye : public Watchy{
    public:
        BadForEye(const watchySettings &s);
        void drawWatchFace();
};

#endif
