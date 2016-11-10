



#ifndef __SCCOMM_H__
#define __SCCOMM_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#pragma pack(4)

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"
#include "ScInterface.h"
#include "UsimPsInterface.h"
#include "msp_diag_comm.h"
/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define   SC_KEY_LEN                (32)                                        /* 密钥的长度 */

#define   SC_HASH_LEN               (32)                                        /* HASH的码流长度 */

#define   SC_FILE_PATH_LEN          (128)

#define   SC_SSK_STRING             "Secure Storage Key"                        /* 生成SSK的字符串 */

#define   SC_SSK_STRLEN             (VOS_StrLen(SC_SSK_STRING))                 /* 生成SSK的字符串长度 */

#define   SC_DSSK_STRING            "Debug port protect Secure Storage Key"     /* 生成DSSK的字符串 */

#define   SC_DSSK_STRLEN            (VOS_StrLen(SC_DSSK_STRING))                /* 生成DSSK的字符串长度 */

#define   SC_IPK_STRING             "Integrity Protection Key"                  /* 生成IPK的字符串 */

#define   SC_IPK_STRLEN             (VOS_StrLen(SC_IPK_STRING))                 /* 生成IPK的字符串长度 */

#define   SC_ASSK_STRING            "AT Secure Storage Key"                     /* 生成ASSK的字符串 */

#define   SC_ASSK_STRLEN            (VOS_StrLen(SC_ASSK_STRING))                /* 生成ASSK的字符串长度 */

#define   SC_SATK_STRING            "Secure AT Key"                             /* 生成SATK的字符串 */

#define   SC_SATK_STRLEN            (VOS_StrLen(SC_SATK_STRING))                /* 生成SATK的字符串长度 */

#define   SC_FILE_EXIST_MAGIC       (0x37ab65cd)                                /* SC备份文件存在标记 */

#define   SC_FILE_PACKET_MAGIC      (0xdeacb589)                                /* SC备份区存在标记 */

#define   SC_LOG_MAX_LEN            (512)

#define   SC_LOG_FILE_MAX_SIZE      (SC_LOG_MAX_LEN * 100 * 2)    /* the max size of log file. */

#define   SC_FILE_SEEK_END          (2)

#define   SC_FILE_SEEK_SET          (0)

#define   SC_FILE_EXIST_OK          (0)

#define   SC_MAX_RECORD_SLICE_INFO  (500)           /* 读写文件最大记录次数 */

#define   SC_NV_IMEI_LENTH          (16)

#define   SC_RECORD_IMEI_MAGIC      (0x5a5a5a5a)                                /* SC存放校验过IMEI标记 */

#if ((OSA_CPU_CCPU == VOS_OSA_CPU))
#define SC_GEN_LOG_MODULE(Level) (/*lint -e1011 -e778*/DIAG_GEN_LOG_MODULE(VOS_GetModemIDFromPid(MAPS_PB_PID), DIAG_MODE_COMM, Level)/*lint +e1011 +e778*/)

#define   SC_NORMAL_LOG(string)                     (VOS_VOID)DIAG_LogReport(SC_GEN_LOG_MODULE(PS_LOG_LEVEL_NORMAL), MAPS_PB_PID, __FILE__, __LINE__, "%s taskid:%d", string, SC_COMM_GetCurrentTaskID());\
                                                    SC_Printf("%s taskid:%d.\r\n", string, SC_COMM_GetCurrentTaskID())

#define   SC_NORMAL_ASCII_LOG(string)               (VOS_VOID)DIAG_LogReport(SC_GEN_LOG_MODULE(PS_LOG_LEVEL_NORMAL), MAPS_PB_PID, __FILE__, __LINE__, "%02x ", string);\
                                                    SC_Printf("%02x ", string)

#define   SC_INFO1_LOG(string, para1)               (VOS_VOID)DIAG_LogReport(SC_GEN_LOG_MODULE(PS_LOG_LEVEL_NORMAL),MAPS_PB_PID, __FILE__, __LINE__, "%s,%d taskid:%d", string, para1, SC_COMM_GetCurrentTaskID());\
                                                    SC_Printf("%s %d taskid:%d.\r\n", string, para1, SC_COMM_GetCurrentTaskID())

#define   SC_NORMAL1_LOG(string, para1)             (VOS_VOID)DIAG_LogReport(SC_GEN_LOG_MODULE(PS_LOG_LEVEL_NORMAL),MAPS_PB_PID, __FILE__, __LINE__, "%s,%d taskid:%d", string, para1, SC_COMM_GetCurrentTaskID());\
                                                    SC_Printf("%s %d taskid:%d.\r\n", string, para1, SC_COMM_GetCurrentTaskID())

