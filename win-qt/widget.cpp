#include "widget.h"
#include "ui_widget.h"
#include <QVBoxLayout>
#include <QTimer>
#include <QDebug>
#include "lvgl/lvgl.h"
#include "lv_port_disp.h"


#include "page_main.h"
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    LCDWidget::initialize(this,MY_DISP_HOR_RES,MY_DISP_VER_RES,3);

    LCDWidget* lcd = LCDWidget::getExistingInstance();

    //lcd->draw_demo();

    resize(lcd->size());

//    lcd->clearScreen();
//    lcd->fillRect(0,0,100,100,true);

    //lcd->setPixel(10,10,true);

    lvgl_init();

    // 创建定时器在UI线程中处理LVGL
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, []() {
        lv_timer_handler();

    });
    timer->start(50); // 每50ms处理一次


    timer2 = new QTimer(this);
    connect(timer2, &QTimer::timeout, this, []() {
        lv_tick_inc(10);
    });
    timer2->start(10); // 每50ms处理一次
}

Widget::~Widget()
{
    delete ui;
}


