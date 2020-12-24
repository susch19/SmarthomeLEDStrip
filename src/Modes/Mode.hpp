#pragma once
#include <Modes/LightMode.hpp>
#include <Arduino.h>

//Is used for signaling food ready
class Mode : public LEDLightMode
{

    virtual void Init(pixelColor_t& pixelColor) override {
        //pixelColor.num = 0;
    }
    virtual void Run(strand_t pStrand, pixelColor_t &pixelColor, uint8_t brightness, int numberOfLeds, bool reverse, int step) override
    {
        CalcPixelColorBrightness(pixelColor, brightness);
        auto ran = (uint8_t)random(0,numberOfLeds);
        switch(ran % 4)
        {
            case 0: pixelColor.r+=1; break;
            case 1: pixelColor.g+=1; break;
            case 2: pixelColor.b+=1; break;
            case 3: pixelColor.w+=1; break;
        }
        pStrand.pixels[ran] = pixelColorBrightness;
    }
    virtual void Stop() override {}
};