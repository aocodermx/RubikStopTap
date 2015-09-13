
#include <pebble.h>
#include "common.h"

void append_time_entry ( int itime ) {

  int total_times = persist_exists ( KEY_PERSIST_SOLVES ) ?
    persist_read_int ( KEY_PERSIST_SOLVES ) : 0;

  int total_average = persist_exists ( KEY_PERSIST_AVERAGE ) ?
    persist_read_int ( KEY_PERSIST_AVERAGE ) : 0;

  int max = persist_exists( KEY_PERSIST_MAX ) ?
    persist_read_int ( KEY_PERSIST_MAX ) : 0;

  int min = persist_exists( KEY_PERSIST_MIN ) ?
    persist_read_int ( KEY_PERSIST_MIN ) : 0;

  max = itime > max ? itime : max;
  min = ( itime < min || min == 0 ) ? itime : min;

  total_average = total_times != 0 ? ( total_average * total_times + itime ) / ( total_times + 1 ) : itime ;
  total_times ++;

  persist_write_int ( KEY_PERSIST_SOLVES, total_times );
  persist_write_int ( KEY_PERSIST_AVERAGE, total_average );
  persist_write_int ( KEY_PERSIST_MAX, max );
  persist_write_int ( KEY_PERSIST_MIN, min );
}

void remove_time_entry ( int itime ) {

}
