#include <pebble.h>
#define NUM_MENU_SECTIONS 1
#define NUM_MENU_ITEMS 4
  
static Window *main_window;
static Window *confirm_window;
static MenuLayer *menu_layer;

static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return NUM_MENU_SECTIONS;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return NUM_MENU_ITEMS;
}

static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  // This is a define provided in pebble.h that you may use for the default height
  return MENU_CELL_BASIC_HEADER_HEIGHT;
}

static void menu_draw_header_callback(GContext *ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  menu_cell_basic_header_draw(ctx, cell_layer, "Requests");
}

static void menu_draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  switch(cell_index->row) {
    case 0:
      menu_cell_basic_draw(ctx, cell_layer, "Ready to Order", "Order up!", NULL);
      break;
    case 1:
      menu_cell_basic_draw(ctx, cell_layer, "Request a Refill", "Feeling thirsty?", NULL);
      break;
    case 2:
      menu_cell_basic_draw(ctx, cell_layer, "Request Check", "All finished!", NULL);
      break;
    case 3:
      menu_cell_basic_draw(ctx, cell_layer, "Need Assistance?", "General purpose help", NULL);
      break;
  }
}

void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  switch(cell_index->row) {
    case 0:
      
      break;
    case 1:
      break;
  }
  window_stack_push(confirm_window, true);
}

static void main_window_load(Window *window) {
  
  Layer *window_layer = window_get_root_layer(main_window);
  GRect bounds = layer_get_frame(window_layer);
  
  menu_layer = menu_layer_create(bounds);
  
  menu_layer_set_callbacks(menu_layer, NULL, (MenuLayerCallbacks){
    .get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = menu_get_num_rows_callback,
    .get_header_height = menu_get_header_height_callback,
    .draw_header = menu_draw_header_callback,
    .draw_row = menu_draw_row_callback,
    .select_click = menu_select_callback,
  });
  
  menu_layer_set_click_config_onto_window(menu_layer, main_window);
  
  layer_add_child(window_layer, menu_layer_get_layer(menu_layer));
  
}

static void main_window_unload(Window *window) {
  menu_layer_destroy(menu_layer);
}

static void confirm_window_load(Window *window) {
  
}

static void confirm_window_unload(Window *window) {
  
}


static void init(void) {
  main_window = window_create();
  window_set_window_handlers(main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  
  confirm_window = window_create();
  window_set_window_handlers(confirm_window, (WindowHandlers) {
    .load = confirm_window_load,
    .unload = confirm_window_unload
  });
  window_stack_push(main_window, true);
}

static void deinit(void) {
  window_destroy(main_window);
  window_destroy(confirm_window);
}
  
int main(void) {
  init();
  app_event_loop();
  deinit();
}