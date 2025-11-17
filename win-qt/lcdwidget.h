#ifndef LCDWIDGET_H
#define LCDWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QImage>
#include <QTimer>
#include <QVBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QMutex>
#include <QMutexLocker>
#include <QKeyEvent>
#include <QSet>

class LCDWidget : public QWidget
{
    Q_OBJECT

private:
    explicit LCDWidget(QWidget *parent = nullptr, int w = 160, int h = 160, int z = 1);

public:
    ~LCDWidget();

    // 删除拷贝构造函数和赋值操作符
    LCDWidget(const LCDWidget&) = delete;
    LCDWidget& operator=(const LCDWidget&) = delete;

    // 获取单例实例
    static LCDWidget* getExistingInstance();

    // 初始化函数（用于在widget.cpp中显式初始化）
    static void initialize(QWidget *parent = nullptr, int w = 160, int h = 160, int z = 1);

    // 获取显示缓冲区的指针（线程安全）
    uint8_t* getDisplayBuffer();

    // 更新显示（线程安全）
    void updateDisplay();

    // 设置像素点（线程安全）
    void setPixel(int x, int y, bool on);

    // 获取像素点状态（线程安全）
    bool getPixel(int x, int y);

    // 清屏（线程安全）
    void clearScreen(bool on = false);

    // 绘制矩形边框（线程安全）
    void drawRect(int x, int y, int w, int h, bool on);

    // 填充矩形（线程安全）
    void fillRect(int x, int y, int w, int h, bool on);

    void draw_demo();

    // 按键相关方法
   bool isKeyPressed(int keyCode);           // 检查指定按键是否按下
   QSet<uint16_t> getPressedKeys();               // 获取所有按下的按键
   void clearKeyState(int keyCode);          // 清除指定按键状态
   void clearAllKeys();                      // 清除所有按键状态
   uint16_t readKey();

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;    // 按键按下事件
    void keyReleaseEvent(QKeyEvent *event) override;  // 按键释放事件
    void focusInEvent(QFocusEvent *event) override;   // 获得焦点事件
    void focusOutEvent(QFocusEvent *event) override;  // 失去焦点事件

private:
    static LCDWidget* instance;
    static bool initialized;

    uint8_t *displayBuffer;
    const int width;
    const int height;
    const int zoom;

    QMutex bufferMutex; // 用于保护显示缓冲区的互斥锁
    QMutex keyMutex;        // 用于保护按键状态的互斥锁
    QSet<uint16_t> pressedKeys;  // 存储当前按下的按键
    bool hasFocus;          // 标记窗口是否有焦点
};

#endif // LCDWIDGET_H
