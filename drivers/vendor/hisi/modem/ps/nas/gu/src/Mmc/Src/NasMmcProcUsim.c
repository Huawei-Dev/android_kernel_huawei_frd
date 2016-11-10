

/*****************************************************************************
  1 头文件包含
*****************************************************************************/


#include "PsTypeDef.h"
#include "NasMmSublayerDef.h"
#include "NasMmcProcUsim.h"
#include "NasMmcFsmSwitchOn.h"
#include "GmmExt.h"
#include "NasMmcCtx.h"
#include "NasMmlCtx.h"
#include "NasComm.h"
#include "NasMmlLib.h"
#include "siappstk.h"
#include "NasMmcComFunc.h"

#include "NasUsimmApi.h"

#include "NasMmcProcNvim.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define    THIS_FILE_ID        PS_FILE_ID_NAS_MMCPROCUSIM_C

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/

/* MMC 收到USIMM_REFRESH_ALL_FILE的refresh消息时，只更新高优先网络文件
   读6F7E会在注册时用TMSI而导致注册失败  */
VOS_UINT16 g_astNasUsimAllRefreshFileID[] =
{
    USIMM_USIM_EFEHPLMN_ID,
    USIMM_USIM_EFHPLMNwACT_ID,
    USIMM_USIM_EFPLMNWACT_ID,
    USIMM_USIM_EFOPLMNWACT_ID,
    USIMM_USIM_EFFPLMN_ID
};

VOS_UINT16 g_astNasGsmAllRefreshFileID[] =
{
    USIMM_GSM_EFHPLMNACT_ID,
    USIMM_GSM_EFPLMNWACT_ID,
    USIMM_GSM_EFOPLMNWACT_ID,
    USIMM_GSM_EFPLMNSEL_ID,
    USIMM_GSM_EFFPLMN_ID
};



/*lint -save -e958 */


VOS_VOID NAS_MMC_RcvAgentUsimReadFileCnf_HPLMN_SEL_FILE_ID(
    PS_USIM_GET_FILE_RSP_FOR_PCLINT_STRU          *pstMsg
)
{
    NAS_MML_SIM_PLMN_WITH_RAT_STRU                         *pstHPlmnWithRatList = VOS_NULL_PTR;
    NAS_MML_SIM_HPLMN_WITH_RAT_INFO_STRU                   *pstHplmnWithRatInfo = VOS_NULL_PTR;
    VOS_UINT8                                               ucHplmnWithRat ;

    if ( VOS_OK == pstMsg->stCmdResult.ulResult )
    {
        /* 获取全局变量的地址 */
        pstHplmnWithRatInfo = NAS_MML_GetSimHplmnWithRatList();
        pstHPlmnWithRatList = pstHplmnWithRatInfo->astHPlmnWithRatInfo;

        /* 初始化全局变量中的HPLMN信息，然后把SIM中读取的PLMN ID和RAT存储到全局变量中 */
        NAS_MML_InitSimHPlmnWithRatInfo(pstHplmnWithRatInfo);

        ucHplmnWithRat = (VOS_UINT8)(pstMsg->usEfLen / NAS_MML_PLMN_WITH_RAT_UNIT_LEN);
        if ( ucHplmnWithRat > NAS_MML_MAX_HPLMN_WITH_RAT_NUM )
        {
            ucHplmnWithRat = NAS_MML_MAX_HPLMN_WITH_RAT_NUM;
        }

        /* 将USIM中保存的PLMN信息转换为内存中的信息 */
        NAS_MMC_ConverPlmnInfoFromSim(pstMsg->aucEf, ucHplmnWithRat, pstHPlmnWithRatList);

        /* 保存PLMN的个数 */
        pstHplmnWithRatInfo->ucHPlmnWithRatNum = ucHplmnWithRat;
    }
    else
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvAgentUsimReadFileCnf_HPLMN_SEL_FILE_ID:ERROR: Read SIM/USIM Mandatory File Error:");
    }

    return;
}


VOS_VOID NAS_MMC_RcvAgentUsimReadFileCnf_FORBIDDEN_PLMN_LIST_FILE_ID(
    PS_USIM_GET_FILE_RSP_FOR_PCLINT_STRU          *pstMsg
)
{
    VOS_UINT8                           ucFplmnNum;
    VOS_UINT32                          ulTempData;
    VOS_UINT8                           ucTempNum;                              /* 存储准MMC中forbidden PlMN个数的临时变量  */
    VOS_UINT32                          i;

    NAS_MML_SIM_FORBIDPLMN_INFO_STRU   *pstForbidPlmnInfo  = VOS_NULL_PTR;
    NAS_MML_PLMN_ID_STRU               *pstForbPlmnIdList  = VOS_NULL_PTR;

    if ( VOS_OK == pstMsg->stCmdResult.ulResult )
    {
        /* 获取保存SIM卡中禁止的PLMN的全局变量信息 */
        pstForbidPlmnInfo   = NAS_MML_GetForbidPlmnInfo();
        pstForbPlmnIdList   = pstForbidPlmnInfo->astForbPlmnIdList;

        if (( NAS_MML_SIM_FILE_LEN_VALID == ( pstMsg->usEfLen % NAS_MML_SIM_PLMN_ID_LEN)))
        {                                                                           /* 长度合法                                 */
            ucFplmnNum = (VOS_UINT8)(pstMsg->usEfLen / NAS_MML_SIM_PLMN_ID_LEN);

            /* 存MML全局变量中SIM的forbidden PLMN个数   */
            pstForbidPlmnInfo->ucUsimForbPlmnNum = ucFplmnNum;

            if ( ucFplmnNum <= NAS_MML_MAX_FORBPLMN_NUM )
            {
                ucTempNum = ucFplmnNum;
            }
            else
            {
                ucTempNum = NAS_MML_MAX_FORBPLMN_NUM;
            }

            ucFplmnNum = 0;

            for (i = 0; i < ucTempNum; i++)
            {
                if ( (NAS_MMC_USIM_FILE_CONTENT_INVALID == pstMsg->aucEf[(i * NAS_MML_SIM_PLMN_ID_LEN)])
                  && (NAS_MMC_USIM_FILE_CONTENT_INVALID == pstMsg->aucEf[(i * NAS_MML_SIM_PLMN_ID_LEN) + 1])
                  && (NAS_MMC_USIM_FILE_CONTENT_INVALID == pstMsg->aucEf[(i * NAS_MML_SIM_PLMN_ID_LEN) + 2]) )
                {                                                                   /* 内容非法                                 */
                    continue;
                }

                /* 将全局变量中的PLMN信息清为0 */
                PS_MEM_SET(&(pstForbPlmnIdList[ucFplmnNum]), NAS_MML_PLMN_ID_ZERO, sizeof(NAS_MML_PLMN_ID_STRU));

                    /*
                       PLMN BCD 编码 方式

                       MCC digit 2,  MCC digit 1, ----OCTET 1
                       MNC digit 3,  MCC digit 3, ----OCTET 2
                       MNC digit 2,  MNC digit 1, ----OCTET 3
                    */
                /* 获取SIM卡中的国家号 */
                ulTempData = NAS_MML_OCTET_LOW_FOUR_BITS & pstMsg->aucEf[i * NAS_MML_SIM_PLMN_ID_LEN];
                pstForbPlmnIdList[ucFplmnNum].ulMcc = ulTempData;

                ulTempData = (NAS_MML_OCTET_HIGH_FOUR_BITS & pstMsg->aucEf[i * NAS_MML_SIM_PLMN_ID_LEN]) >> NAS_MML_OCTET_MOVE_FOUR_BITS;
                pstForbPlmnIdList[ucFplmnNum].ulMcc |= (ulTempData << NAS_MML_OCTET_MOVE_EIGHT_BITS);

                ulTempData = NAS_MML_OCTET_LOW_FOUR_BITS & pstMsg->aucEf[(i * NAS_MML_SIM_PLMN_ID_LEN) + 1];
                pstForbPlmnIdList[ucFplmnNum].ulMcc |= (ulTempData << NAS_MML_OCTET_MOVE_SIXTEEN_BITS);

                /* 获取SIM卡中的网络号 */
                ulTempData = (NAS_MML_OCTET_HIGH_FOUR_BITS & pstMsg->aucEf[(i * NAS_MML_SIM_PLMN_ID_LEN) + 1]) >> NAS_MML_OCTET_MOVE_FOUR_BITS;
                pstForbPlmnIdList[ucFplmnNum].ulMnc |= (ulTempData << NAS_MML_OCTET_MOVE_SIXTEEN_BITS);

                ulTempData = NAS_MML_OCTET_LOW_FOUR_BITS & pstMsg->aucEf[(i * NAS_MML_SIM_PLMN_ID_LEN) + 2];
                pstForbPlmnIdList[ucFplmnNum].ulMnc |= ulTempData;

                ulTempData = (NAS_MML_OCTET_HIGH_FOUR_BITS & pstMsg->aucEf[(i * NAS_MML_SIM_PLMN_ID_LEN) + 2]) >> NAS_MML_OCTET_MOVE_FOUR_BITS;
                pstForbPlmnIdList[ucFplmnNum].ulMnc |= (ulTempData << NAS_MML_OCTET_MOVE_EIGHT_BITS);
                ucFplmnNum++;
            }

            /* 初始化MMC中forbidden PLMN的个数 */
            pstForbidPlmnInfo->ucForbPlmnNum = ucFplmnNum;
        }
        else
        {                                                                           /* 长度非法                                 */
            NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvAgentUsimReadFileCnf_FORBIDDEN_PLMN_LIST_FILE_ID:ERROR: The length of USIM_READ_FILE_CNF is invalid.");
        }
    }
    else
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvAgentUsimReadFileCnf_FORBIDDEN_PLMN_LIST_FILE_ID:ERROR: Read SIM/USIM Mandatory File Error:");
    }

    if (VOS_NULL_PTR != pstForbidPlmnInfo)
    {
        NAS_NORMAL_LOG2(WUEPS_PID_MMC, "NAS_MMC_RcvAgentUsimReadFileCnf_FORBIDDEN_PLMN_LIST_FILE_ID: ucForbPlmnNum, ucUsimForbPlmnNum", pstForbidPlmnInfo->ucForbPlmnNum, pstForbidPlmnInfo->ucUsimForbPlmnNum);
        for (i = 0; i < pstForbidPlmnInfo->ucForbPlmnNum; i++)
        {
            NAS_NORMAL_LOG2(WUEPS_PID_MMC, "NAS_MMC_RcvAgentUsimReadFileCnf_FORBIDDEN_PLMN_LIST_FILE_ID: astForbPlmnIdList.ulMcc, astForbPlmnIdList.ulMnc", pstForbidPlmnInfo->astForbPlmnIdList[i].ulMcc, pstForbidPlmnInfo->astForbPlmnIdList[i].ulMnc);
        }
    }

    return;
}


VOS_VOID NAS_MMC_RcvAgentUsimReadFileCnf_UPLMN_SEL_FILE_ID(
    PS_USIM_GET_FILE_RSP_FOR_PCLINT_STRU          *pstMsg
)
{
    VOS_UINT8                           ucUplmnNum;

    NAS_MML_SIM_USERPLMN_INFO_STRU     *pstUserPlmnInfo = VOS_NULL_PTR;
    NAS_MML_SIM_PLMN_WITH_RAT_STRU     *pstUplmnAddr    = VOS_NULL_PTR;

    if (VOS_OK == pstMsg->stCmdResult.ulResult)
    {
        ucUplmnNum = (VOS_UINT8)(pstMsg->usEfLen / NAS_MML_PLMN_WITH_RAT_UNIT_LEN);

        if ( ucUplmnNum > NAS_MML_MAX_USERPLMN_NUM )
        {
            ucUplmnNum  = NAS_MML_MAX_USERPLMN_NUM;
        }

        /* 获取全局变量的地址 */
        pstUserPlmnInfo = NAS_MML_GetSimUserPlmnList();
        pstUplmnAddr    = pstUserPlmnInfo->astUserPlmnInfo;

        /* 初始化全局变量中的UPLMN信息，然后把SIM中读取的PLMN ID和RAT存储到全局变量中 */
        NAS_MML_InitSimUserPlmnInfo(pstUserPlmnInfo);

        /* 将USIM中保存的PLMN信息转换为内存中的信息 */
        NAS_MMC_ConverPlmnInfoFromSim(pstMsg->aucEf, ucUplmnNum, pstUplmnAddr);

        /* 保存UPLMN的个数 */
        pstUserPlmnInfo->ucUserPlmnNum = ucUplmnNum;
    }
    else
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvAgentUsimReadFileCnf_FORBIDDEN_PLMN_LIST_FILE_ID:ERROR: Read SIM/USIM Mandatory File Error:");
    }

    return;
}


VOS_VOID NAS_MMC_RcvAgentUsimReadFileCnf_OPLMN_SEL_FILE_ID(
    PS_USIM_GET_FILE_RSP_FOR_PCLINT_STRU          *pstMsg
)
{
    VOS_UINT32                          ulRet;
    VOS_UINT16                          usOplmnNum;
    NAS_MML_SIM_OPERPLMN_INFO_STRU     *pstOperPlmnInfo = VOS_NULL_PTR;
    NAS_MML_SIM_PLMN_WITH_RAT_STRU     *pstOplmnAddr    = VOS_NULL_PTR;

    ulRet                               = VOS_TRUE;

    ulRet = NAS_MML_IsNvimOplmnAvail();

    /* 如果用户设置的OPLMN有效，需要将NV中的OPLMN列表保存到MML全局变量中 */
    if (ulRet == VOS_TRUE)
    {
        return;
    }
    if (VOS_OK == pstMsg->stCmdResult.ulResult)
    {
        usOplmnNum = (VOS_UINT16)(pstMsg->usEfLen / NAS_MML_PLMN_WITH_RAT_UNIT_LEN);

        if ( usOplmnNum > NAS_MML_MAX_OPERPLMN_NUM )
        {
            usOplmnNum = NAS_MML_MAX_OPERPLMN_NUM;
        }

        /* 获取全局变量的地址 */
        pstOperPlmnInfo = NAS_MML_GetSimOperPlmnList();
        pstOplmnAddr    = pstOperPlmnInfo->astOperPlmnInfo;

        /* 初始化全局变量中的OPLMN信息，然后把SIM中读取的PLMN ID和RAT存储到全局变量中 */
        NAS_MML_InitSimOperPlmnInfo(pstOperPlmnInfo);

        /* 将USIM中保存的PLMN信息转换为内存中的信息 */
        NAS_MMC_ConverPlmnInfoFromSim(pstMsg->aucEf, usOplmnNum, pstOplmnAddr);

        /* 保存OPLMN的个数 */
        pstOperPlmnInfo->usOperPlmnNum = usOplmnNum;
    }
    else
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvAgentUsimReadFileCnf_OPLMN_SEL_FILE_ID:ERROR: Read SIM/USIM Mandatory File Error:");
    }

    return;
}

#if (FEATURE_ON == FEATURE_CSG)

VOS_VOID NAS_MMC_SndUsimUpdateAllowedCsgListReq(VOS_VOID)
{
    return;
}

#endif

VOS_VOID NAS_MMC_RcvAgentUsimReadFileCnf_LRPLMNSI_FILE_ID(
    PS_USIM_GET_FILE_RSP_FOR_PCLINT_STRU          *pstMsg
)
{
    NAS_MML_LAST_RPLMN_SEL_IND_ENUM_UINT8                  *penPLMNSelInd = VOS_NULL_PTR;
    NAS_MML_SIM_EHPLMN_INFO_STRU                           *pstEHPlmnInfo = VOS_NULL_PTR;

    if (VOS_OK == pstMsg->stCmdResult.ulResult)
    {
        pstEHPlmnInfo   = NAS_MML_GetSimEhplmnList();
        penPLMNSelInd   = &(pstEHPlmnInfo->enPLMNSelInd);

        *penPLMNSelInd  = NAS_MML_LRPLMN_SELECTION_IND_INVALID;

        if ( (NAS_MML_LRPLMNSI_SIM_FILE_LEN == pstMsg->usEfLen)
          && (NAS_MML_LRPLMN_SELECTION_IND_VALID == pstMsg->aucEf[0]) )
        {
            *penPLMNSelInd = NAS_MML_LRPLMN_SELECTION_IND_VALID;
        }
    }
    else
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvAgentUsimReadFileCnf_LRPLMNSI_FILE_ID:ERROR: Read SIM/USIM Mandatory File Error:");
    }

    return;

}

VOS_VOID NAS_MMC_RcvAgentUsimReadFileCnf_EHPLMNPI_FILE_ID(
    PS_USIM_GET_FILE_RSP_FOR_PCLINT_STRU          *pstMsg
)
{
    NAS_MML_SIM_EHPLMN_INFO_STRU                           *pstEHPlmnInfo       = VOS_NULL_PTR;
    NAS_MML_EHPLMN_PRESENT_IND_ENUM_UINT8                  *penEHplmnPresentInd = VOS_NULL_PTR;

    if (VOS_OK == pstMsg->stCmdResult.ulResult)
    {
        /* 取得全局变量地址 */
        pstEHPlmnInfo                       = NAS_MML_GetSimEhplmnList();
        penEHplmnPresentInd                 = &(pstEHPlmnInfo->enEHplmnPresentInd);

        if ( NAS_MML_EHPLMNPI_SIM_FILE_LEN == pstMsg->usEfLen )
        {
            *penEHplmnPresentInd        = pstMsg->aucEf[0];
        }
        else
        {
            *penEHplmnPresentInd        = NAS_MML_EHPLMN_PRESENT_IND_NO_PREF;
        }
    }
    else
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvAgentUsimReadFileCnf_EHPLMNPI_FILE_ID:ERROR: Read SIM/USIM Mandatory File Error:");
    }

    return;


}


VOS_VOID NAS_MMC_RcvAgentUsimReadFileCnf_EHPLMN_FILE_ID(
    PS_USIM_GET_FILE_RSP_FOR_PCLINT_STRU          *pstMsg
)
{
    NAS_MML_PLMN_ID_STRU                stPlmn;
    VOS_UINT32                          ulEHplmnNum;
    VOS_UINT32                          i;
    NAS_MML_SIM_FORMAT_PLMN_ID          stSimPlmn;

    NAS_MML_SIM_EHPLMN_INFO_STRU       *pstSimEHPlmnInfo = VOS_NULL_PTR;

    NAS_MML_SIM_EHPLMN_INFO_STRU        stTempSimEHPlmnInfo;

    VOS_UINT8                           ucUserCfgEhplmnListNum;
    NAS_MML_PLMN_ID_STRU                astUserCfgEhPlmnList[NAS_MML_MAX_USER_CFG_EHPLMN_NUM];

    VOS_UINT8                           ucSimEhplmnListNum;
    NAS_MML_PLMN_ID_STRU                astSimEhPlmnList[NAS_MML_MAX_EHPLMN_NUM];

    PS_MEM_SET(&stPlmn, 0, sizeof(NAS_MML_PLMN_ID_STRU));
    PS_MEM_SET(&stSimPlmn, 0, sizeof(NAS_MML_SIM_FORMAT_PLMN_ID));
    PS_MEM_SET(&stTempSimEHPlmnInfo, 0, sizeof(NAS_MML_SIM_EHPLMN_INFO_STRU));

    ucSimEhplmnListNum                  = 0;
    PS_MEM_SET(astUserCfgEhPlmnList, (VOS_UINT)0X00, sizeof(astUserCfgEhPlmnList));

    ucUserCfgEhplmnListNum              = 0;
    PS_MEM_SET(astSimEhPlmnList, (VOS_UINT)0X00, sizeof(astSimEhPlmnList));

    pstSimEHPlmnInfo = NAS_MML_GetSimEhplmnList();

    /* 如果当前为ACTING-HPLMN，则不更新 */
    if ( NAS_MML_HPLMN_TYPE_ACTING_HPLMN == NAS_MML_GetHPlmnType())
    {
        return;
    }

    /* 读卡文件失败 */
    if (VOS_OK != pstMsg->stCmdResult.ulResult)
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvAgentUsimReadFileCnf_EHPLMN_FILE_ID:ERROR: Read SIM/USIM Mandatory File Error:");
        return;
    }

    /* 读取的卡文件长度不合法，直接返回VOS_FALSE */
    if ((pstMsg->usEfLen < NAS_MML_SIM_PLMN_ID_LEN)
     || (NAS_MML_SIM_FILE_LEN_VALID != (pstMsg->usEfLen % NAS_MML_SIM_PLMN_ID_LEN)))
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvAgentUsimReadFileCnf_EHPLMN_FILE_ID:WARNING: The length of USIM_READ_FILE_CNF is invalid.");
        return;
    }

    ulEHplmnNum = (VOS_UINT32) (pstMsg->usEfLen / NAS_MML_SIM_PLMN_ID_LEN);

    if (ulEHplmnNum > NAS_MML_MAX_EHPLMN_NUM)
    {
        ulEHplmnNum = NAS_MML_MAX_EHPLMN_NUM;
    }


    /* 初始化EHPLMN信息，然后把SIM中读取的PLMN ID和RAT存储到全局变量中 */
    NAS_MML_InitSimEhPlmnInfo(&stTempSimEHPlmnInfo);

    /* 取得用户NV中配置的EHplmn信息，用户配置的EHplmn信息高于SIM卡中的EHPLMN */
    NAS_MML_GetValidUserCfgEhplmnInfo( NAS_MML_GetSimImsi(),
                                       &ucUserCfgEhplmnListNum,
                                       astUserCfgEhPlmnList );

    /* 将用户NV中配置的EHplmn信息加入到SIM中的EHPLMN信息里 */
    NAS_MMC_AddPlmnListIntoSimEhplmnInfo( ucUserCfgEhplmnListNum,
                                          astUserCfgEhPlmnList,
                                          NAS_MMC_PLMN_TYPE_HPLMN,
                                          &stTempSimEHPlmnInfo);


    /* 将USIM中保存的PLMN信息转换为内存中的信息 */
    for ( i = 0; i < ulEHplmnNum; i++ )
    {
        PS_MEM_CPY(stSimPlmn.aucSimPlmn, pstMsg->aucEf + (NAS_MML_SIM_PLMN_ID_LEN * i), NAS_MML_SIM_PLMN_ID_LEN);
        NAS_MMC_ConvertSimPlmnToNasPLMN(&stSimPlmn, &stPlmn);

        if (VOS_TRUE == NAS_MML_IsPlmnIdValid(&stPlmn))
        {
            astSimEhPlmnList[ucSimEhplmnListNum] = stPlmn;
            ucSimEhplmnListNum++;
        }
    }

    NAS_MMC_AddPlmnListIntoSimEhplmnInfo( ucSimEhplmnListNum,
                                          astSimEhPlmnList,
                                          NAS_MMC_PLMN_TYPE_HPLMN,
                                          &stTempSimEHPlmnInfo);


    /* NV中配置的和卡文件中的EHplmn的总个数为0个，则使用IMSI中的HOME PLMN */
    if ( 0 == stTempSimEHPlmnInfo.ucEhPlmnNum)
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvAgentUsimReadFileCnf_EHPLMN_FILE_ID:WARNING: VALID EHplmn number is zero.");

        /* 更新当前HPLMN的类型 */
        NAS_MML_SetHPlmnType(NAS_MML_HPLMN_TYPE_HPLMN);

        NAS_MMC_UpdateHomePlmn( NAS_MML_GetSimImsi() );

        return;
    }


    /* 更新当前HPLMN的类型 */
    NAS_MML_SetHPlmnType(NAS_MML_HPLMN_TYPE_EHPLMN);

    /* NV中配置的和卡文件中的EHplmn的总个数不为0个，则将所有有效的EHplmn信息
       更新到内存中去 */
    pstSimEHPlmnInfo->ucEhPlmnNum = stTempSimEHPlmnInfo.ucEhPlmnNum;

    PS_MEM_CPY(pstSimEHPlmnInfo->astEhPlmnInfo,
               stTempSimEHPlmnInfo.astEhPlmnInfo,
               sizeof(pstSimEHPlmnInfo->astEhPlmnInfo));

    return;
}


