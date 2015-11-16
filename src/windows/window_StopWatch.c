/*
 * StopWatchWindow: Main window to display current time.
 */
#include <pebble.h>
#include "common.h"
#include "window_StopWatch.h"
#include "window_SelectCube.h"
#include "window_ViewStats.h"
#include "window_DeleteStats.h"


static Window    *s_stopwatch_window;
static TextLayer *s_tlayer_time_lv1;
static TextLayer *s_tlayer_time_lv2;
static TextLayer *s_tlayer_phase;
static TextLayer *s_tlayer_times_average;
static TextLayer *s_tlayer_mode;
static GFont      s_custom_font_45;
static GFont      s_custom_font_35;

static ActionBarLayer *action_bar;
static GBitmap        *s_icon_action_mode;
static GBitmap        *s_icon_action_stats;
static GBitmap        *s_icon_action_cancel;
static AppTimer   *s_timer;

static STime stime           = { 0 };
static int   s_start_time    = 0;
static int   s_elapsed_time  = 0;

STOPWATCH_STATE stopwatch_state = STOP;

static void window_load ( Window * );
static void window_appear ( Window * );
static void window_disappear ( Window * window );
static void window_unload ( Window * );
static void app_timer_stopwatch_handler ( void * );
static void accel_tap_stopwatch_handler ( AccelAxisType, int32_t );
static void update_ui_stats ( );
static void config_provider ( void * );
static void up_single_click_handler ( ClickRecognizerRef, void * );
static void select_single_click_handler ( ClickRecognizerRef, void * );
static void down_single_click_handler ( ClickRecognizerRef, void * );
static void down_double_click_handler ( ClickRecognizerRef, void * );
static void down_long_click_handler ( ClickRecognizerRef, void * );


void window_stopwatch_init ( ) {
  s_stopwatch_window = window_create ( );
  // window_set_click_config_provider ( s_stopwatch_window, ( ClickConfigProvider ) config_provider );
  window_set_window_handlers ( s_stopwatch_window, ( WindowHandlers ) {
    .load      = window_load,
    .appear    = window_appear,
    .disappear = window_disappear,
    .unload    = window_unload,
  } );
  #ifdef PBL_BW
    window_set_fullscreen ( s_stopwatch_window, true );
  #endif
  window_stack_push ( s_stopwatch_window, true );
}

void window_stopwatch_deinit ( ) {
  window_destroy ( s_stopwatch_window );
}

static void down_single_click_handler ( ClickRecognizerRef recognizer, void *context ) {
  if ( stopwatch_state == STOP )
    return;

  if ( stopwatch_state == VIEW )
    accel_tap_service_subscribe ( accel_tap_stopwatch_handler );

  text_layer_set_text ( s_tlayer_phase, STOPWATCH_STATE_STOP );
  text_layer_set_text ( s_tlayer_time_lv1, "00:00" );
  text_layer_set_text ( s_tlayer_time_lv2, "000" );
  app_timer_cancel ( s_timer );
  stopwatch_state = STOP;
}

static void up_single_click_handler ( ClickRecognizerRef recognizer, void * context ) {
  down_single_click_handler ( recognizer, context );
  window_viewstats_init ( );
}

static void select_single_click_handler ( ClickRecognizerRef recognizer, void * context ) {
  down_single_click_handler ( recognizer, context );
  window_selectcube_init ( );
  // APP_LOG ( APP_LOG_LEVEL_INFO, "Select button clicked" );
}

static void down_long_click_handler ( ClickRecognizerRef recognizer, void * context ) {
  window_deletemodestats_init ( DELETE_ALL );
}

static void down_double_click_handler ( ClickRecognizerRef recognizer, void *context ) {
  window_deletemodestats_init ( DELETE_MODE );
}

static void config_provider ( void *context ) {
  window_single_click_subscribe ( BUTTON_ID_UP, up_single_click_handler );
  window_single_click_subscribe ( BUTTON_ID_SELECT, select_single_click_handler );
  window_single_click_subscribe ( BUTTON_ID_DOWN, down_single_click_handler );

  window_multi_click_subscribe ( BUTTON_ID_DOWN, 2, 0, 0, true, down_double_click_handler );
  window_long_click_subscribe ( BUTTON_ID_DOWN, 3000, down_long_click_handler, NULL );
}

