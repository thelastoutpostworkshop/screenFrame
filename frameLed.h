#include <Adafruit_NeoPixel.h>

// Neopixels
#define PIXELSPIN 13
#define PIXELSCOUNT 116

// Sections of pixels
uint16_t pixelTopLeft[2] = {48, 60};
uint16_t pixelTopRight[2] = {24, 36};
uint16_t pixelBottomLeft[2] = {82, 93};

Adafruit_NeoPixel pixels(PIXELSCOUNT, PIXELSPIN, NEO_GRB + NEO_KHZ800);

#define COLOR_WHITE pixels.Color(255, 255, 255)
#define COLOR_RED pixels.Color(255, 0, 0)
#define COLOR_GREEN pixels.Color(0, 255, 0)
#define COLOR_BLUE pixels.Color(0, 0, 255)
#define COLOR_BLACK pixels.Color(0, 0, 0)
#define DEFAULT_BRIGHTNESS 128

// Neopixels functions
//
void initPixels(void)
{
    pixels.begin();
    pixels.setBrightness(DEFAULT_BRIGHTNESS);
    pixels.clear();
    pixels.show();
}

void showDrawFrame(uint32_t color, uint32_t speed)
{
    for (int i = 0; i < PIXELSCOUNT; i++)
    {
        pixels.setPixelColor(i, color);
        pixels.show();
        delay(speed);
    }
}

void showColor(uint32_t color)
{
    pixels.fill(color);
    pixels.show();
}

void showRainBow(void)
{
    pixels.rainbow();
    pixels.show();
}

void setPixelSection(uint16_t *section, uint32_t color)
{
    for (uint16_t i = section[0]; i <= section[1]; i++)
    {
        pixels.setPixelColor(i,color);
    }
    
}

void blinkSection(uint16_t *section, uint32_t color, uint32_t speed, unsigned long duration)
{
    unsigned long startTime = millis();
    while (millis() - startTime < duration)
    {
        setPixelSection(section, color);
        pixels.show();
        delay(speed);
        setPixelSection(section, COLOR_BLACK);
        pixels.show();
        delay(speed);
    }
}
