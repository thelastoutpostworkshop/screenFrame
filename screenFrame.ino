#include <Adafruit_NeoPixel.h>
#include <HTTPClient.h>
#include "frameWeb.h"

// Neopixels
#define PIXELSPIN 13
#define PIXELSCOUNT 50

Adafruit_NeoPixel pixels(PIXELSCOUNT, PIXELSPIN, NEO_GRB + NEO_KHZ800);

void setup()
{
    Serial.begin(115200);
    randomSeed(analogRead(0));

    initPixels();
 
}

void loop()
{

}

// Neopixels functions
//
void initPixels(void)
{
    pixels.begin();
    pixels.rainbow();
    pixels.show();
}