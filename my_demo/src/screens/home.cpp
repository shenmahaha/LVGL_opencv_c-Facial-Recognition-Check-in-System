#include "../../inc/home.h"
#include "../../lvgl/src/libs/freetype/lv_freetype.h"

extern mutex mtx;
TCPServer server(8080, "192.168.73.48"); // TODO
vector<Staff> staff_vec;            // 员工表
vector<SignInfo> signIn_vec;        // 今日签到记录
lv_font_t * font;
lv_obj_t * sign_list;
lv_obj_t * date_btn;                          // 日期按钮
lv_obj_t * date_btn_label;                    // 日期按钮上的标签
lv_obj_t * date_picker_container;             // 日期选择器容器
lv_obj_t * year_roller;                       // 年份滚轮
lv_obj_t * month_roller;                      // 月份滚轮
lv_obj_t * day_roller;                        // 日期滚轮
bool date_picker_visible = false;             // 日期选择器是否可见
vector<lv_obj_t *> signIn_label_vec;     // 存储签到时间标签的容器
vector<lv_obj_t *> signIn_btn_vec;       // 存储按钮的容器
vector<lv_obj_t *> signIn_btn_label_vec; // 存储按钮标签的容器
vector<lv_obj_t *> signIn_name_btn_vec;  // 存储姓名按钮的容器
char date_str[20];                            // 存储日期字符串
 char select_date_str[20]; // 存储选择的日期字符串
// TCP 服务端任务线程
void * server_task(void * arg)
{
    server.TCP_server_connect();
    return nullptr;
}

// 点击列表项后的回调函数，显示员工详细签到信息
void staff_item_click(lv_event_t * e)
{
    // 获取事件目标（即点击的按钮）
    lv_obj_t * btn = static_cast<lv_obj_t *>(lv_event_get_target(e));
    // 获取子类
    lv_obj_t * btn_label = lv_obj_get_child(btn, 0);
    // 获取存储在按钮中的用户数据（员工索引），并转换为 intptr_t 类型
    intptr_t index_ptr = reinterpret_cast<intptr_t>(lv_obj_get_user_data(btn));

    // 将 intptr_t 类型转换回 int
    int index = static_cast<int>(index_ptr);

    cout << signIn_vec[index].get_time().c_str() << endl;
    if(signIn_vec[index].get_time() == "未签到") {
        time_t now           = time(nullptr);
        struct tm * timeinfo = localtime(&now);
        char date_str[128];
        strftime(date_str, sizeof(date_str), "%Y-%m-%d %H:%M:%S", timeinfo);
        signIn_vec[index].set_time(date_str);
        cout << "签到成功" << endl;

        lv_obj_set_style_bg_color(btn, lv_color_hex(0xFF0000), 0);
        lv_label_set_text(signIn_label_vec[index], signIn_vec[index].get_time().substr(11, 8).c_str());
        lv_label_set_text(btn_label, "取消签到");
    } else {
        signIn_vec[index].set_time("未签到");
        cout << "取消签到成功" << endl;
        lv_obj_set_style_bg_color(btn, lv_color_hex(0x2196F3), 0);
        lv_label_set_text(signIn_label_vec[index], signIn_vec[index].get_time().c_str());
        lv_label_set_text(btn_label, "签到");
    }
}

// 日期按钮的事件回调函数
static void date_btn_event_handler(lv_event_t * e)
{
    if(date_picker_visible) {
        // 隐藏日期选择器容器
        lv_obj_add_flag(date_picker_container, LV_OBJ_FLAG_HIDDEN);
        date_picker_visible = false;
    } else {
        // 显示日期选择器容器
        lv_obj_clear_flag(date_picker_container, LV_OBJ_FLAG_HIDDEN);
        date_picker_visible = true;
    }
}

