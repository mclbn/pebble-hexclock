#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer;

static void update_time() {
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);
    static char s_buffer[9] = {0};

    if (clock_is_24h_style())
        snprintf(s_buffer, 9, "%02x:%02x", tick_time->tm_hour, tick_time->tm_min);
    else
        snprintf(s_buffer, 9, "%02x:%02x", tick_time->tm_hour % 12, tick_time->tm_min);
    text_layer_set_text(s_time_layer, s_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    update_time();
}

static void window_load(Window *window) {
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    s_time_layer = text_layer_create(
        GRect(0, PBL_IF_ROUND_ELSE(58, 52), bounds.size.w, 50));
    text_layer_set_background_color(s_time_layer, GColorBlack);
    text_layer_set_text_color(s_time_layer, GColorClear);
//    text_layer_set_text(s_time_layer, "00:00");
    text_layer_set_font(s_time_layer,
                        fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
    text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
    layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
}

static void window_unload(Window *window) {
    text_layer_destroy(s_time_layer);
}

static void init(void) {
    s_main_window = window_create();
    window_set_background_color(s_main_window, GColorBlack);
    window_set_window_handlers(s_main_window, (WindowHandlers) {
            .load = window_load,
                .unload = window_unload,
                });
    window_stack_push(s_main_window, true);
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit(void) {
    window_destroy(s_main_window);
}

int main(void) {
    init();
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", s_main_window);
    app_event_loop();
    deinit();
}
