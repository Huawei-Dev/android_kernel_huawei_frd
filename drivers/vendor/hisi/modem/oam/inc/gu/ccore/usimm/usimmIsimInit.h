

#ifndef __USIMM_ISIM_INIT_H__
#define __USIMM_ISIM_INIT_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "usimmbase.h"

#pragma pack(4)

#if (FEATURE_OFF == FEATURE_UE_UICC_MULTI_APP_SUPPORT)
/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define USIMM_USIM_GBA_AUTH_MODE            (0x84)
#define USIMM_ISIM_AKA_AUTH_MODE            (0x81)
#define USIMM_ISIM_HTTP_AUTH_MODE           (0x82)
#define USIMM_ISIM_GBA_NAF_AUTH_MODE        (0x84)

/*******************************************************************************
  3 枚举定义
*******************************************************************************/


/*****************************************************************************
  4 STRUCT定义
*****************************************************************************/


/*****************************************************************************
  5 全局变量声明
*****************************************************************************/


/*****************************************************************************
  6 函数声明
*****************************************************************************/
extern VOS_VOID USIMM_ISIM_InitGlobalVar(VOS_VOID);

extern VOS_UINT8 USIMM_ISIM_GetChannelId(VOS_VOID);

extern VOS_UINT32 USIMM_ISIM_GetRealStatusFlag(VOS_VOID);

extern VOS_VOID USIMM_ISIM_SetRealStatusFlag(VOS_UINT32 ulRealIsimStatusFlag);

extern VOS_UINT32 USIMM_ISIM_OpenChannelForISIM(VOS_VOID);

extern VOS_VOID USIMM_ISIM_InitFiles(VOS_VOID);

extern VOS_UINT32 USIMM_ISIM_SelectFileByDefFileId(
    USIMM_FILE_NEED_FCP_ENUM_UINT32     enEfFcpFlag,
    USIMM_DEF_FILEID_ENUM_UINT32        enDefFileId
);

extern VOS_UINT32 USIMM_ISIM_ReadTfFileByDefFileId(
    USIMM_DEF_FILEID_ENUM_UINT32        enDefFileId,
    VOS_UINT16                          usFileLen,
    VOS_UINT8                          *pucData
);

extern VOS_UINT32 USIMM_ISIM_SelectFile(
    USIMM_SELECT_PARA_STUR        *pstSelectPara,
    USIMM_U16_LVDATA_STRU          *pstFilePath
);

extern VOS_UINT32 USIMM_ISIM_SelectFileWithPath(
    USIMM_SELECT_PARA_STUR          *pstSelectPara,
    USIMM_U16_LVDATA_STRU          *pstFilePath,
    USIMM_CURFILEINFO_ST           *pstCurFileInfo,
    VOS_UINT16                     *pusCurPathLen
);

extern VOS_UINT32 USIMM_ISIM_SelectDFFile(
    VOS_UINT16                        usFileId,
    USIMM_SELECT_PARA_STUR           *pstSelectPara,
    USIMM_CURFILEINFO_ST             *pstCurFileInfo
    );

extern VOS_UINT32 USIMM_ISIM_SelectFileByPath(
    USIMM_SELECT_PARA_STUR         *pstSelectPara,
    USIMM_U16_LVDATA_STRU           *pstFilePath,
    USIMM_CURFILEINFO_ST           *pstCurFileInfo
    );

extern VOS_UINT32 USIMM_ISIM_ReselectFileAPDU(USIMM_APDU_ST   *pstApduInfo);

extern VOS_UINT32 USIMM_ISIM_SelectIsimFile_APDU(USIMM_APDU_ST   *pstApduInfo);

extern VOS_UINT32 USIMM_ISIM_GetRealIsimFileHandle(USIMM_GET_COMM_FILE_STRU *pstFileInfo, USIMM_GETCNF_INFO_STRU *pstCnfInfo);

extern VOS_UINT32 USIMM_ISIM_GetIsimFileCommHandle(USIMM_GET_COMM_FILE_STRU *pstFileInfo, USIMM_GETCNF_INFO_STRU *pstCnfInfo);

extern VOS_UINT32 USIMM_ISIM_GetIsimFileFromCard(USIMM_GET_COMM_FILE_STRU *pstFileInfo, USIMM_GETCNF_INFO_STRU *pstCnfInfo);

extern VOS_UINT32 USIMM_ISIM_ReadFile(
    USIMM_EFFCP_ST                     *pstCurEFFcp,
    VOS_UINT8                           ucRecordNum,
    VOS_UINT16                          usDataLen,
    VOS_UINT8                          *pucData);

extern VOS_UINT32 USIMM_ISIM_ReadTFFile(VOS_UINT16 usLen, VOS_UINT8 *pucContent);

extern VOS_UINT32 USIMM_ISIM_ReadBinary_APDU(VOS_UINT16 usOffset, VOS_UINT8 ucP3);

extern VOS_UINT32 USIMM_ISIM_ReadLFFile(USIMM_EFFCP_ST *pstCurEFFcp, VOS_UINT8 ucRecordNum, VOS_UINT8 *pucContent);

extern VOS_UINT32 USIMM_ISIM_ReadRecord_APDU(VOS_UINT8 ucRecordNum, VOS_UINT8 ucMode, VOS_UINT8 ucLen);

