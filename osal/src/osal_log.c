/**
 * @file osal_log.c
 * @brief  ��־�������API
 * @author  <itarge@itarge.com>
 * @version 1.0.0
 * @date 2017-03-14
 */
/* Copyright (C) 2008-2016, Itarge Inc.
 * All right reserved
 *
 */

#include <os_common.h>
#include <osal_log.h>
#include <syslog.h>
#include <os_moudle.h>

#define __MODULE_NAME_H__
#include <os_module_name.hpp>
#undef __MODULE_NAME_H__

/* ��־ģ��İ汾��, �������޸�ʱ�����޸� */
#define OSAL_LOG_MODULE_VERSION  ("1.2.0")

/******************************************************************************
* define & enum
******************************************************************************/
#define OSAL_LOG_BUF_SIZE        (1024)

/******************************************************************************
* typedef
******************************************************************************/
typedef void (*LOG_CALLBACK)(OS_S8 *);

/******************************************************************************
* Global variable
******************************************************************************/
/* ȫ�ֻص�����ָ�� */
static LOG_CALLBACK g_process_callback = NULL;

/* Ĭ�ϼ���Ϊ OSAL_LOG_INFO */
static OS_U32  g_log_level = OSAL_LOG_INFO;

/* ���֧��8���Զ����ַ���־ */
static OS_S8 g_log_header_mask = 0;
static OS_S8 g_log_header_buf[8][64];

/* ȫ���ַ�����ʽ�����buf */
static OS_S8 g_log_str_buf[OSAL_LOG_BUF_SIZE];

/* ��������ַ������� */
static OS_S8 *g_log_level_str[] = {
    " < PANIC > ",
    " < FATAL > ",
    " < ERROR > ",
    " <WARNING> ",
    " < INFO  > ",
    " <VERBOSE> ",
    " < DEBUG > ",
    " < ##### > "
};


static OS_S8  gProcess[PATH_MAX] = { '\0' };

/******************************************************************************
* Local variable
******************************************************************************/

/******************************************************************************
* Local function
******************************************************************************/

static inline OS_S8 *get_process_name(void)
{
    if (gProcess[0] == '\0') {
        pid_t pid = getpid();
        char path[32];
        char text[PATH_MAX] = { '\0' };
        int  length;

        sprintf(path, "/proc/%d/cmdline", pid);
        int32_t fd = open(path, O_RDONLY);
        if (fd > 0) {
            ssize_t len = read(fd, text, PATH_MAX);
            if (len > 0) {
                text[len] = text[getMaxInvalidId(text, len)] = '\0';
                char *index = strrchr(text, '/');
                if (index != NULL) {
                    strcpy(gProcess, index + 1);
                }
            }
            close(fd);
        }

        if (gProcess[0] == '\0') {
            text[0] = '\0';
            strcpy(path, "/proc/self/exe");
            ssize_t len = readlink(path, text, PATH_MAX);
            if (len > 0) {
                text[len] = text[getMaxInvalidId(text, len)] = '\0';
                char *index = strrchr(text, '/');
                if (index != NULL) {
                    strcpy(gProcess, index + 1);
                }
            }
        }

        if (gProcess[0] == '\0') {
            strcpy(gProcess, "Unknown");
        }

        length = strlen(gProcess);
        if (length > MAX_PROCESS_NAME_LEN) {
            memmove(gProcess,
                    gProcess + length + 1 - MAX_PROCESS_NAME_LEN,
                    MAX_PROCESS_NAME_LEN + 1);
        }
    }

    return gProcess;
}
/* --------------------------------------------------------------------------*/
/**
* @brief __log_vsnprintf
*
* @param pdst
* @param size
* @param pfmt
* @param argptr
*
* @return
*/
/* --------------------------------------------------------------------------*/
static inline OS_U32  __log_vsnprintf(OS_S8* pdst, OS_U32 size,
                                      const OS_S8* pfmt, va_list argptr)
{
    OS_U32 written = 0;

    pdst[0] = '\0';
    written = vsnprintf(pdst, size, pfmt, argptr);

    if ((written >= size) && (size > 0)) {
        // Message length exceeds the buffer limit size
        written = size - 1;
        pdst[size - 1] = '\0';
    }

    return written;
}