VOS_VOID NAS_MMC_RcvAgentUsimReadFileCnf_HPLMN_PERI_FILE_ID(
    PS_USIM_GET_FILE_RSP_FOR_PCLINT_STRU          *pstMsg
)
{
    NAS_MML_BG_SEARCH_CFG_INFO_STRU    *pstBgSearchCfg = VOS_NULL_PTR;

    if (VOS_OK == pstMsg->stCmdResult.ulResult)
    {
        pstBgSearchCfg = NAS_MML_GetBgSearchCfg();

        if (NAS_MML_HPLMN_PERI_FILE_LEN == pstMsg->usEfLen)
        {

            if (VOS_TRUE == NAS_MML_GetScanCtrlEnableFlg())
            {
                /* 0xFF时: R99及以后: 60min R97: 30 min*/
                NAS_MML_UpdateSimHplmnPeriFileContent(pstMsg->aucEf[0]);
                
                if (0xFF == pstMsg->aucEf[0])
                {
                    NAS_MML_SetSimHplmnTimerLen(NAS_MML_ATT_HPPLMN_DEFAULT_TIMER_LEN_R99 * NAS_MML_SIXTY_SECOND * NAS_MML_ONE_THOUSAND_MILLISECOND);
                }
                else if (NAS_MMC_ENS_MAX_PERI_HPLMN_ATTEMP_TIME_LEN/NAS_MMC_PERI_HPLMN_ATTEMP_STEP_SIZE <= pstMsg->aucEf[0])
                {
                    /* ENS最长25小时，但系统只支持18小时，故只允许18小时 */
                    NAS_MML_SetSimHplmnTimerLen(NAS_MMC_ENS_MAX_PERI_HPLMN_ATTEMP_TIME_LEN * NAS_MML_SIXTY_SECOND * NAS_MML_ONE_THOUSAND_MILLISECOND);
                }
                else
                {
                    NAS_MML_SetSimHplmnTimerLen(pstMsg->aucEf[0] * NAS_MMC_PERI_HPLMN_ATTEMP_STEP_SIZE * NAS_MML_SIXTY_SECOND * NAS_MML_ONE_THOUSAND_MILLISECOND);
                }
                
                NAS_NORMAL_LOG1(WUEPS_PID_MMC, "NAS_MMC_RcvAgentUsimReadFileCnf_HPLMN_PERI_FILE_ID:ucScanCtrlFlg true,aucEf[0]=%d", pstMsg->aucEf[0]);

                return;
            }

            /*协议23.011规定，最大搜索间隔为8小时，而在协议31.102中
              ，读出的该值为16进制数，因此当该值大于等于30时，使用默
              认值60min
              31.102规定，6F31这个文件的第一个字节表示时长，单位为6分钟
            */
            if ( pstMsg->aucEf[0] > (NAS_MMC_MAX_PERI_HPLMN_ATTEMP_TIME_LEN/NAS_MMC_PERI_HPLMN_ATTEMP_STEP_SIZE) )
            {
                /* orange需求:当运营商订制时，当SIM卡中的值是无效值时，
                   从NV中获取，默认值是18分钟 */
                if (VOS_TRUE == pstBgSearchCfg->stDefaultMaxHplmnTimerPeriod.ucNvimActiveFlg)
                {
                    /* Orang需求:  运营商定制值*60秒*1000毫秒 */
                    NAS_MML_SetSimHplmnTimerLen(pstBgSearchCfg->stDefaultMaxHplmnTimerPeriod.ulDefaultMaxHplmnLen);
                }
                else
                {
                    /* 默认值为60分钟*60秒*1000毫秒 */
                    NAS_MML_SetSimHplmnTimerLen(NAS_MML_SIXTY_MINUTE * NAS_MML_SIXTY_SECOND * NAS_MML_ONE_THOUSAND_MILLISECOND);
                }
            }
            else
            {
                /* SIM卡取值*6分钟*60秒*1000毫秒 */
                NAS_MML_SetSimHplmnTimerLen(pstMsg->aucEf[0] * NAS_MMC_PERI_HPLMN_ATTEMP_STEP_SIZE * NAS_MML_SIXTY_SECOND * NAS_MML_ONE_THOUSAND_MILLISECOND);
            }
        }
    }
    else
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvAgentUsimReadFileCnf_HPLMN_PERI_FILE_ID:ERROR: Read SIM/USIM Mandatory File Error:");
    }

    return;
}


VOS_VOID NAS_MMC_RcvAgentUsimReadFileCnf_MNC_LENGTH_FILE_ID(
    PS_USIM_GET_FILE_RSP_FOR_PCLINT_STRU          *pstMsg
)
{
#if (FEATURE_ON == FEATURE_CSG)
    VOS_UINT8                           ucCsgDisplayControlFlag;
#endif

    VOS_UINT8                           ucExactlyComparaFlag;

    ucExactlyComparaFlag = NAS_MML_GetPlmnExactlyComparaFlg();

    if (VOS_OK == pstMsg->stCmdResult.ulResult)
    {
        /* 31.102,4.2.18章节，SIM卡文件长度不小于3 */
        if ( pstMsg->usEfLen < 3 )
        {
            PS_NAS_LOG(WUEPS_PID_MMC, VOS_NULL, PS_LOG_LEVEL_WARNING, "Mmc_RcvAgentUsimReadFileCnf:WARNING: The length of USIM_READ_FILE_CNF(MNC length) is invalid.");
        }
        else if ( 3 == pstMsg->usEfLen )
        {
            /* 31.102,4.2.18章节，SIM卡文件的第一个字节表示Operation mode */
            NAS_MML_SetUeOperMode(pstMsg->aucEf[0]);

        #if (FEATURE_ON == FEATURE_CSG)
            ucCsgDisplayControlFlag = (0x02 & pstMsg->aucEf[2]);
            NAS_MML_SetOnlyReportOperatorCsgListFlag(ucCsgDisplayControlFlag);
        #endif

        }
        else
        {
            /* 31.102,4.2.18章节，SIM卡文件的第一个字节表示Operation mode */
            NAS_MML_SetUeOperMode(pstMsg->aucEf[0]);

         #if (FEATURE_ON == FEATURE_CSG)
            ucCsgDisplayControlFlag = (0x02 & pstMsg->aucEf[2]);
            NAS_MML_SetOnlyReportOperatorCsgListFlag(ucCsgDisplayControlFlag);
         #endif

            if (( NAS_MMC_MNC_LENGTH_TWO_BYTES_IN_IMSI == (pstMsg->aucEf[NAS_MMC_MNC_LENGTH_INDICATOR_POSITION-1] & NAS_MML_OCTET_LOW_FOUR_BITS) )
            || ( NAS_MMC_MNC_LENGTH_THREE_BYTES_IN_IMSI == (pstMsg->aucEf[NAS_MMC_MNC_LENGTH_INDICATOR_POSITION-1] & NAS_MML_OCTET_LOW_FOUR_BITS) ))
            {
                if (VOS_TRUE == ucExactlyComparaFlag)
                {
                    NAS_MML_SetUsimMncLen(pstMsg->aucEf[NAS_MMC_MNC_LENGTH_INDICATOR_POSITION-1] & NAS_MML_OCTET_LOW_FOUR_BITS);
                }
            }
            else
            {
                PS_NAS_LOG(WUEPS_PID_MMC, VOS_NULL, PS_LOG_LEVEL_WARNING, "Mmc_RcvAgentUsimReadFileCnf:WARNING: The value of USIM_READ_FILE_CNF(MNC length) is invalid.");
            }
        }
    }
    else
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvAgentUsimReadFileCnf_MNC_LENGTH_FILE_ID:ERROR: Read SIM/USIM Mandatory File Error:");
    }

    return;
}


VOS_VOID NAS_MMC_RcvAgentUsimReadFileCnf_CS_LOC_FILE_ID(
    PS_USIM_GET_FILE_RSP_FOR_PCLINT_STRU          *pstMsg
)
{
    if (VOS_OK == pstMsg->stCmdResult.ulResult)
    {
        if (NAS_MML_CS_LOCI_SIM_FILE_LEN == pstMsg->usEfLen)
        {
            NAS_MMC_UpdateSimInfoFromCsLociFile(pstMsg->aucEf);
        }
        else
        {
            NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvAgentUsimReadFileCnf_CS_LOC_FILE_ID:WARNING: The length of USIM_READ_FILE_CNF(CS location info) is invalid.");
        }
    }
    else
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvAgentUsimReadFileCnf_CS_LOC_FILE_ID:ERROR: Read SIM/USIM Mandatory File Error:");
    }

    return;
}


VOS_VOID NAS_MMC_RcvAgentUsimReadFileCnf_CS_CKIK_FILE_ID(
    PS_USIM_GET_FILE_RSP_FOR_PCLINT_STRU          *pstMsg
)
{
    if (VOS_OK == pstMsg->stCmdResult.ulResult)
    {
        if ( NAS_MML_CS_KEY_USIM_FILE_LEN == pstMsg->usEfLen)
        {
            NAS_MMC_UpdateCsSecutityInfoFromUsimFile(pstMsg->aucEf);
        }
        else
        {
            NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvAgentUsimReadFileCnf_CS_CKIK_FILE_ID:WARNING: The length of USIM_READ_FILE_CNF(CS CK and IK) is invalid.");
        }
    }
    else
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvAgentUsimReadFileCnf_CS_CKIK_FILE_ID:ERROR: Read SIM/USIM Mandatory File Error:");
    }

    return;
}


VOS_VOID NAS_MMC_RcvAgentUsimReadFileCnf_CS_KC_FILE_ID(
    PS_USIM_GET_FILE_RSP_FOR_PCLINT_STRU          *pstMsg
)
{
    if (VOS_OK == pstMsg->stCmdResult.ulResult)
    {
        if ( NAS_MML_CS_KC_SIM_FILE_LEN == pstMsg->usEfLen )
        {
            NAS_MMC_UpdateCsSecutityInfoFromUsimFile(pstMsg->aucEf);
        }
        else
        {
            NAS_WARNING_LOG(WUEPS_PID_MMC, "Mmc_RcvAgentUsimReadFileCnf:WARNING: The length of USIM_READ_FILE_CNF(CS KC) is invalid.");
        }
    }
    else
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "Mmc_RcvAgentUsimReadFileCnf:ERROR: Read SIM/USIM Mandatory File Error:");
    }

    return;
}


VOS_VOID NAS_MMC_RcvAgentUsimReadFileCnf_PS_KEY_FILE_ID(
    PS_USIM_GET_FILE_RSP_FOR_PCLINT_STRU           *pstMsg
)
{
    if (VOS_OK == pstMsg->stCmdResult.ulResult)

    {
        if ( NAS_MML_PS_KEY_USIM_FILE_LEN == pstMsg->usEfLen )
        {
            NAS_MMC_UpdatePsSecutityInfoFromUsimFile(pstMsg->aucEf);
        }
        else
        {
            NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvAgentUsimReadFileCnf_PS_KEY_FILE_ID:WARNING: The length of USIM_READ_FILE_CNF(PS key) is invalid.");
        }
    }
    else
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvAgentUsimReadFileCnf_PS_KEY_FILE_ID:ERROR: Read SIM/USIM Mandatory File Error:");
    }

    return;
}


VOS_VOID NAS_MMC_RcvAgentUsimReadFileCnf_PS_KC_FILE_ID(
    PS_USIM_GET_FILE_RSP_FOR_PCLINT_STRU          *pstMsg
)
{
    if (VOS_OK == pstMsg->stCmdResult.ulResult)
    {
        if ( NAS_MML_PS_KC_SIM_FILE_LEN == pstMsg->usEfLen )
        {
            NAS_MMC_UpdatePsSecutityInfoFromUsimFile(pstMsg->aucEf);
        }
        else
        {
            NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvAgentUsimReadFileCnf_PS_KC_FILE_ID:WARNING: The length of USIM_READ_FILE_CNF(PS KC) is invalid.");
        }
    }
    else
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvAgentUsimReadFileCnf_PS_KC_FILE_ID:ERROR: Read SIM/USIM Mandatory File Error:");
    }

    return;
}


VOS_VOID NAS_MMC_RcvAgentUsimReadFileCnf_PS_LOC_FILE_ID(
    PS_USIM_GET_FILE_RSP_FOR_PCLINT_STRU          *pstMsg
)
{
    VOS_UINT8                           aucPsLocInfo[NAS_MML_PS_LOCI_SIM_FILE_LEN];

    if (VOS_OK == pstMsg->stCmdResult.ulResult)
    {
        if ( NAS_MML_PS_LOCI_SIM_FILE_LEN == pstMsg->usEfLen )
        {
            PS_MEM_SET(aucPsLocInfo, (VOS_CHAR)0xFF, NAS_MML_PS_LOCI_SIM_FILE_LEN);

            PS_MEM_CPY(aucPsLocInfo, pstMsg->aucEf, pstMsg->usEfLen );

            /*更新MML和GMM全局变量中相关内容*/
            NAS_MMC_UpdateSimInfoFromPsLociFile(aucPsLocInfo);
        }
        else
        {
            NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvAgentUsimReadFileCnf_PS_LOC_FILE_ID:WARNING: The length of USIM_READ_FILE_CNF(PS location info) is invalid.");
        }
    }
    else
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvAgentUsimReadFileCnf_PS_LOC_FILE_ID:ERROR: Read SIM/USIM Mandatory File Error:");
    }

    return;
}




VOS_VOID NAS_MMC_RcvAgentUsimReadFileCnf_PLMN_SEL_FILE_ID(
    PS_USIM_GET_FILE_RSP_FOR_PCLINT_STRU           *pstMsg
)
{
    VOS_UINT8                           ucPlmnSetNum;
    VOS_UINT32                          i;
    NAS_MML_SIM_FORMAT_PLMN_ID          stSimPlmn;

    NAS_MML_SIM_SELPLMN_INFO_STRU      *pstSelPlmnInfo = VOS_NULL_PTR;
    NAS_MML_PLMN_ID_STRU               *pstPlmnSelList = VOS_NULL_PTR;

    PS_MEM_SET(&stSimPlmn, 0, sizeof(NAS_MML_SIM_FORMAT_PLMN_ID));

    if (VOS_OK == pstMsg->stCmdResult.ulResult)
    {
        ucPlmnSetNum   = (VOS_UINT8)(pstMsg->usEfLen / NAS_MML_SIM_PLMN_ID_LEN);
        if ( ucPlmnSetNum > NAS_MML_MAX_SELPLMN_NUM )
        {
            ucPlmnSetNum = NAS_MML_MAX_SELPLMN_NUM;
        }

        pstSelPlmnInfo = NAS_MML_GetSimSelPlmnList();
        pstPlmnSelList = pstSelPlmnInfo->astPlmnId;

        /* 初始化全局变量中的UPLMN信息，然后把SIM中读取的PLMN ID存储到全局变量中 */
        NAS_MML_InitSimSelPlmnInfo(pstSelPlmnInfo);

        for ( i = 0; i < ucPlmnSetNum; i++ )
        {
            PS_MEM_CPY(stSimPlmn.aucSimPlmn, pstMsg->aucEf + (NAS_MML_SIM_PLMN_ID_LEN * i), NAS_MML_SIM_PLMN_ID_LEN);

            NAS_MMC_ConvertSimPlmnToNasPLMN(&stSimPlmn, &pstPlmnSelList[i]);
        }

        /* 保存SELPLMN的个数 */
        pstSelPlmnInfo->ucSelPlmnNum = ucPlmnSetNum;
    }
    else
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvAgentUsimReadFileCnf_PLMN_SEL_FILE_ID:ERROR: Read SIM/USIM Mandatory File Error:");
    }

    return;
}


VOS_VOID NAS_MMC_RcvAgentUsimReadFileCnf_ACTINGHPLMN_FILE_ID(
    PS_USIM_GET_FILE_RSP_FOR_PCLINT_STRU                   *pstMsg
)
{
    NAS_MML_PLMN_ID_STRU                stPlmn;
    NAS_MML_SIM_FORMAT_PLMN_ID          stSimPlmn;
    NAS_MML_SIM_EHPLMN_INFO_STRU       *pstEHPlmnInfo   = VOS_NULL_PTR;
    NAS_MML_SIM_MS_IDENTITY_STRU       *pstMsIdentity   = VOS_NULL_PTR;

    PS_MEM_SET(&stPlmn, 0, sizeof(NAS_MML_PLMN_ID_STRU));
    PS_MEM_SET(&stSimPlmn, 0, sizeof(NAS_MML_SIM_FORMAT_PLMN_ID));

    if (VOS_OK == pstMsg->stCmdResult.ulResult)
    {
        if (pstMsg->usEfLen >= NAS_MML_SIM_PLMN_ID_LEN)
        {
            PS_MEM_CPY(stSimPlmn.aucSimPlmn, pstMsg->aucEf, NAS_MML_SIM_PLMN_ID_LEN);
            NAS_MMC_ConvertSimPlmnToNasPLMN(&stSimPlmn, &stPlmn);

            if (VOS_TRUE == NAS_MML_IsPlmnIdValid(&stPlmn))
            {
                pstEHPlmnInfo = NAS_MML_GetSimEhplmnList();

                /* 初始化全局变量中的EHPLMN信息 */
                NAS_MML_InitSimEhPlmnInfo(pstEHPlmnInfo);

                pstEHPlmnInfo->ucEhPlmnNum                  = 1;
                pstEHPlmnInfo->astEhPlmnInfo[0].stPlmnId    = stPlmn;

                /* 更新当前HPLMN的类型 */
                NAS_MML_SetHPlmnType(NAS_MML_HPLMN_TYPE_ACTING_HPLMN);

                return;
            }
        }
        else
        {
            NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvAgentUsimReadFileCnf_ACTINGHPLMN_FILE_ID:WARNING: The length of USIM_READ_FILE_CNF is invalid.");
        }
    }
    else
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvAgentUsimReadFileCnf_ACTINGHPLMN_FILE_ID:ERROR: Read SIM/USIM Mandatory File Error:");
    }

    pstMsIdentity = NAS_MML_GetSimMsIdentity();

    /* ACTING-HPLMN读取失败或无效，需要将HPLMN更新为IMSI中的HPLMN */
    NAS_MMC_UpdateHomePlmn(pstMsIdentity->aucImsi);

    /* 更新当前HPLMN的类型 */
    NAS_MML_SetHPlmnType(NAS_MML_HPLMN_TYPE_HPLMN);

    return;

}

#if (FEATURE_ON == FEATURE_CSG)


VOS_VOID NAS_MMC_ReadUsimmRecordsInBatches(
    VOS_UINT16                          usFileId,
    VOS_UINT8                           ucTotalRecordNum,
    VOS_UINT8                           ucStartRecordNum
)
{
    NAS_USIMM_GETFILE_INFO_STRU         stGetFileInfo;
    VOS_UINT8                           i;
    VOS_UINT8                           ucEndRecord;
    PS_MEM_SET(&stGetFileInfo, 0x00, sizeof(NAS_USIMM_GETFILE_INFO_STRU));

    if (ucTotalRecordNum < (ucStartRecordNum + NAS_MML_READ_SIMFILE_ONCE_MAXNUM))
    {
        ucEndRecord = ucTotalRecordNum;
    }
    else
    {
        ucEndRecord = ucStartRecordNum + NAS_MML_READ_SIMFILE_ONCE_MAXNUM - 1;
    }

    for (i=ucStartRecordNum; i<=ucEndRecord; i++)
    {
        NAS_COMM_BUILD_USIM_GET_FILE_INFO( &stGetFileInfo,
                                            USIMM_GUTL_APP,
                                            usFileId,
                                            i);
        (VOS_VOID)NAS_USIMMAPI_GetFileReq(WUEPS_PID_MMC, 0, &stGetFileInfo);
    }

}


VOS_VOID NAS_MMC_ReadUsimmFileRecordNum(
    VOS_UINT16                          usFileId
)
{
    (VOS_VOID)NAS_USIMMAPI_MaxEFRecordNumReq(WUEPS_PID_MMC, USIMM_GUTL_APP, usFileId);
}


VOS_VOID NAS_MMC_ConvertCsgDisplayIndicator(
    VOS_UINT8                                               ucCsgDisplayIndicator,
    NAS_MML_CSG_ID_DISPLAY_INDICATOR_TYPE_ENUM_UINT8       *penCsgDisplayIndicator
)
{
    switch (ucCsgDisplayIndicator)
    {
        case 0:
            *penCsgDisplayIndicator = NAS_MML_CSG_ID_DISPLAY_INDICATOR_TYPE_DISPLAY_ALL_AVAILABLE_CSG_ID;
            break;

        case 1:
            *penCsgDisplayIndicator = NAS_MML_CSG_ID_DISPLAY_INDICATOR_TYPE_DISPLAY_ONLY_OPERATOR_CSG_ID;
            break;
        default:
            *penCsgDisplayIndicator = NAS_MML_CSG_ID_DISPLAY_INDICATOR_TYPE_NOT_EXIST;
            break;
    }
}


VOS_UINT32 NAS_MMC_PaserCsgListTlvInfo(
    VOS_UINT8                          *pucPtr,
    VOS_UINT16                          ucLen,
    NAS_MML_TLV_INFO_STRU              *pstTlvInfo
)
{
    if ((VOS_NULL_PTR == pucPtr)
     || (ucLen < NAS_MML_MINIMUM_TLV_LEN)
     || (VOS_NULL_PTR == pstTlvInfo))
    {
        return VOS_FALSE;
    }

    PS_MEM_SET(pstTlvInfo, 0, sizeof(NAS_MML_TLV_INFO_STRU));
    pstTlvInfo->ucTag = pucPtr[0];

    /* TLV格式:TAG + LENTH + VALUE 其中LENTH本身所占字节数可能为1或2 */
    /* 当长度小于0x7F时 ，长度字段占一字节*/
    if  (pucPtr[1] <= NAS_MML_EFACSGL_NORMAL_LENGTH_VALUE)
    {
        pstTlvInfo->ucLength = pucPtr[1];
        pstTlvInfo->pucValue = &pucPtr[2];
        pstTlvInfo->ucOffset = 2;
    }
    /* 当长度大于0x7F时 ，长度字段占两字节，且第一个字节为0x81 */
    else if (ucLen >= 3 && NAS_MML_EFACSGL_SPECIAL_LENGTH_VALUE == pucPtr[1])
    {
        pstTlvInfo->ucLength = pucPtr[2];
        pstTlvInfo->pucValue = &pucPtr[3];
        pstTlvInfo->ucOffset = 3;
    }
    else
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}


VOS_VOID NAS_MMC_ConverCsgIdToSim(
    VOS_UINT32                        ulCsgId,
    VOS_UINT8                        *pucCsgBuffer,
    VOS_UINT8                         ucCsgBufferLen
)
{
    if ((pucCsgBuffer == NULL)
     || (ucCsgBufferLen < 4))
    {
        return ;
    }

    /* CSG ID为27bit值，CsgBuffer中依次存储最高bit到最低bit，最后的剩余bit位用1填充 */
    pucCsgBuffer[0] = (VOS_UINT8)(ulCsgId >> 19);
    pucCsgBuffer[1] = (VOS_UINT8)(ulCsgId >> 11);
    pucCsgBuffer[2] = (VOS_UINT8)(ulCsgId >> 3);
    pucCsgBuffer[3] = ((VOS_UINT8)(ulCsgId << 5)) | 0x1F;
}


VOS_UINT32 NAS_MMC_ConverCsgIdFromSim(
  VOS_UINT8                        *pucCsgBuffer,
  VOS_UINT8                         ucCsgBufferLen
)
{
    VOS_UINT32                      ucCsgId = 0x0FFFFFFF;

    if ((pucCsgBuffer == NULL) || (ucCsgBufferLen < 4))
    {
        return ucCsgId;
    }

    /* CSG ID为27bit值，CsgBuffer中依次存储最高bit到最低bit，最后的剩余bit位用1填充 */
    ucCsgId = ( ((VOS_UINT32)pucCsgBuffer[0] << 19) +
                ((VOS_UINT32)pucCsgBuffer[1] << 11) +
                ((VOS_UINT32)pucCsgBuffer[2] << 3)  +
                ((VOS_UINT32)pucCsgBuffer[3] >> 5));

    return ucCsgId;
}


VOS_UINT32 NAS_MMC_DecodeAcsglRecordCsgInformation
(
    NAS_MML_EFACSGL_RECORD_CSG_INFO_STRU         *pstCsgInfo,
    VOS_UINT8                                    *pucCsgStream,
    VOS_UINT8                                    *pucDecodedNum,
    VOS_UINT8                                     ucMaxLen
)
{
    VOS_UINT8                           ucTmpDecodedNum = 0;
    NAS_MML_TLV_INFO_STRU               stTlvInfo;

    if ((VOS_NULL_PTR == pstCsgInfo)
     || (VOS_NULL_PTR == pucCsgStream)
     || (VOS_NULL_PTR == pucDecodedNum))
    {
        return VOS_FALSE;
    }

    PS_MEM_SET(pstCsgInfo, 0, sizeof(NAS_MML_EFACSGL_RECORD_CSG_INFO_STRU));

    /* 解析CSG INFORMATION TLV */
    if (VOS_TRUE != NAS_MMC_PaserCsgListTlvInfo(&(pucCsgStream[ucTmpDecodedNum]), ucMaxLen-ucTmpDecodedNum, &stTlvInfo))
    {
        return VOS_FALSE;
    }
    ucTmpDecodedNum += stTlvInfo.ucOffset;

    /* 如果CSG INFORMATION TAG不正确或者tlv中解出来的长度比剩下的码流长度还要大，则返回错误 */
    if ((NAS_MML_EFACSGL_CSG_INFORMATION_TAG_VALUE != stTlvInfo.ucTag)
     || (stTlvInfo.ucLength > ucMaxLen - ucTmpDecodedNum))
    {
        return VOS_FALSE;
    }

    /* 长度不够解析CSG信息 */
    if (stTlvInfo.ucLength < NAS_MML_EFACSGL_CSG_INFORMATION_MIN_LENGTH)
    {
        return VOS_FALSE;
    }

    /* 保存CSG数据 */
    pstCsgInfo->ucCsgTypeIndication = pucCsgStream[ucTmpDecodedNum];
    pstCsgInfo->ucHNodeBIndication  = pucCsgStream[ucTmpDecodedNum+1];
    pstCsgInfo->ulCsgId = NAS_MMC_ConverCsgIdFromSim(&(pucCsgStream[ucTmpDecodedNum+2]), 4);

    ucTmpDecodedNum += stTlvInfo.ucLength;
    (*pucDecodedNum) += ucTmpDecodedNum;
    return VOS_TRUE;
}


