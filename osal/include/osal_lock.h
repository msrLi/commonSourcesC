/**
 * @file osal_lock.h
 * @brief �̻߳��������API
 * @author binsonli <binsonli666@iccloud.com>
 * @version 1.0.0
 * @date 2017-03-10
 */
/* Copyright (C) 2008-2016, Null Inc.
 * All right reserved
 *
 */

#ifndef __OSAL_LOCK_H__
#define __OSAL_LOCK_H__

#ifdef __cplusplus
extern "C" {
#endif

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
OS_S32 OSAL_LOCK_Get_Module_Version(OS_U32 *major_version, OS_U32 *minor_version, OS_U32 *build_version);

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
OS_S32 OSAL_LOCK_Init(OS_HANDLE *handle);

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
OS_S32 OSAL_LOCK_Deinit(OS_HANDLE handle);

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
OS_S32 OSAL_LOCK_Up(OS_HANDLE handle);

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
OS_S32 OSAL_LOCK_Down(OS_HANDLE handle);

#ifdef __cplusplus
}
#endif

#endif //__OSAL_LOCK_H__