static void update_ui_stats ( ) {
  static char text_cube_size[9];
  static char text_solves_average[25];

  int cube_size       = getCubeSize ( );
  int s_stats_solves  = getCubeSolves  ( cube_size );
  int s_stats_average = getCubeAverage ( cube_size );

  timeFromInt ( &stime, s_stats_average );
  snprintf ( text_solves_average, sizeof ( text_solves_average ), " AVG(%d): %02d:%02d.%03d", s_stats_solves, stime.Minutes, stime.Seconds, stime.MilliSeconds );
  snprintf ( text_cube_size, sizeof ( text_cube_size ), "%dx%dx%d", cube_size, cube_size, cube_size );

  text_layer_set_text ( s_tlayer_times_average, text_solves_average );
  text_layer_set_text ( s_tlayer_mode, text_cube_size );
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
      s_timer = app_timer_register ( STOP_WATCH_PRESICION, app_timer_stopwatch_handler, NULL );
      if ( s_elapsed_time > STOP_WATCH_VIEW_TIME ) {
        vibes_short_pulse ( );
        text_layer_set_text ( s_tlayer_phase, STOPWATCH_STATE_START );
        s_start_time = time ( NULL ) * 1000 + ( double ) time_ms ( NULL, NULL );
        stopwatch_state = START;
      }
      break;
    case START:
      s_timer = app_timer_register ( STOP_WATCH_PRESICION, app_timer_stopwatch_handler, NULL );
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
      s_timer = app_timer_register ( STOP_WATCH_PRESICION, app_timer_stopwatch_handler, NULL );
      stopwatch_state = VIEW;
      accel_tap_service_unsubscribe ( );
      break;
    case VIEW:  // Start real time
      break;
    case START:
      // Check for personal record
      if ( s_elapsed_time < getCubeMin( getCubeSize ( ) ) ) {
        text_layer_set_text ( s_tlayer_phase, STOPWATCH_CONGRATULATIONS );
      } else {
        text_layer_set_text ( s_tlayer_phase, STOPWATCH_STATE_REVIEW );
      }
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

static void window_load ( Window * window ) {
  Layer *window_layer = window_get_root_layer ( window );
  GRect bounds = layer_get_bounds ( window_layer );

  #ifdef PBL_COLOR
    window_set_background_color ( window, COLOR_BACKGROUND );
  #endif
  
  accel_tap_service_subscribe ( accel_tap_stopwatch_handler );

  s_custom_font_45 = fonts_load_custom_font ( resource_get_handle ( RESOURCE_ID_FONT_CUSTOM_45 ) );
  s_custom_font_35 = fonts_load_custom_font ( resource_get_handle ( RESOURCE_ID_FONT_CUSTOM_35 ) );

  s_icon_action_mode   = gbitmap_create_with_resource ( RESOURCE_ID_ICON_ACTION_MODE );
  s_icon_action_stats  = gbitmap_create_with_resource ( RESOURCE_ID_ICON_ACTION_STATS );
  s_icon_action_cancel = gbitmap_create_with_resource ( RESOURCE_ID_ICON_ACTION_CANCEL );

  action_bar = action_bar_layer_create ( );
  action_bar_layer_add_to_window ( action_bar, window );
  action_bar_layer_set_click_config_provider ( action_bar, ( ClickConfigProvider ) config_provider );

  action_bar_layer_set_icon ( action_bar, BUTTON_ID_UP, s_icon_action_stats );
  action_bar_layer_set_icon ( action_bar, BUTTON_ID_SELECT, s_icon_action_mode );
  action_bar_layer_set_icon ( action_bar, BUTTON_ID_DOWN, s_icon_action_cancel );

  s_tlayer_phase = text_layer_create ( GRect ( PBL_IF_ROUND_ELSE ( ACTION_BAR_WIDTH, 0 ), STOP_WATCH_DISPLAY_OFFSET, bounds.size.w - PBL_IF_ROUND_ELSE( 2 * ACTION_BAR_WIDTH, ACTION_BAR_WIDTH ), STOP_WATCH_LAYER_PHASE_HEIGHT ) );
  text_layer_set_background_color( s_tlayer_phase, COLOR_FALLBACK ( COLOR_BACKGROUND, GColorClear ) );
  text_layer_set_text_alignment ( s_tlayer_phase, GTextAlignmentCenter );
  text_layer_set_text_color ( s_tlayer_phase, GColorBlack );
  text_layer_set_font ( s_tlayer_phase, fonts_get_system_font ( FONT_KEY_GOTHIC_18_BOLD ) );
  text_layer_set_text ( s_tlayer_phase, STOPWATCH_STATE_STOP );
  layer_add_child ( window_layer, text_layer_get_layer ( s_tlayer_phase ) );

  s_tlayer_time_lv1 = text_layer_create ( GRect ( PBL_IF_ROUND_ELSE ( ACTION_BAR_WIDTH, 0 ), STOP_WATCH_DISPLAY_OFFSET + STOP_WATCH_LAYER_PHASE_HEIGHT, bounds.size.w - PBL_IF_ROUND_ELSE ( 2 * ACTION_BAR_WIDTH, ACTION_BAR_WIDTH ), STOP_WATCH_LAYER_LV1_HEIGHT ) );
  text_layer_set_background_color( s_tlayer_time_lv1, COLOR_FALLBACK ( COLOR_BACKGROUND, GColorClear ) );
  text_layer_set_text_color ( s_tlayer_time_lv1, COLOR_FALLBACK( GColorOxfordBlue, GColorBlack ) );
  text_layer_set_text_alignment ( s_tlayer_time_lv1, GTextAlignmentCenter );
  text_layer_set_text ( s_tlayer_time_lv1, "00:00" );
  text_layer_set_font ( s_tlayer_time_lv1, PBL_IF_ROUND_ELSE ( s_custom_font_35, s_custom_font_45 ) );
  layer_add_child ( window_layer, text_layer_get_layer ( s_tlayer_time_lv1 ) );

  s_tlayer_time_lv2 = text_layer_create ( GRect ( PBL_IF_ROUND_ELSE ( ACTION_BAR_WIDTH, 0 ), STOP_WATCH_DISPLAY_OFFSET + STOP_WATCH_LAYER_LV1_HEIGHT + STOP_WATCH_LAYER_PHASE_HEIGHT, bounds.size.w - PBL_IF_ROUND_ELSE ( 2 * ACTION_BAR_WIDTH, ACTION_BAR_WIDTH ), STOP_WATCH_LAYER_LV2_HEIGHT ) );
  text_layer_set_background_color( s_tlayer_time_lv2, COLOR_FALLBACK( COLOR_BACKGROUND, GColorClear ) );
  text_layer_set_text_color ( s_tlayer_time_lv2, COLOR_FALLBACK( GColorOxfordBlue, GColorBlack ) );
  text_layer_set_text ( s_tlayer_time_lv2, "000" );
  text_layer_set_text_alignment ( s_tlayer_time_lv2, GTextAlignmentCenter );
  text_layer_set_font ( s_tlayer_time_lv2, s_custom_font_35 );
  layer_add_child ( window_layer, text_layer_get_layer ( s_tlayer_time_lv2 ) );

  s_tlayer_mode = text_layer_create (
    GRect (
      PBL_IF_ROUND_ELSE( ACTION_BAR_WIDTH, 0 ),
      STOP_WATCH_DISPLAY_OFFSET + STOP_WATCH_LAYER_PHASE_HEIGHT + STOP_WATCH_LAYER_LV1_HEIGHT + STOP_WATCH_LAYER_LV2_HEIGHT,
      bounds.size.w - PBL_IF_ROUND_ELSE( 2 * ACTION_BAR_WIDTH, ACTION_BAR_WIDTH ),
      STOP_WATCH_LAYER_STATS_LV1_HEIGHT ) );
  text_layer_set_background_color( s_tlayer_mode, COLOR_FALLBACK( COLOR_BACKGROUND, GColorClear ) );
  text_layer_set_text_alignment ( s_tlayer_mode, GTextAlignmentCenter );
  text_layer_set_text_color ( s_tlayer_mode, GColorBlack );
  text_layer_set_font ( s_tlayer_mode, fonts_get_system_font ( FONT_KEY_GOTHIC_18_BOLD ) );
  layer_add_child ( window_layer, text_layer_get_layer ( s_tlayer_mode ) );

  s_tlayer_times_average = text_layer_create (
    GRect (
      PBL_IF_ROUND_ELSE( ACTION_BAR_WIDTH, 0 ),
      bounds.size.h - STOP_WATCH_LAYER_STATS_LV2_HEIGHT,
      bounds.size.w - PBL_IF_ROUND_ELSE( 2 * ACTION_BAR_WIDTH, ACTION_BAR_WIDTH ),
      STOP_WATCH_LAYER_STATS_LV2_HEIGHT ) );
  text_layer_set_background_color( s_tlayer_times_average, COLOR_FALLBACK( COLOR_BACKGROUND, GColorClear ) );
  text_layer_set_text_alignment ( s_tlayer_times_average, GTextAlignmentLeft );
  text_layer_set_text_color ( s_tlayer_times_average, GColorBlack );
  text_layer_set_font ( s_tlayer_times_average, fonts_get_system_font ( FONT_KEY_GOTHIC_14_BOLD ) );
  // layer_add_child ( window_layer, text_layer_get_layer ( s_tlayer_times_average ) );
}

static void window_appear ( Window * window ) {
  accel_tap_service_subscribe ( accel_tap_stopwatch_handler );
  update_ui_stats ( );
}

static void window_disappear ( Window * window ) {
  accel_tap_service_unsubscribe ( );
}

static void window_unload ( Window * window ) {
  text_layer_destroy ( s_tlayer_time_lv1 );
  text_layer_destroy ( s_tlayer_time_lv2 );
  text_layer_destroy ( s_tlayer_phase );
  text_layer_destroy ( s_tlayer_times_average );
  text_layer_destroy ( s_tlayer_mode );
  fonts_unload_custom_font ( s_custom_font_45 );
  fonts_unload_custom_font ( s_custom_font_35 );
  gbitmap_destroy ( s_icon_action_mode );
  gbitmap_destroy ( s_icon_action_stats );
  gbitmap_destroy ( s_icon_action_cancel );

  accel_tap_service_unsubscribe ( );
}
