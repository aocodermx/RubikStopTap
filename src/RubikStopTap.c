#include <pebble.h>

/*
 * App Common components
 */



/*
 * StatsWindow definitions.
 */
#define KEY_PERSIST_SOLVES      0
#define KEY_PERSIST_TOTAL_TIME  1
#define KEY_PERSIST_MAX         2
#define KEY_PERSIST_MIN         3
#define KEY_PERSIST_MODE        9 // Move this to StopWatchWindow
#define KEY_PERSIST_HISTORY    10

static unsigned int s_stats_solves     = 0;
static unsigned int s_stats_total_time = 0;
static unsigned int s_stats_max        = 0;
static unsigned int s_stats_min        = 0;
// Calculate average at fly
static unsigned int s_elapsed_time     = 0;

/*
 * StatsWindow: Window to kepp track of user stats.
 */

static void stats_refresh ( ) {
  s_stats_solves ++;
  s_stats_total_time += s_elapsed_time;

  s_stats_max = s_elapsed_time > s_stats_max ? s_elapsed_time : s_stats_max;
  s_stats_min = ( s_elapsed_time < s_stats_min || s_stats_min == 0 ) ? s_elapsed_time : s_stats_min;

  APP_LOG ( APP_LOG_LEVEL_INFO, "Solves : %d", s_stats_solves );
  APP_LOG ( APP_LOG_LEVEL_INFO, "Average: %d", ( unsigned int ) ( ( double ) s_stats_total_time / s_stats_solves ) );
  APP_LOG ( APP_LOG_LEVEL_INFO, "Maximum: %d", s_stats_max );
  APP_LOG ( APP_LOG_LEVEL_INFO, "Minimum: %d", s_stats_min );

  persist_write_int ( KEY_PERSIST_SOLVES, s_stats_solves );
  persist_write_int ( KEY_PERSIST_TOTAL_TIME, s_stats_total_time );
  persist_write_int ( KEY_PERSIST_MAX, s_stats_max );
  persist_write_int ( KEY_PERSIST_MIN, s_stats_min );
}

/*
 * StopWatchWindow definitions
 */

#define STOP_WATCH_VIEW_TIME           15
#define STOP_WATCH_REFRESH_TIME       125
#define STOP_WATCH_LAYER_LV1_HEIGHT    45
#define STOP_WATCH_LAYER_LV2_HEIGHT    40
#define STOP_WATCH_LAYER_PHASE_HEIGHT  20
#define STOP_WATCH_LAYER_STATS_LV1_HEIGHT 20
#define STOP_WATCH_LAYER_STATS_LV2_HEIGHT 20
#define STOP_WATCH_LAYER_STATS_PADDING  2

typedef enum { STOP, VIEW, START, REVIEW  } STOPWATCH_STATE;
STOPWATCH_STATE stopwatch_state = STOP;

#define STOPWATCH_STATE_STOP   " Scramble it again"
#define STOPWATCH_STATE_VIEW   " Make your strategy"
#define STOPWATCH_STATE_START  " Solve it"
#define STOPWATCH_STATE_REVIEW " Your time!"

static Window    *s_stopwatch_window;
static TextLayer *s_tlayer_time_lv1;
static TextLayer *s_tlayer_time_lv2;
static TextLayer *s_tlayer_phase;
static TextLayer *s_tlayer_average;
static TextLayer *s_tlayer_times;
static TextLayer *s_tlayer_max;
static TextLayer *s_tlayer_min;

static char       s_the_time[6];
static char       s_the_time_ms[4];
static double     s_start_time;

static void accel_tap_stopwatch_handler ( AccelAxisType, int32_t );

/*
 * StopWatchWindow: Main window to display current time.
 */

