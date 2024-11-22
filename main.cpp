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

#include "./my_demo/inc/home.h"
using namespace std;
using namespace cv;
using namespace cv::face;

// static lv_obj_t * face;
mutex mtx;

#if LV_USE_LINUX_FBDEV
static void lv_linux_disp_init(void)
{
    const char * device = getenv_default("LV_LINUX_FBDEV_DEVICE", "/dev/fb0");
    lv_display_t * disp = lv_linux_fbdev_create();

    lv_linux_fbdev_set_file(disp, device);
}
#elif LV_USE_LINUX_DRM
static void lv_linux_disp_init(void)
{
    const char * device = getenv_default("LV_LINUX_DRM_CARD", "/dev/dri/card0");
    lv_display_t * disp = lv_linux_drm_create();

    lv_linux_drm_set_file(disp, device, -1);
}
#elif LV_USE_SDL
static void lv_linux_disp_init(void)
{
    const int width  = atoi(getenv("LV_SDL_VIDEO_WIDTH") ?: "800");
    const int height = atoi(getenv("LV_SDL_VIDEO_HEIGHT") ?: "480");

    lv_sdl_window_create(width, height);
}
#else
#error Unsupported configuration
#endif

#define SDL_INPUT 1
#define LINUX_INPUT 0

int main(void)
{
    lv_init();
    /*Linux display device init*/
    lv_linux_disp_init();

#if LINUX_INPUT
    // init LINUX_INPUT
    lv_indev_t * ts = lv_evdev_create(LV_INDEV_TYPE_POINTER, "/dev/input/event0");
    lv_evdev_set_calibration(ts, 0, 0, 1024, 600); // 黑色边框的屏幕校准
    // lv_evdev_set_calibration(ts,0,0,800,480);  //蓝色边框的屏幕校准
#endif

    // init SDL_INPUT
#if SDL_INPUT
    lv_sdl_mouse_create();
    lv_sdl_keyboard_create();
    lv_sdl_mousewheel_create();
    lv_sdl_mousewheel_create();
#endif

    home_screen_init();

    /*Handle LVGL tasks*/
    while(1) {
        mtx.lock();         // 手动锁定互斥锁
        lv_timer_handler(); // 刷新界面
        mtx.unlock();       // 手动解锁互斥锁
        usleep(5000);
    }
    return 0;
}
