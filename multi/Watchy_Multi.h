#ifndef WATCHY_MULTI_H
#define WATCHY_MULTI_H

#include <Watchy.h>
#include "Watchy_Multi.h"

class WatchyMulti : public Watchy {
    using Watchy::Watchy;

    public:
        void handleButtonPress();
        void drawWatchFace();
        void drawWatchFace7SEG();
        void drawWatchFaceBadForEye();
        void drawWatchFaceLine();

    private:
        void drawTime7SEG();
        void drawDate7SEG();
        void drawSteps7SEG();
        void drawWeather7SEG();
        void drawBattery7SEG();
        void drawHandLine(uint8_t _radius_, float _angle_, bool _light_);
        void drawWatchFaceBahn();
};

#endif