/* --------------------------------------------------------------------------*/
/**
 * @brief  log_get_level_str
 *         ������־����õ���Ӧ�ļ����ַ���
 *
 * @param[in]  level   ��Ҫ��ȡ�ļ����ַ���ǰ׺����־����
 *
 * @return  Ҫ��ȡ�ļ����ַ���ǰ׺
 */
/* --------------------------------------------------------------------------*/
static inline OS_S8 *log_get_level_str(OS_U32 level)
{
    OS_U32 index = 0;

    index = (level >> 8) & 7;
    if (index >= E_LOG_LEVEL_MAX) {
        return NULL;
    }

    return g_log_level_str[index];
}

/* --------------------------------------------------------------------------*/
/**
 * @brief  log_default_callback
 *         Ĭ�ϵ���־�ص�����������printf��syslog
 *
 * @param[in]  log  ��Ҫ�������־�ڴ�ָ��
 */
/* --------------------------------------------------------------------------*/
static void log_default_callback(OS_S8 *fmt, ...)
{
    va_list args;
    printf("%s %d\n", __func__, __LINE__);
    va_start(args, fmt);
    printf(fmt, args);
    syslog(LOG_INFO, fmt, args);
    va_end(args);
}

/* --------------------------------------------------------------------------*/
/**
 * @brief  log_sprintf
 *         ��־��ʽ������
 *
 * @param[in]  level  ��־�ļ���
 * @param[in]  fmt    �����ʽ��
 * @param[in]  vl     �����ʽ��
 */
/* --------------------------------------------------------------------------*/
static void log_sprintf(const OS_S8 *fmt, const OS_S8 *process, const OS_S8 *module, const OS_S8 *levelType,
                        const OS_S8 *file, const OS_S8 *func, const OS_U32 line, const char *buf);
{
    printf(fmt, process, module, levelType, file, func, line, buf);
    // log_default_callback();
#if 0
DO_ECHO:
    /* ��ӽ����� */
    g_log_str_buf[off] = 0;

    /* ����ͻ�������˽�еĻص�������ʹ���ⲿ������������Ĭ�ϵ� */
    if (g_process_callback) {
        g_process_callback(g_log_str_buf);
    } else {
        log_default_callback(g_log_str_buf);
    }
#endif
}

