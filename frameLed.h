#include <Adafruit_NeoPixel.h>

// Neopixels
#define PIXELSPIN 13
#define PIXELSCOUNT 116

// Sections of pixels
int pixelTopLeft[2] = {48, 60};
int pixelTopRight[2] = {24, 36};
int pixelBottomLeft[2] = {82, 93};
int pixelBottomRight[2] = {104, 116};

Adafruit_NeoPixel pixels(PIXELSCOUNT, PIXELSPIN, NEO_GRB + NEO_KHZ800);

#define COLOR_WHITE pixels.Color(255, 255, 255)
#define COLOR_RED pixels.Color(255, 0, 0)
#define COLOR_GREEN pixels.Color(0, 255, 0)
#define COLOR_BLUE pixels.Color(0, 0, 255)
#define COLOR_BLACK pixels.Color(0, 0, 0)
#define DEFAULT_BRIGHTNESS 128

// Color buffer to preserve original color when doing animation
uint32_t colorBuffer[PIXELSCOUNT + 1];

// Neopixels functions
//
void initPixels(void)
{
    pixels.begin();
    pixels.setBrightness(DEFAULT_BRIGHTNESS);
    pixels.clear();
    pixels.show();
}

void drawFrameFromBothSides(uint32_t color, uint32_t speed)
{
    int t = PIXELSCOUNT / 2;
    int p1, p2;
    pixels.setPixelColor(t, color);
    pixels.show();
    delay(speed);
    p1 = t - 1;
    p2 = t + 1;
    while (p1 >= 0 && p2 <= PIXELSCOUNT + 1)
    {
        pixels.setPixelColor(p1, color);
        pixels.setPixelColor(p2, color);
        pixels.show();
        delay(speed);
        p1 -= 1;
        p2 += 1;
    }
}

void drawFrameAround(uint32_t color, uint32_t speed)
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

void setPixelSection(int *section, uint32_t color)
{
    for (int i = section[0]; i <= section[1]; i++)
    { //
        pixels.setPixelColor(i, color);
    }
}

void savePixelColor(int *section)
{
    for (int i = section[0]; i <= section[1]; i++)
    {
        colorBuffer[i] = pixels.getPixelColor(i);
    }
}

void restorePixelColor(int *section)
{
    for (int i = section[0]; i <= section[1]; i++)
    {
        pixels.setPixelColor(i, colorBuffer[i]);
    }
}

void blinkSection(int *section, uint32_t color, bool preserveOriginalColor, uint32_t speed, unsigned long duration)
{
    savePixelColor(section);
    unsigned long startTime = millis();
    while (millis() - startTime < duration)
    {
        setPixelSection(section, color);
        pixels.show();
        delay(speed);
        if (preserveOriginalColor)
        {
            restorePixelColor(section);
        }
        else
        {
            setPixelSection(section, COLOR_BLACK);
        }
        pixels.show();
        delay(speed);
    }
    restorePixelColor(section);
    pixels.show();
}

void fade(int speed) {
    int brightness = DEFAULT_BRIGHTNESS-1;
    while(brightness >= 0) {
        pixels.setBrightness(brightness);
        pixels.show();
        delay(speed);
        brightness--;
    }
}
