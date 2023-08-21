#include <Adafruit_NeoPixel.h>
#include <HTTPClient.h>
#include "frameWeb.h"

// Neopixels
#define PIXELSPIN 13
#define PIXELSCOUNT 119

Adafruit_NeoPixel pixels(PIXELSCOUNT, PIXELSPIN, NEO_GRB + NEO_KHZ800);

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

    showRainBow();
}

void loop()
{
}

// Neopixels functions
//
void initPixels(void)
{
    pixels.begin();
    pixels.clear();
    pixels.show();
}

void showRainBow(void)
{
    pixels.rainbow();
    pixels.setBrightness(100);
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