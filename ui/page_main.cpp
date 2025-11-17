/**
这里就是跨平台UI编辑处, 这里的函数都是处于一个线程中
可以创建多个UI定时器, 用于刷新UI,  记住不可阻塞
定时器本质还是 lv_timer_handler 来的。

如果要通过通讯拿数据，那么应该在别的线程把数据准备好(枷锁),这里只做取数据显示的活，千万不要和别的进程通讯导致阻塞

*/

#include <chrono>
#include <iomanip>
#include <sstream>


#include "page_main.h"

#include "lvgl/lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"


#ifdef _WIN32
    #include <QDebug>
#endif



// 菜单相关变量
static lv_obj_t * menu_cont;
static lv_obj_t * selected_bg;
static int selected_index = 0;
static const int total_items = 6;

static const char * menu_texts[] = {
    "SYSTEM", "NETWORK", "DISPLAY",
    "AUDIO", "TIME", "ABOUT"
};

// 字体定义 - 使用更适合黑白屏的字体
LV_FONT_DECLARE(lv_font_montserrat_12);

// 尺寸常量
static const int SCREEN_WIDTH = 160;
static const int SCREEN_HEIGHT = 160;
static const int MENU_WIDTH = 140;
static const int MENU_HEIGHT = 100;
static const int ITEM_HEIGHT = 22;
static const int TITLE_BAR_HEIGHT = 24;
static const int STATUS_BAR_HEIGHT = 20;

// 颜色定义 - 黑白屏专用
static const lv_color_t COLOR_WHITE = LV_COLOR_MAKE(0xFF, 0xFF, 0xFF);
static const lv_color_t COLOR_BLACK = LV_COLOR_MAKE(0x00, 0x00, 0x00);
static const lv_color_t COLOR_GRAY = LV_COLOR_MAKE(0x80, 0x80, 0x80);

// 函数声明
static void update_selection(int new_index);
static void menu_item_selected(int index);
static void key_event_handler(lv_event_t * e);

// 在切换页面的函数中

/**
todo 添加新页面的对象到组  定义对应的页面句柄,调用对应的函数, 应当封装一个base 类
*/
void switch_to_new_page(lv_obj_t * new_page) {
    lv_group_t * group = lv_port_indev_get_group();
    if (group) {
        // 清空组中所有对象
        lv_group_remove_all_objs(group);

        //
        //lv_group_add_obj(group, new_page);
        // 设置焦点到新对象
        //lv_group_focus_obj(new_page);
    }

    // 显示新页面，隐藏旧页面等操作...


}

