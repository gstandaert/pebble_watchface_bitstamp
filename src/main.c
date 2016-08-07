#include <pebble.h>
#include "utils.h"
#define KEY_BITSTAMP_LASTPRICE 0
#define KEY_BITSTAMP_LOWPRICE 1
#define KEY_BITSTAMP_HIGHPRICE 2

static bool s_enable_battery_monitor = true;
static bool s_enable_bluetooth_monitor = true;
        
static Window *s_main_window;
static TextLayer *s_time_layer;
static GFont s_time_font;
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;
static TextLayer *s_bitstamp_layer;
static TextLayer *s_bitstamp_pricerange_layer;

static void main_window_load(Window *window) {
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // Create GBitmap
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BITSTAMP);

  // Create BitmapLayer to display the GBitmap
  s_background_layer = bitmap_layer_create(GRect(5, 100, 50, 50));

  // Set the bitmap onto the layer and add to the window
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_background_layer));
    
  // Create the TextLayer with specific bounds
  s_time_layer = text_layer_create(
      GRect(0, 25, bounds.size.w, 60));

  // Create GFont
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_GLORIA_HALLELUJAH_50));
    
  // Improve the layout to be more like a watchface    
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
    
  // Create bitstamp Layer
  s_bitstamp_layer = text_layer_create(
    GRect(25, 100, bounds.size.w, 25));

  // Style the text
  text_layer_set_background_color(s_bitstamp_layer, GColorClear);
  text_layer_set_text_color(s_bitstamp_layer, GColorBlack);
  text_layer_set_font(s_bitstamp_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
  text_layer_set_text_alignment(s_bitstamp_layer, GTextAlignmentCenter);
  text_layer_set_text(s_bitstamp_layer, "Loading...");  

  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_bitstamp_layer));
    
  // Create bitstamp price range Layer
  s_bitstamp_pricerange_layer = text_layer_create(
    GRect(25, 125, bounds.size.w, 25));

  // Style the text
  text_layer_set_background_color(s_bitstamp_pricerange_layer, GColorClear);
  text_layer_set_text_color(s_bitstamp_pricerange_layer, GColorBlack);
  text_layer_set_font(s_bitstamp_pricerange_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(s_bitstamp_pricerange_layer, GTextAlignmentCenter);

  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_bitstamp_pricerange_layer));
}

static void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
    
  // Unload GFont
  fonts_unload_custom_font(s_time_font);
    
  // Destroy GBitmap
  gbitmap_destroy(s_background_bitmap);

  // Destroy BitmapLayer
  bitmap_layer_destroy(s_background_layer);
    
  // Destroy bitstamp elements
  text_layer_destroy(s_bitstamp_layer);
}

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Write the current hours and minutes into a buffer
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ?
                                          "%H:%M" : "%I:%M", tick_time);

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, s_buffer);
}

static void update_bitstamp() {
  // Begin dictionary
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  // Add a key-value pair
  dict_write_uint8(iter, 0, 0);

  // Send the message!
  app_message_outbox_send();
}

static void vibrate_watch() {
  //vibes_short_pulse();
   /* or */
  //vibes_long_pulse();
  /* or */
  //vibes_double_pulse();
    
  //Create an array of ON-OFF-ON etc durations in milliseconds
  uint32_t segments[] = {300, 200, 300, 200, 300, 200, 450, 200, 300, 200, 600};
 
  //Create a VibePattern structure with the segments and length of the pattern as fields
  VibePattern pattern = {
    .durations = segments,
    .num_segments = ARRAY_LENGTH(segments),
  };
 
  //Trigger the custom pattern to be executed
  vibes_enqueue_custom_pattern(pattern);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
  update_bitstamp();
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Store incoming information from PebbleKitJS
  static char bitstamp_lastprice_buffer[8];
  static char bitstamp_pricerange_buffer[16];
  static char bitstamp_layer_buffer[8];
  static char bitstamp_pricerange_layer_buffer[16];

  // Read tuples for data
  Tuple *lastprice_tuple = dict_find(iterator, KEY_BITSTAMP_LASTPRICE);
  Tuple *lowprice_tuple = dict_find(iterator, KEY_BITSTAMP_LOWPRICE);
  Tuple *highprice_tuple = dict_find(iterator, KEY_BITSTAMP_HIGHPRICE);

  // If all data is available, use it
  if(lastprice_tuple) {
    snprintf(bitstamp_lastprice_buffer, sizeof(bitstamp_lastprice_buffer), "%s", lastprice_tuple->value->cstring);
  }
  if(lowprice_tuple && highprice_tuple) {
    snprintf(bitstamp_pricerange_buffer, sizeof(bitstamp_pricerange_buffer), "%s - %s", lowprice_tuple->value->cstring, highprice_tuple->value->cstring);
  }
    
  // Assemble full string and display
  snprintf(bitstamp_layer_buffer, sizeof(bitstamp_layer_buffer), "%s", bitstamp_lastprice_buffer);
  text_layer_set_text(s_bitstamp_layer, bitstamp_layer_buffer);

  // Assemble full string and display
  snprintf(bitstamp_pricerange_layer_buffer, sizeof(bitstamp_pricerange_layer_buffer), "%s", bitstamp_pricerange_buffer);
  text_layer_set_text(s_bitstamp_pricerange_layer, bitstamp_pricerange_layer_buffer);
    
  //vibrate_watch();
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed! Reason: %d", (int)reason);
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
    
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

  // Make sure the time is displayed from the start
  update_time();
    
  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);

  // Open AppMessage
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

  utils_init(s_main_window, s_enable_battery_monitor, s_enable_bluetooth_monitor);
}

static void deinit() {
  // Deinit utils
  utils_deinit();
    
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}