#include <pebble.h>
#include <math.h>

static Window *s_main_window;
static Layer *s_canvas_layer;
static GPoint s_start;
static GPoint s_end;
static AppTimer *timer;
static TextLayer *s_time_layer;

static const int STEP_MS = 1000;
static const int RADIUS = 67;
static const int X_CENTER = 72;
static const int Y_CENTER = 84;

static void get_next_x_y(int16_t *x, int16_t *y, int value, int type){
  int corner = 0;
    
  if(type == 0){
    corner = (value - 15) * 6;
  }else if(type == 1){
    if(value > 12)
      value -= 12;
    //TODO
    //corner = ((value -3) * 30) + ();
  }
  
  
  ///////////
  char *step_count = "1234567890";
  
  snprintf(step_count, sizeof(step_count), "%d", value);
  text_layer_set_text(s_time_layer, step_count);
  ////////////
  
  *x = RADIUS * cos((double)corner * (3.14/180.0)) + X_CENTER;
  *y = RADIUS * sin((double)corner * (3.14/180.0)) + Y_CENTER;
  
  /*if(corner > 359){
    corner = 0;
  }else{
    corner = corner + 3;
  }*/
}

static void timer_callback(void *data){
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);
    
  get_next_x_y(&s_end.x, &s_end.y, tick_time->tm_hour, 1);
  
  /*static char date_buffer[16];
  strftime(date_buffer, sizeof(date_buffer), clock_is_24h_style() ?
                                          "%H:%M:%S" : "%I:%M:%S", tick_time);
  text_layer_set_text(s_time_layer, date_buffer);*/
  
  layer_mark_dirty(s_canvas_layer);
}

static void canvas_update_proc(Layer *layer, GContext *ctx) {
  // Custom drawing happens here!
  
  //s_start = GPoint(10, 10);
  //s_end = GPoint(40, 60);
  
  // Draw a line
  graphics_draw_line(ctx, s_start, s_end);
  
  
  // Redraw this as soon as possible
  layer_mark_dirty(s_canvas_layer);
  
  timer = app_timer_register(STEP_MS, timer_callback, NULL);
}

void window_load(Window *window){
  GRect bounds = layer_get_bounds(window_get_root_layer(window));

  // Create canvas layer
  s_canvas_layer = layer_create(bounds);
  
  // Assign the custom drawing procedure
  layer_set_update_proc(s_canvas_layer, canvas_update_proc);
  
  // Add to Window
  layer_add_child(window_get_root_layer(window), s_canvas_layer);
  
  timer = app_timer_register(SECOND_UNIT, timer_callback, NULL);
  
  s_time_layer = text_layer_create(
      GRect(0, 0, bounds.size.w, 50));
  
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_text(s_time_layer, "00:00");
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentLeft);
  
  layer_add_child(s_canvas_layer, text_layer_get_layer(s_time_layer));
}

void wnidow_unload(Window *window){
  layer_destroy(s_canvas_layer);
  text_layer_destroy(s_time_layer);
}

void init(){
  s_main_window = window_create();
  
  window_set_window_handlers(s_main_window, (WindowHandlers){
    .load = window_load,
    .unload = wnidow_unload
  });
  
  s_start = GPoint(X_CENTER, Y_CENTER);
  s_end = GPoint(72, 10);
   
  window_set_background_color(s_main_window, GColorWhite);
  
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