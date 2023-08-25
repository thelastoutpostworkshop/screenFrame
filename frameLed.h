#include <Adafruit_NeoPixel.h>

// Neopixels
#define PIXELSPIN 13
#define PIXELSCOUNT 118

// Sections of pixels
int pixelTopLeft[2] = {48, 60};
int pixelTopRight[2] = {24, 36};
int pixelBottomLeft[2] = {82, 93};
int pixelBottomRight[2] = {104, 116};

Adafruit_NeoPixel pixels(PIXELSCOUNT, PIXELSPIN, NEO_GRB + NEO_KHZ800);

#define COLOR_RED pixels.Color(255, 0, 0)
#define COLOR_GREEN pixels.Color(0, 255, 0)
#define COLOR_BLUE pixels.Color(0, 0, 255)
#define COLOR_YELLOW pixels.Color(255, 255, 0)
#define COLOR_CYAN pixels.Color(0, 255, 255)
#define COLOR_MAGENTA pixels.Color(255, 0, 255)
#define COLOR_ORANGE pixels.Color(255, 165, 0)
#define COLOR_PURPLE pixels.Color(128, 0, 128)
#define COLOR_BLACK pixels.Color(0, 0, 0)
#define COLOR_WHITE pixels.Color(255, 255, 255)

uint32_t colors[] = {
    COLOR_RED,
    COLOR_GREEN,
    COLOR_BLUE,
    COLOR_YELLOW,
    COLOR_CYAN,
    COLOR_MAGENTA,
    COLOR_ORANGE,
    COLOR_PURPLE};

#define DEFAULT_BRIGHTNESS 255

// Color buffer to preserve original color when doing animation
uint32_t colorBuffer[PIXELSCOUNT + 1];

// Neopixels functions
//

uint32_t getRandomColor()
{
    int randomIndex = esp_random() % (sizeof(colors) / sizeof(colors[0]));
    return colors[randomIndex];
}

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

void showSectionFromCenter(int *section, uint32_t color, bool preserveOriginalColor, uint32_t speed, unsigned long duration)
{
    savePixelColor(section);
    setPixelSection(section, COLOR_BLACK);
    pixels.show();
    unsigned long startTime = millis();
    int middlePixel = ((section[1] - section[0] + 1) / 2) + section[0];
    int p1, p2;
    pixels.setPixelColor(middlePixel, color);
    pixels.show();
    delay(speed);
    p1 = middlePixel - 1;
    p2 = middlePixel + 1;
    while (millis() - startTime < duration)
    {
        while (p1 >= section[0] && p2 <= section[1])
        {
            pixels.setPixelColor(p1, color);
            pixels.setPixelColor(p2, color);
            pixels.show();
            delay(speed);
            p1 -= 1;
            p2 += 1;
        }
        setPixelSection(section, COLOR_BLACK);
        pixels.show();
        p1 = middlePixel - 1;
        p2 = middlePixel + 1;
        delay(speed);
    }

    restorePixelColor(section);
    pixels.show();
}

void scrollSection(int *section, uint32_t color, bool preserveOriginalColor, uint32_t speed, unsigned long duration)
{
    savePixelColor(section);
    setPixelSection(section, COLOR_BLACK);
    pixels.show();
    unsigned long startTime = millis();
    int currentLED = section[0];
    while (millis() - startTime < duration)
    {
        pixels.setPixelColor(currentLED, color);
        pixels.show();
        currentLED++;
        if (currentLED > section[1])
        {
            currentLED = section[0];
            setPixelSection(section, COLOR_BLACK);
            pixels.show();
        }
        delay(speed);
    }
    restorePixelColor(section);
    pixels.show();
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

void fade(int speed)
{
    int brightness = pixels.getBrightness();
    while (brightness >= 0)
    {
        pixels.setBrightness(brightness);
        pixels.show();
        delay(speed);
        brightness -= 2;
    }
    pixels.clear();
    pixels.setBrightness(DEFAULT_BRIGHTNESS);
    pixels.show();
}

void pulse(int speed, unsigned long duration)
{
    unsigned long startTime = millis();
    int brightness;
    while (millis() - startTime < duration)
    {
        // Fading out
        brightness = DEFAULT_BRIGHTNESS;
        while (brightness >= 5)
        {
            pixels.setBrightness(brightness);
            pixels.show();
            delay(speed);
            brightness -= 2; // Adjust this value if fading is too fast or slow
        }

        // Fading in
        brightness = 2;
        while (brightness <= DEFAULT_BRIGHTNESS)
        {
            pixels.setBrightness(brightness);
            pixels.show();
            delay(speed);
            brightness += 2; // Adjust this value if fading is too fast or slow
        }
    }
}

void blinkFrame(int speed, unsigned long duration)
{
    unsigned long startTime = millis();
    uint32_t color = pixels.getPixelColor(0);
    while (millis() - startTime < duration)
    {
        pixels.fill(color);
        pixels.show();
        delay(100);
        pixels.clear();
        pixels.show();
        delay(100);
    }
    pixels.fill(color);
    pixels.show();
}

void frameDemo(void)
{
    pixels.clear();
    pixels.show();
    delay(5000);
    drawFrameFromBothSides(COLOR_BLUE, 25);
    blinkSection(pixelBottomRight, COLOR_RED, false, 200, 3000);
    fade(25);
    drawFrameAround(COLOR_RED, 10);
    blinkSection(pixelTopLeft, COLOR_BLUE, false, 50, 3000);
    pulse(10, 3000);
    scrollSection(pixelTopRight, COLOR_WHITE, true, 25, 2000);
    showSectionFromCenter(pixelBottomLeft, COLOR_GREEN, false, 75, 2000);
}