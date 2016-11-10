

#ifndef __CNAS_HSM_SND_TTF_H__
#define __CNAS_HSM_SND_TTF_H__

/*****************************************************************************
  1 The Include of the header file
*****************************************************************************/
#include "vos.h"
#include "cttf_hrpd_sig_public_pif.h"
#include "nas_cttf_hrpd_rsnp_pif.h"
#include "CnasHsmCtx.h"
#include "CnasHsmEncode.h"

#include "cnas_cttf_hrpd_pa_pif.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#pragma pack(4)

/*****************************************************************************
  2 The Macro Define
*****************************************************************************/


/*****************************************************************************
  3 The Enumeration Define
*****************************************************************************/



/*****************************************************************************
  4 The Declaration Of The Gloabal Variable
*****************************************************************************/

/*****************************************************************************
  5 The Define Of the Message Header
*****************************************************************************/


/*****************************************************************************
  6 The Define of the Message Name
*****************************************************************************/


/*****************************************************************************
  7 The Struct Define
*****************************************************************************/


/*****************************************************************************
  8 The Union Define
*****************************************************************************/


/*****************************************************************************
  9 Other Defines
*****************************************************************************/


/*****************************************************************************
  10 The Declaration Of The Function
*****************************************************************************/
#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)

VOS_VOID CNAS_HSM_SndMacTransmitAtiNotify(
    CNAS_HSM_ATI_RECORD_STRU           *pstTransATI
);

VOS_VOID CNAS_HSM_SndMacReceiveAtiNotify(
    CNAS_HSM_ATI_LIST_INFO_STRU        *pstATIListInfo
);


VOS_VOID CNAS_HSM_SndTtfHrpdPASessionCloseInd(VOS_VOID);


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

#endif /* end of CnasHsmMain.h */











