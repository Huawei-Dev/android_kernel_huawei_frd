

#ifndef __IMSALMMINTERFACE_H__
#define __IMSALMMINTERFACE_H__

/*****************************************************************************
  1 Include Headfile
*****************************************************************************/

#include    "vos.h"
#include    "LPSCommon.h"


/*****************************************************************************
  1.1 Cplusplus Announce
*****************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
#pragma pack(*)    设置字节对齐方式
*****************************************************************************/
#if (VOS_OS_VER != VOS_WIN32)
#pragma pack(4)
#else
#pragma pack(push, 4)
#endif

/*****************************************************************************
  2 macro
*****************************************************************************/





/*****************************************************************************
  3 Massage Declare
*****************************************************************************/

enum    IMSA_LMM_MSG_ID_ENUM
{
    ID_LMM_IMSA_ATTACH_BEGIN_IND     = (PS_MSG_ID_EMM_TO_IMSA_BASE + 0x01),     /* _H2ASN_MsgChoice LMM_IMSA_ATTACH_BEGIN_IND */
    ID_LMM_IMSA_EMC_EST_FAIL_IND     = (PS_MSG_ID_EMM_TO_IMSA_BASE + 0x02),     /* _H2ASN_MsgChoice LMM_IMSA_EMC_EST_FAIL_IND_STRU */
    ID_IMSA_LMM_MSG_ID_ENUM_BUTT
};
typedef VOS_UINT32   IMSA_LMM_MSG_ID_ENUM_UINT32;


enum    IMSA_LMM_EST_RESULT_ENUM
{
    IMSA_LMM_EST_ACCESS_BARRED_IMS_EMC_CALL      = 17,                          /* 紧急接入被bar     */
    IMSA_LMM_EST_NOT_SUPPORT_IMS_EMC_CALL        = 18,                          /* 不支持IMS紧急呼叫 */
    IMSA_LMM_EST_RESULT_ENUM_BUTT
};
typedef VOS_UINT32   IMSA_LMM_EST_RESULT_ENUM_UINT32;

/*****************************************************************************
  4 Enum
*****************************************************************************/


/*****************************************************************************
  5 STRUCT
*****************************************************************************/

typedef struct
{
    VOS_MSG_HEADER                                              /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgName;              /*_H2ASN_Skip*/
}LMM_IMSA_ATTACH_BEGIN_IND;


typedef struct
{
    VOS_MSG_HEADER                                              /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgName;              /*_H2ASN_Skip*/
    IMSA_LMM_EST_RESULT_ENUM_UINT32     enEstResult;            /* 建链失败原因 */
}LMM_IMSA_EMC_EST_FAIL_IND_STRU;
/*****************************************************************************
  6 UNION
*****************************************************************************/


/*****************************************************************************
  7 Extern Global Variable
*****************************************************************************/

/*****************************************************************************
  8 Fuction Extern
*****************************************************************************/


/*****************************************************************************
 函数名称: IMSA_LMM_IsExitImsService
 协议表格:
 ASN.1 描述:
 函数说明:IMSA提供给LMM，判断当前是否存在IMS业务
*****************************************************************************/
extern VOS_UINT32  IMSA_LMM_IsExitImsService
(
    VOS_VOID
);

/*****************************************************************************
 函数名称: LMM_Get3402TimerLen
 协议表格:
 ASN.1 描述:
 函数说明:LMM提供给IMSA接口用于获取T3402定时器时长
*****************************************************************************/
extern VOS_UINT32  LMM_Get3402TimerLen(VOS_VOID);


/*****************************************************************************
  9 OTHERS
*****************************************************************************/


#if (VOS_OS_VER != VOS_WIN32)
#pragma pack()
#else
#pragma pack(pop)
#endif




#ifdef __cplusplus
    #if __cplusplus
            }
    #endif
#endif

#endif /* end of ImsaLrrcInterface.h */









