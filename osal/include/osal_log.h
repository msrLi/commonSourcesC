/**
 * @file osal_log.h
 * @brief  日志功能相关API
 * @author binsonli <binsonli@binsonli.com>
 * @version 1.0.0
 * @date 2017-03-10
 */
/* Copyright (C) 2008-2016, binsonli Inc.
 * All right reserved
 *
 */

#ifndef __OSAL_LOG_H__
#define __OSAL_LOG_H__

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
* define & enum
******************************************************************************/

/* 日志级别枚举 */
enum {
    E_LOG_LEVEL_PANNIC = 0,       /* 崩溃日志， 级别最高 */
    E_LOG_LEVEL_FATAL,            /* 致命性错误日志 */
    E_LOG_LEVEL_ERROR,            /* 错误日志，需要解决 */
    E_LOG_LEVEL_WARNING,          /* 告警日志，需要关注 */
    E_LOG_LEVEL_INFO,             /* 信息日志 */
    E_LOG_LEVEL_VERBOSE,          /* 日志 */
    E_LOG_LEVEL_DEBUG,            /* 调试日志 */
    E_LOG_LEVEL_MAX,              /* 枚举结束 */
};

#define OSAL_LOG_PANIC      (0x000)
#define OSAL_LOG_FATAL      (0x100)
#define OSAL_LOG_ERROR      (0x200)
#define OSAL_LOG_WARNING    (0x300)
#define OSAL_LOG_INFO       (0x400)
#define OSAL_LOG_VERBOSE    (0x500)
#define OSAL_LOG_DEBUG      (0x600)

#ifndef DISABLE_LOGn
#define LOGP(module, fmt, args...) PLOGP(module, fmt, ##args)
#define LOGF(module, fmt, args...) PLOGF(module, fmt, ##args)
#define LOGE(module, fmt, args...) PLOGE(module, fmt, ##args)
#define LOGW(module, fmt, args...) PLOGW(module, fmt, ##args)
#define LOGI(module, fmt, args...) PLOGI(module, fmt, ##args)
#define LOGV(module, fmt, args...) PLOGV(module, fmt, ##args)
#define LOGD(module, fmt, args...) PLOGD(module, fmt, ##args)
#if 0
#define LOGPIF(module, fmt, args...) PLOGPIF(module, fmt, ##args)
#define LOGFIF(module, fmt, args...) PLOGFIF(module, fmt, ##args)
#define LOGEIF(module, fmt, args...) PLOGEIF(module, fmt, ##args)
#define LOGWIF(module, fmt, args...) PLOGWIF(module, fmt, ##args)
#define LOGIIF(module, fmt, args...) PLOGIIF(module, fmt, ##args)
#define LOGVIF(module, fmt, args...) PLOGVIF(module, fmt, ##args)
#define LOGDIF(module, fmt, args...) PLOGDIF(module, fmt, ##args)
#define ASSERT_LOG(module, cond, fmt, args...) ASSERT_PLOG(module, cond, fmt, ##args)
#endif
#else
#define LOGP(module, fmt, args...)
#define LOGF(module, fmt, args...)
#define LOGE(module, fmt, args...)
#define LOGW(module, fmt, args...)
#define LOGI(module, fmt, args...)
#define LOGV(module, fmt, args...)
#define LOGD(module, fmt, args...)
#if 0
#define LOGPIF(module, fmt, args...)
#define LOGFIF(module, fmt, args...)
#define LOGEIF(module, fmt, args...)
#define LOGWIF(module, fmt, args...)
#define LOGIIF(module, fmt, args...)
#define LOGVIF(module, fmt, args...)
#define LOGDIF(module, fmt, args...)
#define ASSERT_LOG(module, cond, fmt, args)
#endif
#endif

