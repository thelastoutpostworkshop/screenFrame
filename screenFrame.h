#include <HTTPClient.h>
#include "frameLed.h"
#include "frameWeb.h"
#include "scheduler.h"

enum General_Mode
{
    MODE_FRAME,
    MODE_SECTION,
    NUM_MODES_GENERAL
};

enum Frame_Mode
{
    MODE_FRAMING,
    MODE_FRAME_PULSE,
    MODE_FRAME_BLINK,
    MODE_FRAME_BOTH_SIDES,
    NUM_FRAME_MODES // This is a trick to get the number of modes
};

enum Section_Mode
{
    MODE_SECTION_BLINK,
    MODE_SECTION_SCROLL,
    MODE_SECTION_CENTER,
    NUM_MODES_SECTION
};

enum Section_Positions
{
    TopLeft,
    TopRight,
    BottomLeft,
    BottomRight,
    NUM_SECTION_POSITION
};

// Task Scheduler called in the Loop function
TaskScheduler scheduler;

bool suspendAnimations = false;