VOS_UINT32 NAS_MMC_DecodeAcsglRecordPlmn
(
    NAS_MML_EFACSGL_RECORD_PLMN_INFO_STRU        *pstPlmnInfo,
    VOS_UINT8                                    *pucPlmnStream,
    VOS_UINT8                                     ucMaxLen
)
{
    VOS_UINT8                           ucDecodedNum = 0;
    NAS_MML_TLV_INFO_STRU               stTlvInfo;
    VOS_UINT32                          ulRslt;
    NAS_MML_SIM_FORMAT_PLMN_ID          stSimPlmn;

    /* 判断入参是否有效 */
    if ((VOS_NULL_PTR == pstPlmnInfo)
     || (VOS_NULL_PTR == pucPlmnStream))
    {
        return VOS_FALSE;
    }

    PS_MEM_SET(pstPlmnInfo, 0, sizeof(NAS_MML_EFACSGL_RECORD_PLMN_INFO_STRU));
    PS_MEM_SET(&stTlvInfo, 0, sizeof(NAS_MML_TLV_INFO_STRU));
    PS_MEM_SET(&stSimPlmn, 0, sizeof(NAS_MML_SIM_FORMAT_PLMN_ID));

    /* 解析PLMN TLV */
    if (VOS_TRUE != NAS_MMC_PaserCsgListTlvInfo(&(pucPlmnStream[ucDecodedNum]), ucMaxLen-ucDecodedNum, &stTlvInfo))
    {
        return VOS_FALSE;
    }
    ucDecodedNum += stTlvInfo.ucOffset;

    /* 如果PLMN TAG不正确或者tlv中解出来的长度比剩下的码流长度还要大，则返回错误 */
    if  ((NAS_MML_EFACSGL_PLMN_TAG_VALUE != stTlvInfo.ucTag)
      || (stTlvInfo.ucLength != NAS_MML_SIM_PLMN_ID_LEN)
      || (stTlvInfo.ucLength > ucMaxLen - ucDecodedNum))
    {
        return VOS_FALSE;
    }

    /* 保存PLMN */
    PS_MEM_CPY( stSimPlmn.aucSimPlmn,
                stTlvInfo.pucValue,
                NAS_MML_SIM_PLMN_ID_LEN);
    NAS_MMC_ConvertSimPlmnToNasPLMN(&stSimPlmn, &(pstPlmnInfo->stPlmnId));
    ucDecodedNum += stTlvInfo.ucLength;

    /* 循环解析CSG INFORMATION */
    while (ucDecodedNum < ucMaxLen)
    {
        ulRslt = NAS_MMC_DecodeAcsglRecordCsgInformation(   &(pstPlmnInfo->astCSGInfo[pstPlmnInfo->ucCsgNum]),
                                                            &(pucPlmnStream[ucDecodedNum]),
                                                            &ucDecodedNum,
                                                            ucMaxLen-ucDecodedNum);
        if (VOS_TRUE != ulRslt)
        {
            return VOS_FALSE;
        }
        pstPlmnInfo->ucCsgNum += 1;
    }

    /* 解析出来CSG个数为0则返回错误 */
    if (0 == pstPlmnInfo->ucCsgNum)
    {
        return VOS_FALSE;
    }
    return VOS_TRUE;
}


VOS_UINT32 NAS_MMC_DecodeAcsglRecord
(
    NAS_MML_EFACSGL_RECORD_INFO_STRU             *pstRecordInfo,
    VOS_UINT8                                    *pucRecordStream,
    VOS_UINT8                                    *pucDecodedNum,
    VOS_UINT8                                     ucMaxLen
)
{
    VOS_UINT32                          ulRslt;
    VOS_UINT8                           ucDecodedNum = 0;
    NAS_MML_TLV_INFO_STRU               stTlvInfo;

    /* 判断入参是否有效 */
    if ((VOS_NULL_PTR == pstRecordInfo)
     || (VOS_NULL_PTR == pucRecordStream)
     || (VOS_NULL_PTR == pucDecodedNum))
    {
        return VOS_FALSE;
    }

    PS_MEM_SET(pstRecordInfo, 0, sizeof(NAS_MML_EFACSGL_RECORD_INFO_STRU));

    while (ucDecodedNum < ucMaxLen)
    {
        /* 后面无效的数据表示已经解析完了 */
        if (0xFF == pucRecordStream[ucDecodedNum])
        {
            break;
        }

        /* 解析CSGLIST TLV */
        if (VOS_TRUE != NAS_MMC_PaserCsgListTlvInfo(&(pucRecordStream[ucDecodedNum]), ucMaxLen-ucDecodedNum, &stTlvInfo))
        {
            return VOS_FALSE;
        }
        ucDecodedNum += stTlvInfo.ucOffset;

        /* 如果CSGLIST TAG不正确或者tlv中解出来的长度比剩下的码流长度还要大，则返回错误 */
        if ((NAS_MML_EFACSGL_CSG_LIST_TAG_VALUE != stTlvInfo.ucTag)
         || (stTlvInfo.ucLength > ucMaxLen - ucDecodedNum))
        {
            return VOS_FALSE;
        }

        /* 解析PLMN数据 */
        ulRslt = NAS_MMC_DecodeAcsglRecordPlmn( &(pstRecordInfo->astRecordPlmnInfo[pstRecordInfo->ucPlmnNum]),
                                                &pucRecordStream[ucDecodedNum],
                                                stTlvInfo.ucLength);
        if (ulRslt != VOS_TRUE)
        {
            return VOS_FALSE;
        }
        pstRecordInfo->ucPlmnNum += 1;
        ucDecodedNum += stTlvInfo.ucLength;
    }

    /* 解析出来PLMN个数为0则返回错误 */
    if (0 == pstRecordInfo->ucPlmnNum)
    {
        return VOS_FALSE;
    }

    *pucDecodedNum = ucDecodedNum;
    return VOS_TRUE;
}


VOS_UINT32 NAS_MMC_CheckSpaceToAddCsgId(
    NAS_MML_ACSGL_RECORD_STREAM_STRU       *pstRecordsStream,
    VOS_UINT32                              ucReordIndex,
    VOS_UINT32                              ucAddTlvLen
)
{
    NAS_MML_RECORD_STREAM_STRU             *pstStream = VOS_NULL_PTR;
    VOS_UINT32                              ulRecordLen = 0;

    pstStream = pstRecordsStream->apstCsgListRecordInfo[ucReordIndex];
    ulRecordLen = pstStream->ulUsedLen + ucAddTlvLen;

    /* 长度字段处于1字节与2字节临界值时处理 */
    if ((pstStream->ulUsedLen - 2 <= NAS_MML_EFACSGL_NORMAL_LENGTH_VALUE)
     && (ulRecordLen - 2  > NAS_MML_EFACSGL_NORMAL_LENGTH_VALUE))
    {
        ulRecordLen += 1;
    }

    if (ulRecordLen <= pstRecordsStream->ulRecordLen)
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}


VOS_UINT32 NAS_MMC_ConverPlmnCsgInfoToTlvObject(
    NAS_MML_EFACSGL_RECORD_PLMN_INFO_STRU                  *pstRecordInfo,
    VOS_UINT8                                              *pucDataBuf,
    VOS_UINT32                                              ulBufferLen
)
{
    VOS_UINT32                          ulCsgListLength;
    VOS_UINT32                          ulTotalLength;
    VOS_UINT32                          ulLength = 0;
    VOS_UINT32                          i;
    NAS_MML_SIM_FORMAT_PLMN_ID          stSimPlmn;

    PS_MEM_SET(&stSimPlmn, 0, sizeof(NAS_MML_SIM_FORMAT_PLMN_ID));

    ulCsgListLength = NAS_MML_PLMN_TLV_LEN + (VOS_UINT32)(NAS_MML_CSGID_TLV_LEN * pstRecordInfo->ucCsgNum);
    ulTotalLength = ulCsgListLength + NAS_MML_MINIMUM_TLV_LEN;

    if (ulCsgListLength > NAS_MML_EFACSGL_NORMAL_LENGTH_VALUE)
    {
        ulTotalLength++;
    }

    /* Buffer长度保护 */
    if (ulTotalLength > ulBufferLen)
    {
        return 0;
    }

    /* CSG List Tag */
    pucDataBuf[ulLength++] = NAS_MML_EFACSGL_CSG_LIST_TAG_VALUE;

    /* CSG List Lenght */
    if (ulCsgListLength > NAS_MML_EFACSGL_NORMAL_LENGTH_VALUE)
    {
        pucDataBuf[ulLength++] = NAS_MML_EFACSGL_SPECIAL_LENGTH_VALUE;
        pucDataBuf[ulLength++] = (VOS_UINT8)ulCsgListLength;
    }
    else
    {
        pucDataBuf[ulLength++] = (VOS_UINT8)ulCsgListLength;
    }

    /* PLMN Tag */
    pucDataBuf[ulLength++] = NAS_MML_EFACSGL_PLMN_TAG_VALUE;

    /* PLMN Lenght */
    pucDataBuf[ulLength++] = NAS_MML_SIM_PLMN_ID_LEN;

    /* PLMN */
    NAS_MMC_ConvertNasPlmnToSimFormat(&(pstRecordInfo->stPlmnId), &(stSimPlmn));
    PS_MEM_CPY(&(pucDataBuf[ulLength]), &(stSimPlmn), NAS_MML_USIM_PLMN_LEN);
    ulLength += NAS_MML_USIM_PLMN_LEN;

    for (i=0; i<pstRecordInfo->ucCsgNum; i++)
    {
        /* CSGID Tag */
        pucDataBuf[ulLength++] = NAS_MML_EFACSGL_CSG_INFORMATION_TAG_VALUE;

        /* CSGID Lenght */
        pucDataBuf[ulLength++] = NAS_MML_EFACSGL_CSG_INFORMATION_MIN_LENGTH;

        /* CSG Type Indication */
        pucDataBuf[ulLength++] = pstRecordInfo->astCSGInfo[i].ucCsgTypeIndication;

        /* HNB Name Indication */
        pucDataBuf[ulLength++] = pstRecordInfo->astCSGInfo[i].ucHNodeBIndication;

        /* CSGID */
        NAS_MMC_ConverCsgIdToSim(pstRecordInfo->astCSGInfo[i].ulCsgId, &(pucDataBuf[ulLength]), NAS_MML_EFACSGL_CSG_ID_MIN_LENGTH);
        ulLength += NAS_MML_EFACSGL_CSG_ID_MIN_LENGTH;
    }

    return ulLength;
}



VOS_UINT32 NAS_MMC_ConvertRecordInfoToStream(
    NAS_MML_EFACSGL_RECORD_INFO_STRU   *pstRecordInfo,
    VOS_UINT8                          *pucEfData
)
{
    VOS_UINT32                          i;
    VOS_UINT32                          ulDataLen = 0;
    VOS_UINT8                          *pucBufPtr;
    VOS_UINT32                          ulBufLen;
    VOS_UINT32                          ulTotalLen = 0;

    pucBufPtr = pucEfData;
    ulBufLen = NAS_MML_GetAllowedCsgListRecordStream()->ulRecordLen;

    /* 将一条记录中的TLV Object逐条转成码流 */
    for (i = 0; i < pstRecordInfo->ucPlmnNum; i++)
    {
        ulDataLen = NAS_MMC_ConverPlmnCsgInfoToTlvObject(&(pstRecordInfo->astRecordPlmnInfo[i]),
                                                         pucBufPtr,
                                                         ulBufLen);
        pucBufPtr += ulDataLen;
        if (ulDataLen >= ulBufLen)
        {
            break;
        }
        ulBufLen -= ulDataLen;
        ulTotalLen += ulDataLen;
    }

    return ulTotalLen;
}



VOS_VOID NAS_MMC_SndUpdateAllowedCsgListReq(
    NAS_MML_EFACSGL_RECORD_INFO_STRU   *pstRecordInfo,
    VOS_UINT32                          ulRecordIndex
)
{
    NAS_USIMM_UPDATEFILE_INFO_STRU      stSetFileInfo;
    VOS_UINT32                          ulEfRecordLen;
    VOS_UINT32                          ulRecordUsedLen;
    VOS_UINT8                           aucEfData[NAS_MML_MAX_USIMM_FILE_RECORD_LEN];
    NAS_MML_RECORD_STREAM_STRU         *pstRecordStream;

    pstRecordStream = NAS_MML_GetAllowedCsgListRecordStream()->apstCsgListRecordInfo[ulRecordIndex];

    /* 默认填充无效字符 */
    PS_MEM_SET(aucEfData, NAS_MML_EFACSGL_INVALID_BIT_VALUE, sizeof(aucEfData));

    /* 转结构体为码流*/
    ulRecordUsedLen = NAS_MMC_ConvertRecordInfoToStream(pstRecordInfo, (VOS_UINT8*)aucEfData);

    /* 保存到内存结构stAllowedCsgListRecordsStream中 */
    pstRecordStream->ulUsedLen = ulRecordUsedLen;
    PS_MEM_CPY(pstRecordStream->aucEf, aucEfData, NAS_MML_MAX_USIMM_FILE_RECORD_LEN);

    ulEfRecordLen = NAS_MML_GetAllowedCsgListRecordStream()->ulRecordLen;
    PS_MEM_SET(&stSetFileInfo, 0x00, sizeof(stSetFileInfo));

    NAS_COMM_BUILD_USIM_SET_FILE_INFO( &stSetFileInfo,
                                        USIMM_GUTL_APP,
                                        USIMM_USIM_EFACSGL_ID,
                                        (VOS_UINT8)(ulRecordIndex+1),
                                        (VOS_UINT8)ulEfRecordLen,
                                        aucEfData);

    (VOS_VOID)NAS_USIMMAPI_SetFileReq(WUEPS_PID_MMC, 0, &stSetFileInfo);

    return;
}


VOS_VOID NAS_MMC_AddCsgIdToPlmnOfRecordInfo(
    NAS_MML_PLMN_WITH_CSG_ID_INFO_STRU                     *pstCsgIdInfo,
    NAS_MML_EFACSGL_RECORD_PLMN_INFO_STRU                  *pstPlmnInfo
)
{
    pstPlmnInfo->astCSGInfo[pstPlmnInfo->ucCsgNum].ulCsgId = pstCsgIdInfo->stPlmnWithCsgId.ulCsgId;
    pstPlmnInfo->astCSGInfo[pstPlmnInfo->ucCsgNum].ucCsgTypeIndication = pstCsgIdInfo->stCsgType.ucCsgTypeIndication;
    pstPlmnInfo->astCSGInfo[pstPlmnInfo->ucCsgNum].ucHNodeBIndication = pstCsgIdInfo->stHomeNodeBName.ucHomeNodebNameIndication;
    pstPlmnInfo->ucCsgNum++;
}


VOS_VOID NAS_MMC_AddAllowedCsgIdInUsimRecord(
    NAS_MML_PLMN_WITH_CSG_ID_INFO_STRU *pstCsgIdInfo
)
{
    NAS_MML_EFACSGL_RECORD_INFO_STRU                       *pstRecordInfo = VOS_NULL_PTR;
    NAS_MML_ACSGL_RECORD_STREAM_STRU                       *pstRecordsStream = VOS_NULL_PTR;
    NAS_MML_EFACSGL_RECORD_PLMN_INFO_STRU                  *pstPlmnInfo = VOS_NULL_PTR;
    VOS_UINT32                                              i;
    VOS_UINT32                                              j;
    VOS_UINT32                                              ulRet;
    VOS_UINT32                                              ulEmptyRecordNum;   /* 记录首条空记录号 */
    VOS_UINT8                                               pucDecodedNum;

    pstRecordInfo    = NAS_MML_GetAllowedCsgListRecordInfo();
    pstRecordsStream = NAS_MML_GetAllowedCsgListRecordStream();

    ulEmptyRecordNum = pstRecordsStream->ulTotalRecordNum;

    for (i=0; i<pstRecordsStream->ulTotalRecordNum; i++)
    {
        /* 是否空记录 */
        if (0 == pstRecordsStream->apstCsgListRecordInfo[i]->ulUsedLen)
        {
            /* 记录首条空记录 */
            if (pstRecordsStream->ulTotalRecordNum == ulEmptyRecordNum)
            {
                ulEmptyRecordNum = i;
            }
            continue;
        }

        /* 解析码流 */
        ulRet = NAS_MMC_DecodeAcsglRecord(  pstRecordInfo,
                                            &(pstRecordsStream->apstCsgListRecordInfo[i]->aucEf[0]),
                                            &pucDecodedNum,
                                            (VOS_UINT8)pstRecordsStream->ulRecordLen);
        if (VOS_FALSE == ulRet)
        {
            continue;
        }

        /* 查找PLMN */
        for (j=0; j<pstRecordInfo->ucPlmnNum; j++)
        {
            pstPlmnInfo = &(pstRecordInfo->astRecordPlmnInfo[j]);
            if (VOS_TRUE == NAS_MML_CompareBcchPlmnwithSimPlmn(  &(pstCsgIdInfo->stPlmnWithCsgId.stPlmnId),
                                                                &(pstPlmnInfo->stPlmnId)))
            {
                /* 是否有剩余空间*/
                if (VOS_TRUE == NAS_MMC_CheckSpaceToAddCsgId(pstRecordsStream, i, NAS_MML_CSGID_TLV_LEN))
                {
                    /* 增加记录 */
                    NAS_MMC_AddCsgIdToPlmnOfRecordInfo(pstCsgIdInfo, pstPlmnInfo);

                    /* 写卡 */
                     NAS_MMC_SndUpdateAllowedCsgListReq(pstRecordInfo, i);
                    return;
                }
            }
        }
    }

    /* 获取空记录 */
    if (ulEmptyRecordNum < pstRecordsStream->ulTotalRecordNum)
    {
        /* 增加记录 */
        PS_MEM_SET(pstRecordInfo, 0, sizeof(NAS_MML_EFACSGL_RECORD_INFO_STRU));
        pstRecordInfo->ucPlmnNum = 1;
        pstPlmnInfo = &(pstRecordInfo->astRecordPlmnInfo[0]);
        PS_MEM_CPY(&(pstPlmnInfo->stPlmnId), &(pstCsgIdInfo->stPlmnWithCsgId.stPlmnId), sizeof(NAS_MML_PLMN_ID_STRU));
        NAS_MMC_AddCsgIdToPlmnOfRecordInfo(pstCsgIdInfo, pstPlmnInfo);

        /* 写卡 */
        NAS_MMC_SndUpdateAllowedCsgListReq(pstRecordInfo, ulEmptyRecordNum);
        return;
    }

    /*  无空记录，找可用的记录 */
    for (i=0; i<pstRecordsStream->ulTotalRecordNum; i++)
    {
        /* 是否有剩余空间*/
        if (VOS_TRUE == NAS_MMC_CheckSpaceToAddCsgId(pstRecordsStream, i, NAS_MML_PLMN_AND_CSGID_TLV_LEN))
        {
            /* 解析该记录 */
            ulRet = NAS_MMC_DecodeAcsglRecord(  pstRecordInfo,
                                                &(pstRecordsStream->apstCsgListRecordInfo[i]->aucEf[0]),
                                                &pucDecodedNum,
                                                (VOS_UINT8)(pstRecordsStream->ulRecordLen));
            if (VOS_FALSE == ulRet)
            {
                continue;
            }

            if (pstRecordInfo->ucPlmnNum < NAS_MML_MAX_PLMN_CSG_ID_NUM)
            {
                /* 增加记录 */
                pstPlmnInfo = &(pstRecordInfo->astRecordPlmnInfo[pstRecordInfo->ucPlmnNum]);
                pstRecordInfo->ucPlmnNum++;
                PS_MEM_CPY(&(pstPlmnInfo->stPlmnId), &(pstCsgIdInfo->stPlmnWithCsgId.stPlmnId), sizeof(NAS_MML_PLMN_ID_STRU));
                NAS_MMC_AddCsgIdToPlmnOfRecordInfo(pstCsgIdInfo, pstPlmnInfo);

                /* 写卡 */
                NAS_MMC_SndUpdateAllowedCsgListReq(pstRecordInfo, i);
                return;
            }
        }
    }

    NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_AddAllowedCsgIdInUsimRecord:ERROR: Set USIM EFACSGL File Error:");

    return;
}


VOS_VOID NAS_MMC_DeleteAllowedCsgIDInUsimRecord(
    NAS_MML_PLMN_WITH_CSG_ID_INFO_STRU              *pstCsgIdInfo
)
{
    NAS_MML_EFACSGL_RECORD_INFO_STRU       *pstRecordInfo;
    NAS_MML_ACSGL_RECORD_STREAM_STRU       *pstRecordsStream;
    NAS_MML_EFACSGL_RECORD_PLMN_INFO_STRU  *pstPlmnInfo;
    NAS_MML_EFACSGL_RECORD_CSG_INFO_STRU   *pstCsgInfo;
    VOS_UINT32                              i;
    VOS_UINT32                              j;
    VOS_UINT32                              k;
    VOS_UINT32                              ulRet;
    VOS_UINT8                               pucDecodedNum;


    pstRecordInfo = NAS_MML_GetAllowedCsgListRecordInfo();
    pstRecordsStream = NAS_MML_GetAllowedCsgListRecordStream();

    for (i=0; i<pstRecordsStream->ulTotalRecordNum; i++)
    {
        /* 解析码流 */
        ulRet = NAS_MMC_DecodeAcsglRecord(  pstRecordInfo,
                                            &(pstRecordsStream->apstCsgListRecordInfo[i]->aucEf[0]),
                                            &pucDecodedNum,
                                            (VOS_UINT8)pstRecordsStream->ulRecordLen);
        if (VOS_FALSE == ulRet)
        {
            continue;
        }

        /* 查找PLMN */
        for (j=0; j<pstRecordInfo->ucPlmnNum; j++)
        {
            pstPlmnInfo = &(pstRecordInfo->astRecordPlmnInfo[j]);
            if (VOS_TRUE == NAS_MML_CompareBcchPlmnwithSimPlmn(  &(pstCsgIdInfo->stPlmnWithCsgId.stPlmnId),
                                                                &(pstPlmnInfo->stPlmnId)))
            {
                for (k=0; k< pstPlmnInfo->ucCsgNum; k++)
                {
                    pstCsgInfo = &(pstPlmnInfo->astCSGInfo[k]);
                    /* 查找CSGID */
                    if (pstCsgIdInfo->stPlmnWithCsgId.ulCsgId == pstCsgInfo->ulCsgId)
                    {
                        /* 删除记录 */
                        if (pstPlmnInfo->ucCsgNum > 1)
                        {
                            if ((k+1) < pstPlmnInfo->ucCsgNum)     /*不是数组最后一个 */
                            {
                                PS_MEM_MOVE(&(pstPlmnInfo->astCSGInfo[k]),
                                            &(pstPlmnInfo->astCSGInfo[k+1]),
                                            (pstPlmnInfo->ucCsgNum - k) * sizeof(NAS_MML_EFACSGL_RECORD_CSG_INFO_STRU));
                            }
                            pstPlmnInfo->ucCsgNum--;
                        }
                        /* PLMN下只有一个CSGID,且记录中不止一个PLMN */
                        else if (pstRecordInfo->ucPlmnNum > 1)
                        {
                            if ((j+1) < pstRecordInfo->ucPlmnNum)
                            {
                                PS_MEM_MOVE(&(pstRecordInfo->astRecordPlmnInfo[j]),
                                            &(pstRecordInfo->astRecordPlmnInfo[j+1]),
                                            (pstRecordInfo->ucPlmnNum - j) * sizeof(NAS_MML_EFACSGL_RECORD_PLMN_INFO_STRU));
                            }
                            pstRecordInfo->ucPlmnNum--;
                        }
                        /* PLMN下只有一个CSGID，且记录中只有一个PLMN */
                        else
                        {
                            pstRecordInfo->ucPlmnNum = 0;
                        }

                        /* 写卡 */
                        NAS_MMC_SndUpdateAllowedCsgListReq(pstRecordInfo, i);
                        return;
                    }
                }
            }
        }
    }

    return;
}