/* 创建菜单容器 */
static void create_menu_container(void)
{
    // 创建菜单容器 - 居中显示
    menu_cont = lv_obj_create(lv_scr_act());
    lv_obj_set_size(menu_cont, MENU_WIDTH, MENU_HEIGHT);
    lv_obj_align(menu_cont, LV_ALIGN_CENTER, 0, 0);

    // 设置样式 - 黑白屏优化
    lv_obj_set_style_border_width(menu_cont, 2, 0);
    lv_obj_set_style_border_color(menu_cont, COLOR_BLACK, 0);
    lv_obj_set_style_bg_color(menu_cont, COLOR_WHITE, 0);
    lv_obj_set_style_radius(menu_cont, 0, 0); // 直角更适合黑白屏
    lv_obj_set_style_pad_all(menu_cont, 2, 0);
    //lv_obj_set_scroll_snap_y(menu_cont, LV_SCROLL_SNAP_CENTER); // 添加这个可以让滚动更平滑

    // 滚动设置 - LVGL 8.4 API
    lv_obj_set_scrollbar_mode(menu_cont, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(menu_cont, LV_DIR_VER);

    // 禁用左右键的滚动功能
    lv_obj_clear_flag(menu_cont, LV_OBJ_FLAG_SCROLL_WITH_ARROW);

    // 选中背景 - 使用反色效果
    selected_bg = lv_obj_create(menu_cont);
    lv_obj_set_size(selected_bg, MENU_WIDTH - 4, ITEM_HEIGHT);
    lv_obj_set_style_bg_color(selected_bg, COLOR_BLACK, 0);
    lv_obj_set_style_bg_opa(selected_bg, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(selected_bg, 0, 0);
    lv_obj_set_style_radius(selected_bg, 0, 0);
    lv_obj_clear_flag(selected_bg, LV_OBJ_FLAG_SCROLLABLE);

    // 修复：确保菜单容器可以接收焦点和按键事件
   lv_obj_add_flag(menu_cont, LV_OBJ_FLAG_CLICKABLE);
   lv_obj_add_flag(menu_cont, LV_OBJ_FLAG_SCROLLABLE);
   lv_obj_clear_flag(menu_cont, LV_OBJ_FLAG_GESTURE_BUBBLE);

   //将按键事件绑定到菜单容器而不是屏幕
   lv_obj_add_event_cb(menu_cont, key_event_handler, LV_EVENT_KEY, NULL);

   // 确保菜单容器获得焦点
   lv_group_t * group = lv_port_indev_get_group();
   if (group) {
       lv_group_add_obj(group,menu_cont);
       lv_group_focus_obj(menu_cont);
   }
}

/* 创建菜单项 */
static void create_menu_items(void)
{
    for(int i = 0; i < total_items; i++) {
        lv_obj_t * label = lv_label_create(menu_cont);
        lv_label_set_text(label, menu_texts[i]);
        lv_obj_set_size(label, MENU_WIDTH - 8, ITEM_HEIGHT);
        lv_obj_set_pos(label, 4, i * ITEM_HEIGHT);

        lv_obj_set_style_text_color(label, COLOR_BLACK, 0);
        lv_obj_set_style_text_font(label, &lv_font_montserrat_12, 0);
        lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_style_pad_top(label, 4, 0); // 垂直居中
    }
    update_selection(0);
}

/* 更新选中项 */
static void update_selection(int new_index)
{
    if(new_index < 0) new_index = 0;
    if(new_index >= total_items) new_index = total_items - 1;

    selected_index = new_index;
    lv_obj_set_y(selected_bg, selected_index * ITEM_HEIGHT);

    // 自动滚动 - LVGL 8.4 API
    int scroll_y = selected_index * ITEM_HEIGHT;
    lv_obj_scroll_to_y(menu_cont, scroll_y, LV_ANIM_ON);

    // 更新文字颜色 - 黑白反色
    for(int i = 0; i < total_items; i++) {
        lv_obj_t * label = lv_obj_get_child(menu_cont, i + 1);  // +1 因为selected_bg是第一个子对象
        if (label && lv_obj_check_type(label, &lv_label_class)) {
            lv_obj_set_style_text_color(label,
                (i == selected_index) ? COLOR_WHITE : COLOR_BLACK, 0);
        }
    }
}

/* 按键事件处理 - LVGL 8.4 API */
static void key_event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_KEY) {
        uint32_t key = lv_indev_get_key(lv_indev_get_act());

        switch(key) {
            case LV_KEY_UP:
                update_selection(selected_index - 1);
                //lv_obj_scroll_to_y(menu_cont, selected_index * ITEM_HEIGHT, LV_ANIM_ON);
                #ifdef _WIN32
                    qDebug() << "UP pressed, selection:" << selected_index;
                #endif

                break;
            case LV_KEY_DOWN:
                update_selection(selected_index + 1);
                //lv_obj_scroll_to_y(menu_cont, selected_index * ITEM_HEIGHT, LV_ANIM_ON);
                #ifdef _WIN32
                qDebug() << "DOWN pressed, selection:" << selected_index;
                #endif
                break;
            case LV_KEY_LEFT:
            case LV_KEY_RIGHT:
                // 明确忽略左右键，不执行任何操作
                #ifdef _WIN32
                qDebug() << "LEFT/RIGHT pressed - ignored";
                #endif
                break;
            case LV_KEY_ENTER:
                menu_item_selected(selected_index);
                #ifdef _WIN32
                qDebug() << "ENTER pressed, selected:" << menu_texts[selected_index];
                #endif
                break;

            default:
                    // 忽略其他按键
                break;
        }
    }
}

