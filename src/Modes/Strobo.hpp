#pragma once
#include <Modes/LightMode.hpp>

class Strobo : public LEDLightMode
{

    pixelColor_t none;
    virtual void Init(pixelColor_t &pixelColor) override {}
    virtual void Run(strand_t pStrand, pixelColor_t &pixelColor, uint8_t brightness, int numberOfLeds, bool reverse, int step) override
    {
        if (step % 2)
        {
            CalcPixelColorBrightness(pixelColor, brightness);
            for (int i = 0; i < numberOfLeds; i++)
                pStrand.pixels[i] = pixelColorBrightness;
        }
        else
        {
            for (int i = 0; i < numberOfLeds; i++)
                pStrand.pixels[i] = none;
        }
    }
    virtual void Stop() override {}
};