/******************************************************************************
* Global function
******************************************************************************/
#if 0
/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_LOG_Get_Module_Version
 *         ��ȡ��־ģ��ĵ�ǰ�汾
 *
 * @param[out]      major_version  ģ�����汾�ŷ���ָ�룬�ⲿ������Чָ�룬����OS_U32
 * @param[out]      minor_version  ģ���Ӱ汾�ŷ���ָ�룬�ⲿ������Чָ�룬����OS_U32
 * @param[out]      build_version  ģ�����汾�ŷ���ָ�룬�ⲿ������Чָ�룬����OS_U32
 *
 * @return
 *          0 -> �汾�Ż�ȡ�ɹ�
 *          1 -> ������������ָ��Ƿ�
 *          2 -> �汾�Ÿ�ʽ����
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_LOG_Get_Module_Version(OS_U32 *major_version, OS_U32 *minor_version, OS_U32 *build_version)
{
    OS_S32 ret = -1;

    /* ������� */
    if ((NULL == major_version) || (NULL == minor_version) || (NULL == build_version)) {
        printf("Invalid argument! (%p, %p, %p)\n", major_version, minor_version, build_version);
        return OSAL_MODULE_LOG | 1;
    }

    /* �汾��ӡ�����ڵ��� */
    printf("%s: %s\n", __func__, OSAL_LOG_MODULE_VERSION);

    /* �汾���أ������ϲ�Ӧ��ͨ���汾���жϣ�ʵ�ֲ��컯���ݴ��� */
    ret = sscanf(OSAL_LOG_MODULE_VERSION, "%d.%d.%d", major_version, minor_version, build_version);
    if (3 != ret) {
        printf("Version format invalid! (Error: %d)\n", ret);
        return OSAL_MODULE_LOG | 2;
    }

    return 0;
}
#endif
/* --------------------------------------------------------------------------*/
/**
 * @brief OSAL_LOG
 *          ������־���������ʵ�������磬���ش洢
 * @param module ģ��
 * @param level  ��־�ļ���
 * @param file   ��ӡ��־�����ļ�
 * @param func   ��ӡ��־���ں���
 * @param line   ��ӡ��־��������
 * @param ...    �ɱ䳤����
 *
 * @return
 *          0 -> �������
 *            -> ־���𳬹���Χ
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_LOG(OS_U32 module, OS_U32 level, OS_S8 *file, OS_S8 *func, OS_U32 line, OS_S8 *fmt, ...)
{
    va_list args;
    OS_S8 buffer[OSAL_LOG_BUF_SIZE];

    if (level > g_log_level) {
        return OSAL_MODULE_LOG | 1;
    }

    memset(buffer, 0, OSAL_LOG_BUF_SIZE);

    va_start(args, fmt);
    __log_vsnprintf(buffer, OSAL_LOG_BUF_SIZE, fmt, args);
    va_end(args);

    log_sprintf("%s %s%s: %s(%s)+%d: ", get_process_name(), OS_Get_Module_Name(module), log_get_level_str(level), \
                file, func, line, buffer);
#if 0

#endif
    return 0;
}

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_LOG_Set_Head
 *         ������־ϵͳ���ǰ����ַ��������8��
 *
 * @param[in]  idx  ��־ǰ׺˳��ţ�0��ǰ�棬7�����
 * @param[in]  fmt  �ɱ䳤����
 * @param[in]  ...  �ɱ䳤����
 *
 * @return
 *          0 -> �������
 *          1 -> ��־���𳬹���Χ
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_LOG_Set_Head(OS_U32 idx, OS_S8 *fmt, ...)
{
    va_list args;

    if (idx > 7) {
        return OSAL_MODULE_LOG | 1;
    }

    va_start(args, fmt);
    g_log_header_mask ^= 1 << idx;
    vsnprintf(g_log_header_buf[idx], 63, fmt, args);
    va_end(args);

    g_log_header_mask |= 1 << idx;

    return 0;
}

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_LOG_Get_Level
 *         ��ȡ��־�������, �Ա��ϲ��ж�
 *
 * @param[out]  level ��ǰ����־�������ָ�룬�ⲿ�����߷���OS_U32�ռ�
 *
 * @return
 *          0 -> �ɹ�
 *          1 -> ������������ָ��Ƿ�
 *          2 -> header����������Χ
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_LOG_Get_Level(OS_U32 *level)
{
    if (NULL == level) {
        return OSAL_MODULE_LOG | 1;
    }
    if (g_log_level > OSAL_LOG_DEBUG) {
        return OSAL_MODULE_LOG | 2;
    }

    *level = g_log_level;

    return 0;
}

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_LOG_Set_Level
 *         ������־�������, �ϲ�ɵ�
 *
 * @param[in]  level ��Ҫ���õ���־�������
 *
 * @return
 *          0 -> �ɹ�
 *          1 -> ��������
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_LOG_Set_Level(OS_U32 level)
{
    if (level > OSAL_LOG_DEBUG) {
        return OSAL_MODULE_LOG | 1;
    }

    g_log_level = level;

    return 0;
}

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_LOG_Register_Callback
 *         ע��ͻ��Լ���ʵ�ֵ���־�ص�������
 *
 * @param[in]  callback
 *
 * @return
 *          0 -> �ɹ�
 *          1 -> ������������ָ��Ƿ�
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_LOG_Register_Callback(void (*callback)(OS_S8 *))
{
    if (NULL == callback) {
        return OSAL_MODULE_LOG | 1;
    }

    g_process_callback = callback;

    return 0;
}
#if 0
#define OSAL_LOG_VSNPRINTF_BUFF_SIZE        (2  * 1024)
/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_LOG_Write_To_File
 *         ����־���浽��־�ļ�
 *
 * @param[in]  file_name         �ļ�·��
 * @param[in]  format            ������־���ݵĸ�ʽ
 *
 * @return
 *          ����3 -> д����־���ַ���
 *          1 -> ��־�ļ�Ԥ����ʧ��
 *          2 -> ��ȡ�ɱ����ʧ��
 *          3 -> ����־�ļ�ʧ��
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_LOG_Write_To_File(OS_S8 *file_name, OS_S8 *format, ...)
{
    OS_S32 offset = 0;
    OS_S8 buf[OSAL_LOG_VSNPRINTF_BUFF_SIZE];
    time_t timep;
    struct tm *p;
    FILE *fp = NULL;
    va_list vaArgPtr;
    OS_S32 retval = 0;

    /* ���buffer */
    memset(buf, '\0', OSAL_LOG_VSNPRINTF_BUFF_SIZE);
    /* ��ȡϵͳ��ǰʱ�� */
    time(&timep);
    p = localtime(&timep);

    sprintf(buf, "[%d-%02d-%02d %02d:%02d:%02d] ", (1900 + p->tm_year), (1 + p->tm_mon), p->tm_mday,
            p->tm_hour, p->tm_min, p->tm_sec);
    offset = strlen(buf);

    va_start(vaArgPtr, format);
    retval = vsnprintf(buf + offset, OSAL_LOG_VSNPRINTF_BUFF_SIZE - 1, format, vaArgPtr);
    if (retval < 0) {
        va_end(vaArgPtr);
        printf("Get params failed! (Error: %d)\n", retval);
        return 1;
    }
    va_end(vaArgPtr);
    fp = fopen(file_name, "a");
    if (fp == NULL) {
        printf("Open the file %s failed! (Error: %p)\n", file_name, fp);
        return 3;
    }
    fputs(buf, fp);
    fclose(fp);
    sync();
    retval += offset;

    return retval;
}
#endif
/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_LOG_API_Test
 *         ��־API���Ժ������Ѹ������е�API
 *
 * @return
 *          0      -> ����ȫ��ͨ��
 *          ����ֵ -> ����ʧ��
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_LOG_API_Test(void)
{
    /* ���ڴ洢�汾�� */
    OS_U32 major_version = 0;
    OS_U32 minor_version = 0;
    OS_U32 build_version = 0;

    OS_S32 ret = 0;
    OS_U32 i = 0;
#if 0
    /* ��ȡ��־ģ��汾�� */
    ret = OSAL_LOG_Get_Module_Version(&major_version, &minor_version, &build_version);
#endif
    /* ���ü��� */
    ret |= OSAL_LOG_Set_Level(OSAL_LOG_DEBUG);
#if 0
    /* ������־ǰ׺�� Ҳ���Բ���� */
    for (i = 0; i < 8; i++) {
        ret |= OSAL_LOG_Set_Head(i, "info%02d:", i);
    }
#endif
    /* �𼶱���� */
    ret |= OSAL_LOG_Set_Level(OSAL_LOG_PANIC);
    ret |= OSAL_LOG(OSAL_LOG_PANIC, "You should see it\n");

    ret |= OSAL_LOG(OSAL_LOG_FATAL, "You should not see it\n");
    ret |= OSAL_LOG_Set_Level(OSAL_LOG_FATAL);
    ret |= OSAL_LOG(OSAL_LOG_FATAL, "You should see it\n");

    ret |= OSAL_LOG(OSAL_LOG_ERROR, "You should not see it\n");
    ret |= OSAL_LOG_Set_Level(OSAL_LOG_ERROR);
    ret |= OSAL_LOG(OSAL_LOG_ERROR, "You should see it\n");

    ret |= OSAL_LOG(OSAL_LOG_WARNING, "You should not see it\n");
    ret |= OSAL_LOG_Set_Level(OSAL_LOG_WARNING);
    ret |= OSAL_LOG(OSAL_LOG_WARNING, "You should see it\n");

    ret |= OSAL_LOG(OSAL_LOG_INFO, "You should not see it\n");
    ret |= OSAL_LOG_Set_Level(OSAL_LOG_INFO);
    ret |= OSAL_LOG(OSAL_LOG_INFO, "You should see it\n");

    ret |= OSAL_LOG(OSAL_LOG_VERBOSE, "You should not see it\n");
    ret |= OSAL_LOG_Set_Level(OSAL_LOG_VERBOSE);
    ret |= OSAL_LOG(OSAL_LOG_VERBOSE, "You should see it\n");

    ret |= OSAL_LOG(OSAL_LOG_DEBUG, "You should not see it\n");
    ret |= OSAL_LOG_Set_Level(OSAL_LOG_DEBUG);
    ret |= OSAL_LOG(OSAL_LOG_DEBUG, "You should see it\n");
#if 0
    /* д����־�ļ� */
    ret |= OSAL_LOG_Write_To_File("/tmp/watchdog.txt", "OSAL_LOG Version %d.%d.%d write success\n", major_version,
                                  minor_version, build_version);
#endif
    return ret;
}

