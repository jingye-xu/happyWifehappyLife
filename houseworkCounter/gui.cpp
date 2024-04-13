#include "core/lv_obj.h"
#include "widgets/lv_roller.h"
#include "core/lv_event.h"
#include <LilyGo_AMOLED.h>
#include "gui.h"
#include <lvgl.h>
#define COLOR_TEXT 0x07cff2


// only visible in this file
static lv_obj_t *tileview;
static lv_obj_t *husband_label;
static lv_obj_t *wife_label;
static lv_obj_t *battVolt_label;
static lv_obj_t *battPct_label;
static lv_obj_t *history0_label;
static lv_obj_t *history1_label;
static lv_obj_t *history2_label;
RTC_DATA_ATTR static WhoDid histories[HISTORY_LEN];
RTC_DATA_ATTR static int idx = 0;
static whoDid global_case;

// globally visible, declared in h with extern
int pageID=0;
unsigned short wife;
unsigned short husband;
float battery;
float battery_percent;

const char* getDayName(enum people person)
{
  switch (person)
  {
    case NOONE: return "No one";
    case HUSBAND: return "Husband";
    case WIFE: return "Wife";
  }
}

const char* getOpsName(enum ops op)
{
  switch (op)
  {
    case ADD: return "did";
    case MINUS: return "did not";
  }
}

const char* getHouseworkName(enum houseworks housework)
{
  switch (housework)
  {
    case SOMETHING: return "something";
    case COOK: return "cook";
    case LUNDARY: return "lundary";
    case CLEAN: return "clean";
    case TRASH: return "trash";
  }
}

void update_board(lv_timer_t *e)
{
  lv_label_set_text_fmt(husband_label, "husband: %4d", husband);
  lv_label_set_text_fmt(wife_label, "wife: %4d", wife);
  lv_label_set_text_fmt(battVolt_label, "%5.3fV", battery);
  lv_label_set_text_fmt(battPct_label, "%5.1f%%", battery_percent);
}

void update_history(lv_timer_t *e)
{
  lv_label_set_text_fmt(history0_label, "%s %s %s", getDayName(histories[(idx+2)%HISTORY_LEN].who), getOpsName(histories[(idx+2)%HISTORY_LEN].did), getHouseworkName(histories[(idx+2)%HISTORY_LEN].housework));
  lv_label_set_text_fmt(history1_label, "%s %s %s", getDayName(histories[(idx+1)%HISTORY_LEN].who), getOpsName(histories[(idx+1)%HISTORY_LEN].did), getHouseworkName(histories[(idx+1)%HISTORY_LEN].housework));
  lv_label_set_text_fmt(history2_label, "%s %s %s", getDayName(histories[(idx)%HISTORY_LEN].who), getOpsName(histories[(idx)%HISTORY_LEN].did), getHouseworkName(histories[(idx)%HISTORY_LEN].housework));
}

void dashBoardUI(lv_obj_t *parent)
{ 
  // dashboard content
  lv_obj_t *dash_cont = lv_obj_create(parent);
  lv_obj_set_size(dash_cont, LV_PCT(100), LV_PCT(100));
  lv_obj_set_style_bg_color(dash_cont, lv_color_black(), 0);

  // dashboard main text
  husband_label = lv_label_create(dash_cont);
  lv_label_set_text_fmt(husband_label, "husband: %4d", husband);
  lv_obj_set_style_text_align(husband_label, LV_TEXT_ALIGN_LEFT, 0);
  lv_obj_set_style_text_color(husband_label, lv_color_hex(COLOR_TEXT), 0);
  lv_obj_set_style_text_font(husband_label, &lv_font_montserrat_40, 0);
  lv_obj_set_size(husband_label, LV_PCT(80), LV_PCT(30));
  lv_obj_set_pos(husband_label, LV_PCT(10), LV_PCT(20));
  
  wife_label = lv_label_create(dash_cont);
  lv_label_set_text_fmt(wife_label, "wife: %4d", wife);
  lv_obj_set_style_text_align(wife_label, LV_TEXT_ALIGN_LEFT, 0);
  lv_obj_set_style_text_color(wife_label, lv_color_hex(COLOR_TEXT), 0);
  lv_obj_set_style_text_font(wife_label, &lv_font_montserrat_40, 0);
  lv_obj_set_size(wife_label, LV_PCT(80), LV_PCT(30));
  lv_obj_set_pos(wife_label, LV_PCT(10), LV_PCT(50));

  battVolt_label = lv_label_create(dash_cont);
  lv_label_set_text_fmt(battVolt_label, "%5.3fV", battery);
  lv_obj_set_style_text_align(battVolt_label, LV_TEXT_ALIGN_LEFT, 0);
  lv_obj_set_style_text_color(battVolt_label, lv_color_hex(COLOR_TEXT), 0);
  lv_obj_set_style_text_font(battVolt_label, &lv_font_montserrat_24, 0);
  lv_obj_set_size(battVolt_label, LV_PCT(50), LV_PCT(10));
  lv_obj_set_pos(battVolt_label, LV_PCT(0), LV_PCT(0));

  battPct_label = lv_label_create(dash_cont);
  lv_label_set_text_fmt(battPct_label, "%5.1f%%", battery_percent);
  lv_obj_set_style_text_align(battPct_label, LV_TEXT_ALIGN_RIGHT, 0);
  lv_obj_set_style_text_color(battPct_label, lv_color_hex(COLOR_TEXT), 0);
  lv_obj_set_style_text_font(battPct_label, &lv_font_montserrat_24, 0);
  lv_obj_set_size(battPct_label, LV_PCT(50), LV_PCT(10));
  lv_obj_set_pos(battPct_label, LV_PCT(50), LV_PCT(0));

  lv_timer_create(update_board, 500, NULL);
}

