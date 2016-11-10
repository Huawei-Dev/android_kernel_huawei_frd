

/*****************************************************************************
  1 Header File Include
*****************************************************************************/
#include "CnasHsmSndTtf.h"
#include "nas_cttf_hrpd_rmac_ctrl_pif.h"
#include "CnasHsmCtx.h"
#include "CnasMntn.h"
#include "CnasHsmComFunc.h"
#include "CnasCcb.h"
#include "CnasHsmMntn.h"
#include "CnasHsmFsmTbl.h"

#include "CnasHsmFsmUatiRequest.h"
#include "Nas_Mem.h"

#ifdef  __cplusplus
#if  __cplusplus
extern "C"{
#endif
#endif

#define THIS_FILE_ID                    PS_FILE_ID_CNAS_HSM_SND_TTF_C

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)

/*****************************************************************************
  2 The Define Of The Gloabal Variable
*****************************************************************************/

/*****************************************************************************
  3 Function Define
*****************************************************************************/

VOS_VOID CNAS_HSM_SndMacTransmitAtiNotify(
    CNAS_HSM_ATI_RECORD_STRU           *pstTransATI
)
{
    CNAS_CTTF_HRPD_TRANSMIT_ATI_NTF_STRU                   *pstTransAtiNtf;
    VOS_UINT32                                              ulLen;

    ulLen = sizeof(CNAS_CTTF_HRPD_TRANSMIT_ATI_NTF_STRU) - VOS_MSG_HEAD_LENGTH;

    pstTransAtiNtf = (CNAS_CTTF_HRPD_TRANSMIT_ATI_NTF_STRU *)PS_ALLOC_MSG(UEPS_PID_HSM, ulLen);

    if (VOS_NULL_PTR == pstTransAtiNtf)
    {
        return;
    }

    NAS_MEM_SET_S(((VOS_UINT8 *)pstTransAtiNtf + VOS_MSG_HEAD_LENGTH),
                  sizeof(CNAS_CTTF_HRPD_TRANSMIT_ATI_NTF_STRU) - VOS_MSG_HEAD_LENGTH,
                  0x00,
                  sizeof(CNAS_CTTF_HRPD_TRANSMIT_ATI_NTF_STRU) - VOS_MSG_HEAD_LENGTH);

    CNAS_CFG_MSG_HDR(pstTransAtiNtf,
                     UEPS_PID_HSM,
                     CTTF_PID_HRPD_RMAC,
                     ID_CNAS_CTTF_HRPD_TRANSMIT_ATI_NTF,
                     ulLen);

    pstTransAtiNtf->usOpId = 0;

    pstTransAtiNtf->stTransmitAtiRecord.enATIType = CNAS_HSM_CovertHsmAtiTypeToTtfAtiType(
                                                        pstTransATI->enATIType);
    NAS_MEM_CPY_S(&(pstTransAtiNtf->stTransmitAtiRecord.aucATI[0]),
                  sizeof(pstTransAtiNtf->stTransmitAtiRecord.aucATI),
                  &(pstTransATI->aucATIValue[0]),
                  CTTF_HRPD_ATI_LENGTH_NUM);

    CNAS_MNTN_LogMsgInfo((MSG_HEADER_STRU*)pstTransAtiNtf);

    (VOS_VOID)PS_SEND_MSG(UEPS_PID_HSM, pstTransAtiNtf);
}