VOS_UINT32 NAS_MMC_PaserCsgListTlvObject(
    NAS_MML_TLV_INFO_STRU                  *pstTlvObject,
    NAS_MML_PLMN_WITH_CSG_ID_LIST_STRU     *pstAllowedCsgList
)
{
    VOS_UINT8                           ucPaserdLen = 0;
    VOS_UINT8                           *pucPtr = VOS_NULL_PTR;
    NAS_MML_TLV_INFO_STRU               stTlvInfo;
    NAS_MML_PLMN_WITH_CSG_ID_INFO_STRU  stPlmnWithCsgIdInfo;
    NAS_MML_SIM_FORMAT_PLMN_ID          stSimPlmn;
    NAS_MML_PLMN_WITH_CSG_ID_INFO_STRU  *pstPlmnWithCsgIdInfo = VOS_NULL_PTR;

    PS_MEM_SET(&stPlmnWithCsgIdInfo, 0, sizeof(NAS_MML_PLMN_WITH_CSG_ID_INFO_STRU));
    PS_MEM_SET(&stSimPlmn, 0, sizeof(NAS_MML_SIM_FORMAT_PLMN_ID));

    /* parsing the PLMN TLV */
    if (VOS_FALSE == NAS_MMC_PaserCsgListTlvInfo(   pstTlvObject->pucValue,
                                            pstTlvObject->ucLength,
                                            &stTlvInfo))
    {
        return VOS_FALSE;
    }

    ucPaserdLen += stTlvInfo.ucOffset;
    if ((NAS_MML_EFACSGL_PLMN_TAG_VALUE != stTlvInfo.ucTag)
     || (stTlvInfo.ucLength > (pstTlvObject->ucLength - ucPaserdLen))
     || (stTlvInfo.ucLength != NAS_MML_SIM_PLMN_ID_LEN) )
    {
        return VOS_FALSE;
    }

    /* Save the PLMN */
    PS_MEM_CPY( stSimPlmn.aucSimPlmn,
                stTlvInfo.pucValue,
                NAS_MML_SIM_PLMN_ID_LEN);
    NAS_MMC_ConvertSimPlmnToNasPLMN(&stSimPlmn, &(stPlmnWithCsgIdInfo.stPlmnWithCsgId.stPlmnId));

    ucPaserdLen += stTlvInfo.ucLength;
    pucPtr = pstTlvObject->pucValue + ucPaserdLen;

    /* parsing the CSG info TLV */
    while (ucPaserdLen < pstTlvObject->ucLength)
    {
        if (pstAllowedCsgList->ulPlmnWithCsgIdListNum >= NAS_MML_MAX_PLMN_CSG_ID_NUM)
        {
            break;
        }

        if (VOS_FALSE == NAS_MMC_PaserCsgListTlvInfo(   pucPtr,
                                                        (pstTlvObject->ucLength - ucPaserdLen),
                                                       &stTlvInfo))
        {
            return VOS_FALSE;
        }

        ucPaserdLen += stTlvInfo.ucOffset;
        if (stTlvInfo.ucLength > (pstTlvObject->ucLength - ucPaserdLen))
        {
            return VOS_FALSE;
        }

        if ((NAS_MML_EFACSGL_CSG_INFORMATION_TAG_VALUE == stTlvInfo.ucTag)
         && (stTlvInfo.ucLength >= NAS_MML_EFACSGL_CSG_INFORMATION_MIN_LENGTH))
        {
            /* Save CSG ID */
            stPlmnWithCsgIdInfo.stPlmnWithCsgId.ulCsgId = NAS_MMC_ConverCsgIdFromSim(&(stTlvInfo.pucValue[2]), NAS_MML_EFACSGL_CSG_ID_MIN_LENGTH);
            stPlmnWithCsgIdInfo.stCsgType.ucCsgTypeIndication = stTlvInfo.pucValue[0];
            stPlmnWithCsgIdInfo.stHomeNodeBName.ucHomeNodebNameIndication = stTlvInfo.pucValue[1];

            PS_MEM_CPY( &(pstAllowedCsgList->astPlmnWithCsgIdInfo[pstAllowedCsgList->ulPlmnWithCsgIdListNum]),
                        &stPlmnWithCsgIdInfo,
                        sizeof(NAS_MML_PLMN_WITH_CSG_ID_INFO_STRU));

            (pstAllowedCsgList->ulPlmnWithCsgIdListNum)++;

            ucPaserdLen += stTlvInfo.ucLength;
        }
        else if (NAS_MML_EFOCSGL_CSG_DISP_INDICATOR_TAG_VALUE == stTlvInfo.ucTag && stTlvInfo.ucLength >= 1)
        {
            /* 此处暂认为0x82为最后一条，且此PLMN必然有0x81，先加到上一条CSGID中，待文件读完统一处理 */
            pstPlmnWithCsgIdInfo = &(pstAllowedCsgList->astPlmnWithCsgIdInfo[pstAllowedCsgList->ulPlmnWithCsgIdListNum - 1]);
            NAS_MMC_ConvertCsgDisplayIndicator(stTlvInfo.pucValue[0], &(pstPlmnWithCsgIdInfo->enCsgDisplayIndicator));
            ucPaserdLen += stTlvInfo.ucLength;
        }
        else
        {
            return VOS_FALSE;
        }

        pucPtr = pstTlvObject->pucValue + ucPaserdLen;
    }

    return VOS_TRUE;
}


VOS_VOID NAS_MMC_PaserCsgListRecord(
    VOS_UINT8                          *pucCsgListRecordInfo,
    VOS_UINT16                          usRecordLen,
    NAS_MML_PLMN_WITH_CSG_ID_LIST_STRU *pstCsgList
)
{
    VOS_UINT8                           ucPaserdLen;
    VOS_UINT8                          *pucPaserPtr = VOS_NULL_PTR;
    NAS_MML_TLV_INFO_STRU               stTlvInfo;
    ucPaserdLen = 0;

    while (ucPaserdLen <= usRecordLen)
    {
        pucPaserPtr = pucCsgListRecordInfo + ucPaserdLen;

        /* 空记录或解析结束.(如果某个字节无效，则认为此记录后面都无效) */
        if (NAS_MML_EFACSGL_INVALID_BIT_VALUE == pucPaserPtr[0])
        {
            break;
        }

        if (VOS_FALSE == NAS_MMC_PaserCsgListTlvInfo(   pucPaserPtr,
                                                usRecordLen - ucPaserdLen,
                                                &stTlvInfo))
        {
            NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_PaserCsgListRecord:ERROR: PASER EFACSG File Error:");
            return;
        }

        /* TLV Object check */
        ucPaserdLen += stTlvInfo.ucOffset;
        if ((NAS_MML_EFACSGL_CSG_LIST_TAG_VALUE != stTlvInfo.ucTag)
         || (stTlvInfo.ucLength > (usRecordLen - ucPaserdLen)))
        {
            NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_PaserCsgListRecord:ERROR: PASER EFACSG File Error:");
            return;
        }

        if (VOS_FALSE == NAS_MMC_PaserCsgListTlvObject(&stTlvInfo, pstCsgList))
        {
            NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_PaserCsgListRecord:ERROR: PASER EFACSG File Error:");
            return;
        }

        ucPaserdLen += stTlvInfo.ucLength;
    }

    return;
}


VOS_VOID NAS_MMC_PaserCsgTypeRecord(
    VOS_UINT8                          *pucReordData,
    VOS_UINT16                          ucRecordLen,
    NAS_MML_CSG_TYPE_STRU              *pstCsgType
)
{
    VOS_UINT8                           ucPaserdLen;
    VOS_UINT8                          *pucPaserPtr = VOS_NULL_PTR;
    NAS_MML_TLV_INFO_STRU               stTlvInfo;
    NAS_MML_CSG_TYPE_STRU               stTextCsgType;

    ucPaserdLen = 0;
    PS_MEM_SET(pstCsgType, 0, sizeof(NAS_MML_CSG_TYPE_STRU));
    PS_MEM_SET(&stTextCsgType, 0, sizeof(NAS_MML_CSG_TYPE_STRU));

    while(ucPaserdLen <= ucRecordLen)
    {
        pucPaserPtr = pucReordData + ucPaserdLen;

        /* 空记录或解析结束. */
        if(NAS_MML_CSG_FILE_RECORD_INVALID_VALUE == pucPaserPtr[0])
        {
            break;
        }

        if(VOS_FALSE == NAS_MMC_PaserCsgListTlvInfo(   pucPaserPtr,
                                                ucRecordLen - ucPaserdLen,
                                                &stTlvInfo))
        {
            NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_PaserCsgTypeRecord:ERROR: PASER EFACSG File Error:");
            return;
        }
        /* TLV Object check */
        ucPaserdLen += stTlvInfo.ucOffset;

        if(((NAS_MML_EFACSGL_TEXT_CSG_TYPE_TAG_VALUE != stTlvInfo.ucTag)
        && (NAS_MML_EFACSGL_GRAPHICS_CSG_TYPE_TAG_FIRST_VALUE != stTlvInfo.ucTag)
        && (NAS_MML_EFACSGL_GRAPHICS_CSG_TYPE_TAG_SECOND_VALUE != stTlvInfo.ucTag))
        || (stTlvInfo.ucLength > ucRecordLen - ucPaserdLen))
        {
            NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_PaserCsgTypeRecord:ERROR: PASER EFACSG File Error:");
            return;
        }

        if (NAS_MML_EFACSGL_TEXT_CSG_TYPE_TAG_VALUE == stTlvInfo.ucTag)
        {
            stTextCsgType.ucCsgTypeLen = NAS_MML_MIN(stTlvInfo.ucLength, NAS_MML_MAX_CSG_TYPE_LEN);
            PS_MEM_CPY(stTextCsgType.aucCsgType,
                    stTlvInfo.pucValue,
                    NAS_MML_MIN(stTlvInfo.ucLength, NAS_MML_MAX_CSG_TYPE_LEN));
        }
        /* 基于目前设计的结构和逻辑暂时只支持TEXT CSG TYPE，不支持GARPHIC CSG TYPE，因此0x80或0x81都暂不解码*/
        else
        {
        }

        ucPaserdLen += stTlvInfo.ucLength;
    }

    pstCsgType->ucCsgTypeLen = NAS_MML_MIN(stTextCsgType.ucCsgTypeLen, NAS_MML_MAX_CSG_TYPE_LEN);
    PS_MEM_CPY(pstCsgType->aucCsgType,
            stTextCsgType.aucCsgType,
            NAS_MML_MIN(stTextCsgType.ucCsgTypeLen, NAS_MML_MAX_CSG_TYPE_LEN));
    return;
}

VOS_VOID NAS_MMC_Process_EFCSGT_FILE_RECORD(
    USIMM_DEF_FILEID_ENUM_UINT32                            enEfId,
    VOS_UINT32                                              ulIsRefresh
)
{
    NAS_MML_CSG_FILES_RECORD_INFO_STRU                     *pstAllowedCsgListFilesInfo  = VOS_NULL_PTR;
    NAS_MML_CSG_FILES_RECORD_INFO_STRU                     *pstOperatorCsgListFilesInfo = VOS_NULL_PTR;
    VOS_UINT8                                               ucTotalRecordNum;
    VOS_UINT8                                               ucReadRecordNum;

    pstAllowedCsgListFilesInfo  = NAS_MML_GetAllowedCsgListFilesInfo();
    pstOperatorCsgListFilesInfo = NAS_MML_GetOperatorCsgListFilesInfo();

    if (USIMM_USIM_EFCSGT_ID == enEfId)
    {
        ucTotalRecordNum = pstAllowedCsgListFilesInfo->stCsgTypeRecordInfo.ucTotalRecordNum;
        ucReadRecordNum  = pstAllowedCsgListFilesInfo->stCsgTypeRecordInfo.ucReadRecordNum;
        ucReadRecordNum++;

        if (ucTotalRecordNum == ucReadRecordNum)
        {
            pstAllowedCsgListFilesInfo->stCsgTypeRecordInfo.ucReadRecordNum = 0;
            NAS_MMC_ClearWaitSimFilesCnfFlg_SwitchOn(NAS_MML_READ_ALLOWED_CSG_TYPE_FILE_ID_FLG);
        }
        else
        {
            if (0 == ucReadRecordNum % NAS_MML_READ_SIMFILE_ONCE_MAXNUM)
            {
                NAS_MMC_ReadUsimmRecordsInBatches(USIMM_USIM_EFCSGT_ID, ucTotalRecordNum, (ucReadRecordNum + 1));
            }
        }
        return;
    }
    if (USIMM_USIM_EFOCSGT_ID == enEfId)
    {
        ucTotalRecordNum = pstOperatorCsgListFilesInfo->stCsgTypeRecordInfo.ucTotalRecordNum;
        ucReadRecordNum  = pstOperatorCsgListFilesInfo->stCsgTypeRecordInfo.ucReadRecordNum;
        ucReadRecordNum++;

        if (ucTotalRecordNum == ucReadRecordNum)
        {
            pstOperatorCsgListFilesInfo->stCsgTypeRecordInfo.ucReadRecordNum = 0;
            NAS_MMC_ClearCsgWaitSimFilesCnfFlg(NAS_MML_READ_OPERATOR_CSG_TYPE_FILE_ID_FLG, ulIsRefresh);
        }
        else
        {
            if (0 == ucReadRecordNum % NAS_MML_READ_SIMFILE_ONCE_MAXNUM)
            {
                NAS_MMC_ReadUsimmRecordsInBatches(USIMM_USIM_EFOCSGT_ID, ucTotalRecordNum, (ucReadRecordNum + 1));
            }
        }
        return;
    }
}


VOS_VOID NAS_MMC_PaserHomeNodeBNameRecord(
    VOS_UINT8                                              *pucReordData,
    VOS_UINT16                                              ucRecordLen,
    NAS_MML_CSG_ID_HOME_NODEB_NAME_STRU                    *pstHomeNodeBName
)
{
    VOS_UINT8                                               ucPaserdLen;
    NAS_MML_TLV_INFO_STRU                                   stTlvInfo;

    ucPaserdLen = 0;
    PS_MEM_SET(pstHomeNodeBName, 0, sizeof(NAS_MML_CSG_ID_HOME_NODEB_NAME_STRU));

    /* 空记录或解析结束. */
    if(NAS_MML_CSG_FILE_RECORD_INVALID_VALUE == pucReordData[0])
    {
        return;
    }
    if(VOS_FALSE == NAS_MMC_PaserCsgListTlvInfo(   pucReordData,
                                            ucRecordLen - ucPaserdLen,
                                            &stTlvInfo))
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_PaserHomeNodeBNameRecord:ERROR: PASER EFACSG File Error:");
        return;
    }
    /* TLV Object check */
    ucPaserdLen += stTlvInfo.ucOffset;

    if((NAS_MML_EFACSGL_HNB_NAME_TAG_VALUE != stTlvInfo.ucTag)
    || (stTlvInfo.ucLength > ucRecordLen - ucPaserdLen))
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_PaserHomeNodeBNameRecord:ERROR: PASER EFACSG File Error:");
        return;
    }

    if (NAS_MML_EFACSGL_HNB_NAME_TAG_VALUE == stTlvInfo.ucTag)
    {
        pstHomeNodeBName->ucHomeNodeBNameLen = NAS_MML_MIN(stTlvInfo.ucLength, NAS_MML_MAX_HOME_NODEB_NAME_LEN);
        PS_MEM_CPY(pstHomeNodeBName->aucHomeNodeBName,
                stTlvInfo.pucValue,
                NAS_MML_MIN(stTlvInfo.ucLength, NAS_MML_MAX_HOME_NODEB_NAME_LEN));
    }

    return;
}

VOS_VOID NAS_MMC_Process_EFHNB_FILE_RECORD(
    USIMM_DEF_FILEID_ENUM_UINT32                            enEfId,
    VOS_UINT32                                              ulIsRefresh
)
{
    NAS_MML_CSG_FILES_RECORD_INFO_STRU                     *pstAllowedCsgListFilesInfo  = VOS_NULL_PTR;
    NAS_MML_CSG_FILES_RECORD_INFO_STRU                     *pstOperatorCsgListFilesInfo = VOS_NULL_PTR;
    VOS_UINT8                                               ucTotalRecordNum;
    VOS_UINT8                                               ucReadRecordNum;

    pstAllowedCsgListFilesInfo  = NAS_MML_GetAllowedCsgListFilesInfo();
    pstOperatorCsgListFilesInfo = NAS_MML_GetOperatorCsgListFilesInfo();

    if (USIMM_USIM_EFHNBN_ID == enEfId)
    {
        ucTotalRecordNum = pstAllowedCsgListFilesInfo->stCsgHomeNodeBNameRecordInfo.ucTotalRecordNum;
        ucReadRecordNum  = pstAllowedCsgListFilesInfo->stCsgHomeNodeBNameRecordInfo.ucReadRecordNum;
        ucReadRecordNum++;
        if (ucTotalRecordNum == ucReadRecordNum)
        {
            pstAllowedCsgListFilesInfo->stCsgHomeNodeBNameRecordInfo.ucReadRecordNum = 0;
            NAS_MMC_ClearWaitSimFilesCnfFlg_SwitchOn(NAS_MML_READ_ALLOWED_CSG_HOME_NODEB_NAME_FILE_ID_FLG);
        }
        else if (0 == ucReadRecordNum % NAS_MML_READ_SIMFILE_ONCE_MAXNUM)
        {
            NAS_MMC_ReadUsimmRecordsInBatches(USIMM_USIM_EFHNBN_ID, ucTotalRecordNum, (ucReadRecordNum + 1));
        }
        return;
    }
    if (USIMM_USIM_EFOHNBN_ID == enEfId)
    {
        ucTotalRecordNum = pstOperatorCsgListFilesInfo->stCsgTypeRecordInfo.ucTotalRecordNum;
        ucReadRecordNum  = pstOperatorCsgListFilesInfo->stCsgTypeRecordInfo.ucReadRecordNum;
        ucReadRecordNum++;

        ucTotalRecordNum = pstOperatorCsgListFilesInfo->stCsgHomeNodeBNameRecordInfo.ucTotalRecordNum;
        ucReadRecordNum  = pstOperatorCsgListFilesInfo->stCsgHomeNodeBNameRecordInfo.ucReadRecordNum;
        ucReadRecordNum++;
        if (ucTotalRecordNum == ucReadRecordNum)
        {
            pstOperatorCsgListFilesInfo->stCsgTypeRecordInfo.ucReadRecordNum = 0;
            NAS_MMC_ClearCsgWaitSimFilesCnfFlg(NAS_MML_READ_OPERATOR_CSG_HOME_NODEB_NAME_FILE_ID_FLG, ulIsRefresh);
        }
        else if (0 == ucReadRecordNum % NAS_MML_READ_SIMFILE_ONCE_MAXNUM)
        {
            NAS_MMC_ReadUsimmRecordsInBatches(USIMM_USIM_EFOHNBN_ID, ucTotalRecordNum, (ucReadRecordNum + 1));
        }

        return;
    }
}


VOS_VOID NAS_MMC_OperatorCsgListUpdateDispalyIndicator(VOS_VOID)
{
    NAS_MML_PLMN_WITH_CSG_ID_LIST_STRU          *pstOperatorCsgList;
    NAS_MML_PLMN_WITH_CSG_ID_INFO_STRU          *pstCsgIdInfo;
    NAS_MML_PLMN_WITH_CSG_ID_INFO_STRU          *pstCsgIdInfotemp;
    VOS_UINT8                                   i;
    VOS_UINT8                                   j;

    pstOperatorCsgList = NAS_MML_GetOperatorCsgList();

    /* 找到需要有Display Indicator的PLMN，并更新到所有该PLMN下的CSGID中 */
    /* 需要修改代码，所有NAS_MML_PLMN_WITH_CSG_ID_INFO_STRU中的Display Indicator初始化为NOT_EXIST */
    for (i=0; i<pstOperatorCsgList->ulPlmnWithCsgIdListNum; i++)
    {
        pstCsgIdInfo = &(pstOperatorCsgList->astPlmnWithCsgIdInfo[i]);
        if ( NAS_MML_CSG_ID_DISPLAY_INDICATOR_TYPE_NOT_EXIST != pstCsgIdInfo->enCsgDisplayIndicator)
        {
            for (j=0; j<pstOperatorCsgList->ulPlmnWithCsgIdListNum && j != i; j++)
            {
                pstCsgIdInfotemp = &(pstOperatorCsgList->astPlmnWithCsgIdInfo[j]);
                if (pstCsgIdInfotemp->enCsgDisplayIndicator != NAS_MML_CSG_ID_DISPLAY_INDICATOR_TYPE_NOT_EXIST)
                {
                    continue;
                }
                if (VOS_TRUE == NAS_MML_CompareBcchPlmnwithSimPlmn( &(pstCsgIdInfotemp->stPlmnWithCsgId.stPlmnId),
                                                                    &(pstCsgIdInfo->stPlmnWithCsgId.stPlmnId)))
                {
                    pstCsgIdInfotemp->enCsgDisplayIndicator = pstCsgIdInfo->enCsgDisplayIndicator;
                }
            }
        }
    }

}


VOS_VOID NAS_MMC_RcvAgentUsimReadFileCnf_EFACSGL_FILE_RECORD(
    PS_USIM_GET_FILE_RSP_FOR_PCLINT_STRU                   *pstMsg
)
{
    NAS_MML_RECORD_STREAM_STRU         *pstCsgListRecordInfo        = VOS_NULL_PTR;
    NAS_MML_ACSGL_RECORD_STREAM_STRU   *pstRecords                  = VOS_NULL_PTR;
    NAS_MML_PLMN_WITH_CSG_ID_LIST_STRU *pstAllowedCsgList           = VOS_NULL_PTR;
    NAS_MML_CSG_FILES_RECORD_INFO_STRU *pstAllowedCsgListFilesInfo  = VOS_NULL_PTR;
    VOS_UINT8                           ucTotalRecordNum;
    VOS_UINT8                           ucReadRecordNum;

    pstAllowedCsgListFilesInfo = NAS_MML_GetAllowedCsgListFilesInfo();

    /* 总记录数 */
    ucTotalRecordNum = pstAllowedCsgListFilesInfo->stCsgListRecordInfo.ucTotalRecordNum;

    /* 已读记录数 */
    pstAllowedCsgListFilesInfo->stCsgListRecordInfo.ucReadRecordNum += 1;
    ucReadRecordNum = pstAllowedCsgListFilesInfo->stCsgListRecordInfo.ucReadRecordNum;

    /* 如果已经收到最后一条消息，则认为CSG List已经读完，可以读CSGT和CSGT了 */
    if (ucReadRecordNum == ucTotalRecordNum)
    {
        pstAllowedCsgListFilesInfo->stCsgListRecordInfo.ucReadRecordNum = 0;
        /* 发送读取CSGT消息 */
        ucTotalRecordNum = pstAllowedCsgListFilesInfo->stCsgTypeRecordInfo.ucTotalRecordNum;
        NAS_MMC_ReadUsimmRecordsInBatches(USIMM_USIM_EFCSGT_ID, ucTotalRecordNum, 1);

        /* 发送读取HNBN消息 */
        ucTotalRecordNum = pstAllowedCsgListFilesInfo->stCsgHomeNodeBNameRecordInfo.ucTotalRecordNum;
        NAS_MMC_ReadUsimmRecordsInBatches(USIMM_USIM_EFHNBN_ID, ucTotalRecordNum, 1);

        /* 置读取OCSGL结束标志位 */
        NAS_MMC_ClearWaitSimFilesCnfFlg_SwitchOn(NAS_MML_READ_ALLOWED_CSG_LIST_FILE_ID_FLG);
    }
    else
    {
        /* 每次发10条，收全后再发下10条 */
        if (0 == ucReadRecordNum % NAS_MML_READ_SIMFILE_ONCE_MAXNUM)
        {
            NAS_MMC_ReadUsimmRecordsInBatches(USIMM_USIM_EFACSGL_ID, ucTotalRecordNum, (pstMsg->ucRecordNum + 1));
        }
    }

    /* 读取失败 */
    if (VOS_OK != pstMsg->stCmdResult.ulResult)
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvAgentUsimReadFileCnf_EFACSGL_FILE_RECORD:ERROR: Read SIM/USIM Mandatory File Error:");
        return;
    }

    /* 保存记录码流 */
    pstCsgListRecordInfo = (NAS_MML_RECORD_STREAM_STRU *)PS_MEM_ALLOC(WUEPS_PID_MMC, sizeof(NAS_MML_RECORD_STREAM_STRU));
    if (VOS_NULL_PTR == pstCsgListRecordInfo)
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvAgentUsimReadFileCnf_EFACSGL_FILE_RECORD:ERROR: Alloc Mem Fail.");
        return;
    }
    PS_MEM_CPY(pstCsgListRecordInfo->aucEf, pstMsg->aucEf, NAS_MML_MAX_USIMM_FILE_RECORD_LEN);

    pstRecords = NAS_MML_GetAllowedCsgListRecordStream();
    pstRecords->apstCsgListRecordInfo[pstMsg->ucRecordNum - 1] = pstCsgListRecordInfo;

    /* 解析 */
    pstAllowedCsgList = NAS_MML_GetAllowedCsgList();
    NAS_MMC_PaserCsgListRecord( pstMsg->aucEf,
                                pstMsg->usEfLen,
                                pstAllowedCsgList);

    return;
}


