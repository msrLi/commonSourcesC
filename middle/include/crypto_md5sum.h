/**
 * @file crypto.h
 * @brief �������API
 * @author  binsonli <binsonli666@icloud.com>
 * @version 1.2.0
 * @date 2019-02-19
 */

/* Copyright(C) 2009-2016, null Inc.
 * All right reserved
 *
 */

#ifndef __CRYPTO_H__
#define __CRYPTO_H__

#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------------------------------------------------*/
/**
 * @brief  CRYPTO_Get_Module_Version
 *         ��ȡCRYPTOģ��ĵ�ǰ�汾
 *
 * @param[in/out]      major_version  ģ�����汾�ŷ���ָ�룬�ⲿ������Чָ�룬����OS_U32
 * @param[in/out]      minor_version  ģ���Ӱ汾�ŷ���ָ�룬�ⲿ������Чָ�룬����OS_U32
 * @param[in/out]      build_version  ģ�����汾�ŷ���ָ�룬�ⲿ������Чָ�룬����OS_U32
 *
 * @return
 *          0 -> �汾�Ż�ȡ�ɹ�
 *          1 -> ������������ָ��Ƿ�
 *          2 -> �汾�Ÿ�ʽ����
 */
/* --------------------------------------------------------------------------*/
OS_S32 CRYPTO_Get_Module_Version(OS_U32 *major_version, OS_U32 *minor_version, OS_U32 *build_version);

/* --------------------------------------------------------------------------*/
/**
 * @brief  CRYPTO_Calulate_Buffer_MD5
 *         �������ݵ�md5ֵ
 *
 * @param[in/out]       buffer      ����ָ��
 * @param[in/out]       len         ���ݳ���
 * @param[in/out]       md5_str     ��Чָ��
 *
 * @return
 *          0 -> �ɹ�
 *          1 -> ��������
 */
/* --------------------------------------------------------------------------*/
OS_S32 CRYPTO_Calulate_Buffer_MD5(OS_U8 *buffer, OS_U32 len, OS_S8 *md5_str);


/* --------------------------------------------------------------------------*/
/**
 * @brief  CRYPTO_Calulate_File_MD5
 *         �����ļ���md5ֵ
 *
 * @param[in/out]       file_path   �ļ�����·��
 * @param[in/out]       md5_str     ��Чָ��
 *
 * @return
 *          0 -> �ɹ�
 *          1 -> ��������
 *          2 -> �ļ���ʧ��
 *          3 -> ��ȡ����ʧ��
 */
/* --------------------------------------------------------------------------*/
OS_S32 CRYPTO_Calulate_File_MD5(OS_S8 *file_path, OS_S8 *md5_str);

/* --------------------------------------------------------------------------*/
/**
 * @brief  main
 *         �������API���Գ����Ѹ������� CRYPTO API����
 *
 * @return
 *          0 -> ����ͨ��
 *          1 -> ��������
 *          2 -> ͨ���ļ�����MD5ʧ��
 *          3 -> ���ļ�ʧ��
 *          4 -> ��ȡ�ļ���Сʧ��
 *          5 -> �����ڴ�ʧ��
 *          6 -> ��ȡ����ʧ��
 *          7 -> ͨ���ڴ����MD5ʧ��
 */
/* --------------------------------------------------------------------------*/
OS_S32 CRYPTO_API_Test(OS_S32 argc, OS_S8 *argv[]);

#ifdef __cplusplus
}       // extern "C" {
#endif  // __cplusplus

#endif  // __CRYPTO_H__
