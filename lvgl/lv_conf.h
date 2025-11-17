#ifndef LV_CONF_H
#define LV_CONF_H

#define LV_CONF_SKIP_INCLUDE 1

/* 显示配置 */
#define LV_HOR_RES_MAX 160
#define LV_VER_RES_MAX 160

/* 颜色配置   */
#define LV_COLOR_DEPTH 1

#define LV_USE_ANTIALIAS 1

/* 内存配置 */
#define LV_MEM_SIZE (32U * 1024U)


#define LV_FONT_MONTSERRAT_8    1
#define LV_FONT_MONTSERRAT_12    1


//#define LV_DRAW_SW_SUPPORT_I1           1
//#define LV_DRAW_SW_I1_LUM_THRESHOLD 127

/* 日志配置 */
#define LV_USE_LOG 1
#if LV_USE_LOG
    #define LV_LOG_LEVEL LV_LOG_LEVEL_WARN
#endif

#endif /* LV_CONF_H */