VOS_VOID NAS_MMC_RcvAgentUsimReadFileCnf_EFOCSGL_FILE_RECORD(
    PS_USIM_GET_FILE_RSP_FOR_PCLINT_STRU                   *pstMsg,
    VOS_UINT32                                              ulIsRefresh
)
{
    NAS_MML_PLMN_WITH_CSG_ID_LIST_STRU *pstOperatorCsgList          = VOS_NULL_PTR;
    NAS_MML_CSG_FILES_RECORD_INFO_STRU *pstOperatorCsgListFilesInfo  = VOS_NULL_PTR;
    VOS_UINT8                           ulTotalRecordNum;
    VOS_UINT8                           ucReadRecordNum;

    pstOperatorCsgListFilesInfo = NAS_MML_GetOperatorCsgListFilesInfo();

    /* 总记录数 */
    ulTotalRecordNum = pstOperatorCsgListFilesInfo->stCsgListRecordInfo.ucTotalRecordNum;

    /* 已读记录数 */
    pstOperatorCsgListFilesInfo->stCsgListRecordInfo.ucReadRecordNum += 1;
    ucReadRecordNum = pstOperatorCsgListFilesInfo->stCsgListRecordInfo.ucReadRecordNum;

    /* 如果已经收到最后一条消息，则认为CSG List已经读完，可以读CSGT和CSGT了 */
    if (ucReadRecordNum == ulTotalRecordNum)
    {
        pstOperatorCsgListFilesInfo->stCsgListRecordInfo.ucReadRecordNum = 0;
        /* 更新CSG Display Indicator */
        NAS_MMC_OperatorCsgListUpdateDispalyIndicator();

        /* 发送读取CSGT消息 */
        ulTotalRecordNum = pstOperatorCsgListFilesInfo->stCsgTypeRecordInfo.ucTotalRecordNum;
        NAS_MMC_ReadUsimmRecordsInBatches(USIMM_USIM_EFOCSGT_ID, ulTotalRecordNum, 1);

        /* 发送读取HNBN消息 */
        ulTotalRecordNum = pstOperatorCsgListFilesInfo->stCsgHomeNodeBNameRecordInfo.ucTotalRecordNum;
        NAS_MMC_ReadUsimmRecordsInBatches(USIMM_USIM_EFOHNBN_ID, ulTotalRecordNum, 1);

        /* 置读取ACSGL结束标志位 */
        NAS_MMC_ClearCsgWaitSimFilesCnfFlg(NAS_MML_READ_OPERATOR_CSG_LIST_FILE_ID_FLG, ulIsRefresh);
    }
    else
    {
        /* 每次发10条，收全后再发下10条 */
        if (0 == ucReadRecordNum % NAS_MML_READ_SIMFILE_ONCE_MAXNUM)
        {
            NAS_MMC_ReadUsimmRecordsInBatches(USIMM_USIM_EFOCSGL_ID, ulTotalRecordNum, (pstMsg->ucRecordNum + 1));
        }
    }

    /* 读取失败 */
    if (VOS_OK != pstMsg->stCmdResult.ulResult)
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvAgentUsimReadFileCnf_EFOCSGL_FILE_RECORD:ERROR: Read SIM/USIM Mandatory File Error:");
        return;
    }

    /* 解析 */
    if (VOS_TRUE == ulIsRefresh)
    {
        pstOperatorCsgList = NAS_MML_GetOperatorCsgListCache();
    }
    else
    {
        pstOperatorCsgList = NAS_MML_GetOperatorCsgList();
    }

    if (VOS_NULL == pstOperatorCsgList)
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvAgentUsimReadFileCnf_EFOCSGL_FILE_RECORD:ERROR: pstOperatorCsgList is NULL");
        return;
    }

    NAS_MMC_PaserCsgListRecord(pstMsg->aucEf, pstMsg->usEfLen, pstOperatorCsgList);

    return;
}


VOS_VOID NAS_MMC_RcvAgentUsimReadFileCnf_EFCSGT_FILE_RECORD(
    PS_USIM_GET_FILE_RSP_FOR_PCLINT_STRU                   *pstMsg,
    USIMM_DEF_FILEID_ENUM_UINT32                            enEfId,
    VOS_UINT32                                              ulIsRefresh
)
{
    NAS_MML_PLMN_WITH_CSG_ID_LIST_STRU                     *pstAllowedCsgList           = VOS_NULL_PTR;
    NAS_MML_PLMN_WITH_CSG_ID_LIST_STRU                     *pstOperatorCsgList          = VOS_NULL_PTR;
    NAS_MML_CSG_TYPE_STRU                                  *pstCsgType                  = VOS_NULL_PTR;
    VOS_UINT8                                               ucLoop;

    pstAllowedCsgList = NAS_MML_GetAllowedCsgList();

    if (VOS_TRUE == ulIsRefresh)
    {
        pstOperatorCsgList = NAS_MML_GetOperatorCsgListCache();
    }
    else
    {
        pstOperatorCsgList = NAS_MML_GetOperatorCsgList();
    }

    if (VOS_NULL == pstOperatorCsgList)
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvAgentUsimReadFileCnf_EFCSGT_FILE_RECORD:ERROR: pstOperatorCsgList is NULL");
        return;
    }

    NAS_MMC_Process_EFCSGT_FILE_RECORD(enEfId, ulIsRefresh);

    pstCsgType = (NAS_MML_CSG_TYPE_STRU *)PS_MEM_ALLOC(WUEPS_PID_MMC, sizeof(NAS_MML_CSG_TYPE_STRU));

    if (VOS_NULL_PTR == pstCsgType)
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvAgentUsimReadFileCnf_EFACSGL_FILE_RECORD:ERROR: Alloc Mem Fail.");
        return;
    }

    PS_MEM_SET(pstCsgType, 0, sizeof(NAS_MML_CSG_TYPE_STRU));

    if (VOS_OK != pstMsg->stCmdResult.ulResult)
    {
        /* 释放空间  */
        PS_MEM_FREE(WUEPS_PID_MMC, pstCsgType);
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvAgentUsimReadFileCnf_EFCSGT_FILE_RECORD:ERROR: Read SIM/USIM Mandatory File Error:");
        return;
    }

    NAS_MMC_PaserCsgTypeRecord(pstMsg->aucEf, pstMsg->usEfLen, pstCsgType);

    if (USIMM_USIM_EFCSGT_ID == enEfId)
    {
        for (ucLoop = 0; ucLoop < pstAllowedCsgList->ulPlmnWithCsgIdListNum; ucLoop++)
        {
            if (pstMsg->ucRecordNum == pstAllowedCsgList->astPlmnWithCsgIdInfo[ucLoop].stCsgType.ucCsgTypeIndication)
            {
                pstAllowedCsgList->astPlmnWithCsgIdInfo[ucLoop].stCsgType.ucCsgTypeLen = NAS_MML_MIN(pstCsgType->ucCsgTypeLen, NAS_MML_MAX_CSG_TYPE_LEN);
                PS_MEM_CPY(pstAllowedCsgList->astPlmnWithCsgIdInfo[ucLoop].stCsgType.aucCsgType,
                    pstCsgType->aucCsgType, NAS_MML_MIN(pstCsgType->ucCsgTypeLen, NAS_MML_MAX_CSG_TYPE_LEN));
            }
        }
        /* 释放空间  */
        PS_MEM_FREE(WUEPS_PID_MMC, pstCsgType);
        return;
    }
    if (USIMM_USIM_EFOCSGT_ID == enEfId)
    {
        for (ucLoop = 0; ucLoop < pstOperatorCsgList->ulPlmnWithCsgIdListNum; ucLoop++)
        {
            if (pstMsg->ucRecordNum == pstOperatorCsgList->astPlmnWithCsgIdInfo[ucLoop].stCsgType.ucCsgTypeIndication)
            {
                pstOperatorCsgList->astPlmnWithCsgIdInfo[ucLoop].stCsgType.ucCsgTypeLen = NAS_MML_MIN(pstCsgType->ucCsgTypeLen, NAS_MML_MAX_CSG_TYPE_LEN);
                PS_MEM_CPY(pstOperatorCsgList->astPlmnWithCsgIdInfo[ucLoop].stCsgType.aucCsgType,
                    pstCsgType->aucCsgType, NAS_MML_MIN(pstCsgType->ucCsgTypeLen, NAS_MML_MAX_CSG_TYPE_LEN));
            }
        }
        /* 释放空间  */
        PS_MEM_FREE(WUEPS_PID_MMC, pstCsgType);
        return;
    }
    /* 释放空间  */
    PS_MEM_FREE(WUEPS_PID_MMC, pstCsgType);
    return;
}

VOS_VOID NAS_MMC_RcvAgentUsimReadFileCnf_EFHNBN_FILE_RECORD(
    PS_USIM_GET_FILE_RSP_FOR_PCLINT_STRU                   *pstMsg,
    USIMM_DEF_FILEID_ENUM_UINT32                            enEfId,
    VOS_UINT32                                              ulIsRefresh
)
{
    NAS_MML_PLMN_WITH_CSG_ID_LIST_STRU                     *pstAllowedCsgList   = VOS_NULL_PTR;
    NAS_MML_PLMN_WITH_CSG_ID_LIST_STRU                     *pstOperatorCsgList  = VOS_NULL_PTR;
    NAS_MML_CSG_ID_HOME_NODEB_NAME_STRU                    *pstHomeNodeBName    = VOS_NULL_PTR;
    VOS_UINT8                                               ucLoop;

    pstAllowedCsgList           = NAS_MML_GetAllowedCsgList();

    if (VOS_TRUE == ulIsRefresh)
    {
        pstOperatorCsgList = NAS_MML_GetOperatorCsgListCache();
    }
    else
    {
        pstOperatorCsgList = NAS_MML_GetOperatorCsgList();
    }

    if (VOS_NULL == pstOperatorCsgList)
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvAgentUsimReadFileCnf_EFHNBN_FILE_RECORD:ERROR: pstOperatorCsgList is NULL");
        return;
    }

    NAS_MMC_Process_EFHNB_FILE_RECORD(enEfId, ulIsRefresh);

    pstHomeNodeBName = (NAS_MML_CSG_ID_HOME_NODEB_NAME_STRU *)PS_MEM_ALLOC(WUEPS_PID_MMC, sizeof(NAS_MML_CSG_ID_HOME_NODEB_NAME_STRU));

    if (VOS_NULL_PTR == pstHomeNodeBName)
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvAgentUsimReadFileCnf_EFHNBN_FILE_RECORD:ERROR: Alloc Mem Fail.");
        return;
    }
    PS_MEM_SET(pstHomeNodeBName, 0, sizeof(NAS_MML_CSG_ID_HOME_NODEB_NAME_STRU));

    if (VOS_OK != pstMsg->stCmdResult.ulResult)
    {
        /* 释放空间  */
        PS_MEM_FREE(WUEPS_PID_MMC, pstHomeNodeBName);

        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvAgentUsimReadFileCnf_EFHNBN_FILE_RECORD:ERROR: Read SIM/USIM Mandatory File Error:");
        return;
    }

    NAS_MMC_PaserHomeNodeBNameRecord(pstMsg->aucEf, pstMsg->usEfLen, pstHomeNodeBName);

    if (USIMM_USIM_EFHNBN_ID == enEfId)
    {
        for (ucLoop = 0; ucLoop < pstAllowedCsgList->ulPlmnWithCsgIdListNum; ucLoop++)
        {
            if (pstMsg->ucRecordNum == pstAllowedCsgList->astPlmnWithCsgIdInfo[ucLoop].stHomeNodeBName.ucHomeNodebNameIndication)
            {
                pstAllowedCsgList->astPlmnWithCsgIdInfo[ucLoop].stHomeNodeBName.ucHomeNodeBNameLen = NAS_MML_MIN(pstHomeNodeBName->ucHomeNodeBNameLen, NAS_MML_MAX_HOME_NODEB_NAME_LEN);
                PS_MEM_CPY(pstAllowedCsgList->astPlmnWithCsgIdInfo[ucLoop].stHomeNodeBName.aucHomeNodeBName,
                pstHomeNodeBName->aucHomeNodeBName, NAS_MML_MIN(pstHomeNodeBName->ucHomeNodeBNameLen, NAS_MML_MAX_HOME_NODEB_NAME_LEN));
            }
        }
        /* 释放空间  */
        PS_MEM_FREE(WUEPS_PID_MMC, pstHomeNodeBName);
        return;
    }
    if (USIMM_USIM_EFOHNBN_ID == enEfId)
    {
        for (ucLoop = 0; ucLoop < pstOperatorCsgList->ulPlmnWithCsgIdListNum; ucLoop++)
        {
            if (pstMsg->ucRecordNum == pstOperatorCsgList->astPlmnWithCsgIdInfo[ucLoop].stHomeNodeBName.ucHomeNodebNameIndication)
            {
                pstOperatorCsgList->astPlmnWithCsgIdInfo[ucLoop].stHomeNodeBName.ucHomeNodeBNameLen = NAS_MML_MIN(pstHomeNodeBName->ucHomeNodeBNameLen, NAS_MML_MAX_HOME_NODEB_NAME_LEN);
                PS_MEM_CPY(pstOperatorCsgList->astPlmnWithCsgIdInfo[ucLoop].stHomeNodeBName.aucHomeNodeBName,
                pstHomeNodeBName->aucHomeNodeBName, NAS_MML_MIN(pstHomeNodeBName->ucHomeNodeBNameLen, NAS_MML_MAX_HOME_NODEB_NAME_LEN));
            }
        }
        /* 释放空间  */
        PS_MEM_FREE(WUEPS_PID_MMC, pstHomeNodeBName);
        return;
    }
    /* 释放空间  */
    PS_MEM_FREE(WUEPS_PID_MMC, pstHomeNodeBName);
    return;
}


VOS_VOID  NAS_MMC_ClearCsgWaitSimFilesCnfFlgByEfId(
    USIMM_DEF_FILEID_ENUM_UINT32        enEfId,
    VOS_UINT32                          ulIsRefresh
)
{
    switch ( enEfId )
    {
        case USIMM_USIM_EFACSGL_ID:
            NAS_MMC_ClearCsgWaitSimFilesCnfFlg(NAS_MML_READ_ALLOWED_CSG_LIST_FILE_ID_FLG, ulIsRefresh);
            NAS_MMC_ClearCsgWaitSimFilesCnfFlg(NAS_MML_READ_ALLOWED_CSG_TYPE_FILE_ID_FLG, ulIsRefresh);
            NAS_MMC_ClearCsgWaitSimFilesCnfFlg(NAS_MML_READ_ALLOWED_CSG_HOME_NODEB_NAME_FILE_ID_FLG, ulIsRefresh);
            break;

        case USIMM_USIM_EFOCSGL_ID:
            NAS_MMC_ClearCsgWaitSimFilesCnfFlg(NAS_MML_READ_OPERATOR_CSG_LIST_FILE_ID_FLG, ulIsRefresh);
            NAS_MMC_ClearCsgWaitSimFilesCnfFlg(NAS_MML_READ_OPERATOR_CSG_TYPE_FILE_ID_FLG, ulIsRefresh);
            NAS_MMC_ClearCsgWaitSimFilesCnfFlg(NAS_MML_READ_OPERATOR_CSG_HOME_NODEB_NAME_FILE_ID_FLG, ulIsRefresh);
            break;

        case USIMM_USIM_EFCSGT_ID:
            NAS_MMC_ClearCsgWaitSimFilesCnfFlg(NAS_MML_READ_ALLOWED_CSG_TYPE_FILE_ID_FLG, ulIsRefresh);
            break;

        case USIMM_USIM_EFOCSGT_ID:
            NAS_MMC_ClearCsgWaitSimFilesCnfFlg(NAS_MML_READ_OPERATOR_CSG_TYPE_FILE_ID_FLG, ulIsRefresh);
            break;

        case USIMM_USIM_EFHNBN_ID:
            NAS_MMC_ClearCsgWaitSimFilesCnfFlg(NAS_MML_READ_ALLOWED_CSG_HOME_NODEB_NAME_FILE_ID_FLG, ulIsRefresh);
            break;

        case USIMM_USIM_EFOHNBN_ID:
            NAS_MMC_ClearCsgWaitSimFilesCnfFlg(NAS_MML_READ_OPERATOR_CSG_HOME_NODEB_NAME_FILE_ID_FLG, ulIsRefresh);
            break;

        default:
            break;
    }

    return;
}


VOS_VOID  NAS_MMC_RcvAgentUsimQueryCsgFileCnf(
    USIMM_QUERYFILE_CNF_STRU           *pstMsg,
    VOS_UINT32                          ulIsRefresh
)
{
    USIMM_DEF_FILEID_ENUM_UINT32        enEfId;
    NAS_MML_ACSGL_RECORD_STREAM_STRU   *pstACsgListRecordsStream    = VOS_NULL_PTR;
    NAS_MML_CSG_FILES_RECORD_INFO_STRU *pstAllowedCsgListRecordInfo = VOS_NULL_PTR;
    NAS_MML_CSG_FILES_RECORD_INFO_STRU *pstOperatorCsgListRecordInfo = VOS_NULL_PTR;

    if (VOS_OK != USIMM_ChangePathToDefFileID(USIMM_GUTL_APP, pstMsg->stFilePath.ulPathLen, pstMsg->stFilePath.acPath, &enEfId))
    {
        return;
    }

    pstAllowedCsgListRecordInfo = NAS_MML_GetAllowedCsgListFilesInfo();
    pstOperatorCsgListRecordInfo = NAS_MML_GetOperatorCsgListFilesInfo();

    if ( 0 == pstMsg->ucRecordNum)
    {
        NAS_MMC_ClearCsgWaitSimFilesCnfFlgByEfId(enEfId, ulIsRefresh);
        return;
    }

    /* 保存记录数及记录长度 */
    switch ( enEfId )
    {
        case USIMM_USIM_EFACSGL_ID:
            pstAllowedCsgListRecordInfo->stCsgListRecordInfo.ucTotalRecordNum = pstMsg->ucRecordNum;
            pstAllowedCsgListRecordInfo->stCsgListRecordInfo.ucRecordLen = pstMsg->ucRecordLen;

            /* 保存长度和记录数到码流结构头中 */
            pstACsgListRecordsStream = NAS_MML_GetAllowedCsgListRecordStream();
            pstACsgListRecordsStream->ulTotalRecordNum = pstMsg->ucRecordNum;
            pstACsgListRecordsStream->ulRecordLen = pstMsg->ucRecordLen;

            /*发起读文件请求 */
            NAS_MMC_ReadUsimmRecordsInBatches((VOS_UINT16)enEfId, pstMsg->ucRecordNum, 1);

            break;

        case USIMM_USIM_EFOCSGL_ID:
            pstOperatorCsgListRecordInfo->stCsgListRecordInfo.ucTotalRecordNum = pstMsg->ucRecordNum;
            pstOperatorCsgListRecordInfo->stCsgListRecordInfo.ucRecordLen = pstMsg->ucRecordLen;

            /*发起读文件请求 */
            NAS_MMC_ReadUsimmRecordsInBatches((VOS_UINT16)enEfId, pstMsg->ucRecordNum, 1);
            break;

        case USIMM_USIM_EFCSGT_ID:
            pstAllowedCsgListRecordInfo->stCsgTypeRecordInfo.ucTotalRecordNum = pstMsg->ucRecordNum;
            pstAllowedCsgListRecordInfo->stCsgTypeRecordInfo.ucRecordLen = pstMsg->ucRecordLen;
            break;

        case USIMM_USIM_EFOCSGT_ID:
            pstOperatorCsgListRecordInfo->stCsgTypeRecordInfo.ucTotalRecordNum = pstMsg->ucRecordNum;
            pstOperatorCsgListRecordInfo->stCsgTypeRecordInfo.ucRecordLen = pstMsg->ucRecordLen;
            break;

        case USIMM_USIM_EFHNBN_ID:
            pstAllowedCsgListRecordInfo->stCsgHomeNodeBNameRecordInfo.ucTotalRecordNum = pstMsg->ucRecordNum;
            pstAllowedCsgListRecordInfo->stCsgHomeNodeBNameRecordInfo.ucRecordLen = pstMsg->ucRecordLen;
            break;

        case USIMM_USIM_EFOHNBN_ID:
            pstOperatorCsgListRecordInfo->stCsgHomeNodeBNameRecordInfo.ucTotalRecordNum = pstMsg->ucRecordNum;
            pstOperatorCsgListRecordInfo->stCsgHomeNodeBNameRecordInfo.ucRecordLen = pstMsg->ucRecordLen;
            break;

        default:
            NAS_WARNING_LOG(WUEPS_PID_MMC, "Mmc_RcvAgentUsimReadFileCnf:WARNING: File Id Abnormal");
            break;
    }

    return;
}
#endif


VOS_VOID  NAS_MMC_RcvAgentUsimReadFileCnf(
    PS_USIM_GET_FILE_RSP_FOR_PCLINT_STRU           *pstMsg
)
{
    USIMM_DEF_FILEID_ENUM_UINT32        enEfId;

    if (VOS_OK != USIMM_ChangePathToDefFileID(USIMM_GUTL_APP, pstMsg->stFilePath.ulPathLen, pstMsg->stFilePath.acPath, &enEfId))
    {
        return;
    }

    switch ( enEfId )
    {
        case USIMM_USIM_EFHPLMNwACT_ID:
        case USIMM_GSM_EFHPLMNACT_ID:
            NAS_MMC_RcvAgentUsimReadFileCnf_HPLMN_SEL_FILE_ID(pstMsg);
            break;

        case USIMM_USIM_EFFPLMN_ID:
        case USIMM_GSM_EFFPLMN_ID:
            NAS_MMC_RcvAgentUsimReadFileCnf_FORBIDDEN_PLMN_LIST_FILE_ID(pstMsg);
            break;

        case USIMM_USIM_EFPLMNWACT_ID:
        case USIMM_GSM_EFPLMNWACT_ID:
            NAS_MMC_RcvAgentUsimReadFileCnf_UPLMN_SEL_FILE_ID(pstMsg);
            break;

        case USIMM_USIM_EFOPLMNWACT_ID:
        case USIMM_GSM_EFOPLMNWACT_ID:
            NAS_MMC_RcvAgentUsimReadFileCnf_OPLMN_SEL_FILE_ID(pstMsg);
            break;

        case USIMM_USIM_EFHPPLMN_ID:
        case USIMM_GSM_EFHPLMN_ID:
            NAS_MMC_RcvAgentUsimReadFileCnf_HPLMN_PERI_FILE_ID(pstMsg);
            break;

        case USIMM_USIM_EFAD_ID:
        case USIMM_GSM_EFAD_ID:
            NAS_MMC_RcvAgentUsimReadFileCnf_MNC_LENGTH_FILE_ID(pstMsg);
            break;

        case USIMM_USIM_EFLOCI_ID:
        case USIMM_GSM_EFLOCI_ID:
            NAS_MMC_RcvAgentUsimReadFileCnf_CS_LOC_FILE_ID(pstMsg);
            break;

        case USIMM_USIM_EFKEYS_ID:
            NAS_MMC_RcvAgentUsimReadFileCnf_CS_CKIK_FILE_ID(pstMsg);
            break;

        /* 增加对SIM卡中的cskc文件读取 */
        case USIMM_GSM_EFKC_ID:
            NAS_MMC_RcvAgentUsimReadFileCnf_CS_KC_FILE_ID(pstMsg);
            break;

        case USIMM_USIM_EFKEYSPS_ID:
            NAS_MMC_RcvAgentUsimReadFileCnf_PS_KEY_FILE_ID(pstMsg);
            break;

        /* 增加对SIM卡中的PSkc文件读取处理 */
        case USIMM_GSM_EFKCGPRS_ID:
            NAS_MMC_RcvAgentUsimReadFileCnf_PS_KC_FILE_ID(pstMsg);
            break;

        case USIMM_USIM_EFPSLOCI_ID:
        case USIMM_GSM_EFLOCIGPRS_ID:
            NAS_MMC_RcvAgentUsimReadFileCnf_PS_LOC_FILE_ID(pstMsg);
            break;

        /* 增加对SIM卡中的cskc文件读取 */
        case USIMM_GSM_EFPLMNSEL_ID:
            NAS_MMC_RcvAgentUsimReadFileCnf_PLMN_SEL_FILE_ID(pstMsg);
            break;

        case USIMM_USIM_EFEHPLMN_ID:
            NAS_MMC_RcvAgentUsimReadFileCnf_EHPLMN_FILE_ID(pstMsg);
            break;

        case USIMM_ATTGSM_EFACTINGHPLMN_ID:
        case USIMM_ATTUSIM_EFACTINGHPLMN_ID:

            NAS_MMC_RcvAgentUsimReadFileCnf_ACTINGHPLMN_FILE_ID(pstMsg);
            break;

        case USIMM_USIM_EFEHPLMNPI_ID:
            NAS_MMC_RcvAgentUsimReadFileCnf_EHPLMNPI_FILE_ID(pstMsg);
            break;

        case USIMM_USIM_EFLRPLMNSI_ID:
            NAS_MMC_RcvAgentUsimReadFileCnf_LRPLMNSI_FILE_ID(pstMsg);
            break;

#if (FEATURE_ON == FEATURE_CSG)
        case USIMM_USIM_EFACSGL_ID:
            NAS_MMC_RcvAgentUsimReadFileCnf_EFACSGL_FILE_RECORD(pstMsg);
            break;
        case USIMM_USIM_EFOCSGL_ID:
            NAS_MMC_RcvAgentUsimReadFileCnf_EFOCSGL_FILE_RECORD(pstMsg, VOS_FALSE);
            break;
        case USIMM_USIM_EFCSGT_ID:
        case USIMM_USIM_EFOCSGT_ID:
            NAS_MMC_RcvAgentUsimReadFileCnf_EFCSGT_FILE_RECORD(pstMsg, enEfId, VOS_FALSE);
            break;
        case USIMM_USIM_EFHNBN_ID:
        case USIMM_USIM_EFOHNBN_ID:
            NAS_MMC_RcvAgentUsimReadFileCnf_EFHNBN_FILE_RECORD(pstMsg, enEfId, VOS_FALSE);
            break;
#endif

        default:
            NAS_WARNING_LOG(WUEPS_PID_MMC, "Mmc_RcvAgentUsimReadFileCnf:WARNING: File Id Abnormal");
            break;
    }

    return ;

}



