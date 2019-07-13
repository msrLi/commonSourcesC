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
enum {
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
OS_S32 OSAL_LOG(OS_U32 module, OS_U32 level, OS_S8 *file, OS_S8 *func, OS_U32 line, OS_S8 *fmt, ...);

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
#if 0
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
OS_S32 OSAL_LOG_API_Test(void);

#ifdef __cplusplus
}
#endif

#endif //__OSAL_LOG_H__

