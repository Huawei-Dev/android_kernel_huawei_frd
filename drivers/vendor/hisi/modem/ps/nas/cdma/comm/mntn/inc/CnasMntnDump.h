

#ifndef __CNAS_MNTN_DUMP_H__
#define __CNAS_MNTN_DUMP_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"
#include "PsCommonDef.h"
#include "PsLogdef.h"

#include "CnasCcb.h"

#include "CnasXsdCtx.h"
#include "CnasXccCtx.h"
#include "CnasXregProcess.h"
#include "CnasHsdCtx.h"
#include "CnasHsmCtx.h"
#include "CnasEhsmCtx.h"
#include "CnasHsdMntn.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#pragma pack(4)


/*****************************************************************************
  2 宏定义
*****************************************************************************/
/* 底软分配的dump空间大小，随意改动会导致复位，改动需要NAS评审 */
#define CNAS_MNTN_SAVE_EXC_LOG_LENGTH_MODEM                 (12 * 1024)

#define CNAS_MNTN_DUMP_BEGIN_TAG                            (0xAA55AA55)
#define CNAS_MNTN_DUMP_END_TAG                              (0xAA55AA55)
#define CNAS_MNTN_DUMP_CCB_INFO_START_TAG                   (0xAA550000)
#define CNAS_MNTN_DUMP_XSD_INFO_START_TAG                   (0xAA550001)
#define CNAS_MNTN_DUMP_XCC_INFO_START_TAG                   (0xAA550002)
#define CNAS_MNTN_DUMP_XREG_INFO_START_TAG                  (0xAA550003)
#define CNAS_MNTN_DUMP_HSM_INFO_START_TAG                   (0xAA550004)
#define CNAS_MNTN_DUMP_EHSM_INFO_START_TAG                  (0xAA550005)
#define CNAS_MNTN_DUMP_HSD_INFO_START_TAG                   (0xAA550006)

#define CNAS_MNTN_DUMP_XSD_REDIRECTION_INFO_CNT             (3)

#define CNAS_MNTN_DUMP_XCC_MAX_SRID_CNT                     (CNAS_XCC_MAX_SRID_NUM + 1)

#define CNAS_MNTN_DUMP_HSD_AVOID_INFO_CNT                   (3)
#define CNAS_MNTN_DUMP_HSD_REDIRECTION_INFO_CNT             (3)

#define CNAS_MNTN_DUMP_EHSM_PDN_CNT                         (3)

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
    VOS_UINT8                                               ucIsMtCallInRoamingAcc;
    VOS_UINT8                                               ucPowerOffCampOnCtrlFlg;
    VOS_UINT8                                               ucHrpdNetWorkSrchingFlg;
    CNAS_CCB_PRL_SOURCE_TYPE_ENUM_UINT8                     enPrlSrcType;
    CNAS_CCB_NW_INFO_STRU                                   stNwInfo;
    CNAS_CCB_1X_HOME_SID_NID_LIST_STRU                      stHomeSidNidList;
    CNAS_CCB_MODEM_INFO_STRU                                stModemInfo;
    CNAS_CCB_PS_RATTYPE_ENUM_UINT32                         enCurrPsRatType;
    CNAS_CCB_PS_RATTYPE_ENUM_UINT32                         enPrevPsRatType;
    CNAS_CCB_1X_CALL_STATE_STRU                             ast1xCallState[CNAS_CCB_1X_CALL_STATE_MAX_NUM];
    CNAS_CCB_1X_RETURN_CAUSE_ENUM_UINT8                     en1xReturnCause;
    VOS_UINT8                                               uc1XCallExistCount;
    VOS_UINT8                                               aucRsved[2];
    CNAS_CCB_OPER_LOCK_SYS_WHITE_LIST_STRU                  stOperLockSysWhiteList;
    CNAS_CCB_CTCC_CUSTOMIZE_INFO_STRU                       stCTCCCustInfo;
    CNAS_CCB_CDMA_STANDARD_CHANNELS_STRU                    stCdmaStandardChan;
    CNAS_PRL_FREQENCY_CHANNEL_STRU                          stOhmFreq;
    CNAS_CCB_CARD_INFO_STRU                                 stCardInfo;
    CNAS_CCB_1X_SRV_INFO_STRU                               st1xSrvInfo;
    CNAS_CCB_MS_SYS_CFG_INFO_STRU                           stMsSysCfg;
}CNAS_CCB_MNTN_SAVE_EXC_LOG_STRU;


