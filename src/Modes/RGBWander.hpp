#pragma once
#include <Modes/LightMode.hpp>

class RGBWander : public LEDLightMode
{

    virtual void Init(pixelColor_t& pixelColor) override {
        pixelColor.w = 0;
        }

    std::vector<uint8_t> transferBetweenTwoColors(uint8_t numPixels, uint8_t offset)
    {
        std::vector<uint8_t> ret;
        ret.push_back((255 - ((255 / numPixels) * offset)));
        ret.push_back(0 + ((255 / numPixels) * offset));
        return ret;
    }

    virtual void Run(strand_t pStrand, pixelColor_t &pixelColor, uint8_t brightness, int numberOfLeds, bool reverse, int step) override
    {
        CalcPixelColorBrightness(pixelColor, brightness);
        uint8_t y;
        for (uint8_t i = 0; i < numberOfLeds - (numberOfLeds % 3); i++)
        {
            std::vector<uint8_t> colors = transferBetweenTwoColors(numberOfLeds / 3, (i % (numberOfLeds / 3)) + 1);
            y = (i + step) % numberOfLeds;
            if (i < numberOfLeds / 3)
            {
                pixelColor.r = colors[0] * (brightness / 255.0);
                pixelColor.g = colors[1] * (brightness / 255.0);
                pStrand.pixels[y] = pixelColor;
            }
            else if (i < (numberOfLeds / 3) * 2)
            {
                pixelColor.g = colors[0] * (brightness / 255.0);
                pixelColor.b = colors[1] * (brightness / 255.0);
                pStrand.pixels[y] = pixelColor;
            }
            else
            {
                pixelColor.b = colors[0] * (brightness / 255.0);
                pixelColor.r = colors[1] * (brightness / 255.0);
                pStrand.pixels[y] = pixelColor;
            }
        }
    }
    virtual void Stop() override {}

};