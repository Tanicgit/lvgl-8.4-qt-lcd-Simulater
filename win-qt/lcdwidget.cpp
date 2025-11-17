#include "LCDWidget.h"

#include <QDebug>
#include <QEventLoop>
#include <QTimer>

// 静态成员初始化
LCDWidget* LCDWidget::instance = nullptr;
bool LCDWidget::initialized = false;

LCDWidget::LCDWidget(QWidget *parent, int w, int h, int z)
    : QWidget(parent), width(w), height(h), zoom(z)
{
    // 分配显示缓冲区
    displayBuffer = new uint8_t[width * height / 8];
    memset(displayBuffer, 0, width * height / 8);

    setFixedSize(width * zoom, height * zoom);

    // 设置焦点策略，确保可以接收按键事件
    setFocusPolicy(Qt::StrongFocus);
}

LCDWidget::~LCDWidget()
{
    delete[] displayBuffer;
    instance = nullptr;
    initialized = false;
}

void LCDWidget::initialize(QWidget *parent, int w, int h, int z)
{
    if (!initialized) {
        instance = new LCDWidget(parent, w, h, z);
        initialized = true;
    }
}


LCDWidget* LCDWidget::getExistingInstance()
{
    return instance;
}

uint8_t* LCDWidget::getDisplayBuffer()
{

    return displayBuffer;
}

void LCDWidget::updateDisplay()
{

    //update();
    // 使用Qt的信号槽机制确保在主线程中更新UI
    //qDebug() << "updateDisplay";
    QMetaObject::invokeMethod(this, "update", Qt::QueuedConnection);
}

void LCDWidget::setPixel(int x, int y, bool on)
{

    if (x >= 0 && x < width && y >= 0 && y < height) {
        int byteIndex = (y / 8) * width + x;
        int bitPosition = y % 8;

        if (on) {
            displayBuffer[byteIndex] |= (1 << bitPosition);
        } else {
            displayBuffer[byteIndex] &= ~(1 << bitPosition);
        }
    }
}

bool LCDWidget::getPixel(int x, int y)
{

    if (x >= 0 && x < width && y >= 0 && y < height) {
        int byteIndex = (y / 8) * width + x;
        int bitPosition = y % 8;
        return (displayBuffer[byteIndex] >> bitPosition) & 0x01;
    }
    return false;
}

void LCDWidget::clearScreen(bool on)
{

    memset(displayBuffer, on ? 0xFF : 0x00, width * height / 8);
}

void LCDWidget::drawRect(int x, int y, int w, int h, bool on)
{

    for (int i = 0; i < w; i++) {
        setPixel(x + i, y, on);
        setPixel(x + i, y + h - 1, on);
    }
    for (int i = 0; i < h; i++) {
        setPixel(x, y + i, on);
        setPixel(x + w - 1, y + i, on);
    }
}

void LCDWidget::fillRect(int x, int y, int w, int h, bool on)
{

    for (int j = 0; j < h; j++) {
        for (int i = 0; i < w; i++) {
            setPixel(x + i, y + j, on);
        }
    }
}

void LCDWidget::draw_demo()
{

    int b = 0;
    for(int y = 0; y < height; y += 20) {
        int a = b;
        for(int x = 0; x < width; x += 20) {
            fillRect(x, y, 20, 20, a);
            a = !a;
        }
        b = !b;
    }
    updateDisplay();
}

void LCDWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, false);

    QImage image(width, height, QImage::Format_RGB32);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            bool pixelOn = getPixel(x, y);
            image.setPixel(x, y, pixelOn ? qRgb(255, 255, 255) : qRgb(0, 0, 0));
        }
    }

    painter.drawImage(rect(), image);
    qDebug() << "Painting completed - Image size:" << image.size() << "Widget size:" << size();
}

// ==================== 按键相关方法实现 ====================

bool LCDWidget::isKeyPressed(int keyCode)
{
    QMutexLocker locker(&keyMutex);
    return pressedKeys.contains(keyCode);
}

QSet<uint16_t> LCDWidget::getPressedKeys()
{
    QMutexLocker locker(&keyMutex);
    return pressedKeys;
}

void LCDWidget::clearKeyState(int keyCode)
{
    QMutexLocker locker(&keyMutex);
    pressedKeys.remove(keyCode);
}

void LCDWidget::clearAllKeys()
{
    QMutexLocker locker(&keyMutex);
    pressedKeys.clear();
}

uint16_t LCDWidget::readKey()
{
    QSet<uint16_t> Keys = getPressedKeys();

    auto code = Keys.begin();
    if(code!=Keys.end())
    {
        return *code;
    }

    return 0;
}


void LCDWidget::keyPressEvent(QKeyEvent *event)
{
    int keyCode = event->key();

    {
        QMutexLocker locker(&keyMutex);
        pressedKeys.insert(keyCode);
    }
    event->accept();  // 标记事件已处理
}

void LCDWidget::keyReleaseEvent(QKeyEvent *event)
{
    int keyCode = event->key();

    {
        QMutexLocker locker(&keyMutex);
        pressedKeys.remove(keyCode);
    }
    event->accept();  // 标记事件已处理
}

void LCDWidget::focusInEvent(QFocusEvent *event)
{
    hasFocus = true;
    QWidget::focusInEvent(event);
}

void LCDWidget::focusOutEvent(QFocusEvent *event)
{
    hasFocus = false;
    // 失去焦点时清除所有按键状态
    clearAllKeys();
    QWidget::focusOutEvent(event);
}

