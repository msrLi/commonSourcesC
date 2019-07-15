/**
 * @file osal_dir.h
 * @brief Ŀ¼����API
 * @author  binsonli <binsonli666@icloud.com>
 * @version 1.0.0
 * @date 2019-03-27
 */
/* Copyright (C) 2008-2019, Null Inc.
 * All right reserved
 *
 */

#ifndef __OSAL_DIR_H__
#define __OSAL_DIR_H__

#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_DIR_Get_Module_Version
 *         ��ȡDIRģ��ĵ�ǰ�汾
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
OS_S32 OSAL_DIR_Get_Module_Version(OS_U32 *major_version, OS_U32 *minor_version, OS_U32 *build_version);

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_DIR_Create_Multi_Dir
 *         �����༶Ŀ¼
 *
 * @param[in]  path       Ŀ¼
 *
 * @return
 *          0 -> �ɹ�
 *          1 -> ��������
 *          2 -> ʧ��
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_DIR_Create_Multi_Dir(OS_S8 *path);

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_DIR_API_Test
 *         DIR���API���ԣ��Ѹ������� DIR API����
 *
 * @return
 *          0 -> ����ͨ��
 *          1 -> ��������
 *          2 -> ����Ŀ¼ʧ��
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_DIR_API_Test(OS_S32 argc, OS_S8 *argv[]);


#ifdef __cplusplus
}
#endif

#endif //__OSAL_DIR_H__

