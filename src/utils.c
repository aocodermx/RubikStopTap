#include "utils.h"

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