// 日期选择器确定按钮的事件回调函数
static void date_picker_confirm_event_handler(lv_event_t * e)
{
    // 获取用户选择的年份、月份和日期
    uint16_t year_sel  = lv_roller_get_selected(year_roller);
    uint16_t month_sel = lv_roller_get_selected(month_roller);
    uint16_t day_sel   = lv_roller_get_selected(day_roller);

    // 年份列表从起始年份开始
    int start_year     = 2020;
    int selected_year  = start_year + year_sel;
    int selected_month = month_sel + 1;
    int selected_day   = day_sel + 1;

    // 更新日期按钮上的标签
    sprintf(select_date_str, "%04d-%02d-%02d", selected_year, selected_month, selected_day);
    lv_label_set_text(date_btn_label, select_date_str);
    printf("选择的日期是：%s\n", select_date_str);

    // 隐藏日期选择器容器
    lv_obj_add_flag(date_picker_container, LV_OBJ_FLAG_HIDDEN);
    date_picker_visible = false;
    // TODO: 根据选择的年份、月份和日期更新签到记录
    for(int i = 0; i < signIn_name_btn_vec.size(); i++) {
        if(signIn_vec[i].get_date() == select_date_str) {
            lv_obj_clear_flag(signIn_name_btn_vec[i], LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(signIn_name_btn_vec[i], LV_OBJ_FLAG_HIDDEN);
        }
    }
}
// 月份滚轮的事件回调函数，用于更新日期滚轮的天数
static void month_roller_event_handler(lv_event_t * e)
{
    // 获取当前选择的年份和月份
    uint16_t year_sel  = lv_roller_get_selected(year_roller);
    uint16_t month_sel = lv_roller_get_selected(month_roller);

    int start_year     = 2020;
    int selected_year  = start_year + year_sel;
    int selected_month = month_sel + 1;

    // 根据年份和月份计算当月的天数
    int days_in_month = 31;
    if(selected_month == 2) {
        // 闰年判断
        if((selected_year % 4 == 0 && selected_year % 100 != 0) || (selected_year % 400 == 0)) {
            days_in_month = 29;
        } else {
            days_in_month = 28;
        }
    } else if(selected_month == 4 || selected_month == 6 || selected_month == 9 || selected_month == 11) {
        days_in_month = 30;
    }

    // 更新日期滚轮的选项
    std::string day_options;
    for(int i = 1; i <= days_in_month; ++i) {
        if(i < 10) day_options += "0";
        day_options += std::to_string(i);
        if(i != days_in_month) {
            day_options += "\n";
        }
    }
    lv_roller_set_options(day_roller, day_options.c_str(), LV_ROLLER_MODE_INFINITE);
}

void * update_sign_list_task(void * arg)
{
    while(1) {
        sleep(1);
        // 上锁
        mtx.lock();
        for(size_t i = 0; i < signIn_vec.size(); i++) {
            if(signIn_vec[i].get_time() == "未签到") {
                lv_label_set_text(signIn_label_vec[i], signIn_vec[i].get_time().c_str());
                lv_obj_set_style_bg_color(signIn_btn_vec[i], lv_color_hex(0x2196F3), 0);
                lv_label_set_text(signIn_btn_label_vec[i], "签到");
            } else {
                lv_label_set_text(signIn_label_vec[i], signIn_vec[i].get_time().substr(11, 8).c_str());
                lv_obj_set_style_bg_color(signIn_btn_vec[i], lv_color_hex(0xFF0000), 0);
                lv_label_set_text(signIn_btn_label_vec[i], "取消签到");
            }
        }
        mtx.unlock();
    }

    return nullptr;
}

// 创建首页显示列表的函数
void home_screen_init()
{
    // 手动添加员工，假设每个员工有姓名
    // staff_vec.push_back(Staff("1731565505", "刘杨"));
    staff_vec.push_back(Staff("1731565669", "张天举"));
    staff_vec.push_back(Staff("1731565650", "何金盛"));

    // 手动添加签到记录
     signIn_vec.push_back(SignInfo("2024-11-20", "1731565669", "张天举", "2024-11-20 09:01:41"));
    signIn_vec.push_back(SignInfo("2024-11-20", "1731565650", "何金盛", "未签到"));
    signIn_vec.push_back(SignInfo("2024-11-19", "1731565669", "张天举", "未签到"));
    signIn_vec.push_back(SignInfo("2024-11-19", "1731565650", "何金盛", "未签到"));
    signIn_vec.push_back(SignInfo("2024-11-18", "1731565669", "张天举", "2024-11-18 09:01:41"));
    signIn_vec.push_back(SignInfo("2024-11-18", "1731565650", "何金盛", "未签到"));


    font = lv_freetype_font_create("./my_demo/data/ttf/DingTalk-JinBuTi.ttf", LV_FREETYPE_FONT_RENDER_MODE_BITMAP, 25,
                                   LV_FREETYPE_FONT_STYLE_NORMAL);
    if(!font) {
        printf("Failed to create font\n");
    }

    // 创建一个线程
    pthread_t server_thread_id;
    pthread_create(&server_thread_id, nullptr, server_task, nullptr);

    // 获取当前屏幕
    lv_obj_t * scr = lv_scr_act();

    // 创建标题
    lv_obj_t * item_label = lv_label_create(scr);
    lv_obj_set_style_text_font(item_label, font, 0);
    lv_label_set_text(item_label, "员工考勤管理");
    lv_obj_align(item_label, LV_ALIGN_TOP_MID, 0, 10);

    // 创建员工列表
    sign_list = lv_list_create(scr);
    lv_obj_set_size(sign_list, LV_PCT(100), LV_PCT(80)); // 设置列表大小
    lv_obj_align(sign_list, LV_ALIGN_TOP_LEFT, 0, 40);   // 设置位置

    // 获取今天的日期
    time_t now           = time(nullptr);
    struct tm * timeinfo = localtime(&now);
    strftime(date_str, sizeof(date_str), "%Y-%m-%d", timeinfo);

    // 添加员工到列表
    for(size_t i = 0; (i < signIn_vec.size()); i++) {

        // 创建列表按钮，显示员工姓名、签到时间和签到状态
        lv_obj_t * list_btn = lv_list_add_btn(sign_list, nullptr, signIn_vec[i].get_name().c_str());
        signIn_name_btn_vec.push_back(list_btn);
        lv_obj_set_style_text_font(list_btn, font, 0);
        lv_obj_t * signIn_label = lv_label_create(list_btn);
        lv_obj_set_style_text_font(signIn_label, font, 0);
        // 设置按钮标签内容，显示员工姓名、签到时间
        if(signIn_vec[i].get_time() == "未签到") {
            lv_label_set_text(signIn_label, signIn_vec[i].get_time().c_str());
        } else {
            lv_label_set_text(signIn_label, signIn_vec[i].get_time().substr(11, 8).c_str());
        }
        signIn_label_vec.push_back(signIn_label);

        lv_obj_t * signIn_btn = lv_btn_create(list_btn);
        signIn_btn_vec.push_back(signIn_btn);
        lv_obj_set_size(signIn_btn, 120, 30);
        // lv_obj_align(edit_btn, LV_ALIGN_IN_BOTTOM_RIGHT, -10, -10);
        lv_obj_set_style_radius(signIn_btn, 10, 0);
        lv_obj_t * signIn_btn_label = lv_label_create(signIn_btn);
        signIn_btn_label_vec.push_back(signIn_btn_label);
        lv_obj_center(signIn_btn_label);

        if(signIn_vec[i].get_time() == "未签到") {
            lv_label_set_text(signIn_btn_label, "签到");
            // 浅红色按钮
        } else {
            lv_obj_set_style_bg_color(signIn_btn, lv_color_hex(0xFF0000), 0);
            lv_label_set_text(signIn_btn_label, "取消签到");
        }
        // 为按钮设置事件回调
        lv_obj_add_event_cb(signIn_btn, staff_item_click, LV_EVENT_CLICKED, nullptr);

        // 设置按钮的用户数据（员工索引）
        lv_obj_set_user_data(signIn_btn, reinterpret_cast<void *>(static_cast<intptr_t>(i)));
    }
    for(int i = 0; i < signIn_name_btn_vec.size(); i++) {
        // 隐藏标签
        if(signIn_vec[i].get_date() != date_str) {
            lv_obj_add_flag(signIn_name_btn_vec[i], LV_OBJ_FLAG_HIDDEN);
        }
    }
    // 创建一个线程
    pthread_t sign_list_thread_id;
    pthread_create(&sign_list_thread_id, nullptr, update_sign_list_task, nullptr);

    // 创建日期按钮
    date_btn = lv_btn_create(scr);
    lv_obj_set_size(date_btn, 200, 50);
    lv_obj_align(date_btn, LV_ALIGN_BOTTOM_MID, 0, -5);
    lv_obj_add_event_cb(date_btn, date_btn_event_handler, LV_EVENT_CLICKED, nullptr);

    // 创建日期按钮上的标签
    date_btn_label = lv_label_create(date_btn);
    lv_obj_set_style_text_font(date_btn_label, font, 0);
    lv_label_set_text(date_btn_label, date_str);
    lv_obj_center(date_btn_label);

    // 创建日期选择器容器，初始状态为隐藏
    date_picker_container = lv_obj_create(scr);
    lv_obj_set_size(date_picker_container, 280, 205);
    lv_obj_align(date_picker_container, LV_ALIGN_BOTTOM_MID, 0, -70);
    lv_obj_add_flag(date_picker_container, LV_OBJ_FLAG_HIDDEN);
    // 移除内边距
    lv_obj_set_style_pad_all(date_picker_container, 5, 0);

    // 创建年份滚轮
    year_roller = lv_roller_create(date_picker_container);
    lv_obj_set_size(year_roller, 100, 200);
    lv_obj_align(year_roller, LV_ALIGN_TOP_LEFT, 10, 5);

    // 设置年份选项
    int start_year = 2020;
    int end_year   = 2030;
    std::string year_options;
    for(int i = start_year; i <= end_year; ++i) {
        year_options += std::to_string(i);
        if(i != end_year) {
            year_options += "\n";
        }
    }
    lv_roller_set_options(year_roller, year_options.c_str(), LV_ROLLER_MODE_INFINITE);
    lv_roller_set_visible_row_count(year_roller, 4);
    lv_obj_set_style_text_font(year_roller, font, 0);

    // 创建月份滚轮
    month_roller = lv_roller_create(date_picker_container);
    lv_obj_set_size(month_roller, 60, 150);
    lv_obj_align(month_roller, LV_ALIGN_TOP_MID, 20, 5);

    // 设置月份选项
    const char * month_options = "01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12";
    lv_roller_set_options(month_roller, month_options, LV_ROLLER_MODE_INFINITE);
    lv_roller_set_visible_row_count(month_roller, 4);
    lv_obj_set_style_text_font(month_roller, font, 0);

    // 创建日期滚轮
    day_roller = lv_roller_create(date_picker_container);
    lv_obj_set_size(day_roller, 60, 150);
    lv_obj_align(day_roller, LV_ALIGN_TOP_RIGHT, -10, 5);

    // 设置日期选项（初始为当前月份的天数）
    int days_in_month = 31;
    std::string day_options;
    for(int i = 1; i <= days_in_month; ++i) {
        if(i < 10) day_options += "0";
        day_options += std::to_string(i);
        if(i != days_in_month) {
            day_options += "\n";
        }
    }
    lv_roller_set_options(day_roller, day_options.c_str(), LV_ROLLER_MODE_INFINITE);
    lv_roller_set_visible_row_count(day_roller, 4);
    lv_obj_set_style_text_font(day_roller, font, 0);

    // 设置滚轮初始值为当前日期
    lv_roller_set_selected(year_roller, timeinfo->tm_year + 1900 - start_year, LV_ANIM_OFF);
    lv_roller_set_selected(month_roller, timeinfo->tm_mon, LV_ANIM_OFF);
    lv_roller_set_selected(day_roller, timeinfo->tm_mday - 1, LV_ANIM_OFF);

    // 添加月份滚轮的事件回调，当月份变化时更新日期滚轮的天数
    lv_obj_add_event_cb(month_roller, month_roller_event_handler, LV_EVENT_VALUE_CHANGED, nullptr);
    // 添加年份滚轮的事件回调，当年份变化时更新日期滚轮的天数（处理闰年）
    lv_obj_add_event_cb(year_roller, month_roller_event_handler, LV_EVENT_VALUE_CHANGED, nullptr);

    // 创建确定按钮
    lv_obj_t * confirm_btn = lv_btn_create(date_picker_container);
    lv_obj_set_size(confirm_btn, 80, 40);
    lv_obj_align(confirm_btn, LV_ALIGN_BOTTOM_MID, 0, -5);
    lv_obj_add_event_cb(confirm_btn, date_picker_confirm_event_handler, LV_EVENT_CLICKED, nullptr);

    lv_obj_t * confirm_label = lv_label_create(confirm_btn);
    lv_label_set_text(confirm_label, "确定");
    lv_obj_set_style_text_font(confirm_label, font, 0);
    lv_obj_center(confirm_label);
}
