/**
 * @file osal_msg_key.h
 * @brief  包含常用头文件
 * @author  <itarge@itarge.com>
 * @version 1.0.0
 * @date 2017-05-16
 */

/* Copyright (C) 2009-2016, Itarge Inc.
 * All right reserved
 *
 */

#ifndef __OSAL_MSG_KEY_H__
#define __OSAL_MSG_KEY_H__

#ifdef __cplusplus
extern "C" {
#endif

#define RELOAD_MSG_KEY                          (0x842c7ae5) /*!< RELOAD_MSG_KEY*/
#define NV_JPEG_ENCODE_KEY                      (0x842c7aa1) //JPEG ENCODE LOCK
#define STREAM_MSG_KEY                          (0x542c7ae5) /*!< STREAM_MSG_KEY*/
#define AVAL_MSG_KEY                            (0x542c7ae6) /*!< AVAL_MSG_KEY  */
#define PERIPHERAL_MSG_KEY                      (0x542c7ae7) /*!< PERIPHERAL_MSG_KEY  */
#define OSAL_MHOST_MSG_KEY                       (0x542c7ae8) /*!< MHOST_MSG_KEY */
#define ISS_ALG_MSG_KEY                         (0x542c7ae9) /*!< ALG_MSG_KEY   */
#define SEARCH_MSG_KEY                          (0x542c7aea) /*!< SEARCH_MSG_KEY*/
#define NICP_MSG_KEY                            (0x542c7aeb) /*!< NICP_MSG_KEY  */
#define WEB_MSG_KEY                             (0x542c7aec) /*!< NICP_MSG_KEY  */

#define OSAL_CONFIG_INFO_KEY                     (0x542c7aee) /*!< 配置信息         */
#define OSAL_TRIGGER_MSG_KEY                     (0x542c7aef) /*!< SDK --->Trigger  */
#define OSAL_TRIGGER_QUEUE_KEY                   (0x542c8aef) /*!< QUEUE--->Trigger */

#define OSAL_WTD_MSG_KEY                         (0x542c7af6) /*!< OSAL_WTD_MSG_KEY   */
#define OSAL_UPGRADE_MSG_KEY                     (0x542c7af0) /*!< 升级传输升级百分比*/
#define OSAL_UPGRADE_NET_MSG_KEY                 (0x542c7af1) /*!< 升级传输升级百分比*/

#define OSAL_CON1_CONFIG_INFO_KEY                (0x542c7ad1) /*!< 配置信息 */
#define OSAL_CON1_CONFIG_INFO_DEF_KEY            (0x542c7ad4) /*!< 配置信息 */
#define OSAL_CON2_CONFIG_INFO_KEY                (0x542c7ad2) /*!< 配置信息 */
#define OSAL_CON2_CONFIG_INFO_DEF_KEY            (0x542c7ad5) /*!< 配置信息 */
#define OSAL_CON3_CONFIG_INFO_KEY                (0x542c7ad3) /*!< 配置信息 */
#define OSAL_CON3_CONFIG_INFO_DEF_KEY            (0x542c7ad6) /*!< 配置信息 */


#define OSAL_VIDE0_0_MSHAER_KEY                  (0x555c7aa0) /*!< 视频队列编码前 YUV */
#define OSAL_VIDE0_1_MSHAER_KEY                  (0x555c7aa1) /*!< 视频队列编码前 YUV */
#define OSAL_VIDE0_2_MSHAER_KEY                  (0x555c7aa2) /*!< 视频队列编码前 YUV */
#define OSAL_VIDE0_3_MSHAER_KEY                  (0x555c7aa3) /*!< 视频队列编码前 YUV */

#define VID_FRAME_MFIFO_COUNT                   (0x4)        /*!<  8 */
#define VID_FRAME_MFIFO_SIZE                       (0x800000)   /*!<  8M */
#define VID_FRAME_MFIFO_4M_SIZE                (0x400000)   /*!<  4M */
#define VID_FRAME_MFIFO_10M_SIZE              (0xA00000)   /*!<  10M */

#define VID_FRAME_MFIFO_1920X1080_SIZE          (1920*1080)   /*!< 1920X1080 */
#define VID_FRAME_MFIFO_2048X1536_SIZE          (2048*1536)   /*!< 2048X1536 */
#define VID_FRAME_MFIFO_4096X2160_SIZE          (4096*2160)   /*!< 4096X2160 */

#define MJPG_SEM_SHM_KEY                        (0x642c7ae1) /*!< 视频队列 编码后 MJPEG */
#define H264_SEM_SHM_KEY                        (0x642c7ae2) /*!< 视频队列 编码后H264 */
#define H264_SEC_SEM_SHM_KEY                (0x742c7ae2) /*!< 视频队列 编码后 H264 */
#define MJPG_SHM_BUFF_SIZE                      (6*1024*1024)/*!<  6M */
#define H264_SHM_BUFF_SIZE                      (820 * 1024)//((512 + 128) * 1024)//(4*1024*1024)/*!<  4M */
#define H264_SEC_SHM_BUFF_SIZE              (3*1024*1024)/*!<  3M */

#define OSAL_BITS_0_MSHARE_KEY                   (0x682c7ae0)
#define OSAL_BITS_1_MSHARE_KEY                   (0x682c7ae1)
#define OSAL_BITS_2_MSHARE_KEY                   (0x682c7ae2)
#define OSAL_BITS_3_MSHARE_KEY                   (0x682c7ae3)
#define OSAL_BITS_CACHE_BUFFER_COUNT      (16)

#define OSAL_CONFIG_INFO_SIZE                    (64*1024)

#define OSAL_EEPROM_SIZE                                 (32*1024)
#define OSAL_SYSCONFIG_EEPROM_OFFSET     (1*1024)
#define OSAL_AiAUTH_EEPROM_OFFSET            ((OSAL_EEPROM_SIZE)-(4*1024))//最后4k 预留保存Ai算法授权信息


#define OSAL_TRIG_INFO_FIFO_KEY                  (0x682c7ae5)
#define OSAL_TRIG_INFO_FIFO_CNT                  (16)// size = sizeof(OSAL_TrigInfo);

#define OSAL_MSG_SVAC_KEY                        (0x682c8cd1)
#define OSAL_MSG_SVAC_CNT                        (8)
#define OSAL_MSG_SVAC_SIZE                       (0x20000) // 128k

#define MSG_UNSUPPORT                           (-2)
#define MSG_FAILED                              (-1)
#define MSG_OK                                     (0)
#define MSG_MASTER                            (1)
#define MSG_CLIENT                              (2)
#define MSG_MASTER_H264                         (3)
#define MSG_MASTER_MJPG                         (4)
#define MSG_ALG_HOST                            (5)
#define MSG_ALG_SLAVE                           (6)
#define MSG_ALG_CAPTURE_EVENT                   (1234)
#define MSG_ALG_SNAP_EVENT                      (1235)


///////////////////////////for struct storage//////////////////////////////////////

#ifdef __cplusplus
}
#endif

#endif //__OSAL_MSG_KEY_H__
