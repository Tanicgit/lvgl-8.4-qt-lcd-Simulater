QT       += core gui
QT   += network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../lvgl/src/core/*.c \
    ../lvgl/src/draw/*.c \
    ../lvgl/src/draw/arm2d/*.c \
    ../lvgl/src/draw/nxp/pxp/*.c \
    ../lvgl/src/draw/nxp/vglite/*.c \
    ../lvgl/src/draw/renesas/*.c \
    ../lvgl/src/draw/sdl/*.c \
    ../lvgl/src/draw/stm32_dma2d/*.c \
    ../lvgl/src/draw/sw/*.c \
    ../lvgl/src/draw/swm341_dma2d/*.c \
    ../lvgl/src/extra/*.c \
    ../lvgl/src/extra/layouts/flex/*.c \
    ../lvgl/src/extra/layouts/grid/*.c \
    ../lvgl/src/extra/libs/bmp/*.c \
    ../lvgl/src/extra/libs/ffmpeg/*.c \
    ../lvgl/src/extra/libs/freetype/*.c \
    ../lvgl/src/extra/libs/fsdrv/*.c \
    ../lvgl/src/extra/libs/gif/*.c \
    ../lvgl/src/extra/libs/png/*.c \
    ../lvgl/src/extra/libs/qrcode/*.c \
    ../lvgl/src/extra/libs/rlottie/*.c \
    ../lvgl/src/extra/libs/sjpg/*.c \
    ../lvgl/src/extra/libs/tiny_ttf/*.c \
    ../lvgl/src/extra/others/fragment/*.c \
    ../lvgl/src/extra/others/gridnav/*.c \
    ../lvgl/src/extra/others/ime/*.c \
    ../lvgl/src/extra/others/imgfont/*.c \
    ../lvgl/src/extra/others/monkey/*.c \
    ../lvgl/src/extra/others/msg/*.c \
    ../lvgl/src/extra/others/snapshot/*.c \
    ../lvgl/src/extra/themes/basic/*.c \
    ../lvgl/src/extra/themes/default/*.c \
    ../lvgl/src/extra/themes/mono/*.c \
    ../lvgl/src/extra/widgets/animimg/*.c \
    ../lvgl/src/extra/widgets/calendar/*.c \
    ../lvgl/src/extra/widgets/chart/*.c \
    ../lvgl/src/extra/widgets/colorwheel/*.c \
    ../lvgl/src/extra/widgets/imgbtn/*.c \
    ../lvgl/src/extra/widgets/keyboard/*.c \
    ../lvgl/src/extra/widgets/led/*.c \
    ../lvgl/src/extra/widgets/list/*.c \
    ../lvgl/src/extra/widgets/menu/*.c \
    ../lvgl/src/extra/widgets/meter/*.c \
    ../lvgl/src/extra/widgets/msgbox/*.c \
    ../lvgl/src/extra/widgets/span/*.c \
    ../lvgl/src/extra/widgets/spinbox/*.c \
    ../lvgl/src/extra/widgets/spinner/*.c \
    ../lvgl/src/extra/widgets/tabview/*.c \
    ../lvgl/src/extra/widgets/tileview/*.c \
    ../lvgl/src/extra/widgets/win/*.c \
    ../lvgl/src/font/*.c \
    ../lvgl/src/hal/*.c \
    ../lvgl/src/misc/*.c \
    ../lvgl/src/widgets/*.c \
    ../ui/*.cpp \
    lv_port_indev.cpp \
    main.cpp \
    widget.cpp \
    lcdwidget.cpp \
    lv_port_disp.cpp


HEADERS += \
    lv_port_indev.h \
    widget.h\
    lcdwidget.h \
    lv_port_disp.h

INCLUDEPATH += ../ui
INCLUDEPATH += ../
FORMS += \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target








