/*
 * StopWatchWindow definitions
 */

#include "../common.h"

#ifndef WINDOW_STOP_WATCH_INCLUDE
#define WINDOW_STOP_WATCH_INCLUDE

#define STOP_WATCH_VIEW_TIME           15000
#define STOP_WATCH_PRESICION             125
#define STOP_WATCH_DISPLAY_OFFSET         13
#define STOP_WATCH_LAYER_PHASE_HEIGHT     25
#define STOP_WATCH_LAYER_LV1_HEIGHT       55
#define STOP_WATCH_LAYER_LV2_HEIGHT       40
#define STOP_WATCH_LAYER_STATS_LV1_HEIGHT 20
#define STOP_WATCH_LAYER_STATS_LV2_HEIGHT 16

#define STOPWATCH_STATE_STOP      "Scramble it"
#define STOPWATCH_STATE_VIEW      "Review it"
#define STOPWATCH_STATE_START     "Solve it"
#define STOPWATCH_STATE_REVIEW    "Your time!"
#define STOPWATCH_CONGRATULATIONS "New Record!"

typedef enum { STOP, VIEW, START, REVIEW  } STOPWATCH_STATE;

void window_stopwatch_init();
void window_stopwatch_deinit();

#endif
