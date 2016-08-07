#include <pebble.h>
#include "utils.h"

static Layer *s_battery_layer;
static Layer *s_bluetooth_layer;
static GBitmap *s_battery_charging_white_bitmap, *s_battery_charging_black_bitmap;
static GBitmap *s_battery_empty_white_bitmap, *s_battery_empty_black_bitmap;
static GBitmap *s_battery_25_white_bitmap, *s_battery_25_black_bitmap;
static GBitmap *s_battery_50_white_bitmap, *s_battery_50_black_bitmap;
static GBitmap *s_battery_75_white_bitmap, *s_battery_75_black_bitmap;
static GBitmap *s_battery_full_white_bitmap, *s_battery_full_black_bitmap;
static GBitmap *s_bluetooth_enabled_white_bitmap, *s_bluetooth_enabled_black_bitmap;
static GBitmap *s_bluetooth_disabled_white_bitmap, *s_bluetooth_disabled_black_bitmap;
static bool is_bluetooth_enabled = false;
static int s_battery_level = 100;
static bool is_battery_charging = false;

void battery_state_change_handler(BatteryChargeState charge_state) {
  // Record the new battery level
  s_battery_level = charge_state.charge_percent;
  is_battery_charging = charge_state.is_charging;
      
  // Update battery icon
  layer_mark_dirty(s_battery_layer);
}

void battery_state_init() {
  battery_state_change_handler(battery_state_service_peek());
}

static void draw_battery_layer(Layer *layer, GContext *ctx) {
  if (is_battery_charging) {
    // Draw charging white
    graphics_context_set_compositing_mode(ctx, GCompOpOr);
    graphics_draw_bitmap_in_rect(ctx, s_battery_charging_white_bitmap, s_battery_charging_white_bitmap->bounds);
    // Draw charging black
    graphics_context_set_compositing_mode(ctx, GCompOpClear);
    graphics_draw_bitmap_in_rect(ctx, s_battery_charging_black_bitmap, s_battery_charging_black_bitmap->bounds);
  } else if (s_battery_level <= 10) {
    // Draw empty white
    graphics_context_set_compositing_mode(ctx, GCompOpOr);
    graphics_draw_bitmap_in_rect(ctx, s_battery_empty_white_bitmap, s_battery_empty_white_bitmap->bounds);
    // Draw empty black
    graphics_context_set_compositing_mode(ctx, GCompOpClear);
    graphics_draw_bitmap_in_rect(ctx, s_battery_empty_black_bitmap, s_battery_empty_black_bitmap->bounds);
  } else if (s_battery_level <= 25) {
    // Draw 25% white
    graphics_context_set_compositing_mode(ctx, GCompOpOr);
    graphics_draw_bitmap_in_rect(ctx, s_battery_25_white_bitmap, s_battery_25_white_bitmap->bounds);
    // Draw 25% black
    graphics_context_set_compositing_mode(ctx, GCompOpClear);
    graphics_draw_bitmap_in_rect(ctx, s_battery_25_black_bitmap, s_battery_25_black_bitmap->bounds);
  } else if (s_battery_level <= 50) {
    // Draw 50% white
    graphics_context_set_compositing_mode(ctx, GCompOpOr);
    graphics_draw_bitmap_in_rect(ctx, s_battery_50_white_bitmap, s_battery_50_white_bitmap->bounds);
    // Draw 50% black
    graphics_context_set_compositing_mode(ctx, GCompOpClear);
    graphics_draw_bitmap_in_rect(ctx, s_battery_50_black_bitmap, s_battery_50_black_bitmap->bounds);
  } else if (s_battery_level <= 75) {
    // Draw 75% white
    graphics_context_set_compositing_mode(ctx, GCompOpOr);
    graphics_draw_bitmap_in_rect(ctx, s_battery_75_white_bitmap, s_battery_75_white_bitmap->bounds);
    // Draw 75% black
    graphics_context_set_compositing_mode(ctx, GCompOpClear);
    graphics_draw_bitmap_in_rect(ctx, s_battery_75_black_bitmap, s_battery_75_black_bitmap->bounds);
  }  else {
    // Draw full white
    graphics_context_set_compositing_mode(ctx, GCompOpOr);
    graphics_draw_bitmap_in_rect(ctx, s_battery_full_white_bitmap, s_battery_full_white_bitmap->bounds);
    // Draw full black
    graphics_context_set_compositing_mode(ctx, GCompOpClear);
    graphics_draw_bitmap_in_rect(ctx, s_battery_full_black_bitmap, s_battery_full_black_bitmap->bounds);
  }
}

