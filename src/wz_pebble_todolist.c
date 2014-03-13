//
//  Created by Erica Chang on 3/12/2014.
//  Copyright (c) 2014 Woozilli, Inc. All rights reserved.
//

#include <pebble.h>

static Window *window;
static MenuLayer *menu_layer;

enum {
  WZ_TODO_KEY_APPEND_TEXT, // Phone -> Watch: append todo item text
  WZ_TODO_KEY_APPEND_ID,   // Phone -> Watch: append todo item id
  WZ_TODO_KEY_DELETE,      // Watch -> Phone: delete todo item by providing id
  WZ_TODO_KEY_FETCH        // Watch -> Phone: fetch todo list items
};

#define MAX_TODO_LIST_COUNT (20)
#define MAX_TODO_ITEM_TEXT_LENGTH (16)

// Each todo list item has a unique item_id
typedef struct {
  int item_id;
  char text[MAX_TODO_ITEM_TEXT_LENGTH];
} TodoListItem;

static TodoListItem todo_list[MAX_TODO_LIST_COUNT];
static int todo_list_count = 0;

char appTitle[] = "WOOZILLI TODO LIST";

// -- Todo list management -- 
static void todo_list_init(void) {
  DictionaryIterator *iter;

  if (app_message_outbox_begin(&iter) != APP_MSG_OK) {
    return;
  }
  if (dict_write_uint8(iter, WZ_TODO_KEY_FETCH, 1) != DICT_OK) {
    return;
  }
  app_message_outbox_send();
}

static TodoListItem* get_todo_list_item_at_index(int index) {
  if (index < 0 || index >= MAX_TODO_LIST_COUNT) {
    return NULL;
  }

  return &todo_list[index];
}

static void todo_list_add(char *data, int item_id) {
  if (todo_list_count == MAX_TODO_LIST_COUNT) { 
    return;
  }

  strcpy(todo_list[todo_list_count].text, data);
  todo_list[todo_list_count].item_id = item_id;
  todo_list_count++;
}

static void todo_list_delete(uint8_t list_index) {
  if (todo_list_count < 1) {
    return;
  }

  todo_list_count--;

  memmove(&todo_list[list_index], &todo_list[list_index + 1],
      ((todo_list_count - list_index) * sizeof(TodoListItem)));
}

// -- Menu Layer callback functions --
// Section header(there's only one header that serves as the application title)
static void draw_header_callback(GContext *ctx, const Layer *cell_layer, uint16_t section_index, void *callback_context)
{
    graphics_context_set_text_color(ctx, GColorBlack);
    graphics_draw_text(
        ctx,
        appTitle,
        fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD),
        GRect(0,0,layer_get_frame(cell_layer).size.w,
        layer_get_frame(cell_layer).size.h),
        GTextOverflowModeTrailingEllipsis,
        GTextAlignmentCenter,
        NULL);
}

// Menu section header fixed to 20
static int16_t get_header_height_callback(struct MenuLayer *menu_layer, uint16_t section_index, void *callback_context)
{
    return 20;
}

// Menu cell height fixed to 44
static int16_t get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  return 44;
}

// Draw an individual row based on cell index
static void draw_row_callback(GContext* ctx, Layer *cell_layer, MenuIndex *cell_index, void *data) {
  TodoListItem* item;
  const int index = cell_index->row;

  if ((item = get_todo_list_item_at_index(index)) == NULL) {
    return;
  }

  menu_cell_basic_draw(ctx, cell_layer, item->text, NULL, NULL);
}

// Returns the current todo list item count
static uint16_t get_num_rows_callback(struct MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return todo_list_count;
}

// Delete the selected todo list item
static void select_long_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  TodoListItem *item;
  const int index = cell_index->row;
  DictionaryIterator *iter;
  if ((item = get_todo_list_item_at_index(index)) == NULL) {
    return;
  }
  if (app_message_outbox_begin(&iter) != APP_MSG_OK) {
    return;
  }

  if (dict_write_int(iter, WZ_TODO_KEY_DELETE, &item->item_id, 4, true) != DICT_OK) {
    return;
  }
  todo_list_delete(index);
  app_message_outbox_send();

  menu_layer_reload_data(menu_layer);
}

static void window_load(Window* window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect window_frame = layer_get_frame(window_layer);
  menu_layer = menu_layer_create(window_frame);
  menu_layer_set_callbacks(menu_layer, NULL, (MenuLayerCallbacks) {
    .draw_header = (MenuLayerDrawHeaderCallback) draw_header_callback,
    .get_header_height = (MenuLayerGetHeaderHeightCallback) get_header_height_callback,
    .get_cell_height = (MenuLayerGetCellHeightCallback) get_cell_height_callback,
    .draw_row = (MenuLayerDrawRowCallback) draw_row_callback,
    .get_num_rows = (MenuLayerGetNumberOfRowsInSectionsCallback) get_num_rows_callback,
    .select_long_click = (MenuLayerSelectCallback) select_long_callback
  });
  menu_layer_set_click_config_onto_window(menu_layer, window);
  layer_add_child(window_layer, menu_layer_get_layer(menu_layer));
}

static void in_received_handler(DictionaryIterator *iter, void *context) {
  Tuple *add_tuple_text = dict_find(iter, WZ_TODO_KEY_APPEND_TEXT);
  Tuple *add_tuple_id = dict_find(iter, WZ_TODO_KEY_APPEND_ID);
  
  if (add_tuple_text && add_tuple_id) {
    todo_list_add(add_tuple_text->value->cstring, add_tuple_id->value->int32);
  }

  menu_layer_reload_data(menu_layer);
}

static void app_message_init(void) {
  // Init message buffers (message inbox size maximum, message outbox size maximum)
  app_message_open(64, 16);
  // Register message handlers
  app_message_register_inbox_received(in_received_handler);
}

int main(void) {
  window = window_create();

  app_message_init();
  todo_list_init();

  // configure window
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
  });
  window_stack_push(window, true); // Animated

  // init done, start event loop
  app_event_loop();

  // clean up
  window_destroy(window);
  menu_layer_destroy(menu_layer);
}