static void update_ui_stats ( ) {
  static char text_solves[15];
  static char text_avg[10];
  static char text_max[10];
  static char text_min[10];

  s_stats_solves     = persist_exists ( KEY_PERSIST_SOLVES )     ? persist_read_int ( KEY_PERSIST_SOLVES )     : 0;
  s_stats_total_time = persist_exists ( KEY_PERSIST_TOTAL_TIME ) ? persist_read_int ( KEY_PERSIST_TOTAL_TIME ) : 0;
  s_stats_max        = persist_exists ( KEY_PERSIST_MAX )        ? persist_read_int ( KEY_PERSIST_MAX )        : 0;
  s_stats_min        = persist_exists ( KEY_PERSIST_MIN )        ? persist_read_int ( KEY_PERSIST_MIN )        : 0;

  snprintf ( text_solves, sizeof ( text_solves ), "AVG(%d):", s_stats_solves );
  snprintf ( text_avg, sizeof ( text_avg ), "%02d.%03d", ( s_stats_total_time / s_stats_solves ) / 1000, ( s_stats_total_time / s_stats_solves ) % 1000 );
  snprintf ( text_max, sizeof ( text_max), "M: %02d.%03d", s_stats_max / 1000, s_stats_max % 1000 );
  snprintf ( text_min, sizeof ( text_min), "m: %02d.%03d", s_stats_min / 1000, s_stats_min % 1000 );

  text_layer_set_text ( s_tlayer_times, text_solves );
  text_layer_set_text ( s_tlayer_average, text_avg );
  text_layer_set_text ( s_tlayer_max, text_max );
  text_layer_set_text ( s_tlayer_min, text_min );
}

static void app_timer_stopwatch_handler ( void * data ) {

  double end_time = time ( NULL ) + ( double ) time_ms ( NULL, NULL ) / 1000;
  double elapsed_time = end_time - s_start_time;

  int elapsed_time_min = ( int ) elapsed_time / 60;
  int elapsed_time_seg = ( int ) elapsed_time  - elapsed_time_min * 60 ;
  int elapsed_time_ms  = ( int ) ( elapsed_time * 1000 ) % 1000;

  snprintf ( s_the_time   , sizeof ( s_the_time    ), "%02d:%02d", elapsed_time_min, elapsed_time_seg );
  snprintf ( s_the_time_ms, sizeof ( s_the_time_ms ), "%03d"     , elapsed_time_ms );
  text_layer_set_text ( s_tlayer_time_lv1, s_the_time );
  text_layer_set_text ( s_tlayer_time_lv2, s_the_time_ms );

  s_elapsed_time = elapsed_time * 1000;

  // APP_LOG ( APP_LOG_LEVEL_INFO, "STATE: %d", stopwatch_state );
  switch ( stopwatch_state ) {
    case STOP:
      break;
    case REVIEW:
      break;
    case VIEW:
      app_timer_register ( STOP_WATCH_REFRESH_TIME, app_timer_stopwatch_handler, NULL );
      if ( elapsed_time > STOP_WATCH_VIEW_TIME ) {
        vibes_short_pulse ( );
        text_layer_set_text ( s_tlayer_phase, STOPWATCH_STATE_START );
        s_start_time = time ( NULL ) + ( double ) time_ms ( NULL, NULL ) / 1000;
        stopwatch_state = START;
      }
      break;
    case START:
      app_timer_register ( STOP_WATCH_REFRESH_TIME, app_timer_stopwatch_handler, NULL );
      if ( elapsed_time > 1 ) {
        accel_tap_service_subscribe ( accel_tap_stopwatch_handler );
      }
      break;
  }
}

static void accel_tap_stopwatch_handler ( AccelAxisType axis, int32_t direction ) {

  switch ( stopwatch_state ) {
    case STOP: // Start review time
      text_layer_set_text ( s_tlayer_phase, STOPWATCH_STATE_VIEW );
      vibes_short_pulse ( );
      s_start_time = time ( NULL ) + ( double ) time_ms ( NULL, NULL ) / 1000;
      app_timer_register ( STOP_WATCH_REFRESH_TIME, app_timer_stopwatch_handler, NULL );
      stopwatch_state = VIEW;
      accel_tap_service_unsubscribe ( );
      break;
    case VIEW:  // Start real time
      // text_layer_set_text ( s_tlayer_phase, STOPWATCH_STATE_START );
      // vibes_short_pulse ( );
      // s_start_time = time ( NULL ) + ( double ) time_ms ( NULL, NULL ) / 1000;
      // stopwatch_state = START;
      break;
    case START:
      // Check for any type of reccord
      text_layer_set_text ( s_tlayer_phase, STOPWATCH_STATE_REVIEW );
      vibes_short_pulse ( );
      stopwatch_state = REVIEW;
      break;
    case REVIEW:
      text_layer_set_text ( s_tlayer_phase, STOPWATCH_STATE_STOP );
      text_layer_set_text ( s_tlayer_time_lv1, "00:00" );
      text_layer_set_text ( s_tlayer_time_lv2, "000" );
      stats_refresh ( );
      update_ui_stats ( );
      vibes_short_pulse ( );
      stopwatch_state = STOP;
      break;
  }
}

