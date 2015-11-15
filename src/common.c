
#include <pebble.h>
#include "common.h"

void timeFromInt ( STime * stime, int itime ) {

  stime -> MilliSeconds = ( int ) itime % 1000; itime /= 1000;
  stime -> Hours        = ( int ) itime / 3600; itime %= 3600;
  stime -> Minutes      = ( int ) itime /   60; itime %=   60;
  stime -> Seconds      = ( int ) itime ;

}

int  intFromTime ( STime * stime ) {
  int itime = 0;

  itime += stime -> MilliSeconds;
  itime += stime -> Seconds *    1000;
  itime += stime -> Minutes *   60000;
  itime += stime -> Hours   * 3600000;

  return itime;
}

int  getCubeSize ( ) {
  return
    persist_exists ( KEY_PERSIST_CUBE_SIZE ) ?
      persist_read_int ( KEY_PERSIST_CUBE_SIZE ) :
      DEFAULT_CUBE_SIZE;
}

void setCubeSize ( int cube_size ) {
  persist_write_int ( KEY_PERSIST_CUBE_SIZE, cube_size );
}

int  getCubeSolves ( int cube_size ) {
  int stats_begin = STATS_START * ( cube_size + 1 );
  return
    persist_exists ( stats_begin + KEY_PERSIST_SOLVES_OFFSET ) ?
      persist_read_int ( stats_begin + KEY_PERSIST_SOLVES_OFFSET ) :
      0;
}

void setCubeSolves ( int cube_size, int total_times ) {
  int stats_begin = STATS_START * ( cube_size + 1 );
  persist_write_int ( stats_begin + KEY_PERSIST_SOLVES_OFFSET, total_times );
}

int  getCubeAverage ( int cube_size ) {
  int stats_begin = STATS_START * ( cube_size + 1 );
  return
    persist_exists ( stats_begin + KEY_PERSIST_AVERAGE_OFFSET ) ?
      persist_read_int ( stats_begin + KEY_PERSIST_AVERAGE_OFFSET ) :
      0;
}

void setCubeAverage ( int cube_size, int total_average ) {
  int stats_begin = STATS_START * ( cube_size + 1 );
  persist_write_int ( stats_begin + KEY_PERSIST_AVERAGE_OFFSET, total_average );
}

int  getCubeMax ( int cube_size ) {
  int stats_begin = STATS_START * ( cube_size + 1 );
  return
    persist_exists( stats_begin + KEY_PERSIST_MAX_OFFSET ) ?
      persist_read_int ( stats_begin + KEY_PERSIST_MAX_OFFSET ) :
      0;
}

void setCubeMax ( int cube_size, int max ) {
  int stats_begin = STATS_START * ( cube_size + 1 );
  persist_write_int ( stats_begin + KEY_PERSIST_MAX_OFFSET, max );
}

int  getCubeMin ( int cube_size ) {
  int stats_begin = STATS_START * ( cube_size + 1 );
  return
    persist_exists( stats_begin + KEY_PERSIST_MIN_OFFSET ) ?
      persist_read_int ( stats_begin + KEY_PERSIST_MIN_OFFSET ) :
      0;
}

void setCubeMin ( int cube_size, int min ) {
  int stats_begin = STATS_START * ( cube_size + 1 );
  persist_write_int ( stats_begin + KEY_PERSIST_MIN_OFFSET, min );
}

void append_time_entry ( int itime ) {
  int cube_size = getCubeSize ( );

  int total_times   = getCubeSolves  ( cube_size );
  int total_average = getCubeAverage ( cube_size );
  int max           = getCubeMax ( cube_size );
  int min           = getCubeMin ( cube_size );

  max = itime > max ? itime : max;
  min = ( itime < min || min == 0 ) ? itime : min;

  total_average = total_times != 0 ? ( total_average * total_times + itime ) / ( total_times + 1 ) : itime ;
  total_times ++;

  setCubeSolves  ( cube_size, total_times );
  setCubeAverage ( cube_size, total_average );
  setCubeMax     ( cube_size, max );
  setCubeMin     ( cube_size, min );
}

void delete_cube_stats ( int cube_size ) {
  APP_LOG ( APP_LOG_LEVEL_INFO, "Delete stats for cube_size %d", cube_size );
  setCubeSolves  ( cube_size, 0 );
  setCubeAverage ( cube_size, 0 );
  setCubeMax     ( cube_size, 0 );
  setCubeMin     ( cube_size, 0 );
}

void delete_all_stats ( ) {
  for (int i = 0; i < MAX_CUBE_SIZE - 1; i++) {
    delete_cube_stats ( i + 2 );
  }
}
