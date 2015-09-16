/*
 * StopWatchWindow definitions
 */

#include "../common.h"

#ifndef WINDOW_STOP_WATCH_INCLUDE
#define WINDOW_STOP_WATCH_INCLUDE

#define STOP_WATCH_VIEW_TIME           15000
#define STOP_WATCH_REFRESH_TIME          125
#define STOP_WATCH_LAYER_PHASE_HEIGHT     25
#define STOP_WATCH_LAYER_LV1_HEIGHT       55
#define STOP_WATCH_LAYER_LV2_HEIGHT       40
#define STOP_WATCH_LAYER_STATS_LV1_HEIGHT 20
#define STOP_WATCH_LAYER_STATS_LV2_HEIGHT 25

#define STOPWATCH_STATE_STOP   "Scramble it again"
#define STOPWATCH_STATE_VIEW   "Make your strategy"
#define STOPWATCH_STATE_START  "Solve it"
#define STOPWATCH_STATE_REVIEW "Your time!"

typedef enum { STOP, VIEW, START, REVIEW  } STOPWATCH_STATE;

void window_stopwatch_init();
void window_stopwatch_deinit();

#endif