static void window_load ( Window *window ) {
  Layer *window_layer = window_get_root_layer ( window );
  GRect bounds = layer_get_bounds ( window_layer );

  accel_tap_service_subscribe ( accel_tap_stopwatch_handler );

  s_tlayer_phase = text_layer_create ( GRect ( STOP_WATCH_LAYER_STATS_PADDING, STOP_WATCH_LAYER_STATS_PADDING, bounds.size.w - 2 * STOP_WATCH_LAYER_STATS_PADDING, STOP_WATCH_LAYER_PHASE_HEIGHT ) );
  text_layer_set_text_alignment ( s_tlayer_phase, GTextAlignmentLeft );
  text_layer_set_background_color ( s_tlayer_phase, GColorBlack );
  text_layer_set_text_color ( s_tlayer_phase, GColorClear );
  text_layer_set_font ( s_tlayer_phase, fonts_get_system_font ( FONT_KEY_GOTHIC_14_BOLD ) );
  text_layer_set_text ( s_tlayer_phase, STOPWATCH_STATE_STOP );
  layer_add_child ( window_layer, text_layer_get_layer ( s_tlayer_phase ) );

  s_tlayer_time_lv1 = text_layer_create ( GRect ( 0, STOP_WATCH_LAYER_PHASE_HEIGHT + STOP_WATCH_LAYER_STATS_PADDING, bounds.size.w, STOP_WATCH_LAYER_LV1_HEIGHT ) );
  text_layer_set_text ( s_tlayer_time_lv1, "00:00" );
  text_layer_set_text_alignment ( s_tlayer_time_lv1, GTextAlignmentCenter );
  text_layer_set_font ( s_tlayer_time_lv1, fonts_get_system_font ( FONT_KEY_BITHAM_42_MEDIUM_NUMBERS ) );
  layer_add_child ( window_layer, text_layer_get_layer ( s_tlayer_time_lv1 ) );

  s_tlayer_time_lv2 = text_layer_create ( GRect ( 0, STOP_WATCH_LAYER_LV1_HEIGHT + STOP_WATCH_LAYER_PHASE_HEIGHT, bounds.size.w, STOP_WATCH_LAYER_LV2_HEIGHT ) );
  text_layer_set_text ( s_tlayer_time_lv2, "000" );
  text_layer_set_text_alignment ( s_tlayer_time_lv2, GTextAlignmentCenter );
  text_layer_set_font ( s_tlayer_time_lv2, fonts_get_system_font ( FONT_KEY_BITHAM_34_MEDIUM_NUMBERS ) );
  layer_add_child ( window_layer, text_layer_get_layer ( s_tlayer_time_lv2 ) );

  s_tlayer_times = text_layer_create (
    GRect (
      STOP_WATCH_LAYER_STATS_PADDING,
      bounds.size.h - STOP_WATCH_LAYER_STATS_LV1_HEIGHT - STOP_WATCH_LAYER_STATS_LV2_HEIGHT - 2 * STOP_WATCH_LAYER_STATS_PADDING,
      bounds.size.w / 2 - 2 * STOP_WATCH_LAYER_STATS_PADDING,
      STOP_WATCH_LAYER_STATS_LV1_HEIGHT ) );
  text_layer_set_text_alignment ( s_tlayer_times, GTextAlignmentCenter );
  text_layer_set_background_color ( s_tlayer_times, GColorBlack );
  text_layer_set_text_color ( s_tlayer_times, GColorClear );
  text_layer_set_font ( s_tlayer_times, fonts_get_system_font ( FONT_KEY_GOTHIC_14 ) );
  text_layer_set_text ( s_tlayer_times, "AVG (0000):" );
  layer_add_child ( window_layer, text_layer_get_layer ( s_tlayer_times ) );

  s_tlayer_average = text_layer_create (
    GRect (
      bounds.size.w / 2 ,
      bounds.size.h - STOP_WATCH_LAYER_STATS_LV1_HEIGHT - STOP_WATCH_LAYER_STATS_LV2_HEIGHT - 2 * STOP_WATCH_LAYER_STATS_PADDING,
      bounds.size.w / 2 - STOP_WATCH_LAYER_STATS_PADDING,
      STOP_WATCH_LAYER_STATS_LV1_HEIGHT ) );
  text_layer_set_text_alignment ( s_tlayer_average, GTextAlignmentCenter );
  text_layer_set_background_color ( s_tlayer_average, GColorBlack );
  text_layer_set_text_color ( s_tlayer_average, GColorClear );
  text_layer_set_font ( s_tlayer_average, fonts_get_system_font ( FONT_KEY_GOTHIC_14 ) );
  text_layer_set_text ( s_tlayer_average, "M: 00:00.000" );
  // text_layer_set_text ( s_tlayer_average, "00:00.000" );
  layer_add_child ( window_layer, text_layer_get_layer ( s_tlayer_average ) );

  s_tlayer_max = text_layer_create (
    GRect (
      STOP_WATCH_LAYER_STATS_PADDING,
      bounds.size.h - STOP_WATCH_LAYER_STATS_LV2_HEIGHT - STOP_WATCH_LAYER_STATS_PADDING,
      bounds.size.w / 2 - 2 * STOP_WATCH_LAYER_STATS_PADDING,
      STOP_WATCH_LAYER_STATS_LV2_HEIGHT ) );
  text_layer_set_text_alignment ( s_tlayer_max, GTextAlignmentCenter );
  text_layer_set_background_color ( s_tlayer_max, GColorBlack );
  text_layer_set_text_color ( s_tlayer_max, GColorClear );
  text_layer_set_font ( s_tlayer_max, fonts_get_system_font ( FONT_KEY_GOTHIC_14 ) );
  text_layer_set_text ( s_tlayer_max, "00:00.000" );
  // text_layer_set_text ( s_tlayer_max, "M: 00:00.000" );
  layer_add_child ( window_layer, text_layer_get_layer ( s_tlayer_max ) );

  s_tlayer_min = text_layer_create (
    GRect (
      bounds.size.w / 2 ,
      bounds.size.h - STOP_WATCH_LAYER_STATS_LV1_HEIGHT - STOP_WATCH_LAYER_STATS_PADDING,
      bounds.size.w / 2 - STOP_WATCH_LAYER_STATS_PADDING,
      STOP_WATCH_LAYER_STATS_LV1_HEIGHT ) );
  text_layer_set_text_alignment ( s_tlayer_min, GTextAlignmentCenter );
  text_layer_set_background_color ( s_tlayer_min, GColorBlack );
  text_layer_set_text_color ( s_tlayer_min, GColorClear );
  text_layer_set_font ( s_tlayer_min, fonts_get_system_font ( FONT_KEY_GOTHIC_14 ) );
  text_layer_set_text ( s_tlayer_min, "m: 00:00.000" );
  layer_add_child ( window_layer, text_layer_get_layer ( s_tlayer_min ) );

  update_ui_stats ( );
}

static void window_unload ( Window *window ) {
  text_layer_destroy ( s_tlayer_time_lv1 );
  text_layer_destroy ( s_tlayer_time_lv2 );
  text_layer_destroy ( s_tlayer_phase );
  accel_tap_service_unsubscribe ( );
}

static void init ( void ) {
  s_stopwatch_window = window_create ( );
  window_set_window_handlers ( s_stopwatch_window, ( WindowHandlers ) {
    .load = window_load,
    .unload = window_unload,
  } );
  window_stack_push ( s_stopwatch_window, true );
}

static void deinit ( void ) {
  window_destroy ( s_stopwatch_window );
}






/*
 * Main application lifecycle
 */
int main ( void ) {
  init ( );
  app_event_loop ( );
  deinit ( );
}
