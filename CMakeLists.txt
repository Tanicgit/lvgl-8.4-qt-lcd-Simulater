cmake_minimum_required(VERSION 3.12.4)

# 设置交叉编译工具链
if(DEFINED ENV{SDKTARGETSYSROOT})
    set(CMAKE_SYSROOT $ENV{SDKTARGETSYSROOT})
    set(CMAKE_C_COMPILER $ENV{SDKTARGETSYSROOT}/../x86_64-pokysdk-linux/usr/bin/aarch64-poky-linux/aarch64-poky-linux-gcc)
    set(CMAKE_CXX_COMPILER $ENV{SDKTARGETSYSROOT}/../x86_64-pokysdk-linux/usr/bin/aarch64-poky-linux/aarch64-poky-linux-g++)
    message(STATUS "aarch64-poky-linux")
else()
    set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/aarch64-none-linux-gnu.cmake)
    message(STATUS "aarch64-none-linux")
endif()

# 名字 版本
project(lvgldemo VERSION 0.0.1 LANGUAGES C CXX)

# 确保生成编译数据库
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 设置C++标准
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 获取当前时间
string(TIMESTAMP BUILD_TIMESTAMP "%Y-%m-%d %H:%M:%S" UTC)
string(TIMESTAMP BUILD_DATE "%Y%m%d" UTC)
string(TIMESTAMP BUILD_TIME "%H:%M:%S" UTC)

# 自动生成的版本变量
message(STATUS "版本号: ${uplinkd_VERSION}")       # 完整版本 (1.2.3)
message(STATUS "主版本号: ${uplinkd_VERSION_MAJOR}") # 1
message(STATUS "次版本号: ${uplinkd_VERSION_MINOR}") # 2
message(STATUS "修订号: ${uplinkd_VERSION_PATCH}")  # 3

message(STATUS "编译时间: ${BUILD_TIMESTAMP}")


# 设置默认构建类型为 Release（带优化）
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build" FORCE)
endif()

# 设置可选的构建类型
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")

# 设置静态还是动态链接 ON/OFF
option(BUILD_SHARED_LIBS "Build shared libraries instead of static" OFF) 

# 根据构建类型设置不同的编译选项
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    # 调试版本配置
    add_compile_options(-O0 -g)  # 无优化，包含调试信息
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -g")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    # 发布版本配置
    add_compile_options(-O3)      # 最高优化级别
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s")  # 自动 strip
endif()


# 添加 LVGL 子目录
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lvgl)

# 可执行文件
file(GLOB UI_SOURCES "ui/*.cpp")
add_executable(${PROJECT_NAME}
    linux/main.cpp
    linux/lv_port_disp.c
    linux/lv_port_indev.cpp
    ui/page_main.cpp
    # 其他源文件
	${UI_SOURCES}
)

# 链接 LVGL 库
target_link_libraries(${PROJECT_NAME} lvgl)

# 包含目录
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/lvgl/src
    ${CMAKE_CURRENT_SOURCE_DIR}/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/linux
)
