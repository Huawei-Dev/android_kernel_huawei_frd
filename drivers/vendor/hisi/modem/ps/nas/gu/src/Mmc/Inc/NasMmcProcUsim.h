

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#ifndef __NASMMCPROCUSIM_H__
#define __NASMMCPROCUSIM_H__


#include "NasMmlCtx.h"
#include "UsimPsInterface.h"
#include "TafAppMma.h"




#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#pragma pack(4)


/*****************************************************************************
  2 宏定义
*****************************************************************************/

/* STK功能 Network Search Mode Change Event 相关宏定义 */
#define MMC_STK_NET_SCH_MODE_MANUAL                         (0)
#define MMC_STK_NET_SCH_MODE_AUTO                           (1)

#define NAS_MMC_USIM_PSLOCI_LAC_POSITION_BEG                (10)                  /* 消息中被删除的RAI中LAC首字节的取值       */
#define NAS_MMC_USIM_PSLOCI_LAC_POSITION_END                (11)

#define NAS_MMC_USIM_FILE_CONTENT_INVALID                   (0xFF)

#define NAS_MMC_PERI_HPLMN_ATTEMP_STEP_SIZE                 (6)                 /* 周期HPLMN搜索，以6分钟位单位增加 */
#define NAS_MMC_MAX_PERI_HPLMN_ATTEMP_TIME_LEN              (480)               /* 周期HPLMN搜索，最长8小时 */
#define NAS_MMC_ENS_MAX_PERI_HPLMN_ATTEMP_TIME_LEN          (1080)              /* 18小时，ENS周期HPLMN搜索，最长25小时，但系统只支持18小时，故只允许18小时: 1080min */

#define NAS_MMC_MNC_LENGTH_INDICATOR_POSITION               (4)                 /* 指示MNC长度的字节在EF文件中的位置 */
#define NAS_MMC_MNC_LENGTH_TWO_BYTES_IN_IMSI                (2)                 /* MNC在IMSI中的长度是2 */
#define NAS_MMC_MNC_LENGTH_THREE_BYTES_IN_IMSI              (3)                 /* MNC在IMSI中的长度是3 */

/*****************************************************************************
  3 枚举定义
*****************************************************************************/

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
    USIMM_CMDHEADER_CNF_STRU            stCmdResult;
    USIMM_FILEPATH_INFO_STRU            stFilePath;
    VOS_UINT16                          usRsv;
    VOS_UINT16                          usEfLen;
    VOS_UINT8                           ucRecordNum;
    VOS_UINT8                           ucTotalNum;
    VOS_UINT8                           ucSW1;
    VOS_UINT8                           ucSW2;
    VOS_UINT8                           aucEf[255];
}PS_USIM_GET_FILE_RSP_FOR_PCLINT_STRU;


typedef struct
{
    VOS_UINT16                          usEfNum;        /* 更新的文件的个数 */
    VOS_UINT8                           aucRsv[2];      /* 预留对齐 */
    USIM_REFRESH_FILE_STRU              astRefreshFile[USIMM_MAX_REFRESH_FILE_NUM];
}NAS_MMC_USIMM_REFRESH_FILE_CONTENT_STRU;

/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/

/*****************************************************************************
  10 函数声明
*****************************************************************************/
VOS_VOID  NAS_MMC_ConverPlmnInfoFromSim(
    VOS_UINT8                          *pucData,
    VOS_UINT32                          ulPlmnNum,
    NAS_MML_SIM_PLMN_WITH_RAT_STRU     *pstPlmnWithRat
);
VOS_VOID  NAS_MMC_ConvertSimPlmnToNasPLMN(
    NAS_MML_SIM_FORMAT_PLMN_ID          *pstSimPlmn,
    NAS_MML_PLMN_ID_STRU                *pstNasPlmn
);
VOS_VOID  NAS_MMC_ConvertNasPlmnToSimFormat(
    NAS_MML_PLMN_ID_STRU               *pstGUNasPlmn,
    NAS_MML_SIM_FORMAT_PLMN_ID         *pstSimPlmn
);

#if (FEATURE_ON == FEATURE_CSG)
VOS_VOID  NAS_MMC_RcvAgentUsimQueryCsgFileCnf(
    USIMM_QUERYFILE_CNF_STRU           *pstMsg,
    VOS_UINT32                          ulIsRefresh
);
#endif


VOS_VOID NAS_MMC_RcvAgentUsimReadFileCnf(
    PS_USIM_GET_FILE_RSP_FOR_PCLINT_STRU                   *pstMsg
);

VOS_VOID NAS_MMC_RcvAgentUsimReadFileCnf_CS_CKIK_FILE_ID(
    PS_USIM_GET_FILE_RSP_FOR_PCLINT_STRU                   *pstMsg
);