VOS_VOID  NAS_MMC_UpdatePtmsiFormPsLociFile(
    VOS_UINT8                          *pucPsLociFileContent
)
{
    VOS_UINT32                          i;
    VOS_UINT8                           ucInvalidCnt;
    NAS_MML_SIM_MS_IDENTITY_STRU       *pstMsIdentity;
    VOS_UINT32                         *pulPtmsiValue = VOS_NULL_PTR;

    NAS_MML_SIM_FORMAT_PLMN_ID          stSimPlmnId;
    NAS_MML_PLMN_ID_STRU                stPlmnId;
    VOS_UINT8                           ucCurrPos;
    VOS_UINT32                          ulValidPtmsiFlag;

    PS_MEM_SET(&stSimPlmnId, 0, sizeof(NAS_MML_SIM_FORMAT_PLMN_ID));
    PS_MEM_SET(&stPlmnId, 0, sizeof(NAS_MML_PLMN_ID_STRU));

    /* 默认PTMSI有效 */
    ulValidPtmsiFlag    = VOS_TRUE;

    ucInvalidCnt        = 0;
    pstMsIdentity       = NAS_MML_GetSimMsIdentity();

    /* 位置区无效,则PTMSI的内容肯定是无效的,直接返回 */
    if ( (NAS_MML_LAC_LOW_BYTE_INVALID == pucPsLociFileContent[NAS_MMC_USIM_PSLOCI_LAC_POSITION_BEG])
      && ( (NAS_MML_LAC_LOW_BYTE_INVALID == pucPsLociFileContent[NAS_MMC_USIM_PSLOCI_LAC_POSITION_END])
        || (NAS_MML_LAC_HIGH_BYTE_INVALID == pucPsLociFileContent[NAS_MMC_USIM_PSLOCI_LAC_POSITION_END])))
    {
        NAS_INFO_LOG(WUEPS_PID_MMC, "NAS_MMC_UpdatePtmsiFormPsLociFile:invalid LAC FFFF OR FFFE");


        /* 不能直接RETURN,而需要显示的更新当前的PTMSI为无效值，譬如NV中的保存的
          是有效的，但文件中的是无效的，则以最后读取的为准 */
        ulValidPtmsiFlag = VOS_FALSE;
    }


    /* 位置区被设置为全0，则认为RAI无效,则PTMSI的内容肯定是无效的 TS24008:10.5.1.3 */
    if ( (NAS_MML_LAC_INVALID_SPECAL_IN_SIM == pucPsLociFileContent[NAS_MMC_USIM_PSLOCI_LAC_POSITION_BEG])
      && (NAS_MML_LAC_INVALID_SPECAL_IN_SIM == pucPsLociFileContent[NAS_MMC_USIM_PSLOCI_LAC_POSITION_END]) )
    {
        NAS_INFO_LOG(WUEPS_PID_MMC, "NAS_MMC_UpdatePtmsiFormPsLociFile:invalid LAC,ALL ZEROS");

        ulValidPtmsiFlag = VOS_FALSE;
    }

    /* 取得SIM卡中的PLMN ID信息，用于判读PLMN是否有效 */
    ucCurrPos           = NAS_MML_RAI_POS_EFPSLOCI_FILE;

    /* 存储PLMN ID */
    PS_MEM_CPY(stSimPlmnId.aucSimPlmn,
               pucPsLociFileContent + ucCurrPos,
               NAS_MML_SIM_PLMN_ID_LEN);

    NAS_MMC_ConvertSimPlmnToNasPLMN(&stSimPlmnId, &stPlmnId);

    /* PLMN ID无效，则不更新PTMSI */
    if ( VOS_FALSE == NAS_MML_IsPlmnIdValid(&stPlmnId) )
    {
        NAS_INFO_LOG(WUEPS_PID_MMC, "NAS_MMC_UpdatePtmsiFormPsLociFile:invalid PLMN ID");

        ulValidPtmsiFlag = VOS_FALSE;
    }

    /* 获取PTMSI的内容 */
    for (i = 0; i < NAS_MML_MAX_PTMSI_LEN; i++)
    {
        if (NAS_MML_PTMSI_INVALID == pucPsLociFileContent[i])
        {
            ucInvalidCnt++;
        }
    }

    /* PTMSI有效时才需要更新,由于PTMSI初始化会更新为无效值 */
    if ( NAS_MML_MAX_PTMSI_LEN == ucInvalidCnt )
    {
        NAS_INFO_LOG(WUEPS_PID_MMC, "NAS_MMC_UpdatePtmsiFormPsLociFile:invalid PTMSI");


        /* 不能直接RETURN,而需要显示的更新当前的PTMSI为无效值，譬如NV中的保存的
          是有效的，但文件中的是无效的，则以最后读取的为准 */
        ulValidPtmsiFlag = VOS_FALSE;
    }


    /* 若PTMSI无效，则显示更新PTMSI为无效值 */
    if ( VOS_FALSE == ulValidPtmsiFlag )
    {
        for ( i = 0 ; i < NAS_MML_MAX_PTMSI_LEN ; i++ )
        {
            pstMsIdentity->aucPtmsi[i] = NAS_MML_PTMSI_INVALID;
        }

        pulPtmsiValue = (VOS_UINT32*)pstMsIdentity->aucPtmsi;

        NAS_INFO_LOG1(WUEPS_PID_MMC, "NAS_MMC_UpdatePtmsiFormPsLociFile:PTMSI ", *pulPtmsiValue);

        return;
    }

    for (i = 0; i < NAS_MML_MAX_PTMSI_LEN; i++)
    {
        pstMsIdentity->aucPtmsi[i]  = pucPsLociFileContent[i];
    }

    pulPtmsiValue = (VOS_UINT32*)pstMsIdentity->aucPtmsi;

    NAS_INFO_LOG1(WUEPS_PID_MMC, "NAS_MMC_UpdatePtmsiFormPsLociFile:PTMSI ", *pulPtmsiValue);
}


VOS_VOID  NAS_MMC_UpdatePtmsiSignatureFormPsLociFile(
    VOS_UINT8                          *pucPsLociFileContent
)
{
    VOS_UINT32                          i;
    VOS_UINT8                           ucInvalidCnt;
    NAS_MML_SIM_MS_IDENTITY_STRU       *pstMsIdentity;

    ucInvalidCnt        = 0;
    pstMsIdentity       = NAS_MML_GetSimMsIdentity();

    for (i = NAS_MML_PTMSI_SIGNATRUE_POS_EFPSLOCI_FILE; i < (NAS_MML_PTMSI_SIGNATRUE_POS_EFPSLOCI_FILE + NAS_MML_MAX_PTMSI_SIGNATURE_LEN); i++)
    {
        if (NAS_MML_PTMSI_SIGNATURE_INVALID == pucPsLociFileContent[i])
        {
            ucInvalidCnt++;
        }
    }

    /* PTMSI Signature有效时才需要更新,由于PTMSI Signature初始化会更新为无效值 */
    if (NAS_MML_MAX_PTMSI_SIGNATURE_LEN == ucInvalidCnt)
    {
        NAS_INFO_LOG(WUEPS_PID_MMC, "NAS_MMC_UpdatePtmsiSignatureFormPsLociFile:invalid PTMSISignature");


        /* 更新PTMSI Signature内容为无效值 */
        for ( i = 0 ; i < NAS_MML_MAX_PTMSI_SIGNATURE_LEN ; i++ )
        {
            pstMsIdentity->aucPtmsiSignature[i] = NAS_MML_PTMSI_SIGNATURE_INVALID;
        }

        return;
    }

    for (i = 0; i < NAS_MML_MAX_PTMSI_SIGNATURE_LEN; i++)
    {
        pstMsIdentity->aucPtmsiSignature[i] = pucPsLociFileContent[i + NAS_MML_PTMSI_SIGNATRUE_POS_EFPSLOCI_FILE];
    }
}


VOS_VOID  NAS_MMC_UpdateRaiFormPsLociFile(
    VOS_UINT8                          *pucPsLociFileContent
)
{
    VOS_UINT32                          i;
    NAS_MML_SIM_FORMAT_PLMN_ID          stSimPlmnId;
    NAS_MML_PS_DOMAIN_INFO_STRU        *pstPsDomainInfo;
    VOS_UINT8                           ucCurrPos;

    PS_MEM_SET(&stSimPlmnId, 0, sizeof(NAS_MML_SIM_FORMAT_PLMN_ID));

    ucCurrPos           = NAS_MML_RAI_POS_EFPSLOCI_FILE;
    pstPsDomainInfo     = NAS_MML_GetPsDomainInfo();

    /* 存储PLMN ID */
    PS_MEM_CPY(stSimPlmnId.aucSimPlmn,
               pucPsLociFileContent + ucCurrPos,
               NAS_MML_SIM_PLMN_ID_LEN);

    NAS_MMC_ConvertSimPlmnToNasPLMN(&stSimPlmnId,
                                    &(pstPsDomainInfo->stLastSuccRai.stLai.stPlmnId));

    /* 存储LAC  */
    ucCurrPos += NAS_MML_SIM_PLMN_ID_LEN;
    for ( i = 0 ; i < NAS_MML_MAX_LAC_LEN ; i++ )
    {
        pstPsDomainInfo->stLastSuccRai.stLai.aucLac[i] = pucPsLociFileContent[ucCurrPos + i];
    }

    if ((NAS_MML_LAC_INVALID_SPECAL_IN_SIM == pstPsDomainInfo->stLastSuccRai.stLai.aucLac[0])
     && (NAS_MML_LAC_INVALID_SPECAL_IN_SIM == pstPsDomainInfo->stLastSuccRai.stLai.aucLac[1]))
    {
        pstPsDomainInfo->stLastSuccRai.stLai.aucLac[0] = NAS_MML_LAC_LOW_BYTE_INVALID;
        pstPsDomainInfo->stLastSuccRai.stLai.aucLac[1] = NAS_MML_LAC_HIGH_BYTE_INVALID;
    }

    /* 如果sim卡中的内容是0xFFFF,则认为LAI为0xFEFF */
    if ((NAS_MML_LAC_LOW_BYTE_INVALID == pstPsDomainInfo->stLastSuccRai.stLai.aucLac[0])
     && (NAS_MML_LAC_LOW_BYTE_INVALID == pstPsDomainInfo->stLastSuccRai.stLai.aucLac[1]))
    {
        pstPsDomainInfo->stLastSuccRai.stLai.aucLac[0] = NAS_MML_LAC_LOW_BYTE_INVALID;
        pstPsDomainInfo->stLastSuccRai.stLai.aucLac[1] = NAS_MML_LAC_HIGH_BYTE_INVALID;
    }

    ucCurrPos += NAS_MML_MAX_LAC_LEN;

    /* 存储RAC */
    pstPsDomainInfo->stLastSuccRai.ucRac    = pucPsLociFileContent[ucCurrPos];

    /* 更新MML中的RAI的内容 */
    NAS_MML_SetSimRai(&pucPsLociFileContent[NAS_MML_RAI_POS_EFPSLOCI_FILE]);
}


VOS_VOID  NAS_MMC_UpdateSimInfoFromPsLociFile(
    VOS_UINT8                          *pucPsLociFileContent
)
{
    NAS_MML_SIM_STATUS_STRU            *pstSimStatus;
    NAS_NVIM_PS_LOCI_SIM_FILES_STRU     stPsLocInfo;
    
    VOS_UINT8                           ucUpdateStatus;

    pstSimStatus    = NAS_MML_GetSimStatus();

    /*

    Identifier: '6F73'  Structure: transparent  Mandatory
    SFI: '0C'
    File size: 14 bytes Update activity: high
    Access Conditions:
        READ    PIN
        UPDATE  PIN
        DEACTIVATE  ADM
        ACTIVATE    ADM

    Bytes   Description M/O Length
    1 to 4  P-TMSI  M   4 bytes
    5 to 7  P-TMSI signature value  M   3 bytes
    8 to13  RAI M   6 bytes
    14  Routing Area update status  M   1 byte

    */

    NAS_MML_UpdateSimPsLociFileContent(pucPsLociFileContent);


    /* 更新PTMSI的内容 */
    NAS_MMC_UpdatePtmsiFormPsLociFile(pucPsLociFileContent);

    /* 更新PTMSI SIGNATURE的内容 */
    NAS_MMC_UpdatePtmsiSignatureFormPsLociFile(pucPsLociFileContent);

    /* 更新RAI的内容 */
    NAS_MMC_UpdateRaiFormPsLociFile(pucPsLociFileContent);

    /* 更新GPRS的状态 */
    ucUpdateStatus = pucPsLociFileContent[NAS_MML_PS_LOCI_SIM_FILE_LEN - 1];
    ucUpdateStatus = (ucUpdateStatus) & NAS_MML_UPDATE_STATUS_MASK_IN_SIM;

    if (ucUpdateStatus > NAS_MML_ROUTING_UPDATE_STATUS_BUTT )
    {
        ucUpdateStatus = NAS_MML_ROUTING_UPDATE_STATUS_BUTT;
    }
    pstSimStatus->enPsUpdateStatus  = ucUpdateStatus;
    
    NAS_MEM_SET_S(&stPsLocInfo, sizeof(stPsLocInfo), 0, sizeof(stPsLocInfo));

    NAS_MEM_CPY_S(&stPsLocInfo, sizeof(stPsLocInfo), pucPsLociFileContent, NAS_MML_PS_LOCI_SIM_FILE_LEN);

    NAS_MMC_WritePsLociNvim(&stPsLocInfo);

    
}


VOS_VOID  NAS_MMC_UpdatePsSecutityInfoFromUsimFile(
    VOS_UINT8                          *pucPsSecutityInfo
)
{
    NAS_MML_SIM_STATUS_STRU            *pstSimStatus;
    NAS_MML_SIM_PS_SECURITY_INFO_STRU  *pstPsSecurityInfo;

    pstSimStatus            = NAS_MML_GetSimStatus();
    pstPsSecurityInfo       = NAS_MML_GetSimPsSecurity();

    /* 存储PS security key */
    if (NAS_MML_SIM_TYPE_USIM == pstSimStatus->enSimType )
    {
        /*
        Identifier: '6F09'  Structure: transparent  Mandatory
        SFI: '08'
        File size: 33 bytes Update activity: high
        Access Conditions:
            READ    PIN
            UPDATE  PIN
            DEACTIVATE  ADM
            ACTIVATE    ADM

        Bytes   Description M/O Length
        1   Key set identifier KSIPS  M   1 byte
        2 to 17 Ciphering key CKPS    M   16 bytes
        18 to 33    Integrity key IKPS    M   16 bytes

        */

        /* 存储CKSN */
        pstPsSecurityInfo->ucCKSN   = (pucPsSecutityInfo[0] & NAS_MML_CKSN_INVALID);

        PS_MEM_CPY(pstPsSecurityInfo->aucUmtsCk,
                   &pucPsSecutityInfo[NAS_MML_CK_POS_EFKEY_FILE],
                   NAS_MML_UMTS_CK_LEN);

        PS_MEM_CPY(pstPsSecurityInfo->aucUmtsIk,
                   &pucPsSecutityInfo[NAS_MML_CK_POS_EFKEY_FILE + NAS_MML_UMTS_CK_LEN],
                   NAS_MML_UMTS_IK_LEN);

        NAS_MML_SecContext3GTO2G(pstPsSecurityInfo->aucUmtsIk,
                                 pstPsSecurityInfo->aucUmtsCk,
                                 pstPsSecurityInfo->aucGsmKc);
    }
    else
    {
        /*
        Identifier: '6F52'  Structure: transparent  Mandatory
        File size: 9 bytes  Update activity: high
        Bytes   Description M/O Length
        1 to 8  Ciphering key KcGPRS    M   8 bytes
        9   Ciphering key sequence number n M   1 byte
        */

        pstPsSecurityInfo->ucCKSN   = (pucPsSecutityInfo[NAS_MML_GSM_KC_LEN] & NAS_MML_CKSN_INVALID);

        PS_MEM_CPY(pstPsSecurityInfo->aucGsmKc,
                   pucPsSecutityInfo,
                   NAS_MML_GSM_KC_LEN);

        NAS_MML_SecContext2GTO3G(pstPsSecurityInfo->aucUmtsIk,
                                 pstPsSecurityInfo->aucUmtsCk,
                                 pstPsSecurityInfo->aucGsmKc);
    }
}



VOS_VOID  NAS_MMC_UpdateTmsiFormCsLociFile(
    VOS_UINT8                          *pucCsLociFileContent
)
{
    VOS_UINT32                          i;
    VOS_UINT8                           ucInvalidCnt;
    NAS_MML_SIM_MS_IDENTITY_STRU       *pstMsIdentity;

    NAS_MML_SIM_FORMAT_PLMN_ID          stSimPlmnId;
    NAS_MML_PLMN_ID_STRU                stPlmnId;
    VOS_UINT8                          *pucLaiPos;

    VOS_UINT32                          ulValidTmsiFlag;

    VOS_UINT32                         *pulTmsiValue = VOS_NULL_PTR;
    PS_MEM_SET(&stSimPlmnId, 0, sizeof(NAS_MML_SIM_FORMAT_PLMN_ID));
    PS_MEM_SET(&stPlmnId, 0, sizeof(NAS_MML_PLMN_ID_STRU));

    /* 默认TMSI有效 */
    ulValidTmsiFlag    = VOS_TRUE;

    ucInvalidCnt        = 0;
    pstMsIdentity       = NAS_MML_GetSimMsIdentity();

    /* 位置区无效,则TMSI的内容肯定是无效的,直接返回 */
    /* 参考31.102, 4.2.17章节, 定义了相应位置的字节含义 */
    if ( ( NAS_MML_LAC_LOW_BYTE_INVALID == pucCsLociFileContent[7] )
      && ( (NAS_MML_LAC_LOW_BYTE_INVALID == pucCsLociFileContent[8])
        || (NAS_MML_LAC_HIGH_BYTE_INVALID == pucCsLociFileContent[8])))
    {
        NAS_INFO_LOG(WUEPS_PID_MMC, "NAS_MMC_UpdateTmsiFormCsLociFile:invalid LAC");


        /* 不能直接RETURN,而需要显示的更新当前的PTMSI为无效值，譬如NV中的保存的
          是有效的，但文件中的是无效的，则以最后读取的为准 */
        ulValidTmsiFlag = VOS_FALSE;
    }


     /* 位置区被设置为全0，则认为RAI无效,则PTMSI的内容肯定是无效的 TS24008:10.5.1.3 */
    if ( (NAS_MML_LAC_INVALID_SPECAL_IN_SIM == pucCsLociFileContent[7])
      && (NAS_MML_LAC_INVALID_SPECAL_IN_SIM == pucCsLociFileContent[8]) )
    {
        NAS_INFO_LOG(WUEPS_PID_MMC, "NAS_MMC_UpdateTmsiFormCsLociFile:invalid LAC,ALL ZEROS");


        /* 不能直接RETURN,而需要显示的更新当前的TMSI为无效值，譬如NV中的保存的
          是有效的，但文件中的是无效的，则以最后读取的为准 */
        ulValidTmsiFlag = VOS_FALSE;
    }

    /* 取得SIM卡中的PLMN ID信息，用于判读PLMN是否有效 */
    pucLaiPos             = pucCsLociFileContent +  NAS_MML_LAI_POS_EFLOCI_FILE;

    /* 存储PLMN ID */
    PS_MEM_CPY(stSimPlmnId.aucSimPlmn,
               pucLaiPos,
               NAS_MML_SIM_PLMN_ID_LEN);

    NAS_MMC_ConvertSimPlmnToNasPLMN(&stSimPlmnId, &stPlmnId);

    /* PLMN ID无效，则不更新TMSI */
    if ( VOS_FALSE == NAS_MML_IsPlmnIdValid(&stPlmnId) )
    {
        NAS_INFO_LOG(WUEPS_PID_MMC, "NAS_MMC_UpdateTmsiFormCsLociFile:invalid PLMN ID");


        /* 不能直接RETURN,而需要显示的更新当前的TMSI为无效值，譬如NV中的保存的
          是有效的，但文件中的是无效的，则以最后读取的为准 */
        ulValidTmsiFlag = VOS_FALSE;
    }

    for (i = 0; i < NAS_MML_MAX_TMSI_LEN; i++)
    {
        if (NAS_MML_TMSI_INVALID == pucCsLociFileContent[i])
        {
            ucInvalidCnt++;
        }
    }

    if ( NAS_MML_MAX_TMSI_LEN == ucInvalidCnt )
    {
        NAS_INFO_LOG(WUEPS_PID_MMC, "NAS_MMC_UpdateTmsiFormCsLociFile:invalid TMSI");

        /* 不能直接RETURN,而需要显示的更新当前的TMSI为无效值，譬如NV中的保存的
          是有效的，但文件中的是无效的，则以最后读取的为准 */
        ulValidTmsiFlag = VOS_FALSE;
    }


    /* 若TMSI无效，则显式更新TMSI为无效值 */
    if ( VOS_FALSE == ulValidTmsiFlag )
    {
        for ( i = 0 ; i < NAS_MML_MAX_TMSI_LEN ; i++ )
        {
            pstMsIdentity->aucTmsi[i] = NAS_MML_TMSI_INVALID;
        }

        pulTmsiValue = (VOS_UINT32*)pstMsIdentity->aucTmsi;

        NAS_INFO_LOG1(WUEPS_PID_MMC,"NAS_MMC_UpdateTmsiFormCsLociFile: tmsi %d", *pulTmsiValue);
        return;
    }

    for (i = 0; i < NAS_MML_MAX_TMSI_LEN; i++ )
    {
        pstMsIdentity->aucTmsi[i] = pucCsLociFileContent[i];
    }
    pulTmsiValue = (VOS_UINT32*)pstMsIdentity->aucTmsi;

    NAS_INFO_LOG1(WUEPS_PID_MMC,"NAS_MMC_UpdateTmsiFormCsLociFile: tmsi %d", *pulTmsiValue);
}


VOS_VOID  NAS_MMC_UpdateLaiFormCsLociFile(
    VOS_UINT8                          *pucCsLociFileContent
)
{
    NAS_MML_SIM_FORMAT_PLMN_ID          stSimPlmnId;
    NAS_MML_CS_DOMAIN_INFO_STRU        *pstCsDomainInfo = VOS_NULL_PTR;
    VOS_UINT8                          *pucLaiPos = VOS_NULL_PTR;
    VOS_UINT32                          i;

    PS_MEM_SET(&stSimPlmnId, 0, sizeof(NAS_MML_SIM_FORMAT_PLMN_ID));

    pstCsDomainInfo       = NAS_MML_GetCsDomainInfo();
    pucLaiPos             = pucCsLociFileContent +  NAS_MML_LAI_POS_EFLOCI_FILE;

    /* 存储PLMN ID */
    PS_MEM_CPY(stSimPlmnId.aucSimPlmn,
               pucLaiPos,
               NAS_MML_SIM_PLMN_ID_LEN);

    NAS_MMC_ConvertSimPlmnToNasPLMN(&stSimPlmnId,
                                    &(pstCsDomainInfo->stLastSuccLai.stPlmnId));


    /* 存储LAI */
    for ( i = 0 ; i < NAS_MML_MAX_LAC_LEN ; i++ )
    {
        pstCsDomainInfo->stLastSuccLai.aucLac[i] = *(pucLaiPos + NAS_MML_SIM_PLMN_ID_LEN + i);
    }

    if ((NAS_MML_LAC_INVALID_SPECAL_IN_SIM == pstCsDomainInfo->stLastSuccLai.aucLac[0])
     && (NAS_MML_LAC_INVALID_SPECAL_IN_SIM == pstCsDomainInfo->stLastSuccLai.aucLac[1]))
    {
        pstCsDomainInfo->stLastSuccLai.aucLac[0] = NAS_MML_LAC_LOW_BYTE_INVALID;
        pstCsDomainInfo->stLastSuccLai.aucLac[1] = NAS_MML_LAC_HIGH_BYTE_INVALID;
    }

    /* 如果sim卡中的内容是0xFFFF,则认为LAI为0xFEFF */
    if ((NAS_MML_LAC_LOW_BYTE_INVALID == pstCsDomainInfo->stLastSuccLai.aucLac[0])
     && (NAS_MML_LAC_LOW_BYTE_INVALID == pstCsDomainInfo->stLastSuccLai.aucLac[1]))
    {
        pstCsDomainInfo->stLastSuccLai.aucLac[0] = NAS_MML_LAC_LOW_BYTE_INVALID;
        pstCsDomainInfo->stLastSuccLai.aucLac[1] = NAS_MML_LAC_HIGH_BYTE_INVALID;
    }

    /* 更新MML中的LAI的内容 */
    NAS_MML_SetSimLai(&pucCsLociFileContent[NAS_MML_LAI_POS_EFLOCI_FILE]);
}


