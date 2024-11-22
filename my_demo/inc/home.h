#ifndef HOME_H // 防止重复包含
#define HOME_H

extern "C" {
#include "lvgl/lvgl.h"
#include "lvgl/demos/lv_demos.h"
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
}

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <opencv2/face.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <lvgl/lvgl.h>
#include <thread>
#include <chrono>
#include <queue>
#include <mutex>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include "staff.h"
#include "TCP_server.h"
#include "signInfo.h"

using namespace std;
using namespace cv;
using namespace cv::face;

extern vector<Staff> staff_vec;
extern vector<SignInfo> signIn_vec;

extern lv_font_t * font;
extern lv_obj_t * sign_list;
extern vector<lv_obj_t *> signIn_label_vec;     // 存储签到时间标签的容器
extern vector<lv_obj_t *> signIn_btn_vec;       // 存储按钮的容器
extern vector<lv_obj_t *> signIn_btn_label_vec; // 存储按钮标签的容器
extern vector<lv_obj_t *> signIn_name_btn_vec;  // 存储姓名按钮的容器

extern void staff_item_click(lv_event_t * e);

extern void home_screen_init();
extern TCPServer server;

#endif 