#undef PLOGX
#define PLOGX(module, leavel, fmt, args...)         \
do {                                                \
    OSAL_LOG(module, leavel, __FILE__,                \
            __FUNCTION__, __LINE__, fmt, ##args);   \
}while(0)

#undef PLOGP
#define PLOGP(module, fmt, args...)                 \
do {                                                \
    PLOGX(module, OSAL_LOG_PANIC, fmt, ##args);     \
}while(0)

#undef PLOGF
#define PLOGF(module, fmt, args...)                 \
do {                                                \
    PLOGX(module, OSAL_LOG_FATAL, fmt, ##args);     \
}while(0)

#undef PLOGE
#define PLOGE(module, fmt, args...)                 \
do {                                                \
    PLOGX(module, OSAL_LOG_ERROR, fmt, ##args);     \
}while(0)

#undef PLOGW
#define PLOGW(module, fmt, args...)                 \
do {                                                \
    PLOGX(module, OSAL_LOG_WARNING, fmt, ##args);   \
}while(0)

#undef PLOGI
#define PLOGI(module, fmt, args...)                 \
do {                                                \
    PLOGX(module, OSAL_LOG_INFO, fmt, ##args);      \
}while(0)

#undef PLOGV
#define PLOGV(module, fmt, args...)                 \
do {                                                \
    PLOGX(module, OSAL_LOG_VERBOSE, fmt, ##args);   \
}while(0)

#undef PLOGD
#define PLOGD(module, fmt, args...)                 \
do {                                                \
    PLOGX(module, OSAL_LOG_DEBUG, fmt, ##args);     \
}while(0)

#define OSAL_CHECK_ERROR(module, cond, retval,                  \
        lable,fmt, args...)                                     \
do {                                                            \
    if(cond) {                                                  \
        LOGE(module, fmt, ##args);                              \
        ret = retval;                                           \
        goto lable;                                             \
    }                                                           \
} while(0)


/* --------------------------------------------------------------------------*/
/**
 * @brief OSAL_LOG
 *          核心日志输出函数，实现了网络，本地存储
 * @param module 模块
 * @param level  日志的级别
 * @param file   打印日志所在文件
 * @param func   打印日志所在函数
 * @param line   打印日志所在行数
 * @param ...    可变长参数
 *
 * @return
 *          0 -> 输出正常
 *            -> 志级别超过范围
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_LOG(OS_U32 module, OS_U32 level, const OS_S8 *file, const OS_S8 *func, OS_U32 line, const OS_S8 *fmt, ...);

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_LOG_Set_Head
 *         设置日志系统输出前面的字符串，最多8级
 *
 * @param[in]  idx  日志前缀顺序号，0最前面，7最后面
 * @param[in]  fmt  可变长参数
 * @param[in]  ...  可变长参数
 *
 * @return
 *          0 -> 输出正常
 *          1 -> 日志级别超过范围
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_LOG_Set_Head(OS_U32 idx, OS_S8 *fmt, ...);

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_LOG_Get_Level
 *         获取日志输出级别, 以便上层判断
 *
 * @param[out]  level 当前的日志输出级别指针，外部调用者分配OS_U32空间
 *
 * @return
 *          0 -> 成功
 *          1 -> 参数错误，输入指针非法
 *          2 -> header级数超过范围
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_LOG_Get_Level(OS_U32 *level);

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_LOG_Set_Level
 *         设置日志输出级别, 上层可调
 *
 * @param[in]  level 需要设置的日志输出级别
 *
 * @return
 *          0 -> 成功
 *          1 -> 参数错误
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_LOG_Set_Level(OS_U32 level);

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_LOG_Register_Callback
 *         注册客户自己的实现的日志回调处理函数
 *
 * @param[in]  callback
 *
 * @return
 *          0 -> 成功
 *          1 -> 参数错误，输入指针非法
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_LOG_Register_Callback(void (*callback)(OS_S8 *));
#if 0
/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_LOG_Wrosal_To_File
 *         将日志保存到日志文件
 *
 * @param[in]  file_name         文件路径
 * @param[in]  format            保存日志内容的格式
 *
 * @return
 *          大于3 -> 写入日志的字符数
 *          1     -> 日志文件预处理失败
 *          2     -> 获取可变参数失败
 *          3     -> 打开日志文件失败
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_LOG_Wrosal_To_File(OS_S8 *file_name, OS_S8 *format, ...);
#endif
/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_LOG_API_Test
 *         日志API测试函数，已覆盖所有的API
 *
 * @return
 *          0      -> 测试全部通过
 *          其他值 -> 测试失败
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_LOG_API_Test(void);

#ifdef __cplusplus
}
#endif

#endif //__OSAL_LOG_H__

