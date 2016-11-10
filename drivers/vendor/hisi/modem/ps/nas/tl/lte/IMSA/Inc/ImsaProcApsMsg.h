

#ifndef __IMSAPROCAPSMSG_H__
#define __IMSAPROCAPSMSG_H__

/*****************************************************************************
  1 Include Headfile
*****************************************************************************/

#include    "vos.h"


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



/*****************************************************************************
  4 Enum
*****************************************************************************/




/*****************************************************************************
  5 STRUCT
*****************************************************************************/
typedef VOS_UINT32 (*IMSA_CONN_PS_EVT_FUNC)(VOS_VOID *pEvtInfo);

/*****************************************************************************
 结构名    : IMSA_CONN_PS_EVT_FUNC_TBL_STRU
 结构说明  : APS PS域事件处理函数表结构
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulEvtId;
    IMSA_CONN_PS_EVT_FUNC               pPsEvtFunc;
} IMSA_CONN_PS_EVT_FUNC_TBL_STRU;


/*****************************************************************************
  6 UNION
*****************************************************************************/


/*****************************************************************************
  7 Extern Global Variable
*****************************************************************************/

/*****************************************************************************
  8 Fuction Extern
*****************************************************************************/
extern VOS_UINT32 IMSA_CONN_ProcTafPsEvtPdpActivateCnf
(
    VOS_VOID                           *pEvtInfo
);
extern VOS_UINT32 IMSA_CONN_ProcTafPsEvtPdpActivateRej
(
    VOS_VOID                           *pEvtInfo
);
extern VOS_UINT32 IMSA_CONN_ProcTafPsEvtPdpActivateInd
(
    VOS_VOID                           *pEvtInfo
);
extern VOS_UINT32 IMSA_CONN_ProcTafPsEvtPdpModifyInd
(
    VOS_VOID                           *pEvtInfo
);
extern VOS_UINT32 IMSA_CONN_ProcTafPsEvtPdpDeactivateCnf
(
    VOS_VOID                           *pEvtInfo
);
extern VOS_UINT32 IMSA_CONN_ProcTafPsEvtPdpDeactivateInd
(
    VOS_VOID                           *pEvtInfo
);
extern VOS_UINT32 IMSA_CONN_ProcTafPsEvtPdpIpv6InfoInd
(
    VOS_VOID                           *pEvtInfo
);
extern VOS_UINT32 IMSA_CONN_ProcTafPsEvtCallEndCnf
(
    VOS_VOID                           *pEvtInfo
);
extern VOS_UINT32 IMSA_CONN_ProcTafPsEvtCallOrigCnf
(
    VOS_VOID                           *pEvtInfo
);

extern VOS_UINT32 IMSA_CONN_ProcTafPsEvtSrvccCancelInd
(
    VOS_VOID                           *pEvtInfo
);
extern VOS_VOID IMSA_CONN_ProcTimerMsgSipSignalPdpOrigExp
(
    const VOS_VOID                     *pMsg
);
extern VOS_VOID IMSA_CONN_ProcTimerMsgSipSignalPdpEndExp
(
    const VOS_VOID                     *pMsg
);
extern VOS_VOID IMSA_CONN_ProcTimerMsgWaitIpv6InfoExp
(
    const VOS_VOID                     *pMsg
);

extern VOS_VOID IMSA_CONN_ProcTafPsEvt
(
    TAF_PS_EVT_STRU                    *pstEvt
);

extern VOS_UINT32 IMSA_CONN_SipSignalPdpActOrig
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType,
    VOS_UINT8                           ucCid,
    IMSA_IP_TYPE_ENUM_UINT8             enIpType
);
extern VOS_VOID  IMSA_CONN_ProcSetuporHoConnSucc
(
    IMSA_CONN_TYPE_ENUM_UINT32                  enConnType,
    IMSA_IMS_PDN_HO_STATE_ENUM_UINT8            enHoFlag
);

extern VOS_UINT32 IMSA_CONN_ProcNoHoTafPsEvtPdpActivateCnf
(
    TAF_PS_CALL_PDP_ACTIVATE_CNF_STRU            *pstPdpActivateCnf

);
extern VOS_UINT32 IMSA_CONN_ProcHoTafPsEvtPdpActivateCnf
(
    TAF_PS_CALL_PDP_ACTIVATE_CNF_STRU            *pstPdpActivateCnf
);

extern VOS_UINT32 IMSA_CONN_ProcHoTafPsEvtPdpActivateRej
(
    TAF_PS_CALL_PDP_ACTIVATE_REJ_STRU  *pstPdpActivateRej
);
extern VOS_UINT32 IMSA_CONN_ProcNoHoTafPsEvtPdpActivateRej
(
    TAF_PS_CALL_PDP_ACTIVATE_REJ_STRU  *pstPdpActivateRej
);
extern VOS_VOID IMSA_CONN_ProcNoHoTimerMsgSipSignalPdpOrigExp
(
    const VOS_VOID                     *pMsg
);
extern VOS_VOID IMSA_CONN_ProcHoTimerMsgSipSignalPdpOrigExp
(
    const VOS_VOID                     *pMsg
);

extern VOS_VOID IMSA_CONN_ProcHoTimerMsgSipSignalPdpEndExp
(
    const VOS_VOID                     *pMsg
);
extern VOS_VOID IMSA_CONN_ProcNoHoTimerMsgSipSignalPdpEndExp
(
    const VOS_VOID                     *pMsg
);

extern VOS_VOID IMSA_CONN_ProcHoEventInDeactInd();

extern VOS_VOID IMSA_CONN_ProcHoEventInActCnf(VOS_VOID);


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

#endif /* end of ImsaProcApsMsg.h */