typedef struct
{
    VOS_UINT8                           ucAvoidFreqNum;
    VOS_UINT8                           aucReserve[3];
    CNAS_XSD_AVOID_FREQ_INFO_STRU       astAvoidFreqInfo[3];
}CNAS_XSD_MNTN_AVOID_FREQ_LIST_STRU;


typedef struct
{
    CNAS_XSD_REDIRECTION_INFO_STRU                          stRedirInfo;
    CNAS_XSD_MNTN_AVOID_FREQ_LIST_STRU                      stMntnAvoidFreqList;
    CNAS_XSD_MRU_LIST_STRU                                  stMruList;
    CNAS_XSD_SCAN_CHANNEL_LIST_STRU                         stChanScanList;
    CNAS_XSD_GEO_LIST_SRCH_INFO_STRU                        stGeoListSrchInfo;
    CNAX_XSD_CAMPED_SYS_INFO_STRU                           stCurCampedSysInfo;
    CNAS_XSD_CALL_REDIAL_CTRL_INFO_STRU                     stCallRedialInfo;
    CNAS_XSD_CFREQ_LOCK_SET_PARA_STRU                       stFreqLockPara;
    CNAS_XSD_OOC_SCHEDULE_INFO_STRU                         stOocScheduleInfo;
    CNAS_XSD_EMC_CALL_CTRL_INFO_STRU                        stEmcCallInfo;
    CNAS_XSD_CDMA_1X_CUSTOM_PREF_CHANNELS_STRU              stCdma1XCustomPrefChan;
    VOS_UINT8                                               ucSrvAcqFlg;
    VOS_UINT8                                               aucReserved[3];
    CNAS_XSD_TEST_CONFIG_STRU                               stTestConfig;
    CNAS_XSD_SYS_ACQ_NVIM_CFG_STRU                          stSysAcqNvimCfg;
}CNAS_XSD_MNTN_SAVE_EXC_LOG_STRU;


typedef struct
{
    CNAS_XCC_MAIN_CTRL_CTX_STRU                             stMainCtrlCtx;
    CNAS_XCC_SRID_INFO_STRU                                 astSridInfo[CNAS_XCC_MAX_SRID_NUM + 1];
    CNAS_XCC_RECORD_ORIG_CALL_INDEX_ORDER_STRU              stOrigCallIndexOrder;
    CNAS_XCC_CALL_NVIM_CFG_STRU                             stCallNvimCfg;
    CNAS_XCC_1X_PAGING_RSP_SO_CFG_STRU                      stPagingRspSoCfg;
    VOS_UINT8                                               ucIsAlreadyNtfLmmCallStart;
    VOS_UINT8                                               aucRsvd[3];
}CNAS_XCC_MNTN_SAVE_EXC_LOG_STRU;


typedef struct
{
    CNAS_HSM_HRPD_CONN_CTRL_STRU                            stHrpdConnCtrlInfo;
    CNAS_HSM_SESSION_CTRL_STRU                              stSessionCtrlInfo;
    CNAS_HSM_HRPD_AMP_NEG_ATTRIB_STRU                       stHrpdAmpNegAttibInfo;
    CNAS_HSM_KEEP_ALIVE_CTRL_CTX_STRU                       stKeepAliveCtrlCtx;
    CNAS_HSM_MULTI_MODE_CTRL_INFO_STRU                      stMultiModeCtrlInfo;
    CNAS_HSM_SNP_DATA_REQ_OPID_CTRL_STRU                    stSnpDataReqCtrlInfo;
    CNAS_HSM_LOW_POWER_CTRL_STRU                            stLowPowerCtrlInfo;
}CNAS_HSM_MNTN_SAVE_EXC_LOG_STRU;


typedef struct
{
    VOS_UINT8                           ucPdnId;
    VOS_UINT8                           ucInUsed;
    VOS_UINT8                           ucCid;
    VOS_UINT8                           ucIsPdnActive;
    VOS_UINT32                          ulEpsbId;
    CNAS_EHSM_PDN_ADDR_STRU             stPdnAddr;
    VOS_UINT8                           ucApnLen;
    VOS_UINT8                           aucApn[7];
    CNAS_EHSM_ATTACH_TYPE_ENUM_UINT8    enAttachType;
    CNAS_EHSM_PDN_TYPE_ENUM_UINT8       enPdnType;
    VOS_UINT16                          usMtu;
    CNAS_EHSM_AUTH_TYPE_ENUM_UINT8      enAuthType;
    VOS_UINT8                           ucRetryTotalCnt;
    VOS_UINT8                           aucReserved[2];
}CNAS_EHSM_MNTN_SAVE_EXC_LOCAL_PDN_INFO_STRU;


