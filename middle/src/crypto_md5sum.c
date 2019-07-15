/**
 * @file ite_crypto.c
 * @brief  �������API
 * @author  itarge <itarge@itarge.com>
 * @version 1.2.0
 * @date 2019-02-19
 */

/* Copyright(C) 2009-2019, Itarge Inc.
 * All right reserved
 *
 */

#include <os_common.h>
#include <osal.h>

#include <ite_file.h>

#include <crypto_md5sum.h>

#ifndef HAVE_OPENSSL
#include "md5sum_private.h"
#else
#include <openssl/md5.h>
#endif

#define CRYPTO_READ_DATA_SIZE       (4096)
#define CRYPTO_MD5_SIZE             (16)
#define CRYPTO_MD5_STR_LEN          (CRYPTO_MD5_SIZE * 2)


/* crypto ģ��İ汾��, �������޸�ʱ�����޸� */
#define CRYPTO_MODULE_VERSION       ("1.2.0")

static OS_S32 usage_print(OS_S8 *name)
{
    LOGI(MODULE_CRYPTO,
         "\r\n           Example: %s -f filepath\r\n"
         "           Options:\r\n"
         "           -h | --help          Print this message\r\n"
         "           -f | --file          File path\r\n"
         "\n", name);
    return 0;
}

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
OS_S32 CRYPTO_Get_Module_Version(OS_U32 *major_version, OS_U32 *minor_version, OS_U32 *build_version)
{
    OS_S32 ret = 0;

    /* �汾��ӡ�����ڵ��� */
    LOGI(MODULE_CRYPTO, "%s: %s\n", __func__, CRYPTO_MODULE_VERSION);

    /* ������� */
    ITE_CHECK_ERROR(((!major_version) || (!minor_version) || (!build_version)), 1, return_err,
                    "Invalid argument! (%p, %p, %p)\n", major_version, minor_version, build_version);

    /* �汾���أ������ϲ�Ӧ��ͨ���汾���жϣ�ʵ�ֲ��컯���ݴ��� */
    ret = sscanf(CRYPTO_MODULE_VERSION, "%d.%d.%d", major_version, minor_version, build_version);
    ITE_CHECK_ERROR((3 != ret), 2, return_err, "Version format invalid! (Error: %d)\n", ret);

    return 0;

    /* ����ִ���쳣ֵ���� */
return_err:
    return (MODULE_CRYPTO | ret);
}

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
OS_S32 CRYPTO_Calulate_Buffer_MD5(OS_U8 *buffer, OS_U32 len, OS_S8 *md5_str)
{
    OS_U8 md5_value[CRYPTO_MD5_SIZE] = {0};
    OS_S32 i = 0;
    OS_S32 ret = 0;
    MD5_CTX md5;

    /* ������� */
    ITE_CHECK_ERROR(((!buffer) || (!len) || (!md5_str)), 1, return_err,
                    "Invalid argument! (%p, %d, %p)\n", buffer, len, md5_str);

    MD5_Init(&md5);

    MD5_Update(&md5, buffer, len);

    MD5_Final(md5_value, &md5);

    for (i = 0; i < CRYPTO_MD5_SIZE; i++) {
        snprintf(md5_str + i * 2, 2 + 1, "%02x", md5_value[i]);
    }

    md5_str[CRYPTO_MD5_STR_LEN] = '\0';

    return 0;

    /* ����ִ���쳣ֵ���� */
return_err:
    return (MODULE_CRYPTO | ret);
}

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
OS_S32 CRYPTO_Calulate_File_MD5(OS_S8 *file_path, OS_S8 *md5_str)
{
    OS_U8 data[CRYPTO_READ_DATA_SIZE] = {0};
    OS_U8 md5_value[CRYPTO_MD5_SIZE] = {0};
    OS_S32 fd = 0;
    OS_S32 i = 0;
    OS_S32 ret = 0;
    MD5_CTX md5;

    /* ������� */
    ITE_CHECK_ERROR(((!file_path) || (!md5_str)), 1, return_err,
                    "Invalid argument! (%p, %p)\n", file_path, md5_str);

    fd = open(file_path, O_RDONLY);
    ITE_CHECK_ERROR((-1 == fd), 2, return_err, "Open %s failed! (Error: %d)\n", file_path, fd);

    MD5_Init(&md5);

    while (1) {
        ret = read(fd, data, CRYPTO_READ_DATA_SIZE);
        ITE_CHECK_ERROR((-1 == ret), 3, return_err, "Failed to read data from file %s! (Error: %d)\n", file_path, ret);

        MD5_Update(&md5, data, ret);

        if (0 == ret || ret < CRYPTO_READ_DATA_SIZE) {
            break;
        }
    }

    close(fd);

    MD5_Final(md5_value, &md5);

    for (i = 0; i < CRYPTO_MD5_SIZE; i++) {
        snprintf(md5_str + i * 2, 2 + 1, "%02x", md5_value[i]);
    }

    md5_str[CRYPTO_MD5_STR_LEN] = '\0';

    return 0;

    /* ����ִ���쳣ֵ���� */
return_err:
    return (MODULE_CRYPTO | ret);
}