VOS_VOID NAS_MMC_RcvAgentUsimReadFileCnf_CS_KC_FILE_ID(
    PS_USIM_GET_FILE_RSP_FOR_PCLINT_STRU                   *pstMsg
);

VOS_VOID NAS_MMC_RcvAgentUsimReadFileCnf_CS_LOC_FILE_ID(
    PS_USIM_GET_FILE_RSP_FOR_PCLINT_STRU                   *pstMsg
);

VOS_VOID NAS_MMC_RcvAgentUsimReadFileCnf_EHPLMNPI_FILE_ID(
    PS_USIM_GET_FILE_RSP_FOR_PCLINT_STRU                   *pstMsg
);

VOS_VOID NAS_MMC_RcvAgentUsimReadFileCnf_EHPLMN_FILE_ID(
    PS_USIM_GET_FILE_RSP_FOR_PCLINT_STRU          *pstMsg
);

VOS_VOID NAS_MMC_RcvAgentUsimReadFileCnf_FORBIDDEN_PLMN_LIST_FILE_ID(
    PS_USIM_GET_FILE_RSP_FOR_PCLINT_STRU          *pstMsg
);
VOS_VOID NAS_MMC_RcvAgentUsimReadFileCnf_HPLMN_PERI_FILE_ID(
    PS_USIM_GET_FILE_RSP_FOR_PCLINT_STRU          *pstMsg
);

VOS_VOID NAS_MMC_RcvAgentUsimReadFileCnf_HPLMN_SEL_FILE_ID(
    PS_USIM_GET_FILE_RSP_FOR_PCLINT_STRU          *pstMsg
);

VOS_VOID NAS_MMC_RcvAgentUsimReadFileCnf_LRPLMNSI_FILE_ID(
    PS_USIM_GET_FILE_RSP_FOR_PCLINT_STRU          *pstMsg
);

VOS_VOID NAS_MMC_RcvAgentUsimReadFileCnf_MNC_LENGTH_FILE_ID(
    PS_USIM_GET_FILE_RSP_FOR_PCLINT_STRU          *pstMsg
);

VOS_VOID NAS_MMC_RcvAgentUsimReadFileCnf_OPLMN_SEL_FILE_ID(
    PS_USIM_GET_FILE_RSP_FOR_PCLINT_STRU          *pstMsg
);

#if (FEATURE_ON == FEATURE_CSG)
VOS_VOID NAS_MMC_ReadUsimmRecordsInBatches(
    VOS_UINT16                          usFileId,
    VOS_UINT8                           ulTotalRecordNum,
    VOS_UINT8                           ucStartRecordNum
);
VOS_VOID NAS_MMC_ReadUsimmFileRecordNum(
    VOS_UINT16                          usFileId
);

VOS_UINT32 NAS_MMC_PaserCsgListTlvInfo(
    VOS_UINT8                              *pucPtr,
    VOS_UINT16                              ucLen,
    NAS_MML_TLV_INFO_STRU                  *pstTlvInfo
);
VOS_VOID NAS_MMC_ConverCsgIdToSim(
    VOS_UINT32                        ulCsgId,
    VOS_UINT8                        *pucCsgBuffer,
    VOS_UINT8                         ucCsgBufferLen
);
VOS_VOID NAS_MMC_ConvertCsgDisplayIndicator(
    VOS_UINT8                                               ucCsgDisplayIndicator,
    NAS_MML_CSG_ID_DISPLAY_INDICATOR_TYPE_ENUM_UINT8       *penCsgDisplayIndicator
);

