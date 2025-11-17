
/*
应当设计为  
0.状态页面    长时间没有按键自动回到状态页面
1.主菜单页面
    分菜单1 ->页面1
    分菜单2 ->页面2
    ...
    分菜单n ->页面n

1.每一个页面创建一个组lv_group_t * group g; 页面中的可操作的控件加入这个组, 
切换页面的时候调用 lv_indev_set_group(g_indev_keypad, g)；切换按键聚焦组
懒得写demoe了
2.所有LVGL的API最好放到一个线程处理, 和别的进程数据通讯 把数据加锁即可，注意不要阻塞。
3.目前仅仅加了6键键盘，原则上可以把USB键盘加进来
4.lvgl中的文件不要动


*/



#include "lvgl/lvgl.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> 
#include <time.h>

#include "page_main.h"

#include <iostream>
#include <chrono>
#include <thread>
#include <atomic>
#include <functional>

class SimpleTimer {
private:
    std::atomic<bool> active_{false};
    std::thread worker_;
    
public:
    void start(int interval_ms, std::function<void()> callback) {
        active_ = true;
        worker_ = std::thread([this, interval_ms, callback]() {
            while (active_) {
                auto start = std::chrono::steady_clock::now();
                callback();
                auto end = std::chrono::steady_clock::now();
                
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
                auto sleep_time = std::chrono::milliseconds(interval_ms) - elapsed;
                
                if (sleep_time.count() > 0) {
                    std::this_thread::sleep_for(sleep_time);
                }
            }
        });
    }
    
    void stop() {
        active_ = false;
        if (worker_.joinable()) {
            worker_.join();
        }
    }
    
    ~SimpleTimer() {
        stop();
    }
};

// 使用示例
int main(int argc, char * argv[]) 
{
    lvgl_init();
    
    SimpleTimer timer;
    
    timer.start(50, []() {
        lv_timer_handler();
    });

    SimpleTimer timer2;
    
    timer2.start(10, []() {
        lv_tick_inc(10);
    });
    
    while(true)
    {
        // usleep(10000);
        // lv_timer_handler();
        // lv_tick_inc(10);
        sleep(1);
    }
}

