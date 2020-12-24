#pragma once
#include <Modes/LightMode.hpp>

class Off : public LEDLightMode
{
    bool alreadyRun;
    virtual void Init(pixelColor_t& pixelColor) override {
        alreadyRun = false;
    }
    virtual void Run(strand_t pStrand, pixelColor_t& pixelColor, uint8_t brightness, int numberOfLeds, bool reverse, int step) override
    {
        if(alreadyRun)
        return;
        CalcPixelColorBrightness(pixelColor, 0);
        for (int i = 0; i < numberOfLeds; i++)
            pStrand.pixels[i] = pixelColorBrightness;
            alreadyRun = true;
    }
    virtual void Stop() override {}
};