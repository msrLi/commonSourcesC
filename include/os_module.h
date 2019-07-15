/**
 * @file module.h
 * @brief  ��������ͷ�ļ�
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

/* ģ��Ŷ��壬 ����ͨ�����󷵻�ֵ���ٶ�λģ�� */
typedef enum E_MODULE {
    /***********  osal  ************/
    E_MODULE_LOG = 0,       /* ��־ģ�� */

    /***********   hal  ************/
    E_MODULE_RTC,           /* RTCģ�� */
    E_MODULE_I2C,           /* I2Cģ�� */
    E_MODULE_SPI,           /* SPIģ�� */
    E_MODULE_UART,          /* UARTģ�� */
    E_MODULE_EEPROM,        /* EEPROMģ�� */
    E_MODULE_GPIO,          /* GPIO ģ�� */

    /*********** middle ************/
    E_MODULE_CRYPTO,        /* �������ģ�� */

    E_MODULE_FPGA,          /* FPGAģ�� */
    E_MODULE_VIN,           /* Video����/�ɼ�ģ�� */
    E_MODULE_TEMPERATURE,   /* �¶ȴ�����ģ�� */
    E_MODULE_LOCAL_BUS,     /* ��������ģ�� */
    E_MODULE_JPEG_SW,       /* jpeg�����ģ�� */
    E_MODULE_MESSAGE,       /* ���ڹ����ڴ����Ϣ����ģ�� */
    E_MODULE_SHM,           /* �����ڴ�ģ�� */
    E_MODULE_MSG,           /* ��Ϣ����ģ�� */
    E_MODULE_SEM,           /* Posix�ź���ģ�� */
    E_MODULE_LOCK,          /* �߳���ģ�� */
    E_MODULE_TASK,          /* ����ģ�� */
    E_MODULE_SWOSD,         /* ���OSD */
    E_MODULE_ISP,           /* ͼ����ģ�� */
    E_MODULE_ENCODE,        /* ����ģ�� */
    E_MODULE_WATCHDOG,      /* ���Ź�ģ�� */
    E_MODULE_SEMAPHORE,     /* System V�ź���ģ�� */
    E_MODULE_SHARE_BUFFER,  /* ����bufferģ�� */
    E_MODULE_SECRET,        /* ����ģ�� */
    E_MODULE_PID,           /* �߳�ģ�� */
    E_MODULE_SYSINFO,       /* ϵͳ��Ϣģ�� */
    E_MODULE_UPGRADE,       /* ����ģ�� */
    E_MODULE_UPGRADE_SERVER,       /* ����ģ�� ������*/
    E_MODULE_MTD,           /* MTDģ�� */
    E_MODULE_IPC,           /* �˼�ͨѶģ�� */
    E_MODULE_NET,           /* NETģ�� */
    E_MODULE_SOCKET,        /* SOCKETģ�� */
    E_MODULE_HASHCMD,       /* HASHCMDģ�� */
    E_MODULE_MEMCPY,        /* MEMCPYģ�� */
    E_MODULE_PINCTRL,       /* PINCTRLģ�� */
    E_MODULE_XC7060,        /* XC7060 ISPģ��(�Ϻ�) */
    E_MODULE_CONVERT,       /* ͼ��ת��ģ�� */
    E_MODULE_SYSCTRL,       /* ϵͳ����ģ�� */
    E_MODULE_HWINFO,        /* HAL ����Ϣģ�� */
    E_MODULE_RESIZE,        /* ͼƬ����ģ�� */
    E_MODULE_VERSION,       /* �汾��Ϣ��ȡģ�� */
    E_MODULE_BOARD_TEST,    /* Ӳ������ģ�� */
    E_MODULE_FILE,          /* �ļ�����ģ�� */
    E_MODULE_LED,           /* LEDģ�� */
    E_MODULE_COMPRESS,      /* ѹ����ѹ��ģ�� */
    E_MODULE_PWM,           /* PWMģ�� */
    E_MODULE_IRIS,          /* �Զ���Ȧģ�� */
    E_MODULE_SIL1227,       /* sil1127ģ�� */
    E_MODULE_MACHXO2,       /* Machxo2 cpld ģ�� */
    E_MODULE_DIR,           /* Ŀ¼��ز���ģ�� */
    E_MODULE_AUTO_TEST,     /* �Զ�����ģ�� */
} E_MODULE;

typedef struct MODULE_SHORT_NAME_TAG {
    OS_U32 module;
    const char *name;
} MODULE_SHORT_NAME_T;

/* bit[31:16] ���ڴ洢ģ��ţ�bit[15:0]���ڴ洢������ */
#define MODULE_SHIFT        (16)
/*  ת������λ�����ڴ���ֱ��ʹ�� */
#define MODULE_VALUE(m)     (m <<MODULE_SHIFT)
#define MODULE_MASK         (0xFFFF0000)
/* bit[15:0] ���ڴ洢������ */
#define ERROR_MASK          (0x0000FFFF)

/* ����ģ��ź꣬����ʹ�� */
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
