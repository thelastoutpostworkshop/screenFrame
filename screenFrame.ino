#include "screenFrame.h"
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

    drawFrameFromBothSides(getRandomColor(), 10);

    // Scheduled tasks in milliseconds
    scheduler.addTask(playModeFrame, 2700000L);
    scheduler.addTask(playModeSection, 900000L);
}

void loop()
{
    scheduler.runTasks();
    delay(10);
}

uint32_t getRandomMillis(uint32_t minMillis, uint32_t maxMillis)
{
    if (minMillis > maxMillis)
    {
        // Swap values if they are in the wrong order
        uint32_t temp = minMillis;
        minMillis = maxMillis;
        maxMillis = temp;
    }
    return minMillis + (esp_random() % (maxMillis - minMillis + 1));
}

void playModeFrame(void)
{
    if (suspendAnimations)
    {
        return;
    }
    Frame_Mode frameMode = static_cast<Frame_Mode>(esp_random() % NUM_FRAME_MODES);

    switch (frameMode)
    {
    case MODE_FRAMING:
        pixels.fill(COLOR_BLACK);
        pixels.show();
        delay(10000L);
        drawFrameAround(getRandomColor(), getRandomMillis(5, 50));
        playModeSection();
        break;
    case MODE_FRAME_PULSE:
        pulse(getRandomMillis(5, 15), getRandomMillis(10000L, 30000L));
        playModeSection();
        break;
    case MODE_FRAME_BLINK:
        blinkFrame(getRandomMillis(100, 500), getRandomMillis(10000L, 20000L));
        playModeSection();
    case MODE_FRAME_BOTH_SIDES:
        pixels.fill(COLOR_BLACK);
        pixels.show();
        delay(10000L);
        drawFrameFromBothSides(getRandomColor(), getRandomMillis(5, 50));
        playModeSection();
        break;
    }
}

void playModeSection(void)
{
    if (suspendAnimations)
    {
        return;
    }
    Section_Mode sectionMode = static_cast<Section_Mode>(esp_random() % NUM_MODES_SECTION);
    Section_Positions sectionPosition = static_cast<Section_Positions>(esp_random() % NUM_SECTION_POSITION);

    int *sectionPixels;
    switch (sectionPosition)
    {
    case TopLeft:
        sectionPixels = pixelTopLeft;
        break;
    case TopRight:
        sectionPixels = pixelTopRight;
        break;
    case BottomLeft:
        sectionPixels = pixelBottomLeft;
        break;
    case BottomRight:
        sectionPixels = pixelBottomRight;
        break;
    }
    switch (sectionMode)
    {
    case MODE_SECTION_BLINK:
        blinkSection(sectionPixels, getRandomColor(), true, getRandomMillis(25, 500), getRandomMillis(60000L, 120000L));
        break;
    case MODE_SECTION_CENTER:
        showSectionFromCenter(sectionPixels, getRandomColor(), true, getRandomMillis(15, 100), getRandomMillis(60000L, 120000L));
        break;
    case MODE_SECTION_SCROLL:
        scrollSection(sectionPixels, getRandomColor(), true, getRandomMillis(15, 100), getRandomMillis(60000L, 120000L));
        break;
    }
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