/* --------------------------------------------------------------------------*/
/**
 * @brief  CRYPTO_API_Test
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
OS_S32 CRYPTO_API_Test(OS_S32 argc, OS_S8 *argv[])
{
    /* ���ڴ洢�汾�� */
    OS_U32 major_version = 0;
    OS_U32 minor_version = 0;
    OS_U32 build_version = 0;

    /* ��ʱ���� */
    OS_S8 *file_path = NULL;
    OS_U32 ret = 0;
    OS_ULONG file_size = 0;

    OS_U8 *buf = NULL;
    FILE *fp = NULL;
    OS_S8 src_md5_str[64] = {0};

    /* �������� */
    OS_S32 c = 0;
    const struct option long_options [] = {
        { "help",           no_argument,            NULL,           'h' },
        { "file",           required_argument,      NULL,           'f' },
        {}
    };
    const OS_S8 short_options [] = "f:h";

    /* ��ӡ CRYPTO �汾�� */
    ret = CRYPTO_Get_Module_Version(&major_version, &minor_version, &build_version);

    /* ������ӡ */
    if (argc == 1) {
        usage_print(argv[0]);
        return MODULE_CRYPTO | 1;
    }

    /* �������� */
    while (1) {
        c = getopt_long(argc, argv, short_options, long_options, NULL);
        if (-1 == c) {
            break;
        }

        switch (c) {
            case 'f':
                file_path = optarg;
                /* ͨ���ļ�����MD5 */
                ret = CRYPTO_Calulate_File_MD5(file_path, src_md5_str);
                ITE_CHECK_ERROR(ret, 2, return_err, "Calulate file %s MD5 failed! (Error: 0x%08x)\n", file_path, ret);
                LOGI(MODULE_CRYPTO, "CRYPTO_Calulate_File_MD5 value: %s\n", src_md5_str);

                fp = fopen(file_path, "r");
                ITE_CHECK_ERROR((!fp), 3, return_err, "Open %s failed! (Error: %p)\n", file_path, fp);

                ret = ITE_FILE_Get_Size(file_path, &file_size);
                ITE_CHECK_ERROR(ret, 4, return_err, "Failed to get file size! (Error: 0x%08x)\n", ret);

                buf = malloc(file_size);
                ITE_CHECK_ERROR((!buf), 5, return_err, "Failed to malloc buffer! (Error: %p)\n", buf);

                ret = fread(buf, 1, file_size, fp);
                ITE_CHECK_ERROR((ret != file_size), 6, return_err, "Failed to get data from test file! (Error: %d)\n", ret);

                memset(src_md5_str, '\0', sizeof(src_md5_str));
                /* ͨ��buffer����MD5 */
                ret = CRYPTO_Calulate_Buffer_MD5(buf, file_size, src_md5_str);
                ITE_CHECK_ERROR(ret, 7, return_err, "Calulate file %s MD5 failed! (Error: 0x%08x)\n", file_path, ret);
                LOGI(MODULE_CRYPTO, "CRYPTO_Calulate_Buffer_MD5 value: %s\n", src_md5_str);

                fclose(fp);
                free(buf);
                buf = NULL;
                break;
            default:
                usage_print(argv[0]);
                break;
        }
    }

    return 0;

    /* ����ִ���쳣ֵ���� */
return_err:
    return (MODULE_CRYPTO | ret);
}

