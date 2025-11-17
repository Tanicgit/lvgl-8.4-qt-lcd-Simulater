# 查找所有源文件
file(GLOB_RECURSE LVGL_SOURCES
    "src/*.c"
)

# 排除不需要的文件（可选）
list(FILTER LVGL_SOURCES EXCLUDE REGEX ".*test.*")
list(FILTER LVGL_SOURCES EXCLUDE REGEX ".*example.*")

# 创建库
add_library(lvgl STATIC ${LVGL_SOURCES})

# 包含目录
target_include_directories(lvgl PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# 编译定义
target_compile_definitions(lvgl PUBLIC 
    LV_CONF_INCLUDE_SIMPLE=1
    LV_LVGL_H_INCLUDE_SIMPLE=1
)

# 安装配置（可选）
install(TARGETS lvgl
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY src/ DESTINATION include/lvgl
    FILES_MATCHING PATTERN "*.h"
)