#define   SC_WARNING1_LOG(string, para1)            (VOS_VOID)DIAG_LogReport(SC_GEN_LOG_MODULE(PS_LOG_LEVEL_NORMAL),MAPS_PB_PID, __FILE__, __LINE__, "%s,%d taskid:%d", string, para1, SC_COMM_GetCurrentTaskID());\
                                                    SC_Printf("%s %d taskid:%d.\r\n", string, para1, SC_COMM_GetCurrentTaskID())

#define   SC_ERROR_LOG(string)                      (VOS_VOID)DIAG_LogReport(SC_GEN_LOG_MODULE(PS_LOG_LEVEL_NORMAL), MAPS_PB_PID, __FILE__, __LINE__, "%s taskid:%d", string, SC_COMM_GetCurrentTaskID());\
                                                    SC_Printf("%s  taskid:%d.\r\n", string, SC_COMM_GetCurrentTaskID())

#define   SC_ERROR1_LOG(string, para1)              (VOS_VOID)DIAG_LogReport(SC_GEN_LOG_MODULE(PS_LOG_LEVEL_NORMAL),MAPS_PB_PID, __FILE__, __LINE__, "%s,%d taskid:%d", string, para1, SC_COMM_GetCurrentTaskID());\
                                                    SC_Printf("%s %d taskid:%d.\r\n", string, para1, SC_COMM_GetCurrentTaskID())

#define   SC_ERROR2_LOG(string, para1, para2)       (VOS_VOID)DIAG_LogReport(SC_GEN_LOG_MODULE(PS_LOG_LEVEL_NORMAL),MAPS_PB_PID, __FILE__, __LINE__, "%s,%d,%d taskid:%d", string, para1, para2, SC_COMM_GetCurrentTaskID());\
                                                    SC_Printf("%s %d,%d taskid:%d.\r\n", string, para1, para2, SC_COMM_GetCurrentTaskID())

#define   SC_ERROR3_LOG(string, para1, para2, para3) (VOS_VOID)DIAG_LogReport(SC_GEN_LOG_MODULE(PS_LOG_LEVEL_NORMAL),MAPS_PB_PID, __FILE__, __LINE__, "%s,%d,%d,%d taskid:%d", string, para1, para2, para3, SC_COMM_GetCurrentTaskID());\
                                                     SC_Printf("%s %d,%d,%d taskid:%d.\r\n", string, para1, para2, para3, SC_COMM_GetCurrentTaskID())

#else
#define   SC_ERROR_LOG(string)                      SC_Printf(string)
#endif

/*****************************************************************************
  3 枚举定义
*****************************************************************************/

enum SC_KEY_TYPE_ENUM
{
    SC_KEY_TYPE_SSK                     = 0x00,             /* SSK */
    SC_KEY_TYPE_DSSK                    = 0x01,             /* DSSK */
    SC_KEY_TYPE_IPK                     = 0x02,             /* IPK */
    SC_KEY_TYPE_BUTT
};
typedef VOS_UINT8 SC_KEY_TYPE_ENUM_UINT8;


enum SC_CRYPTO_PASSWORD_TYPE_ENUM
{
    SC_CRYPTO_PASSWORD_TYPE_CK          = 0x00,             /* CK的密文 */
    SC_CRYPTO_PASSWORD_TYPE_DK          = 0x01,             /* DK的密文 */
    SC_CRYPTO_PASSWORD_TYPE_BUTT
};
typedef VOS_UINT8 SC_CRYPTO_PASSWORD_TYPE_ENUM_UINT8;


enum SC_RECORD_FUNC_ENUM
{
    SC_PERS_VerifyNvReadImei_FUNC       = 0x00,
    SC_PERS_VerifyIMEI_FUNC             = 0x01,
    SC_PERS_CHECK_HUK_VAULE_FUNC        = 0x02,
    SC_COMM_ReadSCFileAndCmpSign_FUNC   = 0x03,
    SC_COMM_ReadFile_IMEI_FUNC          = 0x04,
    SC_COMM_GenerateSignCode_FUNC       = 0x05,
    SC_COMM_ReadSignFile_FUNC           = 0x06,
    SC_COMM_RsaDecrypt_FUNC             = 0x07,
    SC_RECORD_VOS_MemCmp_IMEI_FUNC      = 0x08,

    SC_RECORD_FUNC_BUTT
};
typedef VOS_UINT16 SC_RECORD_FUNC_ENUM_UINT16;


