/*
*	A sqaure professional style watch-face
*	Custom watch face by Max Kross
* 
*/
#include "saphire.h"

#include "pebble.h"

static Window *window;
static Layer *s_date_layer, *s_hands_layer;
static GBitmap *background_image;
static BitmapLayer *watchface_layer;
static TextLayer *s_num_label;
static GFont custom_font;


static GPath *s_minute_arrow, *s_hour_arrow, *s_second_arrow;
static char s_num_buffer[4];

static void hands_update_proc(Layer *layer, GContext *ctx) {
  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  // minute/hour hand
	#ifdef PBL_PLATFORM_APLITE
  	graphics_context_set_fill_color(ctx, GColorBlack);
  	graphics_context_set_stroke_color(ctx, GColorBlack);
	#elif PBL_PLATFORM_BASALT
		graphics_context_set_fill_color(ctx, GColorCobaltBlue);
  	graphics_context_set_stroke_color(ctx, GColorCobaltBlue);
	#endif
		

		gpath_rotate_to(s_second_arrow, TRIG_MAX_ANGLE * t->tm_sec / 60);
	gpath_draw_outline(ctx, s_second_arrow);
  gpath_draw_filled(ctx, s_second_arrow);
	
	

  gpath_rotate_to(s_hour_arrow, (TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 6) + (t->tm_min / 10))) / (12 * 6));
  gpath_draw_filled(ctx, s_hour_arrow);
  gpath_draw_outline(ctx, s_hour_arrow);

		gpath_rotate_to(s_minute_arrow, TRIG_MAX_ANGLE * t->tm_min / 60);
	gpath_draw_outline(ctx, s_minute_arrow);
  gpath_draw_filled(ctx, s_minute_arrow);

	//Large dot
	#ifdef PBL_PLATFORM_APLITE
  	graphics_context_set_fill_color(ctx, GColorBlack);
	#elif PBL_PLATFORM_BASALT
		graphics_context_set_fill_color(ctx, GColorCobaltBlue);
	#endif
		graphics_fill_circle(ctx, GPoint(72,84), 4 );
	
  //small dot in the middle
	#ifdef PBL_PLATFORM_APLITE
  	graphics_context_set_fill_color(ctx, GColorBlack);
	#elif PBL_PLATFORM_BASALT
		graphics_context_set_fill_color(ctx, GColorLightGray);
	#endif
		graphics_fill_circle(ctx, GPoint(72,84), 1 );
}

static void date_update_proc(Layer *layer, GContext *ctx) {
  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  strftime(s_num_buffer, sizeof(s_num_buffer), "%d", t);
  text_layer_set_text(s_num_label, s_num_buffer);
}

static void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(window_get_root_layer(window));
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

	background_image = gbitmap_create_with_resource(RESOURCE_ID_watch_face);
	watchface_layer = bitmap_layer_create(GRect(0,0,144,168));
	bitmap_layer_set_bitmap(watchface_layer, background_image);
  layer_add_child(window_layer, bitmap_layer_get_layer(watchface_layer));

  s_date_layer = layer_create(bounds);
  layer_set_update_proc(s_date_layer, date_update_proc);
  layer_add_child(bitmap_layer_get_layer(watchface_layer), s_date_layer);

  s_num_label = text_layer_create(GRect(124, 77, 14, 14));
  text_layer_set_text(s_num_label, s_num_buffer);
  text_layer_set_background_color(s_num_label, GColorWhite);
  text_layer_set_text_color(s_num_label, GColorBlack);
	custom_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_Franklin12));
  text_layer_set_font(s_num_label, custom_font);
	text_layer_set_text_alignment(s_num_label, GTextAlignmentCenter);

  layer_add_child(s_date_layer, text_layer_get_layer(s_num_label));

  s_hands_layer = layer_create(bounds);
  layer_set_update_proc(s_hands_layer, hands_update_proc);
  layer_add_child(bitmap_layer_get_layer(watchface_layer), s_hands_layer);
	
}

static void window_unload(Window *window) {
	gbitmap_destroy(background_image);
  layer_destroy(bitmap_layer_get_layer(watchface_layer));
  layer_destroy(s_date_layer);

  text_layer_destroy(s_num_label);

  layer_destroy(s_hands_layer);
}

static void init() {
  window = window_create();
		
	#ifdef PBL_BW
		window_set_fullscreen(window,1);
	#endif
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });

	
  window_stack_push(window, true);
 
  s_num_buffer[0] = '\0';

  // init hand paths
  s_minute_arrow = gpath_create(&MINUTE_HAND_POINTS);
  s_hour_arrow = gpath_create(&HOUR_HAND_POINTS);
	
	s_second_arrow = gpath_create(&SECOND_HAND_POINTS);
	

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  GPoint center = grect_center_point(&bounds);
	
	//Centers all drawing paths to the middle of the watch
  gpath_move_to(s_minute_arrow, center);
  gpath_move_to(s_hour_arrow, center);

	gpath_move_to(s_second_arrow, center);

  

  tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick);

}

static void deinit() {
  gpath_destroy(s_minute_arrow);
  gpath_destroy(s_hour_arrow);

	gpath_destroy(s_second_arrow);


 

  tick_timer_service_unsubscribe();
  window_destroy(window);
}

int main() {
  init();
  app_event_loop();
  deinit();
}