void battery_init(Window *window, bool enable_battery_monitor) {
  if (enable_battery_monitor) {
    // Load battery bitmaps
    s_battery_charging_white_bitmap = gbitmap_create_with_resource(RESOURCE_ID_ICON_BATTERY_CHARGING_WHITE);
    s_battery_charging_black_bitmap = gbitmap_create_with_resource(RESOURCE_ID_ICON_BATTERY_CHARGING_BLACK);
    s_battery_empty_white_bitmap = gbitmap_create_with_resource(RESOURCE_ID_ICON_BATTERY_EMPTY_WHITE);
    s_battery_empty_black_bitmap = gbitmap_create_with_resource(RESOURCE_ID_ICON_BATTERY_EMPTY_BLACK);
    s_battery_25_white_bitmap = gbitmap_create_with_resource(RESOURCE_ID_ICON_BATTERY_25_WHITE);
    s_battery_25_black_bitmap = gbitmap_create_with_resource(RESOURCE_ID_ICON_BATTERY_25_BLACK);
    s_battery_50_white_bitmap = gbitmap_create_with_resource(RESOURCE_ID_ICON_BATTERY_50_WHITE);
    s_battery_50_black_bitmap = gbitmap_create_with_resource(RESOURCE_ID_ICON_BATTERY_50_BLACK);
    s_battery_75_white_bitmap = gbitmap_create_with_resource(RESOURCE_ID_ICON_BATTERY_75_WHITE);
    s_battery_75_black_bitmap = gbitmap_create_with_resource(RESOURCE_ID_ICON_BATTERY_75_BLACK);
    s_battery_full_white_bitmap = gbitmap_create_with_resource(RESOURCE_ID_ICON_BATTERY_FULL_WHITE);
    s_battery_full_black_bitmap = gbitmap_create_with_resource(RESOURCE_ID_ICON_BATTERY_FULL_BLACK);
      
    // Create battery Layer & add to window
    s_battery_layer = layer_create(GRect(118, 2, 24, 24));
    layer_set_update_proc(s_battery_layer, draw_battery_layer);
    layer_add_child(window_get_root_layer(window), s_battery_layer);

    // Start monitoring battery state
    battery_state_init();
    battery_state_service_subscribe(battery_state_change_handler);
  }
}

void battery_deinit() {
  // Cleanup battery monitor resources
  gbitmap_destroy(s_battery_charging_white_bitmap);
  gbitmap_destroy(s_battery_charging_black_bitmap);
  gbitmap_destroy(s_battery_empty_white_bitmap);
  gbitmap_destroy(s_battery_empty_black_bitmap);  
  gbitmap_destroy(s_battery_25_white_bitmap);
  gbitmap_destroy(s_battery_25_black_bitmap);
  gbitmap_destroy(s_battery_50_white_bitmap);
  gbitmap_destroy(s_battery_50_black_bitmap);
  gbitmap_destroy(s_battery_75_white_bitmap);
  gbitmap_destroy(s_battery_75_black_bitmap);
  gbitmap_destroy(s_battery_full_white_bitmap);
  gbitmap_destroy(s_battery_full_black_bitmap);
  layer_destroy(s_battery_layer);
}

