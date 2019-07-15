/**
 * @file module.h
 * @brief  包含常用头文件
 * @author  <itarge@itarge.com>
 * @version 1.0.0
 * @date 2017-03-10
 */

/* Copyright (C) 2009-2016, Itarge Inc.
 * All right reserved
 *
 */

#ifndef __MODULE_H__
#define __MODULE_H__


#ifdef __cplusplus
extern "C" {
#endif

/* 模块号定义， 便于通过错误返回值快速定位模块 */
typedef enum E_MODULE {
    /***********  osal  ************/
    E_MODULE_LOG = 0,       /* 日志模块 */

    /***********   hal  ************/
    E_MODULE_RTC,           /* RTC模块 */
    E_MODULE_I2C,           /* I2C模块 */
    E_MODULE_SPI,           /* SPI模块 */
    E_MODULE_UART,          /* UART模块 */
    E_MODULE_EEPROM,        /* EEPROM模块 */
    E_MODULE_GPIO,          /* GPIO 模块 */

    /*********** middle ************/
    E_MODULE_CRYPTO,        /* 加密相关模块 */

    E_MODULE_FPGA,          /* FPGA模块 */
    E_MODULE_VIN,           /* Video输入/采集模块 */
    E_MODULE_TEMPERATURE,   /* 温度传感器模块 */
    E_MODULE_LOCAL_BUS,     /* 本地总线模块 */
    E_MODULE_JPEG_SW,       /* jpeg编解码模块 */
    E_MODULE_MESSAGE,       /* 基于共享内存的消息队列模块 */
    E_MODULE_SHM,           /* 共享内存模块 */
    E_MODULE_MSG,           /* 消息队列模块 */
    E_MODULE_SEM,           /* Posix信号量模块 */
    E_MODULE_LOCK,          /* 线程锁模块 */
    E_MODULE_TASK,          /* 任务模块 */
    E_MODULE_SWOSD,         /* 软件OSD */
    E_MODULE_ISP,           /* 图像处理模块 */
    E_MODULE_ENCODE,        /* 解码模块 */
    E_MODULE_WATCHDOG,      /* 看门狗模块 */
    E_MODULE_SEMAPHORE,     /* System V信号量模块 */
    E_MODULE_SHARE_BUFFER,  /* 共享buffer模块 */
    E_MODULE_SECRET,        /* 加密模块 */
    E_MODULE_PID,           /* 线程模块 */
    E_MODULE_SYSINFO,       /* 系统信息模块 */
    E_MODULE_UPGRADE,       /* 升级模块 */
    E_MODULE_UPGRADE_SERVER,       /* 升级模块 服务器*/
    E_MODULE_MTD,           /* MTD模块 */
    E_MODULE_IPC,           /* 核间通讯模块 */
    E_MODULE_NET,           /* NET模块 */
    E_MODULE_SOCKET,        /* SOCKET模块 */
    E_MODULE_HASHCMD,       /* HASHCMD模块 */
    E_MODULE_MEMCPY,        /* MEMCPY模块 */
    E_MODULE_PINCTRL,       /* PINCTRL模块 */
    E_MODULE_XC7060,        /* XC7060 ISP模块(上海) */
    E_MODULE_CONVERT,       /* 图像转换模块 */
    E_MODULE_SYSCTRL,       /* 系统控制模块 */
    E_MODULE_HWINFO,        /* HAL 层信息模块 */
    E_MODULE_RESIZE,        /* 图片缩放模块 */
    E_MODULE_VERSION,       /* 版本信息获取模块 */
    E_MODULE_BOARD_TEST,    /* 硬件测试模块 */
    E_MODULE_FILE,          /* 文件操作模块 */
    E_MODULE_LED,           /* LED模块 */
    E_MODULE_COMPRESS,      /* 压缩解压缩模块 */
    E_MODULE_PWM,           /* PWM模块 */
    E_MODULE_IRIS,          /* 自动光圈模块 */
    E_MODULE_SIL1227,       /* sil1127模块 */
    E_MODULE_MACHXO2,       /* Machxo2 cpld 模块 */
    E_MODULE_DIR,           /* 目录相关操作模块 */
    E_MODULE_AUTO_TEST,     /* 自动测试模块 */
} E_MODULE;

typedef struct MODULE_SHORT_NAME_TAG {
    OS_U32 module;
    const char *name;
} MODULE_SHORT_NAME_T;

/* bit[31:16] 用于存储模块号，bit[15:0]用于存储错误码 */
#define MODULE_SHIFT        (16)
/*  转换到高位，便于代码直接使用 */
#define MODULE_VALUE(m)     (m <<MODULE_SHIFT)
#define MODULE_MASK         (0xFFFF0000)
/* bit[15:0] 用于存储错误码 */
#define ERROR_MASK          (0x0000FFFF)

/* 定义模块号宏，便于使用 */
#define MODULE_LOG          MODULE_VALUE(E_MODULE_LOG)
#define MODULE_RTC          MODULE_VALUE(E_MODULE_RTC)
#define MODULE_I2C          MODULE_VALUE(E_MODULE_I2C)
#define MODULE_SPI          MODULE_VALUE(E_MODULE_SPI)
#define MODULE_UART         MODULE_VALUE(E_MODULE_UART)
#define MODULE_FPGA         MODULE_VALUE(E_MODULE_FPGA)
#define MODULE_EEPROM       MODULE_VALUE(E_MODULE_EEPROM)
#define MODULE_VIN          MODULE_VALUE(E_MODULE_VIN)
#define MODULE_GPIO         MODULE_VALUE(E_MODULE_GPIO)
#define MODULE_TEMPERATURE  MODULE_VALUE(E_MODULE_TEMPERATURE)
#define MODULE_LOCAL_BUS    MODULE_VALUE(E_MODULE_LOCAL_BUS)
#define MODULE_JPEG_SW      MODULE_VALUE(E_MODULE_JPEG_SW)
#define MODULE_MESSAGE      MODULE_VALUE(E_MODULE_MESSAGE)
#define MODULE_SHM          MODULE_VALUE(E_MODULE_SHM)
#define MODULE_MSG          MODULE_VALUE(E_MODULE_MSG)
#define MODULE_SEM          MODULE_VALUE(E_MODULE_SEM)
#define MODULE_LOCK         MODULE_VALUE(E_MODULE_LOCK)
#define MODULE_TASK         MODULE_VALUE(E_MODULE_TASK)
#define MODULE_SWOSD        MODULE_VALUE(E_MODULE_SWOSD)
#define MODULE_ISP          MODULE_VALUE(E_MODULE_ISP)
#define MODULE_ENCODE       MODULE_VALUE(E_MODULE_ENCODE)
#define MODULE_WATCHDOG     MODULE_VALUE(E_MODULE_WATCHDOG)
#define MODULE_SEMAPHORE    MODULE_VALUE(E_MODULE_SEMAPHORE)
#define MODULE_SHARE_BUFFER MODULE_VALUE(E_MODULE_SHARE_BUFFER)
#define MODULE_SECRET       MODULE_VALUE(E_MODULE_SECRET)
#define MODULE_PID          MODULE_VALUE(E_MODULE_PID)
#define MODULE_SYSINFO      MODULE_VALUE(E_MODULE_SYSINFO)
#define MODULE_UPGRADE      MODULE_VALUE(E_MODULE_UPGRADE)
#define MODULE_UPGRADE_SERVER      MODULE_VALUE(E_MODULE_UPGRADE_SERVER)
#define MODULE_MTD          MODULE_VALUE(E_MODULE_MTD)
#define MODULE_IPC          MODULE_VALUE(E_MODULE_IPC)
#define MODULE_NET          MODULE_VALUE(E_MODULE_NET)
#define MODULE_SOCKET       MODULE_VALUE(E_MODULE_SOCKET)
#define MODULE_HASHCMD      MODULE_VALUE(E_MODULE_HASHCMD)
#define MODULE_MEMCPY       MODULE_VALUE(E_MODULE_MEMCPY)
#define MODULE_PINCTRL      MODULE_VALUE(E_MODULE_PINCTRL)
#define MODULE_XC7060       MODULE_VALUE(E_MODULE_XC7060)
#define MODULE_CONVERT      MODULE_VALUE(E_MODULE_CONVERT)
#define MODULE_SYSCTRL      MODULE_VALUE(E_MODULE_SYSCTRL)
#define MODULE_HWINFO       MODULE_VALUE(E_MODULE_HWINFO)
#define MODULE_RESIZE       MODULE_VALUE(E_MODULE_RESIZE)
#define MODULE_VERSION      MODULE_VALUE(E_MODULE_VERSION)
#define MODULE_BOARD_TEST   MODULE_VALUE(E_MODULE_BOARD_TEST)
#define MODULE_FILE         MODULE_VALUE(E_MODULE_FILE)
#define MODULE_LED          MODULE_VALUE(E_MODULE_LED)
#define MODULE_COMPRESS     MODULE_VALUE(E_MODULE_COMPRESS)
#define MODULE_PWM          MODULE_VALUE(E_MODULE_PWM)
#define MODULE_IRIS         MODULE_VALUE(E_MODULE_IRIS)
#define MODULE_SIL1127      MODULE_VALUE(E_MODULE_SIL1227)
#define MODULE_MACHXO2      MODULE_VALUE(E_MODULE_MACHXO2)
#define MODULE_CRYPTO       MODULE_VALUE(E_MODULE_CRYPTO)
#define MODULE_DIR          MODULE_VALUE(E_MODULE_DIR)
#define MODULE_AUTO_TEST    MODULE_VALUE(E_MODULE_AUTO_TEST)

#ifdef __cplusplus
}
#endif

#endif //__MODULE_H__
