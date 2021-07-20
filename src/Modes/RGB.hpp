#pragma once
#include <Modes/LightMode.hpp>

class RGB : public LEDLightMode{

public:
    RGB() : LEDLightMode(true){};

    virtual void Init(pixelColor_t& pixelColor) override {}
    virtual void Run(strand_t pStrand, pixelColor_t& pixelColor, uint8_t brightness, int numberOfLeds, bool reverse, int step) override
    {
        CalcPixelColorBrightness(pixelColor, brightness);
        int k = step % 3;
        pixelColorBrightness = pixelFromRGBW(k == 0 ? brightness : 0,
        k == 1 ? brightness : 0, k == 2 ? brightness : 0, k == 3 ? brightness : 0);
        for (int i = 0; i < numberOfLeds; i++)
            pStrand.pixels[i] = pixelColorBrightness;
    }
    virtual void Stop() override {}
};