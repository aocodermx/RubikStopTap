/*
 * SelectCubeWindow: Window to select cube size/mode.
 */
 #include <pebble.h>
 #include "window_SelectCube.h"

static Window             *s_selectcube_window;
static SimpleMenuLayer    *s_mlayer_sizes;
static SimpleMenuSection   s_msections[1];
static SimpleMenuItem      s_mitem[MAX_CUBE_SIZE - 1];

static char sizes_text [MAX_CUBE_SIZE - 1][9];

static void window_load ( Window * );
static void window_unload ( Window * );
static void menu_handler ( int, void * );


void window_selectcube_init ( ) {
  s_selectcube_window = window_create ( );
  window_set_window_handlers ( s_selectcube_window, ( WindowHandlers ) {
    .load   = window_load,
    .unload = window_unload,
  } );
  #ifdef PBL_BW
    window_set_fullscreen ( s_selectcube_window, true );
  #endif
  window_stack_push ( s_selectcube_window, true );
}

void window_selectcube_deinit ( ) {
  window_stack_remove ( s_selectcube_window, true );
  window_destroy ( s_selectcube_window );
}

static void window_load ( Window * window ) {
  Layer *window_layer = window_get_root_layer ( window );
  GRect bounds = layer_get_bounds ( window_layer );

  window_set_background_color ( window, COLOR_FALLBACK ( COLOR_BACKGROUND, GColorClear ) );

  for (int i = 0; i < MAX_CUBE_SIZE - 1; i++) {
    snprintf ( sizes_text[i], sizeof ( sizes_text[i]), "%dx%dx%d", i + 2, i + 2, i + 2 );
    s_mitem[i] = ( SimpleMenuItem ) {
      .title = sizes_text[i],
      .callback = menu_handler,
    };
  }

  s_msections[0] = ( SimpleMenuSection ) {
    .title     = "Select your cube",
    .num_items = MAX_CUBE_SIZE - 1,
    .items     = s_mitem,
  };

  s_mlayer_sizes = simple_menu_layer_create ( bounds, window, s_msections, 1, NULL );
  #ifdef PBL_COLOR
    MenuLayer *mlayer = simple_menu_layer_get_menu_layer ( s_mlayer_sizes );
    menu_layer_set_normal_colors ( mlayer, COLOR_BACKGROUND, GColorBlack );
    menu_layer_set_highlight_colors ( mlayer, GColorOxfordBlue, GColorWhite );
  #endif
  layer_add_child ( window_layer, simple_menu_layer_get_layer ( s_mlayer_sizes ) );
}

static void window_unload ( Window * window ) {
  simple_menu_layer_destroy ( s_mlayer_sizes );
}

static void menu_handler ( int index, void * context ) {
  setCubeSize ( index + 2 );
  window_selectcube_deinit ( );
}
