/**
 * @file osal_dir.c
 * @brief Ŀ¼����API
 * @author  binsonli <binsonli666@icloud.com>
 * @version 1.0.0
 * @date 2019-03-27
 */
/* Copyright (C) 2008-2019, null Inc.
 * All right reserved
 *
 */

#include <os_common.h>
#include <osal_file.h>
#include <osal_log.h>
#include <osal_dir.h>



/* DIR ģ��İ汾��, �������޸�ʱ�����޸� */
#define OSAL_DIR_MODULE_VERSION  ("1.2.0")

static OS_S32 usage_print(OS_S8 *name)
{
    LOGI(MODULE_DIR,
         "\r\n           Example: %s -p path\r\n"
         "           Options:\r\n"
         "           -h | --help          Print this message\r\n"
         "           -p | --path          Path\r\n"
         "\n", name);
    return 0;
}

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
OS_S32 OSAL_DIR_Get_Module_Version(OS_U32 *major_version, OS_U32 *minor_version, OS_U32 *build_version)
{
    OS_S32 ret = 0;

    /* �汾��ӡ�����ڵ��� */
    LOGI(OSAL_LOG_INFO, "%s: %s\n", __func__, OSAL_DIR_MODULE_VERSION);

    /* ������� */
    OSAL_CHECK_ERROR(MODULE_DIR, (!major_version) || (!minor_version) || (!build_version), 1, return_err,
                     "Invalid argument! (%p, %p, %p)\n", major_version, minor_version, build_version);

    /* �汾���أ������ϲ�Ӧ��ͨ���汾���жϣ�ʵ�ֲ��컯���ݴ��� */
    ret = sscanf(OSAL_DIR_MODULE_VERSION, "%d.%d.%d", major_version, minor_version, build_version);
    OSAL_CHECK_ERROR(MODULE_DIR, (3 != ret), 2, return_err, "Version format invalid! (Error: %d)\n", ret);

    return 0;

    /* ����ִ���쳣ֵ���� */
return_err:
    return (MODULE_DIR | ret);
}

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
OS_S32 OSAL_DIR_Create_Multi_Dir(OS_S8 *path)
{
    OS_S8 dir_name[256] = {0};
    OS_S32 i = 0, len = 0;
    OS_S32 ret = 0;

    /* ������� */
    OSAL_CHECK_ERROR(MODULE_DIR, (!path), 1, return_err, "Invalid argument! (%p)\n", path);

    strcpy(dir_name, path);
    len = strlen(dir_name);

    if ('/' != dir_name[len - 1]) {
        strcat(dir_name, "/");
        len++;
    }

    for (i = 1; i < len; i++) {
        if ('/' == dir_name[i]) {
            dir_name[i] = '\0';
            if (OSAL_FILE_Check_Exist(dir_name) != 0) {
                umask(0);
                ret = mkdir(dir_name, 0775);
                OSAL_CHECK_ERROR(MODULE_DIR, (ret == -1), 2, return_err, "mkdir() failed! (Error: %d)\n", ret);
            }
            dir_name[i] = '/';
        }
    }

    return 0;

    /* ����ִ���쳣ֵ���� */
return_err:
    return (MODULE_DIR | ret);
}

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
OS_S32 OSAL_DIR_API_Test(OS_S32 argc, OS_S8 *argv[])
{
    /* ���ڴ洢�汾�� */
    OS_U32 major_version = 0;
    OS_U32 minor_version = 0;
    OS_U32 build_version = 0;

    /* ��ʱ���� */
    OS_S8 *path = NULL;
    OS_U32 ret = 0;

    /* �������� */
    OS_S32 c = 0;
    const struct option long_options [] = {
        { "help",           no_argument,            NULL,           'h' },
        { "path",           required_argument,      NULL,           'p' },
        {}
    };
    const OS_S8 short_options [] = "p:h";

    /* ��ӡ DIR �汾�� */
    ret = OSAL_DIR_Get_Module_Version(&major_version, &minor_version, &build_version);

    /* ������ӡ */
    if (argc == 1) {
        usage_print(argv[0]);
        return MODULE_DIR | 1;
    }

    /* �������� */
    while (1) {
        c = getopt_long(argc, argv, short_options, long_options, NULL);
        if (-1 == c) {
            break;
        }

        switch (c) {
            case 'p':
                path = optarg;
                ret = OSAL_DIR_Create_Multi_Dir(path);
                OSAL_CHECK_ERROR(MODULE_DIR, ret, 2, return_err, "Create directory %s failed! (Error: 0x%08x)\n", path, ret);
                LOGD(MODULE_DIR, "Create directory: %s\n", path);
                break;
            default:
                usage_print(argv[0]);
                break;
        }
    }

    return 0;

    /* ����ִ���쳣ֵ���� */
return_err:
    return (MODULE_DIR | ret);
}

