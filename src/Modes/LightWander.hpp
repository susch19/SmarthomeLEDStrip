#pragma once
#include <Modes/LightMode.hpp>

class LightWander : public LEDLightMode
{

    virtual void Init(pixelColor_t& pixelColor) override {}
    virtual void Run(strand_t pStrand, pixelColor_t &pixelColor, uint8_t brightness, int numberOfLeds, bool reverse, int step) override
    {
        CalcPixelColorBrightness(pixelColor, brightness);
        pStrand.pixels[(step + 1) % numberOfLeds] = pixelFromRGBW(0, 0, 0, 0);
        pStrand.pixels[step] = pixelColorBrightness;
        pStrand.pixels[(numberOfLeds + step - 1) % numberOfLeds] = pixelFromRGBW(0, 0, 0, 0);
    }
    virtual void Stop() override {}

};