// button1 for updating
static void event_cb1(lv_event_t * e)
{
  if (pageID == 1)
  {
    appendNewHistory();
  }
}

// button2 for Clearing
static void event_cb2(lv_event_t * e)
{  
  if (pageID == 1)
  {
    clearData();
  }
}

static void person_event_handler(lv_event_t * e)
{
  lv_obj_t *roller = lv_event_get_target(e);
  global_case.who = (people)lv_roller_get_selected(roller);
}

static void op_event_handler(lv_event_t * e)
{
  lv_obj_t *roller = lv_event_get_target(e);
  global_case.did = (ops)lv_roller_get_selected(roller);
}

static void housework_event_handler(lv_event_t * e)
{
  lv_obj_t *roller = lv_event_get_target(e);
  global_case.housework = (houseworks)lv_roller_get_selected(roller);
}

int my_btn1_read()
{
  // button1 GPIO0
  if (digitalRead(BTN1_PIN) == 0)
  {
    time_count = TIMEOUT;
    return 1; // pressed
  }
  else {
    return 0; // released
  }
}

int my_btn2_read()
{
  // button1 GPIO21
  if (digitalRead(BTN2_PIN) == 0)
  {
    time_count = TIMEOUT;
    return 1; // pressed
  }
  else {
    return 0; // released
  }
}

void button1_read(lv_indev_drv_t * drv, lv_indev_data_t*data){
  // simulated encoder 
  static uint32_t last_btn = 0;   /*Store the last pressed button*/
  int btn_pr = my_btn1_read();     /*Get the ID (0,1,2...) of the pressed button*/
  
  switch (btn_pr) {
    case 0: data->enc_diff = 0; break;
    case 1: if (last_btn==0)data->enc_diff = 1; break;
  }
  last_btn = btn_pr;                /*Save the ID of the pressed button*/
  data->btn_id = last_btn;          /*Save the last button*/
}

void button2_read(lv_indev_drv_t * drv, lv_indev_data_t*data){
  // button click
  static uint32_t last_btn = 0;   /*Store the last pressed button*/
  int btn_pr = my_btn2_read();     /*Get the ID (0,1,2...) of the pressed button*/
  
  switch (btn_pr) {
    case 0: data->state = LV_INDEV_STATE_RELEASED; break;
    case 1: data->state = LV_INDEV_STATE_PRESSED; break;
  }

  last_btn = btn_pr;                /*Save the ID of the pressed button*/
  data->btn_id = last_btn;          /*Save the last button*/
}

