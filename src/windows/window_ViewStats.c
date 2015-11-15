#include <pebble.h>
#include "common.h"
#include "window_ViewStats.h"

static Window           *s_viewstats_window;
static SimpleMenuLayer  *s_mlayer_stats;
static SimpleMenuSection s_mlayer_section;
static SimpleMenuItem    s_mlayer_stats_items[STATS_NUMBER];

static void window_load ( Window * );
static void window_unload ( Window * );

void window_viewstats_init ( ) {
  s_viewstats_window = window_create ( );
  window_set_window_handlers ( s_viewstats_window, (WindowHandlers) {
    .load   = window_load,
    .unload = window_unload,
  } );
  #ifdef PBL_BW
    window_set_fullscreen ( s_viewstats_window, true );
  #endif
  window_stack_push ( s_viewstats_window, true );
}

void window_viewstats_deinit ( ) {
  window_stack_remove ( s_viewstats_window, true );
  window_destroy ( s_viewstats_window );

}
static void window_load ( Window * window ) {
  Layer *window_layer = window_get_root_layer ( window );
  GRect bounds = layer_get_bounds ( window_layer );

  window_set_background_color ( window, COLOR_FALLBACK ( COLOR_BACKGROUND, GColorClear ) );

  STime average, max, min;
  int cube_size = getCubeSize ( );
  static char text_title[25];
  static char text_solves[10];
  static char text_average[10];
  static char text_max[10];
  static char text_min[10];

  timeFromInt ( &average, getCubeAverage ( cube_size ) );
  timeFromInt ( &max, getCubeMax ( cube_size ) );
  timeFromInt ( &min, getCubeMin ( cube_size ) );

  snprintf ( text_title, sizeof ( text_title ), "Stats for %dx%dx%d", cube_size, cube_size, cube_size );
  snprintf ( text_solves, sizeof ( text_solves ), "%d", getCubeSolves ( cube_size ) );
  snprintf ( text_average, sizeof ( text_average), "%02d:%02d.%03d", average.Minutes, average.Seconds, average.MilliSeconds );
  snprintf ( text_max, sizeof ( text_max ), "%02d:%02d.%03d", max.Minutes, max.Seconds, max.MilliSeconds );
  snprintf ( text_min, sizeof ( text_min ), "%02d:%02d.%03d", min.Minutes, min.Seconds, min.MilliSeconds );

  s_mlayer_stats_items[0] = ( SimpleMenuItem ) {
    .title    = "Total solves",
    .subtitle = text_solves,
  };

  s_mlayer_stats_items[1] = ( SimpleMenuItem ) {
    .title    = "Average time",
    .subtitle = text_average,
  };

  s_mlayer_stats_items[2] = ( SimpleMenuItem ) {
    .title    = "Best time",
    .subtitle = text_min,
  };

  s_mlayer_stats_items[3] = ( SimpleMenuItem ) {
    .title    = "Worst time",
    .subtitle = text_max,
  };


  s_mlayer_section = ( SimpleMenuSection ) {
    .title     = text_title,
    .num_items = STATS_NUMBER,
    .items     = s_mlayer_stats_items,
  };


  s_mlayer_stats = simple_menu_layer_create ( bounds, window, &s_mlayer_section, 1, NULL );
  #ifdef PBL_COLOR
    MenuLayer *mlayer = simple_menu_layer_get_menu_layer ( s_mlayer_stats );
    menu_layer_set_normal_colors ( mlayer, COLOR_BACKGROUND, GColorBlack );
    menu_layer_set_highlight_colors ( mlayer, GColorOxfordBlue, GColorWhite );
  #endif
  layer_add_child ( window_layer, simple_menu_layer_get_layer ( s_mlayer_stats ) );
}

static void window_unload ( Window * window ) {
  simple_menu_layer_destroy ( s_mlayer_stats );
}