extern VOS_UINT32 USIMM_ISIM_AuthDataFillDataProc(
    USIMM_AUTHENTICATION_REQ_STRU      *pstMsg,
    USIMM_U8_DATA_STRU                 *pstData);

extern VOS_VOID USIMM_ISIM_AuthCnf(
    USIMM_AUTHENTICATION_REQ_STRU       *pstMsg,
    USIMM_IMS_AUTH_INFO_STRU            *pstCnfInfo);

extern VOS_VOID USIMM_ISIM_AkaAuthRspProc(
    USIMM_AUTHENTICATION_REQ_STRU      *pstMsg,
    USIMM_APDU_ST                      *pstRspData);

extern VOS_VOID USIMM_ISIM_HttpAuthRspProc(
    USIMM_AUTHENTICATION_REQ_STRU      *pstMsg,
    USIMM_APDU_ST                      *pstRspData);

extern VOS_VOID USIMM_ISIM_GbaAuthRspProc(
    USIMM_AUTHENTICATION_REQ_STRU      *pstMsg,
    USIMM_APDU_ST                      *pstRspData);

extern VOS_VOID USIMM_ISIM_NafAuthRspProc(
    USIMM_AUTHENTICATION_REQ_STRU      *pstMsg,
    USIMM_APDU_ST                      *pstRspData);

extern VOS_UINT32 USIMM_ISIM_AuthHandle(
    USIMM_AUTHENTICATION_REQ_STRU       *pstMsg
);

extern VOS_UINT32 USIMM_ISIM_AuthByChannelID_APDU(
    VOS_UINT8                           ucP1,
    VOS_UINT8                           ucMode,
    VOS_UINT8                          *pucData,
    VOS_UINT32                          ulDataLen
);

extern VOS_UINT32 USIMM_ISIM_IsServiceAvailable(UICC_SERVICES_TYPE_ENUM_UINT32 enService);

extern VOS_VOID USIMM_ISIM_StateChangeWithUsim(VOS_VOID);

extern VOS_UINT32 USIMM_ISIM_SetRealIsimFileHandle(USIMM_SETFILE_REQ_STRU *pstMsg);

extern VOS_UINT32 USIMM_ISIM_SetFileCommHandle(USIMM_SET_COMM_FILE_STRU  *pstSetFileInfo);

extern VOS_UINT32 USIMM_ISIM_UpdateFile(USIMM_EF_TYPE_ENUM_UINT32  enEFFileType,
                                         VOS_UINT8                   ucRecordNum,
                                         VOS_UINT16                  usDataLen,
                                         VOS_UINT8                   *pucData);

extern VOS_UINT32 USIMM_ISIM_UpdateTFFile(VOS_UINT16 usLen, VOS_UINT8 *pucData);

extern VOS_UINT32 USIMM_ISIM_UpdateLFFile(VOS_UINT8 ucRecordNum, VOS_UINT8 *pucData, VOS_UINT8 ucLen);

extern VOS_UINT32 USIMM_ISIM_UpdateCLFFile(VOS_UINT8 *pucData, VOS_UINT8 ucDataLen);

extern VOS_UINT32 USIMM_ISIM_UpdateBinary_APDU(VOS_UINT16 usOffset, VOS_UINT8 ucP3, VOS_UINT8* pucData);

extern VOS_UINT32 USIMM_ISIM_UpdateRecord_APDU(
    VOS_UINT8                           ucRecordNum,
    VOS_UINT8                           ucMode,
    VOS_UINT8                           ucLen,
    VOS_UINT8                          *pucData);

extern VOS_UINT32 USIMM_ISIM_VerifyRefreshAid(
    VOS_UINT32                          ulAidLen,
    VOS_UINT8                          *pucAid,
    USIMM_CARDAPP_ENUM_UINT32          *penAppType
);

extern VOS_VOID USIMM_ISIM_RefreshFileIndHandle(USIMM_STKREFRESH_REQ_STRU *pstMsg);

extern VOS_UINT32 USIMM_ISIM_RefreshHandle(USIMM_CMDHEADER_REQ_STRU *pMsg);

extern VOS_VOID USIMM_ISIM_RefreshFileRead(USIMM_STKREFRESH_REQ_STRU *pstMsg);

extern VOS_VOID USIMM_ISIM_ClearCurFileInfo(VOS_VOID);

extern VOS_UINT32 USIMM_ISIM_ResumeChannel(VOS_VOID);

extern VOS_UINT32 USIMM_ISIM_SelectApp(VOS_VOID);

extern VOS_UINT32 USIMM_GetRefreshEFPath(
    VOS_UINT8                          *pucFileList,
   VOS_UINT32                           ulFileLen,
   VOS_UINT8                           *pucEFPath,
   VOS_UINT32                          *pulPathLen);

extern VOS_UINT32 USIMM_SelectFileByDefFileId(
    USIMM_CARDAPP_ENUM_UINT32           enAppType,
    USIMM_FILE_NEED_FCP_ENUM_UINT32     enEfFcpFlag,
    USIMM_DEF_FILEID_ENUM_UINT32        enDefFileId
);

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

#endif /* end of UsimmDL.h*/

