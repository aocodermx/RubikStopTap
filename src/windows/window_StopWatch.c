/*
 * StopWatchWindow: Main window to display current time.
 */
#include <pebble.h>
#include "utils.h"
#include "common.h"
#include "window_StopWatch.h"


static Window    *s_stopwatch_window;
static TextLayer *s_tlayer_time_lv1;
static TextLayer *s_tlayer_time_lv2;
static TextLayer *s_tlayer_phase;
static TextLayer *s_tlayer_times_average;
static TextLayer *s_tlayer_max;
static TextLayer *s_tlayer_min;

static STime stime           = { 0 };
static int   s_start_time    = 0;
static int   s_elapsed_time  = 0;

STOPWATCH_STATE stopwatch_state = STOP;

static void window_load ( Window * );
static void window_unload ( Window * );
static void app_timer_stopwatch_handler ( void * );
static void accel_tap_stopwatch_handler ( AccelAxisType, int32_t );
static void update_ui_stats ( );


void window_stopwatch_init ( ) {
  s_stopwatch_window = window_create ( );
  window_set_window_handlers ( s_stopwatch_window, ( WindowHandlers ) {
    .load = window_load,
    .unload = window_unload,
  } );
  window_stack_push ( s_stopwatch_window, true );
}

void window_stopwatch_deinit ( ) {
  window_destroy ( s_stopwatch_window );
}

static void update_ui_stats ( ) {
  static char text_solves_average[25];
  static char text_max[10];
  static char text_min[10];

  int   s_stats_solves  = 0;
  int   s_stats_average = 0;
  int   s_stats_max     = 0;
  int   s_stats_min     = 0;

  s_stats_solves     = persist_exists ( KEY_PERSIST_SOLVES )     ? persist_read_int ( KEY_PERSIST_SOLVES )     : 0;
  s_stats_average    = persist_exists ( KEY_PERSIST_AVERAGE )    ? persist_read_int ( KEY_PERSIST_AVERAGE )    : 0;
  s_stats_max        = persist_exists ( KEY_PERSIST_MAX )        ? persist_read_int ( KEY_PERSIST_MAX )        : 0;
  s_stats_min        = persist_exists ( KEY_PERSIST_MIN )        ? persist_read_int ( KEY_PERSIST_MIN )        : 0;

  timeFromInt ( &stime, s_stats_average );
  snprintf ( text_solves_average, sizeof ( text_solves_average ), "AVG(%d): %02d:%02d.%03d", s_stats_solves, stime.Minutes, stime.Seconds, stime.MilliSeconds );
  timeFromInt ( &stime, s_stats_max );
  snprintf ( text_max, sizeof ( text_max), "M:%02d:%02d.%03d", stime.Minutes, stime.Seconds, stime.MilliSeconds );
  timeFromInt ( &stime, s_stats_min );
  snprintf ( text_min, sizeof ( text_min), "m:%02d:%02d.%03d", stime.Minutes, stime.Seconds, stime.MilliSeconds );

  text_layer_set_text ( s_tlayer_times_average, text_solves_average );
  // text_layer_set_text ( s_tlayer_average, text_avg );
  text_layer_set_text ( s_tlayer_max, text_max );
  text_layer_set_text ( s_tlayer_min, text_min );
}

static void app_timer_stopwatch_handler ( void * data ) {
  static char  s_the_time[6];
  static char  s_the_time_ms[4];

  int end_time = time ( NULL ) * 1000 + ( double ) time_ms ( NULL, NULL );
  s_elapsed_time = end_time - s_start_time;

  timeFromInt ( &stime, s_elapsed_time );
  snprintf ( s_the_time   , sizeof ( s_the_time    ), "%02d:%02d", stime.Minutes, stime.Seconds );
  snprintf ( s_the_time_ms, sizeof ( s_the_time_ms ), "%03d"     , stime.MilliSeconds );
  text_layer_set_text ( s_tlayer_time_lv1, s_the_time );
  text_layer_set_text ( s_tlayer_time_lv2, s_the_time_ms );

  switch ( stopwatch_state ) {
    case STOP:
      break;
    case REVIEW:
      break;
    case VIEW:
      app_timer_register ( STOP_WATCH_REFRESH_TIME, app_timer_stopwatch_handler, NULL );
      if ( s_elapsed_time > STOP_WATCH_VIEW_TIME ) {
        vibes_short_pulse ( );
        text_layer_set_text ( s_tlayer_phase, STOPWATCH_STATE_START );
        s_start_time = time ( NULL ) * 1000 + ( double ) time_ms ( NULL, NULL );
        stopwatch_state = START;
      }
      break;
    case START:
      app_timer_register ( STOP_WATCH_REFRESH_TIME, app_timer_stopwatch_handler, NULL );
      if ( s_elapsed_time > 1000 ) {
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
      s_start_time = time ( NULL ) * 1000 + ( double ) time_ms ( NULL, NULL );
      app_timer_register ( STOP_WATCH_REFRESH_TIME, app_timer_stopwatch_handler, NULL );
      stopwatch_state = VIEW;
      accel_tap_service_unsubscribe ( );
      break;
    case VIEW:  // Start real time
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
      append_time_entry ( s_elapsed_time );
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

  s_tlayer_times_average = text_layer_create (
    GRect (
      STOP_WATCH_LAYER_STATS_PADDING,
      bounds.size.h - STOP_WATCH_LAYER_STATS_LV1_HEIGHT - STOP_WATCH_LAYER_STATS_LV2_HEIGHT - 2 * STOP_WATCH_LAYER_STATS_PADDING,
      // bounds.size.w / 2 - 2 * STOP_WATCH_LAYER_STATS_PADDING,
      bounds.size.w - 2 * STOP_WATCH_LAYER_STATS_PADDING,
      STOP_WATCH_LAYER_STATS_LV1_HEIGHT ) );
  text_layer_set_text_alignment ( s_tlayer_times_average, GTextAlignmentCenter );
  text_layer_set_background_color ( s_tlayer_times_average, GColorBlack );
  text_layer_set_text_color ( s_tlayer_times_average, GColorClear );
  text_layer_set_font ( s_tlayer_times_average, fonts_get_system_font ( FONT_KEY_GOTHIC_14 ) );
  text_layer_set_text ( s_tlayer_times_average, "AVG (0000):" );
  layer_add_child ( window_layer, text_layer_get_layer ( s_tlayer_times_average ) );

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
  text_layer_destroy ( s_tlayer_times_average );
  text_layer_destroy ( s_tlayer_max );
  text_layer_destroy ( s_tlayer_min );

  accel_tap_service_unsubscribe ( );
}