typedef struct
{
    VOS_UINT8                           ucInUsed;
    VOS_UINT8                           ucCid;
    CNAS_EHSM_PDN_TYPE_ENUM_UINT8       enPdnType;                                                     /**< to indicate IPv4 or v6, or both */
    VOS_UINT8                           aucReserved[1];
    VOS_UINT32                          ulEpsbId;
    CNAS_EHSM_PDN_ADDR_STRU             stPdnAddr;
    VOS_UINT8                           ucApnLen;
    VOS_UINT8                           aucApn[7];
}CNAS_EHSM_MNTN_SAVE_EXC_LTE_PDN_INFO_STRU;


typedef struct
{
    CNAS_EHSM_EHRPD_STA_ENUM_UINT32                         enEhrpdState;
    EHSM_HSM_SESSION_TYPE_ENUM_UINT32                       enSessionType;
    VOS_UINT32                                              ulWorkMode;                   /* 0:从模   1:主模 */
    VOS_UINT32                                              ulAirLinkExist;               /* 0:无空口 1:空口链路存在 */
    VOS_UINT32                                              ulAutoAttachFlag;             /* 0:无空口 1:空口链路存在 */
    CNAS_EHSM_MNTN_SAVE_EXC_LOCAL_PDN_INFO_STRU             astLocalPdnBearInfo[CNAS_MNTN_DUMP_EHSM_PDN_CNT];
    CNAS_EHSM_MNTN_SAVE_EXC_LTE_PDN_INFO_STRU               astLtePdnBearInfo[CNAS_MNTN_DUMP_EHSM_PDN_CNT];
}CNAS_EHSM_MNTN_SAVE_EXC_LOG_STRU;


typedef struct
{
    VOS_UINT8                                               ucAvoidItemUsedNum;
    VOS_UINT8                                               aucReserve[2];
    CNAS_HSD_AVOID_REASON_ENUM_UINT8                        enCurrAvoidReason;
    CNAS_HSD_AVOID_FREQ_INFO_STRU                           astAvoidFreqInfo[CNAS_MNTN_DUMP_HSD_AVOID_INFO_CNT];
}CNAS_HSD_MNTN_AVOID_FREQ_LIST_STRU;


typedef struct
{
    VOS_UINT8                                               ucChanNum;
    VOS_UINT8                                               aucReseve[3];
    CAS_CNAS_HRPD_DERECTION_CHANNEL_STRU                    aulChannel[CNAS_MNTN_DUMP_HSD_REDIRECTION_INFO_CNT]; /**< Number of frequencies included in list */
} CNAS_HSD_MNTN_REDIRECTION_STRU;


typedef struct
{
    CNAS_HSD_LOG_MSG_STATUS_STRU                            stCnasHsdLogMsg;
    CNAS_HSD_SYS_MODE_TYPD_ENUM_UINT32                      enModeType;                     /* 区分混合和非混合模式 */
    CNAS_HSD_HRPD_MRU_LIST_STRU                             stMruList;                      /* 存放MRU内容 */
    CNAS_HSD_MNTN_AVOID_FREQ_LIST_STRU                      stAvoidFreqList;                /* avoid表 */
    CNAS_HSD_MNTN_REDIRECTION_STRU                          stRedirection;                  /* 重定向频点信息 */
    CNAS_HSD_HIGHPRIORITY_STRU                              stHighPriority;                 /* 高优先级频点信息 */
    CNAS_HSD_HRPD_SYSTEM_INFO_STRU                          stHrpdSysInfo;                  /* Hrpd系统驻留状态信息 */
    CNAS_HSD_OOC_SCHEDULE_INFO_STRU                         stOocScheduleInfo;              /* OOC调度信息 */
    CNAS_PRL_HRPD_SYSTEM_STRU                               stCasOhmHrpdSys;                /* overhead上报的频点信息 */
    CNAS_HSD_NETWORK_LOST_SYS_RECORD_STRU                   stNetwkLostSysRec;              /* 连续丢网的系统记录 */
    CNAS_PRL_FREQENCY_CHANNEL_STRU                          stSyncFreq;                     /* sync到的系统频点 */
    CNAS_HSD_HRPD_SYS_ACQ_SRLTE_INFO_STRU                   stSysAcqSrlteInfo;              /* srlte下搜网时的信息 */
    CNAS_HSD_1X_SYSTEM_INFO_STRU                            stHrpdMatched1xSysInfo;         /* 保存1X系统信息 */
    CNAS_HSD_SYSTEM_ASSIST_INFO_STRU                        stSysAssistInfo;                /* 系统辅助信息 */
}CNAS_HSD_MNTN_SAVE_EXC_LOG_STRU;