VOS_UINT32 NAS_MMC_ConverCsgIdFromSim(
  VOS_UINT8                        *pucCsgBuffer,
  VOS_UINT8                         ucCsgBufferLen
);
VOS_UINT32 NAS_MMC_DecodeAcsglRecordCsgInformation
(
    NAS_MML_EFACSGL_RECORD_CSG_INFO_STRU         *pstCsgInfo,
    VOS_UINT8                                    *pucCsgStream,
    VOS_UINT8                                    *pucDecodedNum,
    VOS_UINT8                                     ucMaxLen
);
VOS_UINT32 NAS_MMC_DecodeAcsglRecordPlmn
(
    NAS_MML_EFACSGL_RECORD_PLMN_INFO_STRU        *pstPlmnInfo,
    VOS_UINT8                                    *pucPlmnStream,
    VOS_UINT8                                     ucMaxLen
);
VOS_UINT32 NAS_MMC_DecodeAcsglRecord
(
    NAS_MML_EFACSGL_RECORD_INFO_STRU             *pstRecordInfo,
    VOS_UINT8                                    *pucRecordStream,
    VOS_UINT8                                    *pucDecodedNum,
    VOS_UINT8                                     ucMaxLen
);
VOS_UINT32 NAS_MMC_CheckSpaceToAddCsgId(
    NAS_MML_ACSGL_RECORD_STREAM_STRU       *pstRecordsStream,
    VOS_UINT32                              ulReordIndex,
    VOS_UINT32                              ulAddTlvLen
);
VOS_UINT32 NAS_MMC_ConverPlmnCsgInfoToTlvObject(
    NAS_MML_EFACSGL_RECORD_PLMN_INFO_STRU      *pstRecordInfo,
    VOS_UINT8                                  *pucDataBuf,
    VOS_UINT32                                  ulBufferLen
);
VOS_UINT32 NAS_MMC_ConvertRecordInfoToStream(
    NAS_MML_EFACSGL_RECORD_INFO_STRU           *pstRecordInfo,
    VOS_UINT8                                  *pucEfData
);
VOS_VOID NAS_MMC_SndUpdateAllowedCsgListReq(
    NAS_MML_EFACSGL_RECORD_INFO_STRU           *pstRecordInfo,
    VOS_UINT32                                  ulRecordIndex
);
VOS_VOID NAS_MMC_SndUsimUpdateAllowedCsgListReq(VOS_VOID);
VOS_VOID NAS_MMC_DeleteAllowedCsgIDInUsimRecord(
    NAS_MML_PLMN_WITH_CSG_ID_INFO_STRU              *pstCsgIdInfo
);
VOS_UINT32 NAS_MMC_PaserCsgListTlvObject(
    NAS_MML_TLV_INFO_STRU                  *pstTlvObject,
    NAS_MML_PLMN_WITH_CSG_ID_LIST_STRU     *pstAllowedCsgList
);
VOS_VOID NAS_MMC_PaserCsgListRecord(
    VOS_UINT8                          *pucCsgListRecordInfo,
    VOS_UINT16                          usRecordLen,
    NAS_MML_PLMN_WITH_CSG_ID_LIST_STRU *pstAllowedCsgList
);

VOS_VOID NAS_MMC_Process_EFCSGT_FILE_RECORD(
    USIMM_DEF_FILEID_ENUM_UINT32                            enEfId,
    VOS_UINT32                          ulIsRefresh
);

VOS_VOID NAS_MMC_Process_EFHNB_FILE_RECORD(
    USIMM_DEF_FILEID_ENUM_UINT32                            enEfId,
    VOS_UINT32                          ulIsRefresh
);

VOS_VOID NAS_MMC_PaserHomeNodeBNameRecord(
    VOS_UINT8                               *pucReordData,
    VOS_UINT16                               usRecordLen,
    NAS_MML_CSG_ID_HOME_NODEB_NAME_STRU     *pstHomeNodeBName
);
VOS_VOID NAS_MMC_PaserCsgTypeRecord(
    VOS_UINT8                               *pucReordData,
    VOS_UINT16                               usRecordLen,
    NAS_MML_CSG_TYPE_STRU                   *pstCsgType
);

VOS_VOID NAS_MMC_OperatorCsgListUpdateDispalyIndicator(VOS_VOID);

VOS_VOID NAS_MMC_AddAllowedCsgIdInUsimRecord(
    NAS_MML_PLMN_WITH_CSG_ID_INFO_STRU              *pstCsgIdInfo
);
VOS_VOID NAS_MMC_RcvAgentUsimReadFileCnf_EFACSGL_FILE_RECORD(
    PS_USIM_GET_FILE_RSP_FOR_PCLINT_STRU                   *pstMsg
);

VOS_VOID NAS_MMC_RcvAgentUsimReadFileCnf_EFOCSGL_FILE_RECORD(
    PS_USIM_GET_FILE_RSP_FOR_PCLINT_STRU                   *pstMsg,
    VOS_UINT32                                              ulIsRefresh
);

VOS_VOID NAS_MMC_RcvAgentUsimReadFileCnf_EFCSGT_FILE_RECORD(
    PS_USIM_GET_FILE_RSP_FOR_PCLINT_STRU                   *pstMsg,
    USIMM_DEF_FILEID_ENUM_UINT32                            enEfId,
    VOS_UINT32                                              ulIsRefresh
);

VOS_VOID NAS_MMC_RcvAgentUsimReadFileCnf_EFHNBN_FILE_RECORD(
    PS_USIM_GET_FILE_RSP_FOR_PCLINT_STRU                   *pstMsg,
    USIMM_DEF_FILEID_ENUM_UINT32                            enEfId,
    VOS_UINT32                                              ulIsRefresh
);
VOS_VOID NAS_MMC_ReadUsimmCsgFile(VOS_VOID);