VOS_VOID  NAS_MMC_UpdateSimInfoFromCsLociFile(
    VOS_UINT8                          *pucCsLociFileContent
)
{
    NAS_MML_SIM_STATUS_STRU            *pstSimStatus = VOS_NULL_PTR;
    VOS_UINT8                           ucUpdateStatus;
    NAS_NVIM_CS_LOCI_SIM_FILES_STRU     stCsLocInfo;

    pstSimStatus    = NAS_MML_GetSimStatus();

    /*
    Identifier: '6F7E'  Structure: transparent  Mandatory
    SFI: '0B'
    File size: 11 bytes Update activity: high
    Access Conditions:
        READ    PIN
        UPDATE  PIN
        DEACTIVATE  ADM
        ACTIVATE    ADM

    Bytes   Description M/O Length
    1 to 4  TMSI    M   4 bytes
    5 to 9  LAI M   5 bytes
    10  RFU M   1 byte
    11  Location update status  M   1 byte
    */

    NAS_MML_UpdateSimCsLociFileContent(pucCsLociFileContent);

    /* 更新TMSI的内容 */
    NAS_MMC_UpdateTmsiFormCsLociFile(pucCsLociFileContent);

    /* 更新LAI的内容 */
    NAS_MMC_UpdateLaiFormCsLociFile(pucCsLociFileContent);

    /* 更新CS域的状态 */
    ucUpdateStatus = pucCsLociFileContent[NAS_MML_CS_LOCI_SIM_FILE_LEN - 1];
    ucUpdateStatus = (ucUpdateStatus) & NAS_MML_UPDATE_STATUS_MASK_IN_SIM;

    if (ucUpdateStatus > NAS_MML_LOCATION_UPDATE_STATUS_BUTT)
    {
        ucUpdateStatus = NAS_MML_LOCATION_UPDATE_STATUS_BUTT;
    }

    pstSimStatus->enCsUpdateStatus  = ucUpdateStatus;

    NAS_MEM_SET_S(&stCsLocInfo, sizeof(stCsLocInfo), 0, sizeof(stCsLocInfo));

    /* NV结构比实际长度大1个byte，按实际长度拷贝 */
    NAS_MEM_CPY_S(&stCsLocInfo, sizeof(stCsLocInfo), pucCsLociFileContent, NAS_MML_CS_LOCI_SIM_FILE_LEN);

    /* 将卡里面的内容，保存到NV中, 避免再次启动时，误以为卡在别的手机上使用过 */
    NAS_MMC_WriteCsLociNvim(&stCsLocInfo);

}


VOS_VOID  NAS_MMC_UpdateCsSecutityInfoFromUsimFile(
    VOS_UINT8                          *pucCsSecutityInfo
)
{
    NAS_MML_SIM_STATUS_STRU            *pstSimStatus;
    NAS_MML_SIM_CS_SECURITY_INFO_STRU  *pstCsSecurityInfo;

    pstSimStatus            = NAS_MML_GetSimStatus();
    pstCsSecurityInfo       = NAS_MML_GetSimCsSecurity();

    /* 存储CS security key */
    if (NAS_MML_SIM_TYPE_USIM == pstSimStatus->enSimType )
    {
        /*
        Identifier: '6F08'  Structure: transparent  Mandatory
        SFI: '08'
        File size: 33 bytes Update activity: high
        Access Conditions:
            READ    PIN
            UPDATE  PIN
            DEACTIVATE  ADM
            ACTIVATE    ADM

        Bytes   Description M/O Length
        1   Key set identifier KSI  M   1 byte
        2 to 17 Ciphering key CK    M   16 bytes
        18 to 33    Integrity key IK    M   16 bytes
        */
        /* 存储CKSN */
        pstCsSecurityInfo->ucCKSN   = (pucCsSecutityInfo[0] & NAS_MML_CKSN_INVALID);

        PS_MEM_CPY(pstCsSecurityInfo->aucUmtsCk,
                   &pucCsSecutityInfo[NAS_MML_CK_POS_EFKEY_FILE],
                   NAS_MML_UMTS_CK_LEN);

        PS_MEM_CPY(pstCsSecurityInfo->aucUmtsIk,
                   &pucCsSecutityInfo[NAS_MML_CK_POS_EFKEY_FILE + NAS_MML_UMTS_CK_LEN],
                   NAS_MML_UMTS_IK_LEN);

        NAS_MML_SecContext3GTO2G(pstCsSecurityInfo->aucUmtsIk,
                                 pstCsSecurityInfo->aucUmtsCk,
                                 pstCsSecurityInfo->aucGsmKc);
    }
    else
    {
        /*

        Identifier: '6F20'  Structure: transparent  Mandatory
        File size: 9 bytes  Update activity: high
        Access Conditions:
            READ    CHV1
            UPDATE  CHV1
            INVALIDATE  ADM
            REHABILITATE    ADM

        Bytes   Description M/O Length
        1 to 8  Ciphering key Kc    M   8 bytes
        9   Ciphering key sequence number n M   1 byte

        */

        pstCsSecurityInfo->ucCKSN   = (pucCsSecutityInfo[NAS_MML_GSM_KC_LEN] & NAS_MML_CKSN_INVALID);

        PS_MEM_CPY(pstCsSecurityInfo->aucGsmKc,
                   pucCsSecutityInfo,
                   NAS_MML_GSM_KC_LEN);

        NAS_MML_SecContext2GTO3G(pstCsSecurityInfo->aucUmtsIk,
                                 pstCsSecurityInfo->aucUmtsCk,
                                 pstCsSecurityInfo->aucGsmKc);
    }
}


VOS_VOID NAS_MMC_ReadUsimmPlmn(
    NAS_USIMM_GETFILE_INFO_STRU        *pstGetFileInfo
)
{
    VOS_UINT16                          usFileId;

    PS_MEM_SET(pstGetFileInfo, 0x00, sizeof(NAS_USIMM_GETFILE_INFO_STRU));
    if (NAS_MML_SIM_TYPE_USIM == NAS_MML_GetSimType())
    {/* 3G USIM */
        usFileId = USIMM_USIM_EFFPLMN_ID;
    }
    else
    {/* 2G SIM */
        usFileId = USIMM_GSM_EFFPLMN_ID;
    }

    /* 读取PLMN */
    NAS_COMM_BUILD_USIM_GET_FILE_INFO(pstGetFileInfo,
                                    USIMM_GUTL_APP,
                                    usFileId,
                                    0);

    (VOS_VOID)NAS_USIMMAPI_GetFileReq(WUEPS_PID_MMC, 0, pstGetFileInfo);
}


VOS_VOID NAS_MMC_ReadUsimmEfad(
    NAS_USIMM_GETFILE_INFO_STRU        *pstGetFileInfo
)
{
    VOS_UINT16                          usFileId;

    PS_MEM_SET(pstGetFileInfo, 0x00, sizeof(NAS_USIMM_GETFILE_INFO_STRU));

    if (NAS_MML_SIM_TYPE_USIM == NAS_MML_GetSimType())
    {/* 3G USIM */
        usFileId = USIMM_USIM_EFAD_ID;
    }
    else
    {/* 2G SIM */
        usFileId = USIMM_GSM_EFAD_ID;
    }

    /* 读取EFAD */
    NAS_COMM_BUILD_USIM_GET_FILE_INFO(pstGetFileInfo,
                                    USIMM_GUTL_APP,
                                    usFileId,
                                    0);

    (VOS_VOID)NAS_USIMMAPI_GetFileReq(WUEPS_PID_MMC, 0, pstGetFileInfo);
}


VOS_VOID NAS_MMC_ReadUsimmUplmn(
    NAS_USIMM_GETFILE_INFO_STRU        *pstGetFileInfo
)
{
    VOS_UINT16                          usFileId;

    PS_MEM_SET(pstGetFileInfo, 0x00, sizeof(NAS_USIMM_GETFILE_INFO_STRU));

    if (NAS_MML_SIM_TYPE_USIM == NAS_MML_GetSimType())
    {/* 3G USIM */
        usFileId = USIMM_USIM_EFPLMNWACT_ID;
    }
    else
    {/* 2G SIM */
        usFileId = USIMM_GSM_EFPLMNWACT_ID;
    }

    /* 读取UPLMN */
    NAS_COMM_BUILD_USIM_GET_FILE_INFO(pstGetFileInfo,
                                    USIMM_GUTL_APP,
                                    usFileId,
                                    0);

    (VOS_VOID)NAS_USIMMAPI_GetFileReq(WUEPS_PID_MMC, 0, pstGetFileInfo);
}


VOS_VOID NAS_MMC_ReadUsimmOplmn(
    NAS_USIMM_GETFILE_INFO_STRU        *pstGetFileInfo
)
{
    VOS_UINT16                          usFileId;

    PS_MEM_SET(pstGetFileInfo, 0x00, sizeof(NAS_USIMM_GETFILE_INFO_STRU));

    if (NAS_MML_SIM_TYPE_USIM == NAS_MML_GetSimType())
    {/* 3G USIM */
        usFileId = USIMM_USIM_EFOPLMNWACT_ID;
    }
    else
    {/* 2G SIM */
        usFileId = USIMM_GSM_EFOPLMNWACT_ID;
    }

    /* 读取OPLMN */
    NAS_COMM_BUILD_USIM_GET_FILE_INFO(pstGetFileInfo,
                                    USIMM_GUTL_APP,
                                    usFileId,
                                    0);

    (VOS_VOID)NAS_USIMMAPI_GetFileReq(WUEPS_PID_MMC, 0, pstGetFileInfo);
}


VOS_VOID NAS_MMC_ReadUsimmHplmn(
    NAS_USIMM_GETFILE_INFO_STRU        *pstGetFileInfo
)
{
    VOS_UINT16                          usFileId;

    PS_MEM_SET(pstGetFileInfo, 0x00, sizeof(NAS_USIMM_GETFILE_INFO_STRU));

    if (NAS_MML_SIM_TYPE_USIM == NAS_MML_GetSimType())
    {/* 3G USIM */
        usFileId = USIMM_USIM_EFHPLMNwACT_ID;
    }
    else
    {/* 2G SIM */
        usFileId = USIMM_GSM_EFHPLMNACT_ID;
    }

    /* 读取HPLMN */
    NAS_COMM_BUILD_USIM_GET_FILE_INFO(pstGetFileInfo,
                                    USIMM_GUTL_APP,
                                    usFileId,
                                    0);

    (VOS_VOID)NAS_USIMMAPI_GetFileReq(WUEPS_PID_MMC, 0, pstGetFileInfo);
}


VOS_VOID NAS_MMC_ReadUsimmCSLoci(
    NAS_USIMM_GETFILE_INFO_STRU        *pstGetFileInfo
)
{
    VOS_UINT16                          usFileId;

    PS_MEM_SET(pstGetFileInfo, 0x00, sizeof(NAS_USIMM_GETFILE_INFO_STRU));

    if (NAS_MML_SIM_TYPE_USIM == NAS_MML_GetSimType())
    {/* 3G USIM */
        usFileId = USIMM_USIM_EFLOCI_ID;
    }
    else
    {/* 2G SIM */
        usFileId = USIMM_GSM_EFLOCI_ID;
    }

    /* 读取CS LOCI*/
    NAS_COMM_BUILD_USIM_GET_FILE_INFO(pstGetFileInfo,
                                    USIMM_GUTL_APP,
                                    usFileId,
                                    0);

    (VOS_VOID)NAS_USIMMAPI_GetFileReq(WUEPS_PID_MMC, 0, pstGetFileInfo);
}


VOS_VOID NAS_MMC_ReadUsimmAtt(
    NAS_USIMM_GETFILE_INFO_STRU        *pstGetFileInfo
)
{
    VOS_UINT16                          usFileId;

    PS_MEM_SET(pstGetFileInfo, 0x00, sizeof(NAS_USIMM_GETFILE_INFO_STRU));

    if (NAS_MML_SIM_TYPE_USIM == NAS_MML_GetSimType())
    {/* 3G USIM */
        usFileId = USIMM_ATTUSIM_EFACTINGHPLMN_ID;
    }
    else
    {/* 2G SIM */
        usFileId = USIMM_ATTGSM_EFACTINGHPLMN_ID;
    }

    /* 读取ATT */
    NAS_COMM_BUILD_USIM_GET_FILE_INFO(pstGetFileInfo,
                                    USIMM_GUTL_APP,
                                    usFileId,
                                    0);

    (VOS_VOID)NAS_USIMMAPI_GetFileReq(WUEPS_PID_MMC, 0, pstGetFileInfo);
}


VOS_VOID NAS_MMC_ReadUsimmEhplmn(
    NAS_USIMM_GETFILE_INFO_STRU        *pstGetFileInfo
)
{
    PS_MEM_SET(pstGetFileInfo, 0x00, sizeof(NAS_USIMM_GETFILE_INFO_STRU));

    /* 读取EHPLMN */
    NAS_COMM_BUILD_USIM_GET_FILE_INFO(pstGetFileInfo,
                                    USIMM_GUTL_APP,
                                    USIMM_USIM_EFEHPLMN_ID,
                                    0);

    (VOS_VOID)NAS_USIMMAPI_GetFileReq(WUEPS_PID_MMC, 0, pstGetFileInfo);
}


VOS_VOID NAS_MMC_ReadUsimmEhplmnpi(
    NAS_USIMM_GETFILE_INFO_STRU        *pstGetFileInfo
)
{
    PS_MEM_SET(pstGetFileInfo, 0x00, sizeof(NAS_USIMM_GETFILE_INFO_STRU));

    /* 读取EHPLMNPI */
    NAS_COMM_BUILD_USIM_GET_FILE_INFO(pstGetFileInfo,
                                    USIMM_GUTL_APP,
                                    USIMM_USIM_EFEHPLMNPI_ID,
                                    0);

    (VOS_VOID)NAS_USIMMAPI_GetFileReq(WUEPS_PID_MMC, 0, pstGetFileInfo);
}


VOS_VOID NAS_MMC_ReadUsimmLrplmnsi(
    NAS_USIMM_GETFILE_INFO_STRU        *pstGetFileInfo
)
{
    PS_MEM_SET(pstGetFileInfo, 0x00, sizeof(NAS_USIMM_GETFILE_INFO_STRU));

    /* 读取LRPLMNSI */
    NAS_COMM_BUILD_USIM_GET_FILE_INFO(pstGetFileInfo,
                                    USIMM_GUTL_APP,
                                    USIMM_USIM_EFLRPLMNSI_ID,
                                    0);

    (VOS_VOID)NAS_USIMMAPI_GetFileReq(WUEPS_PID_MMC, 0, pstGetFileInfo);
}


VOS_VOID NAS_MMC_ReadUsimmHpplmn(
    NAS_USIMM_GETFILE_INFO_STRU        *pstGetFileInfo
)
{
    VOS_UINT16                          usFileId;

    PS_MEM_SET(pstGetFileInfo, 0x00, sizeof(NAS_USIMM_GETFILE_INFO_STRU));

    if (NAS_MML_SIM_TYPE_USIM == NAS_MML_GetSimType())
    {/* 3G USIM */
        usFileId = USIMM_USIM_EFHPPLMN_ID;
    }
    else
    {/* 2G SIM */
        usFileId = USIMM_GSM_EFHPLMN_ID;
    }

    /* 读取HPPLMN */
    NAS_COMM_BUILD_USIM_GET_FILE_INFO(pstGetFileInfo,
                                    USIMM_GUTL_APP,
                                    usFileId,
                                    0);

    (VOS_VOID)NAS_USIMMAPI_GetFileReq(WUEPS_PID_MMC, 0, pstGetFileInfo);
}


VOS_VOID NAS_MMC_ReadUsimmFile(
    NAS_USIMM_GETFILE_INFO_STRU        *pstGetFileInfo,
    VOS_UINT16                          usFileId
)
{
    PS_MEM_SET(pstGetFileInfo, 0x00, sizeof(NAS_USIMM_GETFILE_INFO_STRU));

    /* 读取 usFileId对应文件 */
    NAS_COMM_BUILD_USIM_GET_FILE_INFO(pstGetFileInfo,
                                    USIMM_GUTL_APP,
                                    usFileId,
                                    0);

    (VOS_VOID)NAS_USIMMAPI_GetFileReq(WUEPS_PID_MMC, 0, pstGetFileInfo);
}


#if (FEATURE_ON == FEATURE_CSG)

VOS_VOID NAS_MMC_ReadUsimmCsgFile(VOS_VOID)
{
    VOS_UINT32                          ulRet;

    /* 读取CSG相关的卡文件，包括EFACSGL/EFCSGT/EFHNBN/EFOCSGL/EFOCSGT/EFOHNBN,参见31102 */
    /* 由于文件类型为Linear fixed，需要先查询卡文件中的记录数，然后逐条读取文件中的记录 */
    ulRet = NAS_USIMMAPI_IsServiceAvailable(NAS_USIM_SVR_CSG_COR_IND);
    if (PS_USIM_SERVICE_AVAILIABLE == ulRet)
    {
        NAS_NORMAL_LOG(WUEPS_PID_MMC, "NAS_MMC_ReadUsimmCsgFile: CSG ALLOWED LIST FILE EXIST");
        /*查询Allowed CSG List相关的卡文件记录数 */
        NAS_MMC_ReadUsimmFileRecordNum(USIMM_USIM_EFACSGL_ID);
        NAS_MMC_ReadUsimmFileRecordNum(USIMM_USIM_EFCSGT_ID);
        NAS_MMC_ReadUsimmFileRecordNum(USIMM_USIM_EFHNBN_ID);

        /*置Allowed CSG List相关的卡文件读取标志 */
        NAS_MMC_SetWaitSimFilesCnfFlg_SwitchOn(NAS_MML_READ_ALLOWED_CSG_LIST_FILE_ID_FLG);
        NAS_MMC_SetWaitSimFilesCnfFlg_SwitchOn(NAS_MML_READ_ALLOWED_CSG_TYPE_FILE_ID_FLG);
        NAS_MMC_SetWaitSimFilesCnfFlg_SwitchOn(NAS_MML_READ_ALLOWED_CSG_HOME_NODEB_NAME_FILE_ID_FLG);
    }

    ulRet = NAS_USIMMAPI_IsServiceAvailable(NAS_USIM_SVR_OP_CSG);
    if (PS_USIM_SERVICE_AVAILIABLE == ulRet)
    {
        NAS_NORMAL_LOG(WUEPS_PID_MMC, "NAS_MMC_ReadUsimmCsgFile: CSG OPERATOR LIST FILE EXIST");
        /* 查询Operator CSG List相关的卡文件记录数 */
        NAS_MMC_ReadUsimmFileRecordNum(USIMM_USIM_EFOCSGL_ID);
        NAS_MMC_ReadUsimmFileRecordNum(USIMM_USIM_EFOCSGT_ID);
        NAS_MMC_ReadUsimmFileRecordNum(USIMM_USIM_EFOHNBN_ID);

        /*置Operator CSG List相关的卡文件读取标志 */
        NAS_MMC_SetWaitSimFilesCnfFlg_SwitchOn(NAS_MML_READ_OPERATOR_CSG_LIST_FILE_ID_FLG);
        NAS_MMC_SetWaitSimFilesCnfFlg_SwitchOn(NAS_MML_READ_OPERATOR_CSG_TYPE_FILE_ID_FLG);
        NAS_MMC_SetWaitSimFilesCnfFlg_SwitchOn(NAS_MML_READ_OPERATOR_CSG_HOME_NODEB_NAME_FILE_ID_FLG);
    }
}
#endif



VOS_VOID   NAS_MMC_SndUsimReadFileReq( VOS_VOID )
{
    VOS_UINT32                          ulRet;
    NAS_MML_SIM_STATUS_STRU            *pstSimStatus = VOS_NULL_PTR;
    NAS_MML_HPLMN_CFG_INFO_STRU        *pstHplmnCfgInfo = VOS_NULL_PTR;

    VOS_UINT32                          ulNvOplmnAvail = VOS_FALSE;

    NAS_USIMM_GETFILE_INFO_STRU         stGetFileInfo;


    pstHplmnCfgInfo = NAS_MML_GetHplmnCfg();

    pstSimStatus    = NAS_MML_GetSimStatus();

    NAS_MMC_ReadUsimmPlmn(&stGetFileInfo);

    NAS_MMC_SetWaitSimFilesCnfFlg_SwitchOn(NAS_MML_READ_FORBIDDEN_PLMN_LIST_FILE_ID_FLG);

    NAS_MMC_ReadUsimmEfad(&stGetFileInfo);

    NAS_MMC_SetWaitSimFilesCnfFlg_SwitchOn(NAS_MML_READ_MNC_LENGTH_FILE_ID_FLG);

    ulRet = NAS_USIMMAPI_IsServiceAvailable(NAS_USIM_SVR_UPLMN_SEL_WACT);

    if (PS_USIM_SERVICE_AVAILIABLE == ulRet)
    {
        NAS_MMC_ReadUsimmUplmn(&stGetFileInfo);

        NAS_MMC_SetWaitSimFilesCnfFlg_SwitchOn(NAS_MML_READ_UPLMN_SEL_FILE_ID_FLG);
    }

    ulRet = NAS_USIMMAPI_IsServiceAvailable(NAS_USIM_SVR_OPLMN_SEL_WACT);

    ulNvOplmnAvail = NAS_MML_IsNvimOplmnAvail();
    if ((PS_USIM_SERVICE_AVAILIABLE == ulRet)
     && (VOS_FALSE == ulNvOplmnAvail))
    {
        NAS_MMC_ReadUsimmOplmn(&stGetFileInfo);

        NAS_MMC_SetWaitSimFilesCnfFlg_SwitchOn(NAS_MML_READ_OPLMN_SEL_FILE_ID_FLG);
    }

    ulRet = NAS_USIMMAPI_IsServiceAvailable(NAS_USIM_SVR_HPLMN_WACT);

    if (PS_USIM_SERVICE_AVAILIABLE == ulRet)
    {
        NAS_MMC_ReadUsimmHplmn(&stGetFileInfo);

        NAS_MMC_SetWaitSimFilesCnfFlg_SwitchOn(NAS_MML_READ_HPLMN_SEL_FILE_ID_FLG);
    }

    NAS_MMC_ReadUsimmCSLoci(&stGetFileInfo);

    NAS_MMC_SetWaitSimFilesCnfFlg_SwitchOn(NAS_MML_READ_CS_LOC_FILE_ID_FLG);


    ulRet = NAS_USIMMAPI_IsServiceAvailable(NAS_USIM_SVR_EHPLMN);

    /* ACTING-HPLMN特性开关打开并且EHPLMN不存在时才需要获取ACTING-HPLMN，否则读取EHPLMN */
    if ( (VOS_TRUE == pstHplmnCfgInfo->ucActingHPlmnSupportFlg)
      && (PS_USIM_SERVICE_NOT_AVAILIABLE == ulRet))
    {
        NAS_MMC_ReadUsimmAtt(&stGetFileInfo);

        NAS_MMC_SetWaitSimFilesCnfFlg_SwitchOn(NAS_MML_READ_ACTING_HPLMN_FILE_ID_FLG);
    }
    else
    {
        /* EHPLMN特性开关打开时才需要获取 */
        if ( (VOS_TRUE == pstHplmnCfgInfo->ucEhplmnSupportFlg)
          && (PS_USIM_SERVICE_AVAILIABLE == ulRet))
        {
            NAS_MMC_ReadUsimmEhplmn(&stGetFileInfo);

            NAS_MMC_SetWaitSimFilesCnfFlg_SwitchOn(NAS_MML_READ_EHPLMN_FILE_ID_FLG);
        }
    }

    ulRet = NAS_USIMMAPI_IsServiceAvailable(NAS_USIM_SVR_EHPLMN_IND);

    if (PS_USIM_SERVICE_AVAILIABLE == ulRet)
    {
        NAS_MMC_ReadUsimmEhplmnpi(&stGetFileInfo);

        NAS_MMC_SetWaitSimFilesCnfFlg_SwitchOn(NAS_MML_READ_EHPLMNPI_FILE_ID_FLG);
    }

    ulRet = NAS_USIMMAPI_IsServiceAvailable(NAS_USIM_SVR_RPLMN_LACT);

    if (PS_USIM_SERVICE_AVAILIABLE == ulRet)
    {
        NAS_MMC_ReadUsimmLrplmnsi(&stGetFileInfo);

        NAS_MMC_SetWaitSimFilesCnfFlg_SwitchOn(NAS_MML_READ_LRPLMNSI_FILE_ID_FLG);
    }

    NAS_MMC_ReadUsimmHpplmn(&stGetFileInfo);

    NAS_MMC_SetWaitSimFilesCnfFlg_SwitchOn(NAS_MML_READ_HPLMN_PERI_FILE_ID_FLG);

    if ( NAS_MML_SIM_TYPE_SIM == pstSimStatus->enSimType)
    {
        /* 读取PS LOCI */
        NAS_MMC_ReadUsimmFile(&stGetFileInfo, USIMM_GSM_EFLOCIGPRS_ID);  /* NAS_MML_READ_SIM_PS_LOC_FILE_ID */

        NAS_MMC_SetWaitSimFilesCnfFlg_SwitchOn(NAS_MML_READ_PS_LOC_FILE_ID_FLG);

        /* 读取CS KC */
        NAS_MMC_ReadUsimmFile(&stGetFileInfo, USIMM_GSM_EFKC_ID);

        NAS_MMC_SetWaitSimFilesCnfFlg_SwitchOn(NAS_MML_READ_CS_KC_FILE_ID_FLG);

        /* 读取PS KC */
        NAS_MMC_ReadUsimmFile(&stGetFileInfo, USIMM_GSM_EFKCGPRS_ID);

        NAS_MMC_SetWaitSimFilesCnfFlg_SwitchOn(NAS_MML_READ_PS_KC_FILE_ID_FLG);

        ulRet = NAS_USIMMAPI_IsServiceAvailable(NAS_USIM_SVR_PLMN_SELECTOR);

        if (PS_USIM_SERVICE_AVAILIABLE == ulRet)
        {
            /* SIM卡下替代UPLMN和OPLMN */
            NAS_MMC_ReadUsimmFile(&stGetFileInfo, USIMM_GSM_EFPLMNSEL_ID);

            NAS_MMC_SetWaitSimFilesCnfFlg_SwitchOn(NAS_MML_READ_PLMN_SEL_FILE_ID_FLG);
        }
    }
    else
    {
        /* 读取PS LOCI */
        NAS_MMC_ReadUsimmFile(&stGetFileInfo, USIMM_USIM_EFPSLOCI_ID);

        NAS_MMC_SetWaitSimFilesCnfFlg_SwitchOn(NAS_MML_READ_PS_LOC_FILE_ID_FLG);

        /* 读取CS CK IK */
        NAS_MMC_ReadUsimmFile(&stGetFileInfo, USIMM_USIM_EFKEYS_ID);

        NAS_MMC_SetWaitSimFilesCnfFlg_SwitchOn(NAS_MML_READ_CS_CKIK_FILE_ID_FLG);

        /* 读取PS KEY */
        NAS_MMC_ReadUsimmFile(&stGetFileInfo, USIMM_USIM_EFKEYSPS_ID);

        NAS_MMC_SetWaitSimFilesCnfFlg_SwitchOn(NAS_MML_READ_PS_KEY_FILE_ID_FLG);
    }
#if (VOS_WIN32 != VOS_OS_VER)
#if (FEATURE_ON == FEATURE_CSG)
    NAS_MMC_ReadUsimmCsgFile();
#endif
#endif
}





