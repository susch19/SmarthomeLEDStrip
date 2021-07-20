#pragma once
#include <esp32_digital_led_lib.h>
class LEDLightMode
{


public:


    virtual void Init(pixelColor_t& pixelColor) = 0;
    
    virtual void Run(strand_t pStrand, pixelColor_t& pixelColor, uint8_t brightness, int numberOfLeds, bool reverse, int step) = 0;
    virtual void Stop() = 0;
    bool steppingEnabled = false;

protected:
    LEDLightMode(bool enableStepping = false): steppingEnabled(enableStepping){};
    
    pixelColor_t oldColor;
    uint8_t oldBrightness;
    pixelColor_t pixelColorBrightness;
    void CalcPixelColorBrightness(pixelColor_t pixelColor, uint8_t userBrightness)
    {
        if (oldColor.num != pixelColor.num || userBrightness != oldBrightness)
        {
            oldColor = pixelColor;
            oldBrightness = userBrightness;
            pixelColorBrightness = pixelFromRGBW(pixelColor.r * (userBrightness / 255.0), pixelColor.g * (userBrightness / 255.0), pixelColor.b * (userBrightness / 255.0), pixelColor.w * (userBrightness / 255.0));
        }
    }
};