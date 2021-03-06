/*
 * App Common components
 */

#ifndef COMMON_INCLUDE
#define COMMON_INCLUDE

// Colors
#define COLOR_BACKGROUND GColorCeleste

// Config section
#define KEY_PERSIST_CUBE_SIZE      0
#define KEY_PERSIST_PRESICION      1

// Stats save section
#define STATS_START   10
#define KEY_PERSIST_SOLVES_OFFSET    0
#define KEY_PERSIST_AVERAGE_OFFSET   1
#define KEY_PERSIST_MAX_OFFSET       2
#define KEY_PERSIST_MIN_OFFSET       3
#define KEY_PERSIST_WORLD_MIN_OFFSET 4
#define KEY_PERSIST_WORLD_MAX_OFFSET 5
#define KEY_PERSIST_CONTINENT_MIN_OFFSET 6
#define KEY_PERSIST_CONTINENT_MAX_OFFSET 7
#define KEY_PERSIST_COUNTRY_MIN_OFFSET 8
#define KEY_PERSIST_COUNTRY_MAX_OFFSET 9

#define DEFAULT_CUBE_SIZE   3
#define MAX_CUBE_SIZE      12

typedef struct {
  int Hours;
  int Minutes;
  int Seconds;
  int MilliSeconds;
} STime;

int  getCubeSize     ( );
int  getCubeSolves   ( int );
int  getCubeAverage  ( int );
int  getCubeMax      ( int );
int  getCubeMin      ( int );
int  getCubeWorldMin ( int );
int  getCubeWorldMax ( int );
int  getCubeContinentMin ( int );
int  getCubeContinentMax ( int );
int  getCubeCountryMin ( int );
int  getCubeCountryMax ( int );

void setCubeSize     ( int );
void setCubeSolves   ( int, int );
void setCubeAverage  ( int, int );
void setCubeMax      ( int, int );
void setCubeMin      ( int, int );
void setCubeWorldMin ( int, int );
void setCubeWorldMax ( int, int );
void setCubeContinentMin ( int, int );
void setCubeContinentMax ( int, int );
void setCubeCountryMin ( int, int );
void setCubeCountryMax ( int, int );

void timeFromInt       ( STime *, int );
int  intFromTime       ( STime * );
void append_time_entry ( int );
void delete_cube_stats ( int );
void delete_all_stats  ( );

#endif
