#ifndef WATCHY_MULTI_H
#define WATCHY_MULTI_H

#include <Watchy.h>
#include "Watchy_7_SEG.h"
#include "Watchy_BadForEye.h"

#define FACE_COUNT 2

class WatchyMulti : public Watchy {

    //using Watchy::Watchy;

    public:
        WatchyMulti(const watchySettings &s) : Watchy(s) {  //constructor
            this->faces[0] = Watchy7SEG(s);
            this->faces[1] = BadForEye(s);
        }

        void drawWatchFace();
        void handleButtonPress();

    private:
        Watchy faces[FACE_COUNT];
};

#endif
