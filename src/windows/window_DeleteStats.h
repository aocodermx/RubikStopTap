
#include "../common.h"

#ifndef WINDOW_DELETE_MODE_STATS_INCLUDE
#define WINDOW_DELETE_MODE_STATS_INCLUDE

#define DELETE_MODE 0
#define DELETE_ALL  1

#define DELETE_MODE_STATS_MESSAGE_START PBL_IF_ROUND_ELSE ( 15, 30 )

void window_deletemodestats_init ( int );
void window_deletemodestats_deinit ( );

#endif
