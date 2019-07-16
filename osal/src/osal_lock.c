/**
 * @file  osal_lock.c
 * @brief �̻߳��������API
 * @author  <binsonli666@icloud.com>
 * @version 1.0.0
 * @date 2017-03-31
 */
/* Copyright (C) 2008-2016, Null Inc.
 * All right reserved
 *
 */

#include <os_common.h>
#include <osal.h>
#include <osal_lock.h>

/* �̻߳�����ģ��İ汾��, �������޸�ʱ�����޸� */
#define OSAL_LOCK_MODULE_VERSION  ("1.2.0")

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_LOCK_Get_Module_Version
 *         ��ȡLOCKģ��ĵ�ǰ�汾
 *
 * @param[in/out]   major_version  ģ�����汾�ŷ���ָ�룬��Ҫ�ⲿ������Чָ��
 * @param[in/out]   minor_version  ģ���Ӱ汾�ŷ���ָ�룬��Ҫ�ⲿ������Чָ��
 * @param[in/out]   build_version  ģ�����汾�ŷ���ָ�룬��Ҫ�ⲿ������Чָ��
 *
 * @return
 *          0 -> �汾�Ż�ȡ�ɹ�
 *          1 -> ������������ָ��Ƿ�
 *          2 -> �汾�Ÿ�ʽ����
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_LOCK_Get_Module_Version(OS_U32 *major_version, OS_U32 *minor_version, OS_U32 *build_version)
{
    OS_S32 ret = 0;

    /* �汾��ӡ�����ڵ��� */
    LOGI(MODULE_LOCK, "%s: %s\n", __func__, OSAL_LOCK_MODULE_VERSION);

    /* ������� */
    OSAL_CHECK_ERROR(MODULE_LOCK, (!major_version) || (!minor_version) || (!build_version), 1, return_err,
                     "Invalid argument! (%p, %p, %p)\n", major_version, minor_version, build_version);

    /* �汾���أ������ϲ�Ӧ��ͨ���汾���жϣ�ʵ�ֲ��컯���ݴ��� */
    ret = sscanf(OSAL_LOCK_MODULE_VERSION, "%d.%d.%d", major_version, minor_version, build_version);
    OSAL_CHECK_ERROR(MODULE_LOCK, (3 != ret), 2, return_err, "Version format invalid! (Error: %d)\n", ret);

    return 0;

    /* ����ִ���쳣ֵ���� */
return_err:
    return (MODULE_LOCK | ret);
}

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_LOCK_Init
 *         ��ʼ���̻߳�����
 *
 * @param[in/out]   handle  �����������ɹ��󷵻صľ��ָ��,
 *                          ��Ҫ�ⲿ������Чָ�룬����ΪNULL
 *
 * @return
 *          0 -> ��ʼ���ɹ�
 *          1 -> ��������
 *          2 -> �ڴ治��
 *          3 -> pthread_mutex_init����ʧ��
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_LOCK_Init(OS_HANDLE *handle)
{
    OS_HANDLE mutex_handle = NULL;
    OS_S32 ret = 0;

    /* ������� */
    OSAL_CHECK_ERROR(MODULE_LOCK, (!handle), 1, return_err, "Invalid argument! (%p)\n", handle);

    /* ���以������Դ */
    mutex_handle = malloc(sizeof(pthread_mutex_t));
    OSAL_CHECK_ERROR(MODULE_LOCK, (!mutex_handle), 2, return_err, "Malloc %d bytes fail! (Error: %p)\n",
                     sizeof(pthread_mutex_t), mutex_handle);

    /* ��ʼ���̻߳����� */
    ret = pthread_mutex_init((pthread_mutex_t*) mutex_handle, NULL);
    OSAL_CHECK_ERROR(MODULE_LOCK, (ret != 0), 3, return_err, "Call pthread_mutex_init fail! (Error: %d)\n", ret);

    /* �����Ч��������� */
    *handle = mutex_handle;
    return 0;

return_err:
    /* �ͷ���Դ */
    if (mutex_handle) {
        free(mutex_handle);
    }

    return (MODULE_LOCK | ret);
}

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_LOCK_Deinit
 *         ����ʼ���̻߳�����
 *
 * @param[in]  handle  ��Ҫ����ʼ���Ļ���������� �ο�OSAL_LOCK_Init
 *
 * @return
 *          0 -> ����ʼ���ɹ�
 *          1 -> ��������
 *          2 -> pthread_mutex_destroy����ʧ��
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_LOCK_Deinit(OS_HANDLE handle)
{
    OS_S32 ret = 0;

    /* ������� */
    OSAL_CHECK_ERROR(MODULE_LOCK, (!handle), 1, return_err, "Invalid argument! (%p)\n", handle);

    pthread_mutex_lock((pthread_mutex_t*)handle);
    pthread_mutex_unlock((pthread_mutex_t*)handle);

    /* ���ٻ����� */
    ret = pthread_mutex_destroy((pthread_mutex_t*)handle);
    OSAL_CHECK_ERROR(MODULE_LOCK, (ret != 0), 2, return_err, "Call pthread_mutex_destroy fail! (Error: %d)\n", ret);
    free(handle);

    return 0;

return_err:
    return (MODULE_LOCK | ret);
}

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_LOCK_Up
 *         �̻߳���������
 *
 * @param[in]  handle  ��Ҫ�����Ļ���������� �ο�OSAL_LOCK_Init
 *
 * @return
 *          0 -> �����ɹ�
 *          1 -> ��������
 *          2 -> ����ʧ��
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_LOCK_Up(OS_HANDLE handle)
{
    OS_S32 ret = 0;

    /* ������� */
    OSAL_CHECK_ERROR(MODULE_LOCK, (!handle), 1, return_err, "Invalid argument! (%p)\n", handle);

    /* ���������� */
    ret = pthread_mutex_lock((pthread_mutex_t*) handle);
    OSAL_CHECK_ERROR(MODULE_LOCK, (0 != ret), 2, return_err, "Call pthread_mutex_lock fail! (Error: %d)\n", ret);

    return 0;

return_err:
    return (MODULE_LOCK | ret);
}

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_LOCK_Down
 *         �̻߳���������
 *
 * @param[in]  handle  ��Ҫ�����Ļ���������� �ο�OSAL_LOCK_Init
 *
 * @return
 *          0 -> �����ɹ�
 *          1 -> ��������
 *          2 -> ����ʧ��
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_LOCK_Down(OS_HANDLE handle)
{
    OS_S32 ret = 0;

    /* ������� */
    OSAL_CHECK_ERROR(MODULE_LOCK, (!handle), 1, return_err, "Invalid argument! (%p)\n", handle);

    /* ���������� */
    ret = pthread_mutex_unlock((pthread_mutex_t*) handle);
    OSAL_CHECK_ERROR(MODULE_LOCK, (0 != ret), 2, return_err, "Call pthread_mutex_unlock fail! (Error: %d)\n", ret);

    return 0;

return_err:
    return (MODULE_LOCK | ret);
}