VOS_VOID CNAS_HSM_SndMacReceiveAtiNotify(
    CNAS_HSM_ATI_LIST_INFO_STRU        *pstATIListInfo
)
{
    CNAS_CTTF_HRPD_RECEIVE_ATI_NTF_STRU                    *pstRcvAtiNtf;
    VOS_UINT8                                               i;
    VOS_UINT32                                              ulLen;

    /* 添加钩包ReceiveATIList */
    CNAS_HSM_LogReceiveATIListInfo(pstATIListInfo);

    ulLen  = sizeof(CNAS_CTTF_HRPD_RECEIVE_ATI_NTF_STRU) - VOS_MSG_HEAD_LENGTH;

    pstRcvAtiNtf = (CNAS_CTTF_HRPD_RECEIVE_ATI_NTF_STRU *)PS_ALLOC_MSG(UEPS_PID_HSM, ulLen);

    if (VOS_NULL_PTR == pstRcvAtiNtf)
    {
        return;
    }

    NAS_MEM_SET_S(((VOS_UINT8 *)pstRcvAtiNtf + VOS_MSG_HEAD_LENGTH),
                  sizeof(CNAS_CTTF_HRPD_RECEIVE_ATI_NTF_STRU) - VOS_MSG_HEAD_LENGTH,
                  0x00,
                  sizeof(CNAS_CTTF_HRPD_RECEIVE_ATI_NTF_STRU) - VOS_MSG_HEAD_LENGTH);

    CNAS_CFG_MSG_HDR(pstRcvAtiNtf,
                     UEPS_PID_HSM,
                     CTTF_PID_HRPD_RMAC,
                     ID_CNAS_CTTF_HRPD_RECEIVE_ATI_NTF,
                     ulLen);

    pstRcvAtiNtf->usOpId    = 0;

    pstRcvAtiNtf->ulAtiRecordNum = CNAS_MIN(pstATIListInfo->ulATIRecordNum, CNAS_HSM_MAX_UATI_REC_NUM);

    for (i = 0; i < CNAS_MIN(pstATIListInfo->ulATIRecordNum, CNAS_HSM_MAX_UATI_REC_NUM); i++)
    {
        pstRcvAtiNtf->astAtiRecord[i].enATIType = CNAS_HSM_CovertHsmAtiTypeToTtfAtiType(
                                                      pstATIListInfo->astATIEntry[i].enATIType);
        NAS_MEM_CPY_S(&(pstRcvAtiNtf->astAtiRecord[i].aucATI[0]),
                      sizeof(pstRcvAtiNtf->astAtiRecord[i].aucATI),
                      &(pstATIListInfo->astATIEntry[i].aucATIValue[0]),
                      CTTF_HRPD_ATI_LENGTH_NUM);
    }

    /* 初始化ATItype值 */
    for ( ; i < CNAS_HSM_MAX_UATI_REC_NUM; i++)
    {
        pstRcvAtiNtf->astAtiRecord[i].enATIType = CTTF_HRPD_INACTIVE_ATI;
    }

    CNAS_MNTN_LogMsgInfo((MSG_HEADER_STRU*)pstRcvAtiNtf);

    (VOS_VOID)PS_SEND_MSG(UEPS_PID_HSM, pstRcvAtiNtf);
}



VOS_VOID CNAS_HSM_SndTtfHrpdPASessionCloseInd(VOS_VOID)
{
    CNAS_CTTF_HRPD_PA_SESSION_CLOSED_IND_STRU              *pstSessionCloseInd;
    VOS_UINT32                                              ulLen;

    ulLen = sizeof(CNAS_CTTF_HRPD_PA_SESSION_CLOSED_IND_STRU) - VOS_MSG_HEAD_LENGTH;

    pstSessionCloseInd = (CNAS_CTTF_HRPD_PA_SESSION_CLOSED_IND_STRU *)PS_ALLOC_MSG(UEPS_PID_HSM, ulLen);

    if (VOS_NULL_PTR == pstSessionCloseInd)
    {
        return;
    }

    PS_MEM_SET(((VOS_UINT8 *)pstSessionCloseInd + VOS_MSG_HEAD_LENGTH),
               0x00,
               sizeof(CNAS_CTTF_HRPD_PA_SESSION_CLOSED_IND_STRU) - VOS_MSG_HEAD_LENGTH);

    CNAS_CFG_MSG_HDR(pstSessionCloseInd,
                     UEPS_PID_HSM,
                     CTTF_PID_HRPD_RPA,
                     ID_CNAS_CTTF_HRPD_PA_SESSION_CLOSED_IND,
                     ulLen);

    (VOS_VOID)PS_SEND_MSG(UEPS_PID_HSM, pstSessionCloseInd);
}


#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */


