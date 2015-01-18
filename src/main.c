#include <pebble.h>
#define NUM_MENU_SECTIONS 1
#define NUM_MENU_ITEMS 4
  
enum {
  ID_KEY = 0x0,
  REQUEST_KEY = 0x1
};
  
static Window *connect_window;  
static Window *main_window;
static Window *confirm_window;
static BitmapLayer *connect_layer;
static GBitmap *connect_bitmap;
static MenuLayer *menu_layer;
static BitmapLayer *confirm_layer;
static GBitmap *confirm_bitmap;

static int idNum;

// Write message to buffer and send
void send_message(uint8_t reqNum) {
  DictionaryIterator *iter;
  
  app_message_outbox_begin(&iter);
  dict_write_uint8(iter, REQUEST_KEY, reqNum);
  
  dict_write_end(iter);
  app_message_outbox_send();
}

static void in_received_handler(DictionaryIterator *received, void *context) {
  Tuple *tuple;
  
  tuple = dict_find(received, ID_KEY);
  if(tuple) {
    idNum = (int) tuple->value->uint32;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "ID Num: %d", idNum);
    window_stack_push(main_window, true);
  }
}

// Called when an incoming message from PebbleKitJS is dropped
static void in_dropped_handler(AppMessageResult reason, void *context) {	
}

// Called when PebbleKitJS does not acknowledge receipt of a message
static void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
}

static void select_clicked(ClickRecognizerRef recognizer, void* context) {
  send_message(0x0);
}

static void click_config_provider(void* context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_clicked);
}

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
      send_message(0x01);
      window_stack_push(confirm_window, true);
      break;
    case 1:
      send_message(0x02);
      window_stack_push(confirm_window, true);
      break;
    case 2:
      send_message(0x03);
      window_stack_pop(true);
      break;
    case 3:
      window_stack_push(confirm_window, true);
      send_message(0x04);
      break;
  }
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

static void connect_window_load(Window *window) {
  connect_bitmap = gbitmap_create_with_resource(RESOURCE_ID_CONNECT_BACKGROUND);
  connect_layer = bitmap_layer_create(GRect(0, 0, 144, 152));
  bitmap_layer_set_bitmap(connect_layer, connect_bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(connect_layer));
}

static void connect_window_unload(Window *window) {
  gbitmap_destroy(connect_bitmap);
  bitmap_layer_destroy(connect_layer);
}

static void confirm_window_load(Window *window) {
  confirm_bitmap = gbitmap_create_with_resource(RESOURCE_ID_CONFIRM_BACKGROUND);
  confirm_layer = bitmap_layer_create(GRect(0, 0, 144, 152));
  bitmap_layer_set_bitmap(confirm_layer, confirm_bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(confirm_layer));
}

static void confirm_window_unload(Window *window) {
  gbitmap_destroy(confirm_bitmap);
  bitmap_layer_destroy(confirm_layer);
}


static void init(void) {
  connect_window = window_create();
  window_set_click_config_provider(connect_window, click_config_provider);
  window_set_window_handlers(connect_window, (WindowHandlers) {
    .load = connect_window_load,
    .unload = connect_window_unload
  });
  
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
  
  	app_message_register_inbox_received(in_received_handler); 
	app_message_register_inbox_dropped(in_dropped_handler); 
	app_message_register_outbox_failed(out_failed_handler);
		
	app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  
  window_stack_push(connect_window, true);
}

static void deinit(void) {
  app_message_deregister_callbacks();
  window_destroy(connect_window);
  window_destroy(main_window);
  window_destroy(confirm_window);
}
  
int main(void) {
  init();
  app_event_loop();
  deinit();
}