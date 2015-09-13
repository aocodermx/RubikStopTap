/*
 * Main application lifecycle
 */

#include <pebble.h>
#include "windows/window_StopWatch.h"


static void app_init ( void ) {
  window_stopwatch_init ( );
}

static void app_deinit ( void ) {
  window_stopwatch_deinit ( );
}

int main ( void ) {
  app_init ( );
  app_event_loop ( );
  app_deinit ( );
}
