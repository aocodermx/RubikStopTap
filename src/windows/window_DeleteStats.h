
#include "../common.h"

#ifndef WINDOW_DELETE_MODE_STATS_INCLUDE
#define WINDOW_DELETE_MODE_STATS_INCLUDE

#define DELETE_MODE 0
#define DELETE_ALL  1

#define DELETE_MODE_STATS_MESSAGE_START 30
#define DELETE_MODE_STATS_MESSAGE "Are you sure to delete the 3x3x3 stats?"
#define DELETE_ALL_STATS_MESSAGE  "Are you sure to delete all the stats?"

void window_deletemodestats_init ( int );
void window_deletemodestats_deinit ( );

#endif