void updateUI(lv_obj_t *parent)
{ 
  // group
  lv_group_t *group = lv_group_create();

  // HW button1
  static lv_indev_drv_t indev_drv1;
  lv_indev_drv_init(&indev_drv1);          /*Basic initialization*/
  indev_drv1.type = LV_INDEV_TYPE_ENCODER;
  indev_drv1.read_cb = button1_read;
  /*Register the driver in LVGL and save the created input device object*/
  lv_indev_t * my_indev1 = lv_indev_drv_register(&indev_drv1);
  // add hw button to group
  lv_indev_set_group(my_indev1, group);
  
  // HW button2
  static lv_indev_drv_t indev_drv2;
  lv_indev_drv_init(&indev_drv2);          /*Basic initialization*/
  indev_drv2.type = LV_INDEV_TYPE_ENCODER;
  indev_drv2.read_cb = button2_read;
  /*Register the driver in LVGL and save the created input device object*/
  lv_indev_t * my_indev2 = lv_indev_drv_register(&indev_drv2);
  // add hw button to group
  lv_indev_set_group(my_indev2, group);


  // update main content
  lv_obj_t *update_cont = lv_obj_create(parent);
  lv_obj_set_size(update_cont, LV_PCT(100), LV_PCT(100));
  lv_obj_set_style_bg_color(update_cont, lv_color_black(), 0);

  // label1  person
  lv_obj_t *label1 = lv_label_create(update_cont);
  lv_label_set_text(label1, "Person -> ");
  lv_obj_set_size(label1, LV_PCT(30), LV_PCT(20));
  lv_obj_set_pos(label1, LV_PCT(5), LV_PCT(4));
  lv_obj_set_style_text_align(label1, LV_TEXT_ALIGN_RIGHT, 0);
  lv_obj_set_style_text_color(label1, lv_color_hex(0xCCCCCC), 0);
  lv_obj_set_style_text_font(label1, &lv_font_montserrat_20, 0);

  // roller1 for person selection
  lv_obj_t *roller1 = lv_roller_create(update_cont);
  lv_roller_set_options(roller1, "No one\nHusband\nWife", LV_ROLLER_MODE_INFINITE);
  lv_roller_set_visible_row_count(roller1, 2);
  lv_obj_set_style_shadow_width(roller1, 10, LV_STATE_FOCUSED);
  lv_obj_set_style_shadow_color(roller1, lv_color_hex(0x006DFF), LV_STATE_FOCUSED);
  lv_obj_set_style_shadow_color(roller1, lv_color_hex(0xFF6DFF), LV_STATE_EDITED);
  lv_obj_set_style_shadow_spread(roller1, 10, LV_STATE_FOCUSED);
  lv_obj_set_size(roller1, LV_PCT(60), LV_PCT(20));
  lv_obj_set_pos(roller1, LV_PCT(35), LV_PCT(4));
  lv_obj_add_event_cb(roller1, person_event_handler, LV_EVENT_VALUE_CHANGED, NULL);

  // label2  add or minus
  lv_obj_t *label2 = lv_label_create(update_cont);
  lv_label_set_text(label2, "Operation -> ");
  lv_obj_set_size(label2, LV_PCT(30), LV_PCT(20));
  lv_obj_set_pos(label2, LV_PCT(5), LV_PCT(28));
  lv_obj_set_style_text_align(label2, LV_TEXT_ALIGN_RIGHT, 0);
  lv_obj_set_style_text_color(label2, lv_color_hex(0xCCCCCC), 0);
  lv_obj_set_style_text_font(label2, &lv_font_montserrat_20, 0);

  // roller2 for operation selection
  lv_obj_t *roller2 = lv_roller_create(update_cont);
  lv_roller_set_options(roller2, "Add\nMinus", LV_ROLLER_MODE_INFINITE);
  lv_roller_set_visible_row_count(roller2, 2);
  lv_obj_set_style_shadow_width(roller2, 10, LV_STATE_FOCUSED);
  lv_obj_set_style_shadow_color(roller2, lv_color_hex(0x006DFF), LV_STATE_FOCUSED);
  lv_obj_set_style_shadow_color(roller2, lv_color_hex(0xFF6DFF), LV_STATE_EDITED);
  lv_obj_set_style_shadow_spread(roller2, 10, LV_STATE_FOCUSED);
  lv_obj_set_size(roller2, LV_PCT(60), LV_PCT(20));
  lv_obj_set_pos(roller2, LV_PCT(35), LV_PCT(28));
  lv_obj_add_event_cb(roller2, op_event_handler, LV_EVENT_VALUE_CHANGED, NULL);


  // label3 what kind of housework
  lv_obj_t *label3 = lv_label_create(update_cont);
  lv_label_set_text(label3, "Housework -> ");
  lv_obj_set_size(label3, LV_PCT(30), LV_PCT(20));
  lv_obj_set_pos(label3, LV_PCT(5), LV_PCT(52));
  lv_obj_set_style_text_align(label3, LV_TEXT_ALIGN_RIGHT, 0);
  lv_obj_set_style_text_color(label3, lv_color_hex(0xCCCCCC), 0);
  lv_obj_set_style_text_font(label3, &lv_font_montserrat_20, 0);

  // roller3 for housework selection
  lv_obj_t *roller3 = lv_roller_create(update_cont);
  lv_roller_set_options(roller3, "Something\nCook\nLundary\nClean\nTrash", LV_ROLLER_MODE_INFINITE);
  lv_roller_set_visible_row_count(roller3, 2);
  lv_obj_set_style_shadow_width(roller3, 10, LV_STATE_FOCUSED);
  lv_obj_set_style_shadow_color(roller3, lv_color_hex(0x006DFF), LV_STATE_FOCUSED);
  lv_obj_set_style_shadow_color(roller3, lv_color_hex(0xFF6DFF), LV_STATE_EDITED);
  lv_obj_set_style_shadow_spread(roller3, 10, LV_STATE_FOCUSED);
  lv_obj_set_size(roller3, LV_PCT(60), LV_PCT(20));
  lv_obj_set_pos(roller3, LV_PCT(35), LV_PCT(52));
  lv_obj_add_event_cb(roller3, housework_event_handler, LV_EVENT_VALUE_CHANGED, NULL);


  // button1 for Update
  lv_obj_t * btn1 = lv_btn_create(update_cont);
  lv_obj_set_size(btn1, LV_PCT(40), LV_PCT(20));
  lv_obj_set_pos(btn1, LV_PCT(5), LV_PCT(76));
  lv_obj_set_style_shadow_width(btn1, 10, LV_STATE_FOCUSED);
  lv_obj_set_style_shadow_color(btn1, lv_color_hex(0x006DFF), LV_STATE_FOCUSED);
  lv_obj_set_style_shadow_spread(btn1, 10, LV_STATE_FOCUSED);
  lv_obj_add_event_cb(btn1, event_cb1, LV_EVENT_CLICKED, NULL);

  lv_obj_t * label_btn1 = lv_label_create(btn1);
  lv_label_set_text(label_btn1, "Update");
  lv_obj_center(label_btn1);

  // button2 for Clear
  lv_obj_t * btn2 = lv_btn_create(update_cont);
  lv_obj_set_size(btn2, LV_PCT(40), LV_PCT(20));
  lv_obj_set_pos(btn2, LV_PCT(55), LV_PCT(76));
  lv_obj_set_style_shadow_width(btn2, 10, LV_STATE_FOCUSED);
  lv_obj_set_style_shadow_color(btn2, lv_color_hex(0x006DFF), LV_STATE_FOCUSED);
  lv_obj_set_style_shadow_spread(btn2, 10, LV_STATE_FOCUSED);
  lv_obj_add_event_cb(btn2, event_cb2, LV_EVENT_CLICKED, NULL);

  lv_obj_t * label_btn2 = lv_label_create(btn2);
  lv_label_set_text(label_btn2, "Clear");
  lv_obj_center(label_btn2);

  lv_group_add_obj(group, roller1);
  lv_group_add_obj(group, roller2);
  lv_group_add_obj(group, roller3);
  lv_group_add_obj(group, btn1);
  lv_group_add_obj(group, btn2);
}

