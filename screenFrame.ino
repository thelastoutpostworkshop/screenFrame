#include <HTTPClient.h>
#include "frameLed.h"
#include "frameWeb.h"

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

    drawFrameFromBothSides(COLOR_BLUE, 25);
    blinkSection(pixelBottomRight, COLOR_GREEN, false, 200, 10000);
    fade(100);
}

void loop()
{
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