enum SC_SECRET_FILE_TYPE_ENUM
{
    SC_SECRET_FILE_TYPE_CK              = 0x00,             /* CK-FILE */
    SC_SECRET_FILE_TYPE_DK              = 0x01,             /* DK-FILE */
    SC_SECRET_FILE_TYPE_AK              = 0x02,             /* AK-FILE */
    SC_SECRET_FILE_TYPE_PI              = 0x03,             /* PI-FILE */
#if  ( FEATURE_MULTI_MODEM == FEATURE_ON )
#if (3 == MULTI_MODEM_NUMBER)
    SC_SECRET_FILE_TYPE_IMEI_I0         = 0x04,             /* IMEI-FILE 卡0 */
    SC_SECRET_FILE_TYPE_IMEI_I1         = 0x05,             /* IMEI-FILE 卡1 */
    SC_SECRET_FILE_TYPE_IMEI_I2         = 0x06,             /* IMEI-FILE 卡2 */
#else
    SC_SECRET_FILE_TYPE_IMEI_I0         = 0x04,             /* IMEI-FILE 卡0 */
    SC_SECRET_FILE_TYPE_IMEI_I1         = 0x05,             /* IMEI-FILE 卡1 */
#endif
#else
    SC_SECRET_FILE_TYPE_IMEI_I0         = 0x04,             /* IMEI-FILE 卡0 */
#endif

    SC_SECRET_FILE_TYPE_BUTT
};
typedef VOS_UINT8 SC_SECRET_FILE_TYPE_ENUM_UINT8;

/*****************************************************************************
  4 全局变量声明
*****************************************************************************/
/*****************************************************************************
  5 消息头定义
*****************************************************************************/
/*****************************************************************************
  6 消息定义
*****************************************************************************/
/*****************************************************************************
  7 STRUCT定义
*****************************************************************************/


typedef struct
{
    VOS_UINT32                          ulFileMagicMUM;
    VOS_UINT32                          ulFileLen;
    VOS_UINT32                          ulFileOffset;
    VOS_CHAR                            acFilePath[SC_FILE_PATH_LEN];
}SC_BACKUP_EACH_FILE_INFO_STRU;


typedef struct
{
    VOS_UINT32                         ulBackMagicMUM;
    VOS_UINT32                         ulTotaleSize;
    SC_BACKUP_EACH_FILE_INFO_STRU      astSCEachFileInfo[SC_SECRET_FILE_TYPE_BUTT*2];
    VOS_CHAR                           aucFileData[4];
}SC_BACKUP_FILE_INFO_STRU;


typedef struct
{
    VOS_UINT32                       ulTaskId;      /* 任务ID */
    SC_RECORD_FUNC_ENUM_UINT16       enFuncID;      /* 函数ID */
    MODEM_ID_ENUM_UINT16             enModemID;     /* 操作modemID */
    VOS_UINT32                       ulOperateSn;   /* 操作SN */
    VOS_UINT32                       ulSliceStart;  /* 起止时间 */
    VOS_UINT32                       ulSliceEnd;    /* 结束时间 */

}SC_RECORD_INFO_STRU;


typedef struct
{
    VOS_UINT32                       ulOperateCount;
    SC_RECORD_INFO_STRU              stRecordInfo[SC_MAX_RECORD_SLICE_INFO];
}SC_RECORD_INFO_CTX_STRU;


typedef struct
{
    VOS_UINT32                       ulStartFalg;   /* 起始Flag */
    VOS_UINT8                        ucImeiInfo[SC_NV_IMEI_LENTH];
    VOS_UINT32                       ulEndFalg;     /* 结束Flag  */

}SC_RECORD_VERIFY_IMEI_INFO_CTX_STRU;

/*****************************************************************************
  8 UNION定义
*****************************************************************************/
/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/
/*****************************************************************************
  10 函数声明
*****************************************************************************/
#if ((OSA_CPU_CCPU == VOS_OSA_CPU))

/*****************************************************************************
 函 数 名  : SC_COMM_AllocRecImeiMem
 功能描述  : 为存放IMIE申请空间
 输入参数  :

 输出参数  :

 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
VOS_UINT32 SC_COMM_AllocRecImeiMem(MODEM_ID_ENUM_UINT16     enModemID);

/*****************************************************************************
 函 数 名  : SC_COMM_CeckImeiRecordMem
 功能描述  : 检查存放IMIE空间合法性
 输入参数  :

 输出参数  :

 返 回 值  : VOS_BOOL
 调用函数  :
 被调函数  :
*****************************************************************************/
VOS_BOOL SC_COMM_CeckImeiRecordMemFlag(MODEM_ID_ENUM_UINT16   enModemID);

/*****************************************************************************
 函 数 名  : SC_COMM_SetVerifyImeiValue
 功能描述  : 更新校验通过IMEI
 输入参数  :

 输出参数  :

 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
VOS_VOID SC_COMM_SetVerifyImeiValue(
    MODEM_ID_ENUM_UINT16                    enModemID,
    VOS_UINT8                              *pucImie
);

/*****************************************************************************
 函 数 名  : SC_COMM_CeckImeiRecordMem
 功能描述  : 检查存放IMIE空间合法性
 输入参数  :

 输出参数  :

 返 回 值  : SC_RECORD_IMEI_INFO_CTX_STRU*
 调用函数  :
 被调函数  :
*****************************************************************************/
SC_RECORD_VERIFY_IMEI_INFO_CTX_STRU* SC_COMM_GetVerifyImeiValue(MODEM_ID_ENUM_UINT16   enModemID);

