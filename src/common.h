/*
 * App Common components
 */

#ifndef COMMON_INCLUDE
#define COMMON_INCLUDE

#define KEY_PERSIST_SOLVES      0
#define KEY_PERSIST_AVERAGE     1
#define KEY_PERSIST_MAX         2
#define KEY_PERSIST_MIN         3
#define KEY_PERSIST_MODE        9 // Move this to StopWatchWindow
#define KEY_PERSIST_HISTORY    10

void append_time_entry ( int itime );
void remove_time_entry ( int itime );

#endif