void historyUI(lv_obj_t *parent)
{
  // history content
  lv_obj_t *history_cont = lv_obj_create(parent);
  lv_obj_set_size(history_cont, LV_PCT(100), LV_PCT(100));
  lv_obj_set_style_bg_color(history_cont, lv_color_black(), 0);

  // history title
  lv_obj_t *history_title = lv_label_create(history_cont);
  lv_label_set_text(history_title, "Latest Histories:");
  lv_obj_set_style_text_align(history_title, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_set_style_text_color(history_title, lv_color_hex(COLOR_TEXT), 0);
  lv_obj_set_style_text_font(history_title, &lv_font_montserrat_28, 0);
  lv_obj_set_size(history_title, LV_PCT(100), LV_PCT(10));
  lv_obj_set_pos(history_title, LV_PCT(0), LV_PCT(10));

  // history 0 text
  history0_label = lv_label_create(history_cont);
  lv_label_set_text_fmt(history0_label, "%s %s %s", getDayName(histories[(idx+2)%HISTORY_LEN].who), getOpsName(histories[(idx+2)%HISTORY_LEN].did), getHouseworkName(histories[(idx+2)%HISTORY_LEN].housework));
  lv_obj_set_style_text_align(history0_label, LV_TEXT_ALIGN_LEFT, 0);
  lv_obj_set_style_text_color(history0_label, lv_color_hex(COLOR_TEXT), 0);
  lv_obj_set_style_text_font(history0_label, &lv_font_montserrat_22, 0);
  lv_obj_set_size(history0_label, LV_PCT(80), LV_PCT(10));
  lv_obj_set_pos(history0_label, LV_PCT(20), LV_PCT(40));

  // history 1 text
  history1_label = lv_label_create(history_cont);
  lv_label_set_text_fmt(history1_label, "%s %s %s", getDayName(histories[(idx+1)%HISTORY_LEN].who), getOpsName(histories[(idx+1)%HISTORY_LEN].did), getHouseworkName(histories[(idx+1)%HISTORY_LEN].housework));
  lv_obj_set_style_text_align(history1_label, LV_TEXT_ALIGN_LEFT, 0);
  lv_obj_set_style_text_color(history1_label, lv_color_hex(COLOR_TEXT), 0);
  lv_obj_set_style_text_font(history1_label, &lv_font_montserrat_22, 0);
  lv_obj_set_size(history1_label, LV_PCT(80), LV_PCT(10));
  lv_obj_set_pos(history1_label, LV_PCT(20), LV_PCT(60));

  // history 2 text
  history2_label = lv_label_create(history_cont);
  lv_label_set_text_fmt(history2_label, "%s %s %s", getDayName(histories[(idx)%HISTORY_LEN].who), getOpsName(histories[(idx)%HISTORY_LEN].did), getHouseworkName(histories[(idx)%HISTORY_LEN].housework));
  lv_obj_set_style_text_align(history2_label, LV_TEXT_ALIGN_LEFT, 0);
  lv_obj_set_style_text_color(history2_label, lv_color_hex(COLOR_TEXT), 0);
  lv_obj_set_style_text_font(history2_label, &lv_font_montserrat_22, 0);
  lv_obj_set_size(history2_label, LV_PCT(80), LV_PCT(10));
  lv_obj_set_pos(history2_label, LV_PCT(20), LV_PCT(80));

  lv_timer_create(update_history, 500, NULL);
}

void mainGUI(void)
{
  // main style
  static lv_style_t bgStyle;
  lv_style_init(&bgStyle);
  lv_style_set_bg_color(&bgStyle, lv_color_black());

  // tile view properties
  tileview = lv_tileview_create(lv_scr_act());
  lv_obj_add_style(tileview, &bgStyle, LV_PART_MAIN);
  lv_obj_set_size(tileview, LV_PCT(100), LV_PCT(100));
  lv_obj_set_scrollbar_mode(tileview, LV_SCROLLBAR_MODE_OFF);

  lv_obj_t *t1 = lv_tileview_add_tile(tileview, 0, 0, LV_DIR_HOR | LV_DIR_BOTTOM);
  lv_obj_t *t2 = lv_tileview_add_tile(tileview, 1, 0, LV_DIR_HOR | LV_DIR_BOTTOM);
  lv_obj_t *t3 = lv_tileview_add_tile(tileview, 2, 0, LV_DIR_HOR | LV_DIR_BOTTOM);

  dashBoardUI(t1);
  updateUI(t2);
  historyUI(t3);
  
  // go to first page by default
  lv_obj_set_tile_id(tileview, pageID, 0, LV_ANIM_ON);
}

void nextPage(void)
{
  pageID++;
  pageID %= 3;
  lv_obj_set_tile_id(tileview, pageID, 0, LV_ANIM_ON);
}

void appendNewHistory(void)
{
  histories[idx].who = global_case.who;
  histories[idx].did = global_case.did;
  histories[idx].housework = global_case.housework;
  idx += 1;
  idx %= HISTORY_LEN;
  if (global_case.did == ADD)
  {
    if (global_case.who == HUSBAND) husband += 1;
    else if (global_case.who == WIFE) wife += 1;
  }
  else {
    if (global_case.who == HUSBAND) husband -= 1;
    else if (global_case.who == WIFE) wife -= 1;
  }
}

void clearData(void)
{
  global_case.who = NOONE;
  global_case.did = ADD;
  global_case.housework = SOMETHING;
}