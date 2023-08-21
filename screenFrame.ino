#include <Adafruit_NeoPixel.h>
#include <HTTPClient.h>
#include "frameWeb.h"

// Neopixels
#define PIXELSPIN 13
#define PIXELSCOUNT 119

Adafruit_NeoPixel pixels(PIXELSCOUNT, PIXELSPIN, NEO_GRB + NEO_KHZ800);

#define COLOR_WHITE pixels.Color(255, 255, 255)
#define COLOR_RED   pixels.Color(255, 0, 0)
#define COLOR_GREEN pixels.Color(0, 255, 0)
#define COLOR_BLUE  pixels.Color(0, 0, 255)
#define DEFAULT_BRIGHTNESS 100

void setup()
{
    Serial.begin(115200);
    randomSeed(analogRead(0));
    initPixels();
    initWebServer();

    xTaskCreatePinnedToCore(
        handleBrowserCalls,   /* Function to implement the task */
        "handleBrowserCalls", /* Name of the task */
        10000,                /* Stack size in words */
        NULL,                 /* Task input parameter */
        1,                    /* Priority of the task */
        NULL,                 /* Task handle. */
        1);                   /* Core where the task should run */

    showColor(COLOR_BLUE);
}

void loop()
{
}

// Neopixels functions
//
void initPixels(void)
{
    pixels.begin();
    pixels.setBrightness(DEFAULT_BRIGHTNESS);
    pixels.clear();
    pixels.show();
}

void showDrawFrame(uint32_t color) {
    for (int i = 0; i < PIXELSCOUNT; i++)
    {
        pixels.setPixelColor(i,color);
        pixels.show();
        delay(50);
    }
}

void showColor(uint32_t color) {
    pixels.fill(color);
    pixels.show();
}

void showRainBow(void)
{
    pixels.rainbow();
    pixels.show();
}

// Task for the web browser
//
void handleBrowserCalls(void *parameter)
{
    for (;;)
    {
        server.handleClient();
        delay(1); // allow other tasks to run
    }
}