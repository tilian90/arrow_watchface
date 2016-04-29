#include <pebble.h>
#include <math.h>

static Window *s_main_window;
static Layer *s_canvas_layer;
static GPoint s_start, s_end_minute, s_end_hour;
//static TextLayer *s_time_layer;
static BitmapLayer *s_logo_layer;
static GBitmap *s_logo_bitmap;

static const int RADIUS_MIN = 67;
static const int RADIUS_HOUR = 57;
static const int X_CENTER = 72;
static const int Y_CENTER = 84;

static void get_next_x_y(int16_t *x, int16_t *y, int corner, int radius){   
  *x = radius * cos((double)corner * (3.14/180.0)) + X_CENTER;
  *y = radius * sin((double)corner * (3.14/180.0)) + Y_CENTER;  
}

static void time_calc(){
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);
  int16_t corner, t_x, t_y;
  
  //Drawing minute arrow
  corner = (tick_time->tm_min - 15) * 6;
  //corner = tick_time->tm_min * 6;
  s_end_minute.x = RADIUS_MIN * cos(corner * (3.14/180.0)) + X_CENTER;
  s_end_minute.y = RADIUS_MIN * sin(corner * (3.14/180.0)) + Y_CENTER; 
  //donothing(corner);
  //get_next_x_y(&s_end_minute.x, &s_end_minute.y, corner, RADIUS_MIN);
    
  //Drawing hour arrow
  //corner = ((tick_time->tm_min - 3) * 30);//+ (int)(tick_time->tm_min / 2);
  if(tick_time->tm_hour > 12)
    corner = ((tick_time->tm_hour - 12 - 3) * 30) + (int)(tick_time->tm_min / 2);
  else
    corner = ((tick_time->tm_hour - 3) * 30) + (int)(tick_time->tm_min / 2);
  //corner = -9;
  //nothing(t_x, t_y);
  
  /*
  s_end_minute.x = t_x;
  s_end_minute.y = t_y;*/
  
  //s_end_hour.x = (int16_t)(RADIUS_HOUR * cos(corner * (3.14/180.0)) + X_CENTER);
  //s_end_hour.y = (int16_t)(RADIUS_HOUR * sin(corner * (3.14/180.0)) + Y_CENTER);
  s_end_hour.x = RADIUS_HOUR * cos(corner * (3.14/180.0)) + X_CENTER;
  s_end_hour.y = RADIUS_HOUR * sin(corner * (3.14/180.0)) + Y_CENTER;
  
  /////////// DEBUG
  char *step_count = "1234567890";
  
  snprintf(step_count, sizeof(step_count), "%d", s_end_hour.x);
  //text_layer_set_text(s_time_layer, step_count);
  ////////////
  
  //get_next_x_y(&s_end_hour.x, &s_end_hour.y, corner, RADIUS_HOUR);
    
  layer_mark_dirty(s_canvas_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  time_calc();
}

static void canvas_update_proc(Layer *layer, GContext *ctx) {  
  //Setting color
  graphics_context_set_stroke_color(ctx, GColorWhite);
  
  //graphics_draw_circle(ctx, GPoint(X_CENTER, Y_CENTER), RADIUS_MIN + 4);
  //s_start = GPoint(X_CENTER, Y_CENTER);
  
  // Draw a line
  graphics_draw_line(ctx, s_start, s_end_minute);
  graphics_draw_line(ctx, s_start, s_end_hour);
    
  // Redraw this as soon as possible
  //layer_mark_dirty(s_canvas_layer);
  
  //timer = app_timer_register(STEP_MS, timer_callback, NULL);
}

void window_load(Window *window){
  GRect bounds = layer_get_bounds(window_get_root_layer(window));
  
  //Create bitmap
  s_logo_bitmap = gbitmap_create_with_resource(RESOURCE_ID_RESOURCE_ID_BATLOGO);
  
  //Create bitmap layer
  s_logo_layer = bitmap_layer_create(bounds);
  bitmap_layer_set_bitmap(s_logo_layer, s_logo_bitmap);

  // Create canvas layer
  s_canvas_layer = layer_create(bounds);
  
  // Assign the custom drawing procedure
  layer_set_update_proc(s_canvas_layer, canvas_update_proc);
  
  // Add to Window
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_logo_layer));
  layer_add_child(window_get_root_layer(window), s_canvas_layer);
  
  //timer = app_timer_register(SECOND_UNIT, timer_callback, NULL);
  
  
  /*s_time_layer = text_layer_create(
      GRect(0, 0, bounds.size.w, 50));
  
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_text(s_time_layer, "00:00");
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentLeft);*/
  
  //layer_add_child(s_canvas_layer, text_layer_get_layer(s_time_layer));
}

void wnidow_unload(Window *window){
  layer_destroy(s_canvas_layer);
  gbitmap_destroy(s_logo_bitmap);
  bitmap_layer_destroy(s_logo_layer);
  //text_layer_destroy(s_time_layer);
}

void init(){
  s_main_window = window_create();
  
  window_set_window_handlers(s_main_window, (WindowHandlers){
    .load = window_load,
    .unload = wnidow_unload
  });
  
  s_start = GPoint(X_CENTER, Y_CENTER);
  s_end_minute = GPoint(72, 17);
  s_end_hour = GPoint(72, 27);
  
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  window_set_background_color(s_main_window, GColorBlack);
  
  window_stack_push(s_main_window, true);
}

void deinit(){
  window_destroy(s_main_window);
}


int main(){
  init();
  app_event_loop();
  deinit();
}