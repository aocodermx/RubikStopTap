/*
 *  DeleteModeStats: Window to delete current mode stats.
 */
#include <pebble.h>
#include "common.h"
#include "window_DeleteStats.h"

static Window         *s_deletemodestats_window;
static TextLayer      *s_tlayer_message;
static ActionBarLayer *s_action_bar;
static GBitmap        *s_icon_action_ok;
static GBitmap        *s_icon_action_cancel;

static void window_load ( Window * );
static void window_unload ( Window * );
static void window_click_config_provider ( void * );
static void window_up_click_handler ( ClickRecognizerRef, void * );
static void window_down_click_handler ( ClickRecognizerRef, void * );

static int delete_instruction = DELETE_MODE;

void window_deletemodestats_init ( int to_delete ) {
  delete_instruction = to_delete;
  s_deletemodestats_window = window_create ( );
  window_set_window_handlers ( s_deletemodestats_window, ( WindowHandlers ) {
    .load   = window_load,
    .unload = window_unload,
  } );
  #ifdef PBL_BW
    window_set_fullscreen ( s_deletemodestats_window, true );
  #endif
  window_stack_push ( s_deletemodestats_window, true );
}

void window_deletemodestats_deinit ( ) {
  window_stack_remove ( s_deletemodestats_window, true );
  window_destroy ( s_deletemodestats_window );
}

static void window_load ( Window * window ) {
  static char message[50];
  Layer *window_layer = window_get_root_layer ( window );
  GRect bounds = layer_get_bounds ( window_layer );

  int cube_size = getCubeSize ( );

  s_icon_action_ok = gbitmap_create_with_resource ( RESOURCE_ID_ICON_ACTION_OK );
  s_icon_action_cancel = gbitmap_create_with_resource ( RESOURCE_ID_ICON_ACTION_CANCEL );

  s_action_bar = action_bar_layer_create ( );
  action_bar_layer_add_to_window ( s_action_bar, window );
  action_bar_layer_set_icon ( s_action_bar, BUTTON_ID_UP, s_icon_action_ok );
  action_bar_layer_set_icon ( s_action_bar, BUTTON_ID_DOWN, s_icon_action_cancel );
  action_bar_layer_set_click_config_provider ( s_action_bar, ( ClickConfigProvider ) window_click_config_provider );

  s_tlayer_message = text_layer_create ( GRect( 0, DELETE_MODE_STATS_MESSAGE_START, bounds.size.w - ACTION_BAR_WIDTH, bounds.size.h - DELETE_MODE_STATS_MESSAGE_START ) );
  text_layer_set_text_alignment ( s_tlayer_message, GTextAlignmentCenter );
  text_layer_set_text_color ( s_tlayer_message, GColorBlack );
  text_layer_set_font ( s_tlayer_message, fonts_get_system_font ( FONT_KEY_GOTHIC_28 ) );
  layer_add_child ( window_layer, text_layer_get_layer ( s_tlayer_message ) );

  switch ( delete_instruction ) {
    case DELETE_MODE:
      snprintf ( message, sizeof ( message ), "Are you sure to delete the %dx%dx%d stats?", cube_size, cube_size, cube_size );
      text_layer_set_text ( s_tlayer_message, message );
      break;
    case DELETE_ALL:
      text_layer_set_text ( s_tlayer_message, "Are you sure to delete the stats for all the cubes?" );
      break;
    default:
      APP_LOG ( APP_LOG_LEVEL_INFO, "Unespecified delete instruction." );
      break;
  }
}

static void window_unload ( Window * window ) {
  action_bar_layer_destroy ( s_action_bar );
  text_layer_destroy ( s_tlayer_message );
  gbitmap_destroy ( s_icon_action_ok );
  gbitmap_destroy ( s_icon_action_cancel );
}

static void window_up_click_handler ( ClickRecognizerRef recognizer, void *context ) {
  switch ( delete_instruction ) {
    case DELETE_MODE:
      delete_cube_stats ( getCubeSize ( ) );
      break;
    case DELETE_ALL:
      delete_all_stats ( );
      break;
    default:
      APP_LOG ( APP_LOG_LEVEL_INFO, "Unespecified delete instruction." );
      break;
  }
  window_deletemodestats_deinit ( );
}

static void window_down_click_handler ( ClickRecognizerRef recognizer, void *context ) {
  window_deletemodestats_deinit ( );
}

static void window_click_config_provider ( void * context ) {
  window_single_click_subscribe ( BUTTON_ID_UP, window_up_click_handler );
  window_single_click_subscribe ( BUTTON_ID_DOWN, window_down_click_handler );
}
