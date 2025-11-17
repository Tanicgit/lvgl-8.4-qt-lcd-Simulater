#ifndef KEYBOARD_READER_H
#define KEYBOARD_READER_H

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>
#include <cstring>
#include <string>
#include <map>

class KeyboardReader {
private:
    int fd;
    std::string devicePath;
    std::map<int, std::string> keyMap;
	bool is_block;

public:
    KeyboardReader(const std::string& path = "/dev/input/event0",bool block=false) : devicePath(path),is_block(block), fd(-1) {
        initializeKeyMap();
    }

    ~KeyboardReader() {
        closeDevice();
    }

    // 初始化键值映射
    void initializeKeyMap() {
        keyMap = {
            {KEY_ESC, "ESC"},
            {KEY_1, "1"}, {KEY_2, "2"}, {KEY_3, "3"}, {KEY_4, "4"}, {KEY_5, "5"},
            {KEY_6, "6"}, {KEY_7, "7"}, {KEY_8, "8"}, {KEY_9, "9"}, {KEY_0, "0"},
            {KEY_MINUS, "-"}, {KEY_EQUAL, "="}, {KEY_BACKSPACE, "Backspace"},
            {KEY_TAB, "Tab"}, {KEY_Q, "Q"}, {KEY_W, "W"}, {KEY_E, "E"}, {KEY_R, "R"},
            {KEY_T, "T"}, {KEY_Y, "Y"}, {KEY_U, "U"}, {KEY_I, "I"}, {KEY_O, "O"},
            {KEY_P, "P"}, {KEY_LEFTBRACE, "["}, {KEY_RIGHTBRACE, "]"}, {KEY_ENTER, "Enter"},
            {KEY_LEFTCTRL, "Left Ctrl"}, {KEY_A, "A"}, {KEY_S, "S"}, {KEY_D, "D"},
            {KEY_F, "F"}, {KEY_G, "G"}, {KEY_H, "H"}, {KEY_J, "J"}, {KEY_K, "K"},
            {KEY_L, "L"}, {KEY_SEMICOLON, ";"}, {KEY_APOSTROPHE, "'"}, {KEY_GRAVE, "`"},
            {KEY_LEFTSHIFT, "Left Shift"}, {KEY_BACKSLASH, "\\"}, {KEY_Z, "Z"},
            {KEY_X, "X"}, {KEY_C, "C"}, {KEY_V, "V"}, {KEY_B, "B"}, {KEY_N, "N"},
            {KEY_M, "M"}, {KEY_COMMA, ","}, {KEY_DOT, "."}, {KEY_SLASH, "/"},
            {KEY_RIGHTSHIFT, "Right Shift"}, {KEY_LEFTALT, "Left Alt"}, {KEY_SPACE, "Space"},
            {KEY_CAPSLOCK, "Caps Lock"}, {KEY_F1, "F1"}, {KEY_F2, "F2"}, {KEY_F3, "F3"},
            {KEY_F4, "F4"}, {KEY_F5, "F5"}, {KEY_F6, "F6"}, {KEY_F7, "F7"}, {KEY_F8, "F8"},
            {KEY_F9, "F9"}, {KEY_F10, "F10"}, {KEY_F11, "F11"}, {KEY_F12, "F12"},
            {KEY_RIGHTCTRL, "Right Ctrl"}, {KEY_RIGHTALT, "Right Alt"},
            {KEY_UP, "Up Arrow"}, {KEY_LEFT, "Left Arrow"},
            {KEY_RIGHT, "Right Arrow"}, {KEY_DOWN, "Down Arrow"}
        };
    }

    // 打开设备
    bool openDevice() {
        if(is_block)
            fd = open(devicePath.c_str(), O_RDONLY);  
        else
            fd = open(devicePath.c_str(), O_RDONLY|O_NONBLOCK);  

        if (fd < 0) {
            std::cerr << "Error: Cannot open device " << devicePath << ": " << strerror(errno) << std::endl;
            return false;
        }
        
        // 检查设备是否是键盘
        unsigned long evbit;
        if (ioctl(fd, EVIOCGBIT(0, sizeof(evbit)), &evbit) < 0) {
            std::cerr << "Error: Cannot get event bits" << std::endl;
            closeDevice();
            return false;
        }

        if (!(evbit & (1 << EV_KEY))) {
            std::cerr << "Error: Device does not support key events" << std::endl;
            closeDevice();
            return false;
        }

        std::cout << "Successfully opened keyboard device: " << devicePath << std::endl;
        return true;
    }

    // 关闭设备
    void closeDevice() {
        if (fd >= 0) {
            close(fd);
            fd = -1;
        }
    }

    // 读取按键事件
    uint32_t readKeys() {
        if (fd < 0) {
            std::cerr << "Device not opened" << std::endl;
            return 0;
        }

        

        struct input_event ev;
            
        ssize_t bytes = read(fd, &ev, sizeof(ev));
        if(bytes==-1)
        {
            if(errno == EAGAIN)//没有事件
            {
                return 0;
            }
            else
            {
                return 0;//错误
            }
        }

        else if (bytes != sizeof(ev)) {
            std::cerr << "Read error" << std::endl;
            return 0;//事件不完整
        } else {
            
        }

        if (ev.type == EV_KEY) {

            // processKeyEvent(ev);
            // if(ev.value == 0)
            // {
            //     return ev.code;
            // }

            if(ev.value == 1)
            {
                return ev.code;
            }

        }
            
		return 0;
        
    }

    // 处理按键事件
    void processKeyEvent(const input_event& ev) {
        std::string keyName = "Unknown";
        if (keyMap.find(ev.code) != keyMap.end()) {
            keyName = keyMap[ev.code];
        }

        std::string state;
        switch (ev.value) {
            case 0: state = "RELEASED"; break;
            case 1: state = "PRESSED"; break;
            case 2: state = "REPEAT"; break;
            default: state = "UNKNOWN"; break;
        }

        std::cout << "Key: " << keyName << " (" << ev.code << ") - " << state << std::endl;
    }

};

#endif // KEYBOARD_READER_H