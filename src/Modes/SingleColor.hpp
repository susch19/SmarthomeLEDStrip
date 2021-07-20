#pragma once
#include <Modes/LightMode.hpp>

class SingleColor : public LEDLightMode
{
public:
    SingleColor() : LEDLightMode(false){};

    virtual void Init(pixelColor_t& pixelColor) override {
    }
    virtual void Run(strand_t pStrand, pixelColor_t& pixelColor, uint8_t brightness, int numberOfLeds, bool reverse, int step) override
    {
        CalcPixelColorBrightness(pixelColor, brightness);
        for (int i = 0; i < numberOfLeds; i++)
            pStrand.pixels[i] = pixelColorBrightness;
    }
    virtual void Stop() override {}
};