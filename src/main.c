#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer;

static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;

static int bg_x;
static int bg_y;
static int bg_w;
static int bg_h;

static int text_x;
static int text_y;
static int text_w;
static int text_h;

// #define SCREENSHOT 1

static void setSize(GRect window_bounds) {
//  int margin_x = window_bounds.w
  bg_w = window_bounds.size.w;
  bg_h = window_bounds.size.h;
  bg_x = window_bounds.size.w / 2 - bg_w / 2;
  bg_y = window_bounds.size.h / 2 - bg_h / 2;
//  text_w = 154;
  text_w = window_bounds.size.w;
  text_h = 50;
//  text_x = ( window_bounds.size.w / 2 - text_w / 2 );
  text_x = 0;
  text_y = window_bounds.size.h / 2 - 28;

  printf("window w: %d", window_bounds.size.w);
  printf("window h: %d", window_bounds.size.h);
  printf("text: %d, %d, %d, %d", text_x, text_y, text_w, text_h);

/*
  printf("WATCH_INFO_MODEL_UNKNOWN: %d", WATCH_INFO_MODEL_UNKNOWN);
  printf("WATCH_INFO_MODEL_PEBBLE_ORIGINAL: %d", WATCH_INFO_MODEL_PEBBLE_ORIGINAL);
  printf("WATCH_INFO_MODEL_PEBBLE_STEEL: %d", WATCH_INFO_MODEL_PEBBLE_STEEL);
  printf("WATCH_INFO_MODEL_PEBBLE_TIME: %d", WATCH_INFO_MODEL_PEBBLE_TIME);
  printf("WATCH_INFO_MODEL_PEBBLE_TIME_STEEL: %d", WATCH_INFO_MODEL_PEBBLE_TIME_STEEL);
  printf("WATCH_INFO_MODEL_PEBBLE_TIME_ROUND_14: %d", WATCH_INFO_MODEL_PEBBLE_TIME_ROUND_14);
  printf("WATCH_INFO_MODEL_PEBBLE_TIME_ROUND_20: %d", WATCH_INFO_MODEL_PEBBLE_TIME_ROUND_20);

  printf("watch_info_get_model(): %d", watch_info_get_model());
*/
/*
  switch(watch_info_get_model()) {
    case WATCH_INFO_MODEL_PEBBLE_TIME_ROUND_14:
      text_y = 54;
      break;
    case WATCH_INFO_MODEL_PEBBLE_TIME_ROUND_20:
      text_y = 54;
      break;
    case WATCH_INFO_MODEL_UNKNOWN:
      text_y=56;
      break;
    default:
      text_y = 56;
  }
*/
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer);
  setSize(window_bounds);

  // Create GBitmap, then set to created BitmapLayer
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);
  s_background_layer = bitmap_layer_create(GRect(bg_x, bg_y, bg_w, bg_h));
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));

  // Create time TextLayer
  s_time_layer = text_layer_create(GRect(text_x, text_y, text_w, text_h));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_text(s_time_layer, "00:00");

  // Improve the layout to be more like a watchface
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
}

static void main_window_unload(Window *window) {
  // Destroy GBitmap
  gbitmap_destroy(s_background_bitmap);

  // Destroy BitmapLayer
  bitmap_layer_destroy(s_background_layer);

  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
}

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "00:00";

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    // Use 24 hour format
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    // Use 12 hour format
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }

#ifdef SCREENSHOT
  text_layer_set_text(s_time_layer, "19:40");
#else
  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, buffer);
#endif

}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void init() {
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);

  // Make sure the time is displayed from the start
  update_time();

}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