VOS_UINT32 SC_COMM_RecordSliceStart(
    SC_RECORD_FUNC_ENUM_UINT16          enFuncID,
    MODEM_ID_ENUM_UINT16                enModemID,
    VOS_UINT32                          ulTaskId
);

#define SC_COMM_RECORDSTART(enFuncID, enModemID) SC_COMM_RecordSliceStart(enFuncID, enModemID, SC_COMM_GetCurrentTaskID())

VOS_VOID SC_COMM_RecordEndSlice(VOS_UINT32 ulIndex);

extern SC_ERROR_CODE_ENUM_UINT32 SC_COMM_GenerateKey(
    VOS_CHAR                           *pcData,
    VOS_UINT32                          ulLen,
    VOS_UINT8                          *pucKey,
    VOS_UINT32                          ulKeyLen
);

extern SC_ERROR_CODE_ENUM_UINT32 SC_COMM_ReadSCFileAndCmpSign(
    SC_SECRET_FILE_TYPE_ENUM_UINT8      enFileType,
    VOS_UINT8                          *pucContent,
    VOS_UINT32                          ulContentLen
);

extern SC_ERROR_CODE_ENUM_UINT32 SC_COMM_WriteSecretFile(
    SC_SECRET_FILE_TYPE_ENUM_UINT8      enFileType,
    VOS_UINT8                          *pucContent,
    VOS_UINT32                          ulContentLen
);

extern SC_ERROR_CODE_ENUM_UINT32 SC_COMM_GenerateCryptoPwd(
    SC_CRYPTO_PASSWORD_TYPE_ENUM_UINT8  enPwdType,
    VOS_UINT8                          *pucPwd,
    VOS_UINT8                           ucPwdLen,
    VOS_UINT8                          *pucCryptoPwd,
    VOS_UINT32                          ulCryptoPwdLen
);

extern SC_ERROR_CODE_ENUM_UINT32 SC_COMM_RsaEncrypt(
    VOS_UINT8                          *pucRawData,
    VOS_UINT32                          ulLen,
    VOS_UINT8                          *pucCipherData,
    VOS_UINT32                         *pulCipherLen
);

extern SC_ERROR_CODE_ENUM_UINT32 SC_COMM_RsaDecrypt(
    VOS_UINT8                          *pucPwd,
    VOS_UINT32                          ulCipherLen,
    VOS_UINT8                          *pucPubContent,
    VOS_UINT32                         *pulPubLen
);


SC_ERROR_CODE_ENUM_UINT32 SC_COMM_GenerateSignCode(
    VOS_UINT8                          *pucContent,
    VOS_UINT32                          ulContentLen,
    VOS_UINT8                          *pucSignCode,
    VOS_UINT32                          ulSignCodeLen
);


extern SC_ERROR_CODE_ENUM_UINT32 SC_COMM_WriteFile(
    VOS_CHAR                           *pcFilePath,
    VOS_UINT8                          *pucContent,
    VOS_UINT32                          ulContentLen
);


extern SC_ERROR_CODE_ENUM_UINT32 SC_COMM_ReadFile(
    VOS_CHAR                           *pcFilePath,
    VOS_UINT8                          *pucContent,
    VOS_UINT32                          ulContentLen
);


extern VOS_UINT32 SC_COMM_GetUsimmCachedFile(
    VOS_CHAR                           *pucFilePath,
    VOS_UINT32                         *pulDataLen,
    VOS_UINT8                         **ppucData,
    USIMM_CARDAPP_ENUM_UINT32           enAppType,
    MODEM_ID_ENUM_UINT16                enModemID);


extern VOS_UINT32 SC_COMM_IsUsimServiceAvailable(
    UICC_SERVICES_TYPE_ENUM_UINT32  enService,
    MODEM_ID_ENUM_UINT16            enModemID);


VOS_UINT32 SC_COMM_GetCurrentTaskID(VOS_VOID);
#endif

#if ((OSA_CPU_ACPU == VOS_OSA_CPU))


VOS_UINT32 SC_COMM_Restore(VOS_VOID);

extern VOS_VOID SC_Printf(VOS_CHAR *pcData);

#else
/*lint -e960 */
extern VOS_VOID SC_Printf(const VOS_CHAR *pcformat, ...);
/*lint +e960 */
#endif

#if ((VOS_OS_VER == VOS_WIN32) || (VOS_OS_VER == VOS_NUCLEUS))
#pragma pack()
#else
#pragma pack(0)
#endif


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif
