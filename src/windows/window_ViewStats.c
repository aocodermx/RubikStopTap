#include <pebble.h>
#include "common.h"
#include "window_ViewStats.h"

static Window           *s_viewstats_window;
static SimpleMenuLayer  *s_mlayer_stats;
static SimpleMenuSection s_mlayer_section[SECTIONS_NUMBER];
static SimpleMenuItem    s_mlayer_stats_items_1[STATS_NUMBER_MAIN];
static SimpleMenuItem    s_mlayer_stats_items_2[STATS_NUMBER_WORLD];
static SimpleMenuItem    s_mlayer_stats_items_3[STATS_NUMBER_CONTINENT];
static SimpleMenuItem    s_mlayer_stats_items_4[STATS_NUMBER_COUNTRY];

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

  #ifdef PBL_COLOR
    window_set_background_color ( window, COLOR_BACKGROUND );
  #endif

  STime average, max, min;
  int cube_size = getCubeSize ( );
  static char text_title[25];
  static char text_solves[10];
  static char text_average[10];
  static char text_max[10];
  static char text_min[10];
  static char text_world_max[20];
  static char text_world_min[20];
  static char text_continent_max[20];
  static char text_continent_min[20];
  static char text_country_max[20];
  static char text_country_min[20];

  timeFromInt ( &average, getCubeAverage ( cube_size ) );
  timeFromInt ( &max, getCubeMax ( cube_size ) );
  timeFromInt ( &min, getCubeMin ( cube_size ) );

  snprintf ( text_title, sizeof ( text_title ), "Stats for %dx%dx%d", cube_size, cube_size, cube_size );
  snprintf ( text_solves, sizeof ( text_solves ), "%d", getCubeSolves ( cube_size ) );
  snprintf ( text_average, sizeof ( text_average), "%02d:%02d.%03d", average.Minutes, average.Seconds, average.MilliSeconds );
  snprintf ( text_max, sizeof ( text_max ), "%02d:%02d.%03d", max.Minutes, max.Seconds, max.MilliSeconds );
  snprintf ( text_min, sizeof ( text_min ), "%02d:%02d.%03d", min.Minutes, min.Seconds, min.MilliSeconds );
  snprintf ( text_world_min, sizeof ( text_world_min ), "Position #%d", getCubeWorldMin ( cube_size ) );
  snprintf ( text_world_max, sizeof ( text_world_max ), "Position #%d", getCubeWorldMax ( cube_size ) );
  snprintf ( text_continent_min, sizeof ( text_continent_min ), "Position #%d", getCubeContinentMin ( cube_size ) );
  snprintf ( text_continent_max, sizeof ( text_continent_max ), "Positoin #%d", getCubeContinentMax ( cube_size ) );
  snprintf ( text_country_min, sizeof ( text_country_min ), "Position #%d", getCubeCountryMin ( cube_size ) );
  snprintf ( text_country_max, sizeof ( text_country_max), "Position #%d", getCubeCountryMax ( cube_size ) );

  s_mlayer_stats_items_1[0] = ( SimpleMenuItem ) {
    .title    = "Total solves",
    .subtitle = text_solves,
  };

  s_mlayer_stats_items_1[1] = ( SimpleMenuItem ) {
    .title    = "Average time",
    .subtitle = text_average,
  };

  s_mlayer_stats_items_1[2] = ( SimpleMenuItem ) {
    .title    = "Best time",
    .subtitle = text_min,
  };

  s_mlayer_stats_items_1[3] = ( SimpleMenuItem ) {
    .title    = "Worst time",
    .subtitle = text_max,
  };


  s_mlayer_stats_items_2[0] = ( SimpleMenuItem ) {
    .title    = "Best Time",
    .subtitle = text_world_min,
  };

  s_mlayer_stats_items_2[1] = ( SimpleMenuItem ) {
    .title    = "Average Time",
    .subtitle = text_world_max,
  };

  s_mlayer_stats_items_3[0] = ( SimpleMenuItem ) {
    .title    = "Best Time",
    .subtitle = text_continent_min,
  };

  s_mlayer_stats_items_3[1] = ( SimpleMenuItem ) {
    .title    = "Average Time",
    .subtitle = text_continent_max,
  };

  s_mlayer_stats_items_4[0] = ( SimpleMenuItem ) {
    .title    = "Best Time",
    .subtitle = text_country_min,
  };

  s_mlayer_stats_items_4[1] = ( SimpleMenuItem ) {
    .title    = "Best Time",
    .subtitle = text_country_max,
  };


  #ifdef PBL_RECT
  s_mlayer_section[0] = ( SimpleMenuSection ) {
    .title     = text_title,
    .num_items = STATS_NUMBER_MAIN,
    .items     = s_mlayer_stats_items_1,
  };

  s_mlayer_section[1] = ( SimpleMenuSection ) {
    .title     = "Virtual World Rank",
    .num_items = STATS_NUMBER_WORLD,
    .items     = s_mlayer_stats_items_2,
  };

  s_mlayer_section[2] = ( SimpleMenuSection ) {
    .title     = "Virtual Continent Rank",
    .num_items = STATS_NUMBER_CONTINENT,
    .items     = s_mlayer_stats_items_3,
  };

  s_mlayer_section[3] = ( SimpleMenuSection ) {
    .title     = "Virtual Country Rank",
    .num_items = STATS_NUMBER_COUNTRY,
    .items     = s_mlayer_stats_items_4,
  };
  #else
  s_mlayer_section[0] = ( SimpleMenuSection ) {
    .num_items = STATS_NUMBER_MAIN,
    .items     = s_mlayer_stats_items_1,
  };

  s_mlayer_section[1] = ( SimpleMenuSection ) {
    .num_items = STATS_NUMBER_WORLD,
    .items     = s_mlayer_stats_items_2,
  };

  s_mlayer_section[2] = ( SimpleMenuSection ) {
    .num_items = STATS_NUMBER_CONTINENT,
    .items     = s_mlayer_stats_items_3,
  };

  s_mlayer_section[3] = ( SimpleMenuSection ) {
    .num_items = STATS_NUMBER_COUNTRY,
    .items     = s_mlayer_stats_items_4,
  };
  #endif


  s_mlayer_stats = simple_menu_layer_create ( bounds, window, s_mlayer_section, SECTIONS_NUMBER, NULL );
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
