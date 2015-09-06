#include <pebble.h>

/*
 * App Common components
 */

/*
 * StopWatchWindow: Main window to display current time.
 */

#define STOP_WATCH_VIEW_TIME 15
#define STOP_WATCH_REFRESH_TIME 125
#define STOP_WATCH_LAYER_PHASE_HEIGHT 25
#define STOP_WATCH_LAYER_LV1_HEIGHT 50
#define STOP_WATCH_LAYER_LV2_HEIGHT 50

typedef enum { STOP, VIEW, START, REVIEW  } STOPWATCH_STATE;
STOPWATCH_STATE stopwatch_state = STOP;

static Window    *s_stopwatch_window;
static TextLayer *s_tlayer_time_lv1;
static TextLayer *s_tlayer_time_lv2;
static TextLayer *s_tlayer_phase;

static char       the_time[6];
static char       the_time_ms[4];
static double     s_start_time;

static void app_timer_stopwatch_handler ( void * data ) {

  double end_time = time ( NULL ) + ( double ) time_ms ( NULL, NULL ) / 1000;
  double elapsed_time = end_time - s_start_time;

  int elapsed_time_min = ( int ) elapsed_time / 60;
  int elapsed_time_seg = ( int ) elapsed_time  - elapsed_time_min * 60 ;
  int elapsed_time_ms  = ( int ) ( elapsed_time * 1000 ) % 1000;

  //if ( elapsed_time > 60 )
    snprintf ( the_time, sizeof ( the_time ), "%02d:%02d", elapsed_time_min, elapsed_time_seg );
  //else
  //  snprintf ( the_time, sizeof ( the_time ), "%02d", elapsed_time_seg );
  snprintf ( the_time_ms, sizeof ( the_time_ms ), "%03d", elapsed_time_ms );
  text_layer_set_text ( s_tlayer_time_lv1, the_time );
  text_layer_set_text ( s_tlayer_time_lv2, the_time_ms );

  // APP_LOG ( APP_LOG_LEVEL_INFO, "STATE: %d", stopwatch_state );
  switch ( stopwatch_state ) {
    case STOP:
    case REVIEW:
      break;
    case VIEW:
      app_timer_register ( STOP_WATCH_REFRESH_TIME, app_timer_stopwatch_handler, NULL );
      if ( elapsed_time > STOP_WATCH_VIEW_TIME ) {
        vibes_short_pulse ( );
        text_layer_set_text ( s_tlayer_phase, "Solve it" );
        s_start_time = time ( NULL ) + ( double ) time_ms ( NULL, NULL ) / 1000;
        stopwatch_state = START;
      }
      break;
    case START:
      app_timer_register ( STOP_WATCH_REFRESH_TIME, app_timer_stopwatch_handler, NULL );
      break;
  }
}

static void accel_tap_stopwatch_handler ( AccelAxisType axis, int32_t direction ) {

  switch ( stopwatch_state ) {
    case STOP: // Start review time
      text_layer_set_text ( s_tlayer_phase, "Make your strategy" );
      vibes_short_pulse ( );
      s_start_time = time ( NULL ) + ( double ) time_ms ( NULL, NULL ) / 1000;
      app_timer_register ( STOP_WATCH_REFRESH_TIME, app_timer_stopwatch_handler, NULL );
      stopwatch_state = VIEW;
      break;
    case VIEW:  // Start real time
      text_layer_set_text ( s_tlayer_phase, "Solve it" );
      vibes_short_pulse ( );
      s_start_time = time ( NULL ) + ( double ) time_ms ( NULL, NULL ) / 1000;
      stopwatch_state = START;
      break;
    case START:
      // Check for any type of reccord
      text_layer_set_text ( s_tlayer_phase, "Your time" );
      vibes_short_pulse ( );
      stopwatch_state = REVIEW;
      break;
    case REVIEW:
      text_layer_set_text ( s_tlayer_phase, "Scramble it again" );
      text_layer_set_text ( s_tlayer_time_lv1, "00:00" );
      text_layer_set_text ( s_tlayer_time_lv2, "000" );
      vibes_short_pulse ( );
      stopwatch_state = STOP;
      break;
  }
}

static void window_load ( Window *window ) {
  Layer *window_layer = window_get_root_layer ( window );
  GRect bounds = layer_get_bounds ( window_layer );

  accel_tap_service_subscribe ( accel_tap_stopwatch_handler );

  s_tlayer_phase = text_layer_create ( GRect ( 5, 0, bounds.size.w, STOP_WATCH_LAYER_PHASE_HEIGHT ) );
  text_layer_set_text ( s_tlayer_phase, "Scramble" );
  text_layer_set_text_alignment ( s_tlayer_phase, GTextAlignmentLeft );
  text_layer_set_font ( s_tlayer_phase, fonts_get_system_font ( FONT_KEY_GOTHIC_24 ) );
  layer_add_child ( window_layer, text_layer_get_layer ( s_tlayer_phase ) );

  s_tlayer_time_lv1 = text_layer_create ( GRect ( 0, STOP_WATCH_LAYER_PHASE_HEIGHT, bounds.size.w - 10, STOP_WATCH_LAYER_LV1_HEIGHT ) );
  text_layer_set_text ( s_tlayer_time_lv1, "00:00" );
  text_layer_set_text_alignment ( s_tlayer_time_lv1, GTextAlignmentRight );
  text_layer_set_font ( s_tlayer_time_lv1, fonts_get_system_font ( FONT_KEY_ROBOTO_BOLD_SUBSET_49 ) );
  layer_add_child ( window_layer, text_layer_get_layer ( s_tlayer_time_lv1 ) );

  s_tlayer_time_lv2 = text_layer_create ( GRect ( 0, STOP_WATCH_LAYER_PHASE_HEIGHT + STOP_WATCH_LAYER_LV1_HEIGHT, bounds.size.w - 10, STOP_WATCH_LAYER_LV2_HEIGHT ) );
  text_layer_set_text ( s_tlayer_time_lv2, "000" );
  text_layer_set_text_alignment ( s_tlayer_time_lv2, GTextAlignmentRight );
  text_layer_set_font ( s_tlayer_time_lv2, fonts_get_system_font ( FONT_KEY_ROBOTO_BOLD_SUBSET_49 ) );
  layer_add_child ( window_layer, text_layer_get_layer ( s_tlayer_time_lv2 ) );
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