typedef struct
{
    CNAS_XREG_REGSTATE_STRU                 stCnasXregStateInfo;
    CNAS_XREG_SYS_MSG_CONTENT_STRU          stCnasXregSysMsgCont;
}CNAS_XREG_MNTN_SAVE_EXC_LOG_STRU;


typedef struct
{
    VOS_UINT32                                              ulCnasBeginTag;
    CNAS_CCB_LOG_MSG_STATUS_STRU                            stCnasLogMsgState;
    VOS_UINT32                                              ulCnasCcbInfoStart;
    CNAS_CCB_MNTN_SAVE_EXC_LOG_STRU                         stCnasCcbMntnSaveExcLog;
    VOS_UINT32                                              ulCnasXsdInfoStart;
    CNAS_XSD_MNTN_SAVE_EXC_LOG_STRU                         stCnasXsdMntnSaveExcLog;
    VOS_UINT32                                              ulCnasXccInfoStart;
    CNAS_XCC_MNTN_SAVE_EXC_LOG_STRU                         stCnasXccMntnSaveExcLog;
    VOS_UINT32                                              ulCnasXregInfoStart;
    CNAS_XREG_MNTN_SAVE_EXC_LOG_STRU                        stCnasXregMntnSaveExcLog;
    VOS_UINT32                                              ulCnasHsmInfoStart;
    CNAS_HSM_MNTN_SAVE_EXC_LOG_STRU                         stCnasHsmMntnSaveExcLog;
    VOS_UINT32                                              ulCnasEHsmInfoStart;
    CNAS_EHSM_MNTN_SAVE_EXC_LOG_STRU                        stCnasEHsmMntnSaveExcLog;
    VOS_UINT32                                              ulCnasHsdInfoStart;
    CNAS_HSD_MNTN_SAVE_EXC_LOG_STRU                         stCnasHsdMntnSaveExcLog;
    VOS_UINT32                                              ulCnasEndTag;
}CNAS_MNTN_SAVE_EXC_LOG_STRU;

/*****************************************************************************
  8 UNION定义
*****************************************************************************/

/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/

/*****************************************************************************
  10 函数声明
*****************************************************************************/
#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
VOS_VOID CNAS_MNTN_SaveCnasLogMsgState(
    CNAS_MNTN_SAVE_EXC_LOG_STRU        *pstCnasMntnSaveExcLog
);

VOS_VOID CNAS_MNTN_SaveCnasCcbGlobalInfo(
    CNAS_MNTN_SAVE_EXC_LOG_STRU        *pstCnasMntnSaveExcLog
);

VOS_VOID CNAS_MNTN_SaveCnasXsdGlobalInfo(
    CNAS_MNTN_SAVE_EXC_LOG_STRU        *pstCnasMntnSaveExcLog
);

VOS_VOID CNAS_MNTN_SaveCnasXccGlobalInfo(
    CNAS_MNTN_SAVE_EXC_LOG_STRU        *pstCnasMntnSaveExcLog
);

VOS_VOID CNAS_MNTN_SaveCnasHsmGlobalInfo(
    CNAS_MNTN_SAVE_EXC_LOG_STRU        *pstCnasMntnSaveExcLog
);

VOS_VOID CNAS_MNTN_SaveCnasEHsmGlobalInfo(
    CNAS_MNTN_SAVE_EXC_LOG_STRU        *pstCnasMntnSaveExcLog
);

VOS_VOID CNAS_MNTN_SaveCnasHsdGlobalInfo(
    CNAS_MNTN_SAVE_EXC_LOG_STRU        *pstCnasMntnSaveExcLog
);

VOS_VOID CNAS_MNTN_SaveCnasXregGlobalInfo(
    CNAS_MNTN_SAVE_EXC_LOG_STRU        *pstCnasMntnSaveExcLog
);

VOS_VOID  CNAS_MNTN_SaveExcLog(
    VOS_UINT8                          *pucAddr,
    VOS_UINT32                          ulLength
);

VOS_VOID CNAS_MNTN_SaveCnasExcLog( VOS_VOID );

VOS_VOID CNAS_MNTN_RegisterDumpCallBack( VOS_VOID );

#endif

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

#endif /* end of CnasMntnDump.h */
