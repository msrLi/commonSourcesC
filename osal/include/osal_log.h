/**
 * @file osal_log.h
 * @brief  ��־�������API
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

/* ��־����ö�� */
enum
{
    E_LOG_LEVEL_PANNIC = 0,       /* ������־�� ������� */
    E_LOG_LEVEL_FATAL,            /* �����Դ�����־ */
    E_LOG_LEVEL_ERROR,            /* ������־����Ҫ��� */
    E_LOG_LEVEL_WARNING,          /* �澯��־����Ҫ��ע */
    E_LOG_LEVEL_INFO,             /* ��Ϣ��־ */
    E_LOG_LEVEL_VERBOSE,          /* ��־ */
    E_LOG_LEVEL_DEBUG,            /* ������־ */
    E_LOG_LEVEL_MAX,              /* ö�ٽ��� */
};

#define OSAL_LOG_PANIC      (0x000)
#define OSAL_LOG_FATAL      (0x100)
#define OSAL_LOG_ERROR      (0x200)
#define OSAL_LOG_WARNING    (0x300)
#define OSAL_LOG_INFO       (0x400)
#define OSAL_LOG_VERBOSE    (0x500)
#define OSAL_LOG_DEBUG      (0x600)

#define OSAL_LOG_Error(fmt, args...)                             \
do {                                                            \
    OSAL_LOG(OSAL_LOG_ERROR,                                      \
            "at %s:%d, %s()\n",                                 \
            __FILE__, __LINE__, __func__);                      \
    OSAL_LOG(OSAL_LOG_ERROR, fmt, ##args);                        \
} while(0)

#define OSAL_LOG_Warning(fmt, args...)                           \
do {                                                            \
    OSAL_LOG(OSAL_LOG_WARNING,                                    \
            "at %s:%d, %s()\n",                                 \
            __FILE__, __LINE__, __func__);                      \
    OSAL_LOG(OSAL_LOG_WARNING, fmt, ##args);                      \
} while(0)

#define OSAL_LOG_Info(fmt, args...)                              \
do {                                                            \
    OSAL_LOG(OSAL_LOG_INFO,                                       \
            "at %s:%d, %s()\n",                                 \
            __FILE__, __LINE__, __func__);                      \
    OSAL_LOG(OSAL_LOG_INFO, fmt, ##args);                         \
} while(0)

#define OSAL_LOG_Verbose(fmt, args...)                           \
do {                                                            \
    OSAL_LOG(OSAL_LOG_VERBOSE,                                    \
            "at %s:%d, %s()\n",                                 \
            __FILE__, __LINE__, __func__);                      \
    OSAL_LOG(OSAL_LOG_VERBOSE, fmt, ##args);                      \
} while(0)

#define OSAL_LOG_Debug(fmt, args...)                             \
do {                                                            \
    OSAL_LOG(OSAL_LOG_DEBUG,                                      \
            "at %s:%d, %s()\n",                                 \
            __FILE__, __LINE__, __func__);                      \
    OSAL_LOG(OSAL_LOG_DEBUG, fmt, ##args);                        \
} while(0)

#define OSAL_CHECK_DEBUG(cond, retval, lable, fmt, args...)      \
do {                                                            \
    if(cond) {                                                  \
        OSAL_LOG(OSAL_LOG_DEBUG, fmt, ##args);                    \
        ret = retval;                                           \
        goto lable;                                             \
    }                                                           \
} while(0)

#define OSAL_CHECK_INFO(cond, retval, lable, fmt, args...)       \
do {                                                            \
    if(cond) {                                                  \
        OSAL_LOG(OSAL_LOG_INFO, fmt, ##args);                     \
        ret = retval;                                           \
        goto lable;                                             \
    }                                                           \
} while(0)

#define OSAL_CHECK_WARNING(cond, retval, lable, fmt, args...)    \
do {                                                            \
    if(cond) {                                                  \
        OSAL_LOG(OSAL_LOG_WARNING,                                \
                "at %s:%d, %s()\n",                             \
                __FILE__, __LINE__, __func__);                  \
        OSAL_LOG(OSAL_LOG_WARNING, fmt, ##args);                  \
        ret = retval;                                           \
        goto lable;                                             \
    }                                                           \
} while(0)

#define OSAL_CHECK_ERROR(cond, retval, lable, fmt, args...)      \
do {                                                            \
    if(cond) {                                                  \
        OSAL_LOG(OSAL_LOG_ERROR,                                  \
                "at %s:%d, %s()\n",                             \
                __FILE__, __LINE__, __func__);                  \
        OSAL_LOG(OSAL_LOG_ERROR, fmt, ##args);                    \
        OSAL_LOG(OSAL_LOG_ERROR, "%s() return %d\n",              \
                __func__, retval);                              \
        ret = retval;                                           \
        goto lable;                                             \
    }                                                           \
} while(0)

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_LOG
 *         ������־���������ʵ�������磬���ش洢
 *
 * @param[in]  level  ��־�ļ���
 * @param[in]  ...    �ɱ䳤����
 *
 * @return
 *          0 -> �������
 *          1 -> ��־���𳬹���Χ
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_LOG(OS_U32 level, OS_S8 *fmt, ...);

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
OS_S32 OSAL_LOG_Set_Head(OS_U32 idx, OS_S8 *fmt, ...);

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
OS_S32 OSAL_LOG_Get_Level(OS_U32 *level);

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
OS_S32 OSAL_LOG_Set_Level(OS_U32 level);

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
OS_S32 OSAL_LOG_Register_Callback(void (*callback)(OS_S8 *));

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_LOG_Wrosal_To_File
 *         ����־���浽��־�ļ�
 *
 * @param[in]  file_name         �ļ�·��
 * @param[in]  format            ������־���ݵĸ�ʽ
 *
 * @return
 *          ����3 -> д����־���ַ���
 *          1     -> ��־�ļ�Ԥ����ʧ��
 *          2     -> ��ȡ�ɱ����ʧ��
 *          3     -> ����־�ļ�ʧ��
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_LOG_Wrosal_To_File(OS_S8 *file_name, OS_S8 *format, ...);

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
OS_S32 OSAL_LOG_API_Test(void);

#ifdef __cplusplus
}
#endif

#endif //__OSAL_LOG_H__

