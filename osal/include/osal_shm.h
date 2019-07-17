/**
 * @file  osal_shm.h
 * @brief  �����ڴ�ģ�����API
 * @author binsonli <itarge@itarge.com>
 * @version 1.0.0
 * @date 2017-03-31
 */
/* Copyright (C) 2008-2016, Null  Inc.
 * All right reserved
 *
 */

#ifndef __OSAL_SHM_H__
#define __OSAL_SHM_H__

#ifdef __cplusplus
extern "C" {
#endif

#define OSAL_SHM_PROC_MEM_SIZE 8192
#define OSAL_SHM_MAX_SHARE_PROC  8

typedef enum OSAL_SHM_FLAG_TAG {
    OSAL_SHM_FLAG_CREAT = 0,
    OSAL_SHM_FLAG_CREAT_ERR = 1,
    OSAL_SHM_FLAG_CREAT_EXCL = 2,
}
OSAL_SHM_FLAG_T;

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_SHM_Get_Module_Version
 *         ��ȡSHMģ��ĵ�ǰ�汾
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
OS_S32 OSAL_SHM_Get_Module_Version(OS_U32 *major_version, OS_U32 *minor_version, OS_U32 *build_version);

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_SHM_Init
 *         �����ڴ��ʼ��
 *
 * @param[in]       key     �����ڴ�Key
 * @param[in]       size    �����ڴ��С,
 *                          ����0���������½��Ĺ����ڴ��С�����ֽ�Ϊ��λ,
 *                          0:ֻ��ȡ�����ڴ�ʱָ��Ϊ0
 *                          ������������0
 *
 * @param[in]       flag    �����ڴ��־
 *                          OSAL_SHM_FLAG_CREAT: �����ڴ治���ڴ��������ڷ��ض�Ӧ��id,
 *                          OSAL_SHM_FLAG_CREAT_ERR: �����ڴ��ʶ�������������򷵻ش���,
 *                          OSAL_SHM_FLAG_CREAT_EXCL: �����ڴ治���ڴ����������򷵻ش���
 *                          ������������0
 * @param[in/out]   mid     �����ڴ�idָ�룬��Ҫ�ⲿ������Чָ�룬����ΪNULL
 *
 * @return
 *          0 -> ��ʼ���ɹ�
 *          1 -> ��������
 *          2 -> ��ʼ��ʧ��
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_SHM_Init(OS_S32 key, OS_U32 size, OS_U32 flag, OS_S32 *mid);

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_SHM_Get_Addr
 *         �ѹ����ڴ�������ӳ�䵽���ý��̵ĵ�ַ�ռ�
 *
 * @param[in]       mid     �����ڴ�id, �ο�OSAL_SHM_Init
 * @param[in/out]   addr    �����ڴ��ַָ�룬��Ҫ�ⲿ������Чָ�룬����ΪNULL
 *
 * @return
 *          0 -> ��ȡ�����ڴ��ַ�ɹ�
 *          1 -> ��������
 *          2 -> ��ȡ�����ڴ��ַʧ��
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_SHM_Get_Addr(OS_S32 mid, OS_HANDLE *addr);

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_SHM_Detach_Addr
 *         ȡ�������ڴ��ַӳ��
 *
 * @param[in]  addr     �����ڴ��ַ
 *
 * @return
 *          0 -> ȡ�������ڴ��ַӳ��ɹ�
 *          1 -> ��������
 *          2 -> ȡ�������ڴ��ַӳ��ʧ��
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_SHM_Detach_Addr(OS_HANDLE addr);

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_SHM_Read
 *         �����ڴ��ȡ����
 *
 * @param[in]       mid     �����ڴ�id, �ο�OSAL_SHM_Init
 * @param[in]       offset  �����ڴ� offset
 * @param[in/out]   buf     ��Ź������ݵ�ָ�룬��Ҫ�ⲿ���룬����ΪNULL
 * @param[in]       len     ��Ҫ��ȡ�����ݳ��ȣ�(offset+len)���ܳ��������ڴ��С
 *
 * @return
 *          0 -> ��ȡ�����ڴ����ݳɹ�
 *          1 -> ��������
 *          2 -> ӳ�乲���ڴ浽��ǰ���̴���
 *          3 -> ȡ�������ڴ�ӳ�����
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_SHM_Read(OS_S32 mid, OS_S32 offset, OS_HANDLE buf, OS_U32 len);

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_SHM_Write
 *         �����ڴ�д�����
 *
 * @param[in]   mid         �����ڴ�id, �ο�OSAL_SHM_Init
 * @param[in]   offset      �����ڴ� offset
 * @param[in]   buf         ��Ҫд��������ڴ�ָ��
 * @param[in]   len         ��Ҫд������ݳ���
 *
 * @return
 *          0 -> �����ڴ�д��ɹ�
 *          1 -> ��������
 *          2 -> ӳ�乲���ڴ浽��ǰ���̴���
 *          3 -> ȡ�������ڴ�ӳ�����
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_SHM_Write(OS_S32 mid, OS_S32 offset, OS_HANDLE buf, OS_U32 len);

#ifdef __cplusplus
}
#endif

#endif // __OSAL_SHM_H__
