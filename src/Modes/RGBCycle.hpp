#pragma once
#include <Modes/LightMode.hpp>

class RGBCycle : public LEDLightMode
{

public:
    RGBCycle() : LEDLightMode(true){};

    bool redDown;
    bool greenDown;
    bool blueDown;

    virtual void Init(pixelColor_t& pixelColor) override
    {
        blueDown = greenDown = false;
        redDown = true;
        
        pixelColor.w = 0;
    }

    virtual void Run(strand_t pStrand, pixelColor_t& pixelColor, uint8_t brightness, int numberOfLeds, bool reverse, int step) override
    {
        CalcPixelColorBrightness(pixelColor, brightness);
        for (int i = 0; i < numberOfLeds; i++)
            pStrand.pixels[i] = pixelColorBrightness;

        if (reverse)
        {
            if (pixelColor.r == 254 && redDown)
            {
                redDown = false;
                blueDown = true;
            }
            else if (pixelColor.b == 254 && blueDown)
            {
                blueDown = false;
                greenDown = true;
            }
            else if (pixelColor.g == 254 && greenDown)
            {
                greenDown = false;
                redDown = true;
            }
            if (redDown)
            {
                pixelColor.r += 1;
                pixelColor.g -= 1;
            }
            else if (greenDown)
            {
                pixelColor.g += 1;
                pixelColor.b -= 1;
            }
            else if (blueDown)
            {
                pixelColor.b += 1;
                pixelColor.r -= 1;
            }
        }
        else
        {
            if (pixelColor.r == 0 && redDown)
            {
                redDown = false;
                greenDown = true;
            }
            else if (pixelColor.g == 0 && greenDown)
            {

                blueDown = true;
                greenDown = false;
            }
            else if (pixelColor.b == 0 && blueDown)
            {
                blueDown = false;
                redDown = true;
            }
            if (redDown)
            {
                pixelColor.r -= 1;
                pixelColor.g += 1;
            }
            else if (greenDown)
            {
                pixelColor.g -= 1;
                pixelColor.b += 1;
            }
            else if (blueDown)
            {
                pixelColor.b -= 1;
                pixelColor.r += 1;
            }
        }
    }
    virtual void Stop() override {}
};