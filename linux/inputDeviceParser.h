#ifndef INPUT_DEVICE_PARSER_H   
#define INPUT_DEVICE_PARSER_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <algorithm>
#include <cctype>

class InputDeviceParser {
public:
    // 设备信息结构体
    struct InputDevice {
        std::string id;          // I: 行信息
        std::string name;        // N: 名称
        std::string phys;        // P: 物理路径
        std::string sysfs;       // S: Sysfs路径
        std::string uniq;        // U: 唯一标识
        std::string handlers;    // H: 处理器（包含eventX信息）
        std::string properties;  // B: PROP
        std::string events;      // B: EV
        std::string keys;        // B: KEY
        std::string misc;        // B: MSC
        std::string leds;        // B: LED
        
        // 获取event编号（从handlers中提取）
        std::string getEventNumber() const {
            size_t pos = handlers.find("event");
            if (pos != std::string::npos) {
                size_t start = pos + 5; // "event"的长度是5
                size_t end = handlers.find(' ', start);
                if (end == std::string::npos) {
                    end = handlers.length();
                }
                return handlers.substr(start, end - start);
            }
            return "";
        }
        
        // 判断是否是特定类型的设备
        bool isKeyboard() const {
            return name.find("keyboard") != std::string::npos || 
                   name.find("Keyboard") != std::string::npos;
        }
        
        bool isMouse() const {
            return name.find("mouse") != std::string::npos || 
                   name.find("Mouse") != std::string::npos ||
                   handlers.find("mouse") != std::string::npos;
        }
        
        bool isTouchpad() const {
            return name.find("touchpad") != std::string::npos || 
                   name.find("TouchPad") != std::string::npos;
        }
    };

private:
    std::vector<InputDevice> devices;
    std::string filename;

public:
    // 构造函数
    InputDeviceParser(const std::string& file = "/proc/bus/input/devices") 
        : filename(file) {}
    
    // 解析文件
    bool parse() {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open file " << filename << std::endl;
            return false;
        }
        
        devices.clear();
        InputDevice currentDevice;
        std::string line;
        
        while (std::getline(file, line)) {
            if (line.empty()) {
                // 空行表示一个设备信息结束
                if (!currentDevice.id.empty()) {
                    devices.push_back(currentDevice);
                    currentDevice = InputDevice();
                }
                continue;
            }
            
            // 解析每行的前缀
            if (line.length() > 2 && line[1] == ':') {
                char prefix = line[0];
                std::string value = line.substr(3); // 跳过 "X: "
                
                switch (prefix) {
                    case 'I': currentDevice.id = value; break;
                    case 'N': 
                        // 移除Name=后面的引号
                        if (value.find("Name=") == 0) {
                            currentDevice.name = value.substr(5);
                            // 移除可能的引号
                            if (!currentDevice.name.empty() && currentDevice.name.front() == '"') {
                                currentDevice.name = currentDevice.name.substr(1);
                            }
                            if (!currentDevice.name.empty() && currentDevice.name.back() == '"') {
                                currentDevice.name.pop_back();
                            }
                        }
                        break;
                    case 'P': 
                        if (value.find("Phys=") == 0) {
                            currentDevice.phys = value.substr(5);
                        }
                        break;
                    case 'S': 
                        if (value.find("Sysfs=") == 0) {
                            currentDevice.sysfs = value.substr(6);
                        }
                        break;
                    case 'U': 
                        if (value.find("Uniq=") == 0) {
                            currentDevice.uniq = value.substr(5);
                        }
                        break;
                    case 'H': 
                        if (value.find("Handlers=") == 0) {
                            currentDevice.handlers = value.substr(9);
                        }
                        break;
                    case 'B':
                        // 处理不同类型的B:行
                        if (value.find("PROP=") == 0) {
                            currentDevice.properties = value.substr(5);
                        } else if (value.find("EV=") == 0) {
                            currentDevice.events = value.substr(3);
                        } else if (value.find("KEY=") == 0) {
                            currentDevice.keys = value.substr(4);
                        } else if (value.find("MSC=") == 0) {
                            currentDevice.misc = value.substr(4);
                        } else if (value.find("LED=") == 0) {
                            currentDevice.leds = value.substr(4);
                        }
                        break;
                }
            }
        }
        
        // 添加最后一个设备
        if (!currentDevice.id.empty()) {
            devices.push_back(currentDevice);
        }
        
        file.close();
        return true;
    }
    
    // 获取所有设备
    const std::vector<InputDevice>& getDevices() const {
        return devices;
    }
    
    // 根据设备类型过滤设备
    std::vector<InputDevice> getDevicesByType(const std::string& type) const {
        std::vector<InputDevice> result;
        for (const auto& device : devices) {
            if (device.name.find(type) != std::string::npos) {
                result.push_back(device);
            }
        }
        return result;
    }
    
    // 获取键盘设备
    std::vector<InputDevice> getKeyboards() const {
        std::vector<InputDevice> result;
        for (const auto& device : devices) {
            if (device.isKeyboard()) {
                result.push_back(device);
            }
        }
        return result;
    }
    
    // 获取鼠标设备
    std::vector<InputDevice> getMice() const {
        std::vector<InputDevice> result;
        for (const auto& device : devices) {
            if (device.isMouse()) {
                result.push_back(device);
            }
        }
        return result;
    }
    
    // 获取触摸板设备
    std::vector<InputDevice> getTouchpads() const {
        std::vector<InputDevice> result;
        for (const auto& device : devices) {
            if (device.isTouchpad()) {
                result.push_back(device);
            }
        }
        return result;
    }
    
    // 根据event编号查找设备
    InputDevice getDeviceByEventNumber(const std::string& eventNum) const {
        for (const auto& device : devices) {
            if (device.getEventNumber() == eventNum) {
                return device;
            }
        }
        return InputDevice();
    }
    
    // 根据设备名称查找设备
    InputDevice getDeviceByName(const std::string& name) const {
        for (const auto& device : devices) {
            if (device.name == name) {
                return device;
            }
        }
        return InputDevice();
    }
    
    // 打印所有设备信息
    void printAllDevices() const {
        std::cout << "Found " << devices.size() << " input devices:\n";
        std::cout << "=============================================\n";
        
        for (size_t i = 0; i < devices.size(); ++i) {
            const auto& device = devices[i];
            std::cout << "Device " << i + 1 << ":\n";
            std::cout << "  Event: event" << device.getEventNumber() << "\n";
            std::cout << "  Name: " << device.name << "\n";
            std::cout << "  Physical: " << device.phys << "\n";
            std::cout << "  Sysfs: " << device.sysfs << "\n";
            std::cout << "  Handlers: " << device.handlers << "\n";
            std::cout << "  Events: " << device.events << "\n";
            std::cout << "  Keys: " << device.keys << "\n";
            std::cout << "---------------------------------------------\n";
        }
    }
    
    // 打印特定类型的设备
    void printDevicesByType(const std::string& type) const {
        auto filtered = getDevicesByType(type);
        std::cout << "Found " << filtered.size() << " " << type << " devices:\n";
        for (const auto& device : filtered) {
            std::cout << "  " << device.name << " (event" << device.getEventNumber() << ")\n";
        }
    }
};


#endif // INPUT_DEVICE_PARSER_H