void bluetooth_enabled(bool vibrate) {
  is_bluetooth_enabled = true;
  layer_mark_dirty(s_bluetooth_layer);
  
  // Vibrate watch to notify user
  if (vibrate) {
    vibes_short_pulse();
  }
}

void bluetooth_disabled(bool vibrate) {
  is_bluetooth_enabled = false;
  layer_mark_dirty(s_bluetooth_layer);
    
  // Vibrate watch to notify user
  if (vibrate) {
    vibes_long_pulse();
  }
}

void bluetooth_state_change_handler(bool connected) {
  if (connected) {
    bluetooth_enabled(true);
  } else {
    bluetooth_disabled(true);
  }
}

void bluetooth_state_init() {
  if (bluetooth_connection_service_peek()) {
    bluetooth_enabled(false);
  } else {
    bluetooth_disabled(false);
  }
}

static void draw_bluetooth_layer(Layer *layer, GContext *ctx) {
  if (is_bluetooth_enabled) {
    // Draw white
    graphics_context_set_compositing_mode(ctx, GCompOpOr);
    graphics_draw_bitmap_in_rect(ctx, s_bluetooth_enabled_white_bitmap, s_bluetooth_enabled_white_bitmap->bounds);

    // Draw black
    graphics_context_set_compositing_mode(ctx, GCompOpClear);
    graphics_draw_bitmap_in_rect(ctx, s_bluetooth_enabled_black_bitmap, s_bluetooth_enabled_black_bitmap->bounds);
  } else {
    // Draw white
    graphics_context_set_compositing_mode(ctx, GCompOpOr);
    graphics_draw_bitmap_in_rect(ctx, s_bluetooth_disabled_white_bitmap, s_bluetooth_disabled_white_bitmap->bounds);

    // Draw black
    graphics_context_set_compositing_mode(ctx, GCompOpClear);
    graphics_draw_bitmap_in_rect(ctx, s_bluetooth_disabled_black_bitmap, s_bluetooth_disabled_black_bitmap->bounds);
  }
}

void bluetooth_init(Window *window, bool enable_bluetooth_monitor) {
  if (enable_bluetooth_monitor) {
    // Load bluetooth bitmaps
    s_bluetooth_enabled_white_bitmap = gbitmap_create_with_resource(RESOURCE_ID_ICON_BLUETOOTH_ENABLED_WHITE);
    s_bluetooth_enabled_black_bitmap = gbitmap_create_with_resource(RESOURCE_ID_ICON_BLUETOOTH_ENABLED_BLACK);
    s_bluetooth_disabled_white_bitmap = gbitmap_create_with_resource(RESOURCE_ID_ICON_BLUETOOTH_DISABLED_WHITE);
    s_bluetooth_disabled_black_bitmap = gbitmap_create_with_resource(RESOURCE_ID_ICON_BLUETOOTH_DISABLED_BLACK);
      
    // Create bluetooth Layer & add to window
    s_bluetooth_layer = layer_create(GRect(2, 2, 16, 16));
    layer_set_update_proc(s_bluetooth_layer, draw_bluetooth_layer);
    layer_add_child(window_get_root_layer(window), s_bluetooth_layer);

    // Start monitoring bluetooth connection
    bluetooth_state_init();
    bluetooth_connection_service_subscribe(bluetooth_state_change_handler);
  }
}

void bluetooth_deinit() {
  // Cleanup bluetooth monitor resources
  gbitmap_destroy(s_bluetooth_enabled_white_bitmap);
  gbitmap_destroy(s_bluetooth_enabled_black_bitmap);
  gbitmap_destroy(s_bluetooth_disabled_white_bitmap);
  gbitmap_destroy(s_bluetooth_disabled_black_bitmap);
  layer_destroy(s_bluetooth_layer);
}

void utils_init(Window *window, bool enable_battery_monitor, bool enable_bluetooth_monitor) {
  battery_init(window, enable_battery_monitor);
  bluetooth_init(window, enable_bluetooth_monitor);
}

void utils_deinit() {
  battery_deinit();
  bluetooth_deinit();
}