/* 菜单项选中处理 */
static void menu_item_selected(int index)
{
    #ifdef _WIN32
    qDebug() << "Menu item selected:" << index << "-" << menu_texts[index];
    #endif
    // 创建简单的反馈效果 - 黑白屏优化
    lv_obj_t * msg_label = lv_label_create(lv_scr_act());
    lv_label_set_text_fmt(msg_label, "Selected: %s", menu_texts[index]);
    lv_obj_set_style_text_color(msg_label, COLOR_WHITE, 0);
    lv_obj_set_style_bg_color(msg_label, COLOR_BLACK, 0);
    lv_obj_set_style_bg_opa(msg_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(msg_label, &lv_font_montserrat_12, 0);
    lv_obj_set_style_border_width(msg_label, 2, 0);
    lv_obj_set_style_border_color(msg_label, COLOR_BLACK, 0);
    lv_obj_set_size(msg_label, SCREEN_WIDTH - 20, 30);
    lv_obj_align(msg_label, LV_ALIGN_TOP_MID, 0, TITLE_BAR_HEIGHT + 10);

    // 2秒后自动删除提示
    lv_timer_t * timer = lv_timer_create([](lv_timer_t * timer) {
        lv_obj_t * label = (lv_obj_t *)timer->user_data;
        lv_obj_del(label);
        lv_timer_del(timer);
    }, 1000, msg_label);
    lv_timer_set_repeat_count(timer, 1);
}

/* 创建标题栏 */
static void create_title_bar(void)
{
    lv_obj_t * title_bar = lv_obj_create(lv_scr_act());
    lv_obj_set_size(title_bar, SCREEN_WIDTH, TITLE_BAR_HEIGHT);
    lv_obj_align(title_bar, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_bg_color(title_bar, COLOR_BLACK, 0);
    lv_obj_set_style_border_width(title_bar, 0, 0);
    lv_obj_set_style_radius(title_bar, 0, 0);

    lv_obj_t * title_label = lv_label_create(title_bar);
    lv_label_set_text(title_label, "MAIN MENU");
    lv_obj_set_style_text_color(title_label, COLOR_WHITE, 0);
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_12, 0);
    lv_obj_center(title_label);
}

/* 创建状态栏 */
static void create_status_bar(void)
{
    lv_obj_t * status_bar = lv_obj_create(lv_scr_act());
    lv_obj_set_size(status_bar, SCREEN_WIDTH, STATUS_BAR_HEIGHT);
    lv_obj_align(status_bar, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_color(status_bar, COLOR_WHITE, 0);
    lv_obj_set_style_border_width(status_bar, 1, 0);
    lv_obj_set_style_border_color(status_bar, COLOR_BLACK, 0);
    lv_obj_set_style_radius(status_bar, 0, 0);

    // 添加简单的状态信息
    lv_obj_t * status_label = lv_label_create(status_bar);
    lv_label_set_text(status_label, "Use UP/DOWN + ENTER");
    lv_obj_set_style_text_color(status_label, COLOR_BLACK, 0);
    lv_obj_set_style_text_font(status_label, &lv_font_montserrat_12, 0);
    lv_obj_center(status_label);
}

/* 创建主界面 */
void create_ui(void)
{
    /* 获取当前屏幕 */
    lv_obj_t * scr = lv_scr_act();

    // 设置屏幕背景色
    lv_obj_set_style_bg_color(scr, COLOR_WHITE, 0);
    lv_obj_set_size(scr, SCREEN_WIDTH, SCREEN_HEIGHT);

    // 创建界面元素
    create_title_bar();
    create_status_bar();
    create_menu_container();
    create_menu_items();


}

void lvgl_init()
{
    lv_init();
    lv_port_disp_init();
    lv_port_indev_init();
    create_ui();
    #ifdef _WIN32
    qDebug() << "LVGL 8.4 Menu initialized successfully for 160x160 B&W screen";
    #endif
}