#endif
VOS_VOID NAS_MMC_RcvAgentUsimReadFileCnf_PLMN_SEL_FILE_ID(
    PS_USIM_GET_FILE_RSP_FOR_PCLINT_STRU           *pstMsg
);

VOS_VOID NAS_MMC_RcvAgentUsimReadFileCnf_PS_KC_FILE_ID(
    PS_USIM_GET_FILE_RSP_FOR_PCLINT_STRU          *pstMsg
);

VOS_VOID NAS_MMC_RcvAgentUsimReadFileCnf_PS_KEY_FILE_ID(
    PS_USIM_GET_FILE_RSP_FOR_PCLINT_STRU           *pstMsg
);

VOS_VOID NAS_MMC_RcvAgentUsimReadFileCnf_PS_LOC_FILE_ID(
    PS_USIM_GET_FILE_RSP_FOR_PCLINT_STRU          *pstMsg
);

VOS_VOID NAS_MMC_RcvAgentUsimReadFileCnf_UPLMN_SEL_FILE_ID(
    PS_USIM_GET_FILE_RSP_FOR_PCLINT_STRU          *pstMsg
);

VOS_VOID NAS_MMC_RcvAgentUsimReadFileCnf_ACTINGHPLMN_FILE_ID(
    PS_USIM_GET_FILE_RSP_FOR_PCLINT_STRU          *pstMsg
);

VOS_VOID NAS_MMC_SaveOperatorPlmnInfo(
    VOS_UINT32                          ulDataLen,
    VOS_UINT8                          *pucData
);

VOS_VOID   NAS_MMC_SndUsimReadFileReq( VOS_VOID );

VOS_VOID NAS_MMC_SndUsimUpdateForbPlmnReq(VOS_VOID);

VOS_VOID  NAS_MMC_UpdateCsSecutityInfoFromUsimFile(
    VOS_UINT8                          *pucCsSecutityInfo
);

VOS_VOID  NAS_MMC_UpdateLaiFormCsLociFile(
    VOS_UINT8                          *pucCsLociFileContent
);

VOS_VOID  NAS_MMC_UpdatePsSecutityInfoFromUsimFile(
    VOS_UINT8                          *pucPsSecutityInfo
);

VOS_VOID  NAS_MMC_UpdatePtmsiFormPsLociFile(
    VOS_UINT8                          *pucPsLociFileContent
);

VOS_VOID  NAS_MMC_UpdatePtmsiSignatureFormPsLociFile(
    VOS_UINT8                          *pucPsLociFileContent
);

VOS_VOID  NAS_MMC_UpdateRaiFormPsLociFile(
    VOS_UINT8                          *pucPsLociFileContent
);

VOS_VOID  NAS_MMC_UpdateSimInfoFromCsLociFile(
    VOS_UINT8                          *pucCsLociFileContent
);

VOS_VOID  NAS_MMC_UpdateSimInfoFromPsLociFile(
    VOS_UINT8                          *pucPsLociFileContent
);

VOS_VOID  NAS_MMC_UpdateTmsiFormCsLociFile(
    VOS_UINT8                          *pucCsLociFileContent
);

TAF_PH_PLMN_SEL_MODE NAS_MMC_ConvertSrchModeToStkSrchMode(
   VOS_UINT8                            ucReselMode
);


VOS_VOID NAS_MMC_SndStkSearchModeChgEvt(
    VOS_UINT8                           ucReselMode
);

VOS_UINT32 NAS_MMC_IsNeedSndStkSearchModeChgEvt(VOS_VOID);

VOS_VOID  NAS_MMC_UpdateOPlmnInfoFromNvim(
    VOS_UINT8                          *pucOPlmnData,
    VOS_UINT16                          usOPlmnNum
);

VOS_VOID  NAS_MMC_InsertNvimSupplementOplmnInfo(
    NAS_MML_CUSTOM_SUPPLEMENT_OPLMN_INFO_STRU              *pstNvimSupplementOplmnInfo
);

VOS_UINT8  NAS_MMC_GetUsimAllRefreshFileNumber(VOS_VOID);
VOS_UINT16  NAS_MMC_GetUsimAllRefreshFileId(VOS_UINT8 ucIndex);

VOS_VOID NAS_MMC_BuildRefreshFileList(
    USIMM_STKREFRESH_IND_STRU                   *pstUsimRefreshInd,
    NAS_MMC_USIMM_REFRESH_FILE_CONTENT_STRU     *pstRefreshFile
);



#if (VOS_OS_VER == VOS_WIN32)
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