VOS_VOID NAS_MMC_SndUsimUpdateForbPlmnReq(VOS_VOID)
{
    VOS_UINT8                          *pucFileContent = VOS_NULL_PTR;          /* 指向File内容的指针                       */
    VOS_UINT8                           ucForbPlmnNum;                          /* MMC forbidden PLMN的个数                 */
    VOS_UINT8                           ucUsimForbPlmnNum;                      /* MMC forbidden PLMN的个数                 */
    VOS_UINT8                           i;                                      /* 循环控制变量                             */
    VOS_UINT8                           ucDif;                                  /* MMC与SIM中forbidden PLMN个数之差         */
    VOS_UINT8                           ucSetUsimPlmnNum;
    NAS_MML_SIM_FORBIDPLMN_INFO_STRU   *pstForbidPlmnInfo = VOS_NULL_PTR;
    NAS_USIMM_UPDATEFILE_INFO_STRU      stSetFileInfo;
    VOS_UINT16                          usFileId;

    usFileId = USIMM_DEF_FILEID_BUTT;
    PS_MEM_SET(&stSetFileInfo, 0x00, sizeof(stSetFileInfo));

    pstForbidPlmnInfo           = NAS_MML_GetForbidPlmnInfo();
    ucForbPlmnNum               = pstForbidPlmnInfo->ucForbPlmnNum;
    ucUsimForbPlmnNum           = pstForbidPlmnInfo->ucUsimForbPlmnNum;
    ucSetUsimPlmnNum            = pstForbidPlmnInfo->ucUsimForbPlmnNum;

    NAS_NORMAL_LOG2(WUEPS_PID_MMC, "NAS_MMC_SndUsimUpdateForbPlmnReq: ucForbPlmnNum, ucUsimForbPlmnNum", ucForbPlmnNum, ucUsimForbPlmnNum);

    pucFileContent = (VOS_UINT8 *)PS_MEM_ALLOC(WUEPS_PID_MMC, ucUsimForbPlmnNum * 3);

    if (VOS_NULL_PTR == pucFileContent)
    {
        PS_NAS_LOG(WUEPS_PID_MMC, VOS_NULL, PS_LOG_LEVEL_ERROR, "NAS_MMC_SndUsimUpdateForbPlmnReq:ERROR: Memory Alloc Error for pMsg");

        return;
    }

    PS_MEM_SET(pucFileContent, 0, ucUsimForbPlmnNum * 3);

    /* 将MMC内部的forbidden PLMN转化为SIM中的格式 */
    if (ucForbPlmnNum >= ucUsimForbPlmnNum)
    {
        /* MMC forbidden PLMN的个数>=SIM中的个数    */
        ucDif = ucForbPlmnNum - ucUsimForbPlmnNum;

        /* 将MMC中最新的ucUsimForbPlmnNum个forbidden PLMN转化为SIM中的格式 */
        /* 参见3GPP 31.102, 4.2.16章节，对于SIM卡文件格式的描述*/
        for (i=0; i < ucUsimForbPlmnNum; i++)
        {
            *(pucFileContent + (i*3))
              = (VOS_UINT8)(0x0000000F & pstForbidPlmnInfo->astForbPlmnIdList[ucDif + i].ulMcc);

            *(pucFileContent + (i*3))
             |= (VOS_UINT8)((0x00000F00 & pstForbidPlmnInfo->astForbPlmnIdList[ucDif + i].ulMcc) >> 4);

            *(pucFileContent + (i*3) + 1)
             = (VOS_UINT8)((0x000F0000 & pstForbidPlmnInfo->astForbPlmnIdList[ucDif + i].ulMcc) >> 16);

            *(pucFileContent + (i*3) + 1)
             |= (VOS_UINT8)((0x000F0000 & pstForbidPlmnInfo->astForbPlmnIdList[ucDif + i].ulMnc) >> 12);

            *(pucFileContent + (i*3) + 2)
             = (VOS_UINT8)(0x0000000F & pstForbidPlmnInfo->astForbPlmnIdList[ucDif + i].ulMnc);

            *(pucFileContent + (i*3) + 2)
             |= (VOS_UINT8)((0x00000F00 & pstForbidPlmnInfo->astForbPlmnIdList[ucDif + i].ulMnc) >> 4);
        }
    }
    else
    {
        /* MMC forbidden PLMN的个数小于SIM中的个数 */
        if (NAS_MML_MAX_FORBPLMN_NUM > ucUsimForbPlmnNum)
        {
            ucDif = ucUsimForbPlmnNum - ucForbPlmnNum;
        }
        else
        {
            ucDif = NAS_MML_MAX_FORBPLMN_NUM - ucForbPlmnNum;

            /* 不更新USIM内超出UE支持范围的FPLMN */
            ucSetUsimPlmnNum = NAS_MML_MAX_FORBPLMN_NUM;
        }

        /* 将MMC中forbidden PLMN转化为SIM中的格式 */
        /* 参见3GPP 31.102, 4.2.16章节，对于SIM卡文件格式的描述 */
        for (i=0; i < ucForbPlmnNum; i++)
        {
            *(pucFileContent + (i*3))
              = (VOS_UINT8)(0x0000000F & pstForbidPlmnInfo->astForbPlmnIdList[i].ulMcc);

            *(pucFileContent + (i*3))
             |= (VOS_UINT8)((0x00000F00 & pstForbidPlmnInfo->astForbPlmnIdList[i].ulMcc) >> NAS_MML_OCTET_MOVE_FOUR_BITS);

            *(pucFileContent + (i*3) + 1)
             = (VOS_UINT8)((0x000F0000 & pstForbidPlmnInfo->astForbPlmnIdList[i].ulMcc) >> NAS_MML_OCTET_MOVE_SIXTEEN_BITS);
            *(pucFileContent + (i*3) + 1)
             |= (VOS_UINT8)((0x000F0000 & pstForbidPlmnInfo->astForbPlmnIdList[i].ulMnc) >> NAS_MML_OCTET_MOVE_TWELVE_BITS);

            *(pucFileContent + (i*3) + 2)
             = (VOS_UINT8)(0x0000000F & pstForbidPlmnInfo->astForbPlmnIdList[i].ulMnc);

            *(pucFileContent + (i*3) + 2)
             |= (VOS_UINT8)((0x00000F00 & pstForbidPlmnInfo->astForbPlmnIdList[i].ulMnc) >> NAS_MML_OCTET_MOVE_FOUR_BITS);
        }

        /* 剩余空间用oxFF埴充  */
        PS_MEM_SET((pucFileContent + (ucForbPlmnNum*3)), (VOS_CHAR)0xFF, (ucDif*3));

    }

    if (NAS_MML_SIM_TYPE_USIM == NAS_MML_GetSimType())
    {/* 3G USIM */
        usFileId = USIMM_USIM_EFFPLMN_ID;
    }
    else
    {/* 2G SIM */
        usFileId = USIMM_GSM_EFFPLMN_ID;
    }

    NAS_COMM_BUILD_USIM_SET_FILE_INFO(&stSetFileInfo,
                                    USIMM_GUTL_APP,
                                    usFileId,
                                    0,
                                    (VOS_UINT8)(ucSetUsimPlmnNum*NAS_MML_SIM_FORMAT_PLMN_LEN),
                                    pucFileContent);

    (VOS_VOID)NAS_USIMMAPI_SetFileReq(WUEPS_PID_MMC, 0, &stSetFileInfo);


    NAS_EventReport(WUEPS_PID_MMC, NAS_OM_EVENT_FPLMN_LIST,
                    pucFileContent, (i*3));

    /* 释放空间  */
    PS_MEM_FREE(WUEPS_PID_MMC, pucFileContent);
}


VOS_VOID  NAS_MMC_ConvertSimPlmnToNasPLMN(
    NAS_MML_SIM_FORMAT_PLMN_ID          *pstSimPlmn,
    NAS_MML_PLMN_ID_STRU                *pstNasPlmn
)
{

    VOS_UINT32                          ulTempData;
    NAS_MML_PLMN_ID_STRU                stPlmn;

    /*
       PLMN BCD 编码 方式

       MCC digit 2,  MCC digit 1, ----OCTET 1
       MNC digit 3,  MCC digit 3, ----OCTET 2
       MNC digit 2,  MNC digit 1, ----OCTET 3
    */
    ulTempData = NAS_MML_OCTET_LOW_FOUR_BITS & (pstSimPlmn->aucSimPlmn[0]);
    stPlmn.ulMcc = ulTempData;
    ulTempData = (NAS_MML_OCTET_HIGH_FOUR_BITS & (pstSimPlmn->aucSimPlmn[0])) >> NAS_MML_OCTET_MOVE_FOUR_BITS;
    stPlmn.ulMcc |= ulTempData << NAS_MML_OCTET_MOVE_EIGHT_BITS;

    ulTempData = NAS_MML_OCTET_LOW_FOUR_BITS & (pstSimPlmn->aucSimPlmn[1]);
    stPlmn.ulMcc |= ulTempData << NAS_MML_OCTET_MOVE_SIXTEEN_BITS;
    ulTempData = (NAS_MML_OCTET_HIGH_FOUR_BITS & (pstSimPlmn->aucSimPlmn[1])) >> NAS_MML_OCTET_MOVE_FOUR_BITS;

    stPlmn.ulMnc = ulTempData << NAS_MML_OCTET_MOVE_SIXTEEN_BITS;
    ulTempData = NAS_MML_OCTET_LOW_FOUR_BITS & (pstSimPlmn->aucSimPlmn[2]);
    stPlmn.ulMnc |= ulTempData;
    ulTempData = (NAS_MML_OCTET_HIGH_FOUR_BITS & (pstSimPlmn->aucSimPlmn[2])) >> NAS_MML_OCTET_MOVE_FOUR_BITS;
    stPlmn.ulMnc |= ulTempData << NAS_MML_OCTET_MOVE_EIGHT_BITS;

    *pstNasPlmn = stPlmn;


    return ;

}


VOS_VOID  NAS_MMC_ConvertNasPlmnToSimFormat(
    NAS_MML_PLMN_ID_STRU               *pstGUNasPlmn,
    NAS_MML_SIM_FORMAT_PLMN_ID         *pstSimPlmn
)
{
    pstSimPlmn->aucSimPlmn[0]
      = (VOS_UINT8)(0x0000000F & pstGUNasPlmn->ulMcc);
    pstSimPlmn->aucSimPlmn[0]
     |= (VOS_UINT8)((0x00000F00 & pstGUNasPlmn->ulMcc) >> 4);

    pstSimPlmn->aucSimPlmn[1]
     = (VOS_UINT8)((0x000F0000 & pstGUNasPlmn->ulMcc) >> 16);

    pstSimPlmn->aucSimPlmn[1]
     |= (VOS_UINT8)((0x000F0000 & pstGUNasPlmn->ulMnc) >> 12);

    pstSimPlmn->aucSimPlmn[2]
     = (VOS_UINT8)(0x0000000F & pstGUNasPlmn->ulMnc);
    pstSimPlmn->aucSimPlmn[2]
     |= (VOS_UINT8)((0x00000F00 & pstGUNasPlmn->ulMnc) >> 4);

    return;

}



VOS_VOID  NAS_MMC_ConverPlmnInfoFromSim(
    VOS_UINT8                          *pucUsimFileData,
    VOS_UINT32                          ulPlmnNum,
    NAS_MML_SIM_PLMN_WITH_RAT_STRU     *pstPlmnWithRat
)
{
    VOS_UINT32                          i;
    NAS_MML_PLMN_ID_STRU                stTempPlmn;
    NAS_MML_SIM_FORMAT_PLMN_ID          stSimPlmn;
    VOS_UINT16                          usTempData;

    PS_MEM_SET(&stTempPlmn, 0, sizeof(NAS_MML_PLMN_ID_STRU));
    PS_MEM_SET(&stSimPlmn, 0, sizeof(NAS_MML_SIM_FORMAT_PLMN_ID));

    for ( i = 0; i < ulPlmnNum; i++ )
    {
        /*
            31102
            1 to 3 PLMN (highest priority) M 3 bytes st
            4 to 5 PLMN Access Technology Identifier M 2 bytes
        */

        PS_MEM_CPY(stSimPlmn.aucSimPlmn, pucUsimFileData + (i * NAS_MML_PLMN_WITH_RAT_UNIT_LEN), NAS_MML_SIM_PLMN_ID_LEN);

        /* 转换PLMN ID为MMC内部的格式并存储 */
        NAS_MMC_ConvertSimPlmnToNasPLMN(&stSimPlmn, &stTempPlmn);
        pstPlmnWithRat[i].stPlmnId = stTempPlmn;

        /* 存储PLMN对应的RAT */
        usTempData = (VOS_UINT16)(pucUsimFileData[(i * NAS_MML_PLMN_WITH_RAT_UNIT_LEN) + NAS_MML_PLMN_WITH_RAT_FIRST_RAT_OFFSET] << NAS_MML_OCTET_MOVE_EIGHT_BITS)
                                | pucUsimFileData[(i * NAS_MML_PLMN_WITH_RAT_UNIT_LEN) + NAS_MML_PLMN_WITH_RAT_SECOND_RAT_OFFSET];
        pstPlmnWithRat[i].usSimRat = usTempData;

    }
}



VOS_UINT32 NAS_MMC_IsNeedSndStkSearchModeChgEvt(VOS_VOID)
{
    if (VOS_TRUE == NAS_MML_GetSvlteSupportFlag())
    {
       if (VOS_TRUE == NAS_MML_IsCsServDomainAvail(NAS_MML_GetMsMode()))
       {
           return VOS_TRUE;
       }
       return VOS_FALSE;
    }
    else
    {
        return VOS_TRUE;
    }

}




TAF_PH_PLMN_SEL_MODE NAS_MMC_ConvertSrchModeToStkSrchMode(
                                         VOS_UINT8 ucReselMode
                                         )
{
    switch (ucReselMode)
    {
        case NAS_MMC_PLMN_SELECTION_MODE_AUTO:
             return TAF_PH_PLMN_SEL_MODE_AUTO;

        case NAS_MMC_PLMN_SELECTION_MODE_MANUAL:
             return TAF_PH_PLMN_SEL_MODE_MANUAL;

        default:
            PS_NAS_LOG(WUEPS_PID_MMC, VOS_NULL, PS_LOG_LEVEL_WARNING, "NAS_MMC_ConvertSrchModeToStkSrchMode: The input is invalid");
            return TAF_PH_PLMN_SEL_MODE_BUTT;
    }
}



VOS_VOID NAS_MMC_SndStkSearchModeChgEvt(
    VOS_UINT8                           ucReselMode
)
{
    VOS_UINT32                          ulRet;
    VOS_UINT32                          ulMsgLength;
    NAS_STK_SRCHMODE_CHG_EVENT_STRU    *pstSearchModeChgEvent = VOS_NULL_PTR;

    ulMsgLength    = sizeof(NAS_STK_SRCHMODE_CHG_EVENT_STRU);

    pstSearchModeChgEvent = (NAS_STK_SRCHMODE_CHG_EVENT_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                                           (ulMsgLength - VOS_MSG_HEAD_LENGTH));
    if (VOS_NULL_PTR == pstSearchModeChgEvent)
    {
        PS_NAS_LOG(WUEPS_PID_MMC, VOS_NULL, PS_LOG_LEVEL_WARNING, "NAS_MMC_SndStkSearchModeChgEvt: Alloc Msg Fail!");
        return;
    }

    PS_MEM_SET(&(pstSearchModeChgEvent->stSrchModeChgEvent), 0, sizeof(NAS_STK_SRCHMODE_CHG_EVENT_INFO_STRU));

    pstSearchModeChgEvent->ulReceiverPid = NAS_GetOamReceivePid(MAPS_STK_PID);
    pstSearchModeChgEvent->ulMsgId       = ID_NAS_STK_NET_SRCH_MODE_CHG_EVENT;

    pstSearchModeChgEvent->stSrchModeChgEvent.ucSrchMode = NAS_MMC_ConvertSrchModeToStkSrchMode(ucReselMode);

    ulRet = PS_SEND_MSG(WUEPS_PID_MMC, pstSearchModeChgEvent);
    if (VOS_OK != ulRet)
    {
        PS_NAS_LOG(WUEPS_PID_MMC, VOS_NULL, PS_LOG_LEVEL_WARNING, "NAS_MMC_SndStkSearchModeChgEvt: Send Msg Fail!");
        return;
    }

    NAS_EventReport(WUEPS_PID_MMC,
                    NAS_OM_EVENT_STK_NETWORK_SEARCH_MODE_CHANGE_EVENT,
                    (VOS_VOID *)&ucReselMode,
                    NAS_OM_EVENT_STK_NETWORK_SEARCH_MODE_CHANGE_LEN);


    return;
}


VOS_VOID  NAS_MMC_UpdateOPlmnInfoFromNvim(
    VOS_UINT8                         *pucOPlmnData,
    VOS_UINT16                         usOPlmnNum
)
{
    NAS_MML_SIM_OPERPLMN_INFO_STRU     *pstOperPlmnInfo = VOS_NULL_PTR;
    NAS_MML_SIM_PLMN_WITH_RAT_STRU     *pstOplmnAddr    = VOS_NULL_PTR;

    /* 获取全局变量的地址 */
    pstOperPlmnInfo = NAS_MML_GetSimOperPlmnList();
    pstOplmnAddr    = pstOperPlmnInfo->astOperPlmnInfo;

    /* 初始化全局变量中的OPLMN信息，然后把SIM中读取的PLMN ID和RAT存储到全局变量中 */
    NAS_MML_InitSimOperPlmnInfo(pstOperPlmnInfo);

    /* 将NV中保存的用户设置的OPLMN信息转换为内存中的信息 */
    NAS_MMC_ConverPlmnInfoFromSim(pucOPlmnData, usOPlmnNum, pstOplmnAddr);

    /* 保存OPLMN的个数 */
    pstOperPlmnInfo->usOperPlmnNum = usOPlmnNum;

    return;
}


VOS_VOID  NAS_MMC_InsertNvimSupplementOplmnInfo(
    NAS_MML_CUSTOM_SUPPLEMENT_OPLMN_INFO_STRU              *pstNvimSupplementOplmnInfo
)
{
    NAS_MML_SIM_OPERPLMN_INFO_STRU     *pstOperPlmnInfo;
    NAS_MML_SIM_OPERPLMN_INFO_STRU     *pstOldOperPlmnInfo;
    VOS_UINT32                          i;
    VOS_UINT32                          j;

    if (VOS_FALSE == NAS_MML_IsNvimCustomSupplementOplmnAvail())
    {
        return;
    }

    pstOldOperPlmnInfo = (NAS_MML_SIM_OPERPLMN_INFO_STRU *)PS_MEM_ALLOC(WUEPS_PID_MMC, sizeof(NAS_MML_SIM_OPERPLMN_INFO_STRU));

    if (VOS_NULL_PTR == pstOldOperPlmnInfo)
    {
        return;
    }

    pstOperPlmnInfo = NAS_MML_GetSimOperPlmnList();

    PS_MEM_CPY(pstOldOperPlmnInfo, pstOperPlmnInfo, sizeof(NAS_MML_SIM_OPERPLMN_INFO_STRU));

    NAS_MML_InitSimOperPlmnInfo(pstOperPlmnInfo);

    for (i = 0,j = 0; i < pstNvimSupplementOplmnInfo->ucOplmnListNum; i++)
    {
        pstOperPlmnInfo->astOperPlmnInfo[j].stPlmnId = pstNvimSupplementOplmnInfo->astOPlmnList[i];
        pstOperPlmnInfo->astOperPlmnInfo[j].usSimRat = NAS_MML_SIM_ALL_RAT_SUPPORT_LTE;

        j++;
    }

    for (i = 0; i < pstOldOperPlmnInfo->usOperPlmnNum; i++)
    {
        pstOperPlmnInfo->astOperPlmnInfo[j].stPlmnId = pstOldOperPlmnInfo->astOperPlmnInfo[i].stPlmnId;
        pstOperPlmnInfo->astOperPlmnInfo[j].usSimRat = pstOldOperPlmnInfo->astOperPlmnInfo[i].usSimRat;

        j++;

        if (j >= NAS_MML_MAX_OPERPLMN_NUM)
        {
            break;
        }
    }

    pstOperPlmnInfo->usOperPlmnNum = (VOS_UINT16)j;

    PS_MEM_FREE(WUEPS_PID_MMC, pstOldOperPlmnInfo);

    return;
}






VOS_UINT8  NAS_MMC_GetUsimAllRefreshFileNumber(VOS_VOID)
{
    VOS_UINT8                   ucNumber;
    NAS_MML_SIM_TYPE_ENUM_UINT8 ucSimType = NAS_MML_GetSimType();

    if (NAS_MML_SIM_TYPE_USIM == ucSimType)
    {
        ucNumber = sizeof(g_astNasUsimAllRefreshFileID)/sizeof(g_astNasUsimAllRefreshFileID[0]);
    }
    else if (NAS_MML_SIM_TYPE_SIM == ucSimType)
    {
        ucNumber = sizeof(g_astNasGsmAllRefreshFileID)/sizeof(g_astNasGsmAllRefreshFileID[0]);
    }
    else
    {
        ucNumber = 0;
    }

    return ucNumber;
}


VOS_UINT16  NAS_MMC_GetUsimAllRefreshFileId(VOS_UINT8 ucIndex)
{
    NAS_MML_SIM_TYPE_ENUM_UINT8 ucSimType = NAS_MML_GetSimType();

    if (NAS_MML_SIM_TYPE_USIM == ucSimType)
    {
        if (NAS_MMC_GetUsimAllRefreshFileNumber() <= ucIndex)
        {
            return USIMM_DEF_FILEID_BUTT;
        }

        return g_astNasUsimAllRefreshFileID[ucIndex];
    }
    else if (NAS_MML_SIM_TYPE_SIM == ucSimType)
    {
        if (NAS_MMC_GetUsimAllRefreshFileNumber() <= ucIndex)
        {
            return USIMM_DEF_FILEID_BUTT;
        }

        return g_astNasGsmAllRefreshFileID[ucIndex];
    }
    else
    {
        return USIMM_DEF_FILEID_BUTT;
    }
}


VOS_VOID NAS_MMC_BuildRefreshFileList(
    USIMM_STKREFRESH_IND_STRU                  *pstUsimRefreshInd,
    NAS_MMC_USIMM_REFRESH_FILE_CONTENT_STRU    *pstRefreshFile
)
{
    VOS_UINT32      ulLoop;
    VOS_UINT16      usFileId;
    VOS_UINT16      usNumber;
    VOS_UINT16      usIndex;

    /* file list 类型时，copy 消息体的EF ID */
    if (USIMM_REFRESH_FILE_LIST == pstUsimRefreshInd->enRefreshType)
    {
        pstRefreshFile->usEfNum = pstUsimRefreshInd->usEfNum;

        if (USIMM_MAX_REFRESH_FILE_NUM < pstRefreshFile->usEfNum)
        {
            pstRefreshFile->usEfNum = USIMM_MAX_REFRESH_FILE_NUM;
        }


        for (ulLoop = 0; ulLoop < pstRefreshFile->usEfNum; ulLoop++)
        {
            PS_MEM_CPY(&pstRefreshFile->astRefreshFile[ulLoop], &pstUsimRefreshInd->astEfId[ulLoop], sizeof(USIM_REFRESH_FILE_STRU));
        }

        return;

    }

    /* ALL FILE 类型时，认为FILE ID 是能触发高优先级搜网的文件，并且根据卡类型分开读 */
    usNumber = NAS_MMC_GetUsimAllRefreshFileNumber();
    usIndex  = 0;

    for (ulLoop = 0; ulLoop < usNumber; ulLoop++)
    {
        usFileId = NAS_MMC_GetUsimAllRefreshFileId((VOS_UINT8)ulLoop);

        if (USIMM_DEF_FILEID_BUTT == usFileId)
        {
            continue;
        }

        pstRefreshFile->astRefreshFile[usIndex].usFileId     = usFileId;
        pstRefreshFile->astRefreshFile[usIndex].enAppType    = USIMM_GUTL_APP;

        pstRefreshFile->astRefreshFile[usIndex].usPathLen    = 0;
        PS_MEM_SET(pstRefreshFile->astRefreshFile[usIndex].aucPath, 0x0, sizeof(pstRefreshFile->astRefreshFile[usIndex].aucPath));

        usIndex++;
    }

    pstRefreshFile->usEfNum = usIndex;
}


/*lint -restore */


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif


