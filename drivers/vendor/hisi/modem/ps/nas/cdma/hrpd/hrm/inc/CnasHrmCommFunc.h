

#ifndef __CNAS_HRM_COMM_FUNC_H__
#define __CNAS_HRM_COMM_FUNC_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"
#include "PsCommonDef.h"
#include "PsLogdef.h"
#include "hrm_pif.h"
#include "cas_hrpd_airlinkmgmt_rfmng_pif.h"
#include "nas_cttf_hrpd_rsnp_pif.h"
#include "NasOmInterface.h"
#include "CnasHrmCtx.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#pragma pack(4)


/*****************************************************************************
  2 宏定义
*****************************************************************************/

/*****************************************************************************
  3 枚举定义
*****************************************************************************/

enum CNAS_HRM_HSM_AMP_MSG_ENUM
{
    CNAS_HRM_HSM_AMP_MSG_UATI_REQ                          = 0x0000,
    CNAS_HRM_HSM_AMP_MSG_UATI_ASSIGN                       = 0x0001,
    CNAS_HRM_HSM_AMP_MSG_UATI_COMPLETE                     = 0x0002,
    CNAS_HRM_HSM_AMP_MSG_HARDWAREID_REQ                    = 0x0003,
    CNAS_HRM_HSM_AMP_MSG_HARDWAREID_RESP                   = 0x0004,
    CNAS_HRM_HSM_AMP_MSG_BUTT
};
typedef VOS_UINT16 CNAS_HRM_HSM_AMP_MSG_ENUM_UINT16;


enum CNAS_HRM_HSM_SMP_MSG_ENUM
{
    CNAS_HRM_HSM_SMP_MSG_SESSION_CLOSE                 = 0x0001,
    CNAS_HRM_HSM_SMP_MSG_KEEP_ALIVE_REQ                = 0x0002,
    CNAS_HRM_HSM_SMP_MSG_KEEP_ALIVE_RESP               = 0x0003,
    CNAS_HRM_HSM_SMP_MSG_BUTT
};
typedef VOS_UINT16 CNAS_HRM_HSM_SMP_MSG_ENUM_UINT16;


enum CNAS_HRM_HLU_MSG_ENUM
{
    CNAS_HRM_HLU_HRPD_LOC_REQUEST                = 0x0003,
    CNAS_HRM_HLU_HRPD_LOC_NOTIFICATION           = 0x0004,
    CNAS_HRM_HLU_HRPD_LOC_ASSIGNMENT             = 0x0005,
    CNAS_HRM_HLU_HRPD_LOC_COMPLETE               = 0x0006,

    CNAS_HRM_HLU_HRPD_BLOB_REQUEST               = 0x0010,
    CNAS_HRM_HLU_HRPD_BLOB_NOTIFICATION          = 0x0011,
    CNAS_HRM_HLU_HRPD_BLOB_ASSIGNMENT            = 0x0012,
    CNAS_HRM_HLU_HRPD_BLOB_COMPLETE              = 0x0013,

    CNAS_HRM_HLU_MSG_BUTT
};

typedef VOS_UINT16   CNAS_HRM_HLU_MSG_ENUM_UINT16;


enum CNAS_HRM_INTERNAL_MSG_TYPE_ENUM
{
    ID_CNAS_HRM_HRM_LOG_CTX                                 = 0x0000,           /* _H2ASN_MsgChoice CNAS_HRM_HRM_LOG_CTX_STRU */

    ID_CNAS_HRM_HRM_PIF_MSG_TYPE_ENUM_BUTT
};
typedef VOS_UINT16 CNAS_HRM_INTERNAL_MSG_TYPE_ENUM_UINT16;

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
    VOS_UINT16                          usHrmMsgId;
    NAS_OTA_MSG_ID_ENUM_UINT16          enOtaMsgId;
}CNAS_HRM_MSG_TO_OTA_MSG_STRU;


typedef struct
{
    VOS_UINT32                          ulDataReqIndex;
    CNAS_HRM_DATA_REQ_TYPE_ENUM_UINT32  enDataReqType;
}CNAS_HRM_MSG_INDEX_AND_TYPE_MAP_STRU;


typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */
    CNAS_HRM_INTERNAL_MSG_TYPE_ENUM_UINT16                  enMsgId;            /* _H2ASN_Skip */
    VOS_UINT16                                              usOpId;
    CNAS_HRM_CTX_STRU                                       stHrmCtx;
}CNAS_HRM_HRM_LOG_CTX_STRU;

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
CNAS_HRM_DATA_REQ_TYPE_ENUM_UINT32 CNAS_HRM_GetDataReqTypeByMsgBuffIndex(
    VOS_UINT32                          ulBuffIndex
);

VOS_UINT8 CNAS_HRM_GetMsgBuffIndexByDataReqType(
    CNAS_HRM_DATA_REQ_TYPE_ENUM_UINT32  enDataReqType
);

VOS_UINT8 CNAS_HRM_GetMsgBuffIndexByHrmOpId(
    VOS_UINT16                          usOpId
);

RFUSER_CAS_HRPD_TASK_TYPE_ENUM_UINT16 CNAS_HRM_GetTaskTypeByDataReqType(
    CNAS_HRM_DATA_REQ_TYPE_ENUM_UINT32  enDataReqType
);

VOS_UINT8 CNAS_HRM_GetHsmMsgBuffIndexByTaskTypeExceptUati(
    RFUSER_CAS_HRPD_TASK_TYPE_ENUM_UINT16                   enTaskType
);

VOS_UINT16 CNAS_HRM_ConvertBitLenToByteLen(VOS_UINT16 usMsgBitLen);

VOS_UINT8 CNAS_HRM_IsValidTaskType(
    RFUSER_CAS_HRPD_TASK_TYPE_ENUM_UINT16                   enTaskType
);

VOS_VOID CNAS_HRM_UpdateLuResApplyState(
    CNAS_HRM_RES_APPLY_STATE_ENUM_UINT8                     enLuResApplyState
);

VOS_UINT32 CNAS_HRM_IsLuDataReqBuffExisted(VOS_VOID);

VOS_UINT32 CNAS_HRM_IsDsds1FullFlowResProtect(
    VOS_UINT8                           ucBuffIndex
);

VOS_VOID CNAS_HRM_LogOmAirMsgUp(
    CNAS_CTTF_HRPD_SNP_DATA_REQ_STRU                    *pstDataReq
);

VOS_VOID CNAS_HRM_LogHrmCtx(VOS_VOID);

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

#endif /* end of CnasHrmCommFunc.h */

