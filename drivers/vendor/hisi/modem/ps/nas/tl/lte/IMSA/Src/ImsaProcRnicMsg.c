

/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/
#include    "ImsaProcRnicMsg.h"
#include    "ImsaEntity.h"
#include    "ImsaPublic.h"
#include    "AtRnicInterface.h"

/*lint -e767*/
#define    THIS_FILE_ID       PS_FILE_ID_IMSAPROCRNICMSG_C
#define    THIS_NAS_FILE_ID   NAS_FILE_ID_IMSAPROCRNICMSG_C
/*lint +e767*/


/*****************************************************************************
  1.1 Cplusplus Announce
*****************************************************************************/

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
#if (FEATURE_ON == FEATURE_IMS)

/*****************************************************************************
  2 Declare the Global Variable
*****************************************************************************/


/*****************************************************************************
  3 Function
*****************************************************************************/




VOS_VOID IMSA_SndMsgRnicPdnActInd(const IMSA_PDP_CNTXT_INFO_STRU     *pstPdpInfo)
{

    IMSA_RNIC_PDN_ACT_IND_STRU          *pstRnicPdnActInd;
    VOS_UINT8                           aucTemp[IMSA_IPV6_ADDR_PREFIX_LEN]   = {0};

    IMSA_NORM_LOG("IMSA_SndMsgRnicPdnActInd enter!");
    TLPS_PRINT2LAYER_INFO(IMSA_SndMsgRnicPdnActInd_ENUM, LNAS_ENTRY);

    /*分配消息空间*/
    pstRnicPdnActInd = (VOS_VOID*)IMSA_ALLOC_MSG(sizeof(IMSA_RNIC_PDN_ACT_IND_STRU));

    /*检测是否分配成功*/
    if (VOS_NULL_PTR == pstRnicPdnActInd)
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_SndMsgRnicPdnActInd:ERROR:Alloc Msg fail!");
        TLPS_PRINT2LAYER_ERROR(IMSA_SndMsgRnicPdnActInd_ENUM, LNAS_NULL_PTR);
        return ;
    }

    IMSA_MEM_SET_S( IMSA_GET_MSG_ENTITY(pstRnicPdnActInd),
                    IMSA_GET_MSG_LENGTH(pstRnicPdnActInd),
                    0,
                    IMSA_GET_MSG_LENGTH(pstRnicPdnActInd));

    IMSA_WRITE_RNIC_MSG_HEAD(pstRnicPdnActInd,ID_IMSA_RNIC_PDN_ACT_IND);

    /*填写消息内容*/
    pstRnicPdnActInd->stPdnInfo.ucRabId = pstPdpInfo->ucPdpId;
    pstRnicPdnActInd->stPdnInfo.enModemId = MODEM_ID_0;
    pstRnicPdnActInd->stPdnInfo.enRatType = IMSA_RNIC_IMS_RAT_TYPE_LTE;

    if ((IMSA_IP_TYPE_IPV4 == pstPdpInfo->stPdpAddr.enIpType)
        || (IMSA_IP_TYPE_IPV4V6 == pstPdpInfo->stPdpAddr.enIpType))
    {
        /* 配置IPV4地址 */
        pstRnicPdnActInd->stPdnInfo.bitOpIpv4PdnInfo    = IMSA_OP_TRUE;
        IMSA_MEM_CPY_S( pstRnicPdnActInd->stPdnInfo.stIpv4PdnInfo.aucIpV4Addr,
                        IMSA_RNIC_IPV4_ADDR_LENGTH,
                        pstPdpInfo->stPdpAddr.aucIpV4Addr,
                        IMSA_IPV4_ADDR_LEN);

        /* 配置IPV4 DNS地址 */
        pstRnicPdnActInd->stPdnInfo.stIpv4PdnInfo.bitOpDnsPrim = pstPdpInfo->stPdpIpv4Dns.bitOpPriDns;
        IMSA_MEM_CPY_S( pstRnicPdnActInd->stPdnInfo.stIpv4PdnInfo.aucDnsPrimAddr,
                        IMSA_RNIC_IPV4_ADDR_LENGTH,
                        pstPdpInfo->stPdpIpv4Dns.aucPriDns,
                        IMSA_IPV4_ADDR_LEN);

        pstRnicPdnActInd->stPdnInfo.stIpv4PdnInfo.bitOpDnsSec = pstPdpInfo->stPdpIpv4Dns.bitOpSecDns;
        IMSA_MEM_CPY_S( pstRnicPdnActInd->stPdnInfo.stIpv4PdnInfo.aucDnsSecAddr,
                        IMSA_RNIC_IPV4_ADDR_LENGTH,
                        pstPdpInfo->stPdpIpv4Dns.aucSecDns,
                        IMSA_IPV4_ADDR_LEN);
    }

    if ((IMSA_IP_TYPE_IPV6 == pstPdpInfo->stPdpAddr.enIpType)
        || (IMSA_IP_TYPE_IPV4V6 == pstPdpInfo->stPdpAddr.enIpType))
    {
        if (0 != IMSA_MEM_CMP(aucTemp, pstPdpInfo->stPdpAddr.aucIpV6Addr, IMSA_IPV6_ADDR_PREFIX_LEN))
        {
            /* 配置IPV6地址 */
            pstRnicPdnActInd->stPdnInfo.bitOpIpv6PdnInfo    = IMSA_OP_TRUE;
            pstRnicPdnActInd->stPdnInfo.stIpv6PdnInfo.ulBitPrefixLen = IMSA_IPV6_ADDR_PREFIX_BIT_LEN;
            IMSA_MEM_CPY_S( pstRnicPdnActInd->stPdnInfo.stIpv6PdnInfo.aucIpV6Addr,
                            IMSA_RNIC_IPV6_ADDR_LENGTH,
                            pstPdpInfo->stPdpAddr.aucIpV6Addr,
                            IMSA_IPV6_ADDR_LEN);

            /* 配置IPV6 DNS地址 */
            pstRnicPdnActInd->stPdnInfo.stIpv6PdnInfo.bitOpDnsPrim = pstPdpInfo->stPdpIpv6Dns.bitOpPriDns;
            IMSA_MEM_CPY_S( pstRnicPdnActInd->stPdnInfo.stIpv6PdnInfo.aucDnsPrimAddr,
                            IMSA_RNIC_IPV6_ADDR_LENGTH,
                            pstPdpInfo->stPdpIpv6Dns.aucPriDns,
                            IMSA_IPV6_ADDR_LEN);

            pstRnicPdnActInd->stPdnInfo.stIpv6PdnInfo.bitOpDnsSec = pstPdpInfo->stPdpIpv6Dns.bitOpSecDns;
            IMSA_MEM_CPY_S( pstRnicPdnActInd->stPdnInfo.stIpv6PdnInfo.aucDnsSecAddr,
                            IMSA_RNIC_IPV6_ADDR_LENGTH,
                            pstPdpInfo->stPdpIpv6Dns.aucSecDns,
                            IMSA_IPV6_ADDR_LEN);
        }
    }

    /*调用消息发送函数 */
    IMSA_SND_MSG(pstRnicPdnActInd);
    return;
}

VOS_VOID IMSA_SndMsgRnicPdnDeactInd
(
    VOS_UINT8                           ucPdpId,
    IMSA_RNIC_IMS_RAT_TYPE_ENUM_UINT8   enRatType

)
{
    IMSA_RNIC_PDN_DEACT_IND_STRU          *pstRnicPdnDeactInd;
    /*VOS_UINT8                              aucTemp[IMSA_IPV6_ADDR_PREFIX_LEN]   = {0};*/

    /*分配消息空间*/
    pstRnicPdnDeactInd = (VOS_VOID*)IMSA_ALLOC_MSG(sizeof(IMSA_RNIC_PDN_DEACT_IND_STRU));

    /*检测是否分配成功*/
    if (VOS_NULL_PTR == pstRnicPdnDeactInd)
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_SndMsgRnicPdnDeactInd:ERROR:Alloc Msg fail!");
        TLPS_PRINT2LAYER_ERROR(IMSA_SndMsgRnicPdnDeactInd_ENUM, LNAS_NULL_PTR);
        return ;
    }

    IMSA_MEM_SET_S( IMSA_GET_MSG_ENTITY(pstRnicPdnDeactInd),
                    IMSA_GET_MSG_LENGTH(pstRnicPdnDeactInd),
                    0,
                    IMSA_GET_MSG_LENGTH(pstRnicPdnDeactInd));

    IMSA_WRITE_RNIC_MSG_HEAD(pstRnicPdnDeactInd,ID_IMSA_RNIC_PDN_DEACT_IND);

    /*填写消息内容*/
    pstRnicPdnDeactInd->ucRabId = ucPdpId;
    pstRnicPdnDeactInd->enModemId = MODEM_ID_0;

    pstRnicPdnDeactInd->enRatType   = enRatType;

    /*调用消息发送函数 */
    IMSA_SND_MSG(pstRnicPdnDeactInd);

    return;
}



VOS_VOID IMSA_SndMsgRnicPdnModifyInd
(
    const IMSA_PDP_CNTXT_INFO_STRU     *pstPdpInfo
)
{
    IMSA_RNIC_PDN_MODIFY_IND_STRU             *pstRnicPdnModifyInd;
    VOS_UINT8                           aucTemp[IMSA_IPV6_ADDR_PREFIX_LEN]   = {0};
    
    IMSA_NORM_LOG("IMSA_SndMsgRnicPdnModifyInd enter!");
    TLPS_PRINT2LAYER_INFO(IMSA_SndMsgRnicPdnActInd_ENUM, LNAS_ENTRY);
    
    /*分配消息空间*/
    pstRnicPdnModifyInd = (VOS_VOID*)IMSA_ALLOC_MSG(sizeof(IMSA_RNIC_PDN_ACT_IND_STRU));
    
    /*检测是否分配成功*/
    if (VOS_NULL_PTR == pstRnicPdnModifyInd)
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_SndMsgRnicPdnModifyInd:ERROR:Alloc Msg fail!");
        TLPS_PRINT2LAYER_ERROR(IMSA_SndMsgRnicPdnActInd_ENUM, LNAS_NULL_PTR);
        return ;
    }
    IMSA_MEM_SET_S( IMSA_GET_MSG_ENTITY(pstRnicPdnModifyInd),
                    IMSA_GET_MSG_LENGTH(pstRnicPdnModifyInd),
                    0,
                    IMSA_GET_MSG_LENGTH(pstRnicPdnModifyInd));
    IMSA_WRITE_RNIC_MSG_HEAD(pstRnicPdnModifyInd,ID_IMSA_RNIC_PDN_MODIFY_IND);
    
    /*填写消息内容*/
    pstRnicPdnModifyInd->stPdnInfo.ucRabId = pstPdpInfo->ucPdpId;
    pstRnicPdnModifyInd->stPdnInfo.enModemId = MODEM_ID_0;
    pstRnicPdnModifyInd->stPdnInfo.enRatType = IMSA_RNIC_IMS_RAT_TYPE_LTE;
    
    if ((IMSA_IP_TYPE_IPV4 == pstPdpInfo->stPdpAddr.enIpType)
        || (IMSA_IP_TYPE_IPV4V6 == pstPdpInfo->stPdpAddr.enIpType))
    {
        /* 配置IPV4地址 */
        pstRnicPdnModifyInd->stPdnInfo.bitOpIpv4PdnInfo    = IMSA_OP_TRUE;
        IMSA_MEM_CPY_S(   pstRnicPdnModifyInd->stPdnInfo.stIpv4PdnInfo.aucIpV4Addr,
                 		IMSA_RNIC_IPV4_ADDR_LENGTH,
                        pstPdpInfo->stPdpAddr.aucIpV4Addr,
                        IMSA_IPV4_ADDR_LEN);
    
        /* 配置IPV4 DNS地址 */
        pstRnicPdnModifyInd->stPdnInfo.stIpv4PdnInfo.bitOpDnsPrim = pstPdpInfo->stPdpIpv4Dns.bitOpPriDns;
        IMSA_MEM_CPY_S(   pstRnicPdnModifyInd->stPdnInfo.stIpv4PdnInfo.aucDnsPrimAddr,
		                IMSA_RNIC_IPV4_ADDR_LENGTH,
                        pstPdpInfo->stPdpIpv4Dns.aucPriDns,
                        IMSA_IPV4_ADDR_LEN);
    
        pstRnicPdnModifyInd->stPdnInfo.stIpv4PdnInfo.bitOpDnsSec = pstPdpInfo->stPdpIpv4Dns.bitOpSecDns;
        IMSA_MEM_CPY_S(   pstRnicPdnModifyInd->stPdnInfo.stIpv4PdnInfo.aucDnsSecAddr,
		                IMSA_RNIC_IPV4_ADDR_LENGTH,
                        pstPdpInfo->stPdpIpv4Dns.aucSecDns,
                        IMSA_IPV4_ADDR_LEN);
    }
    
    if ((IMSA_IP_TYPE_IPV6 == pstPdpInfo->stPdpAddr.enIpType)
        || (IMSA_IP_TYPE_IPV4V6 == pstPdpInfo->stPdpAddr.enIpType))
    {
        if (0 != IMSA_MEM_CMP(aucTemp, pstPdpInfo->stPdpAddr.aucIpV6Addr, IMSA_IPV6_ADDR_PREFIX_LEN))
        {
            /* 配置IPV6地址 */
            pstRnicPdnModifyInd->stPdnInfo.bitOpIpv6PdnInfo    = IMSA_OP_TRUE;
            pstRnicPdnModifyInd->stPdnInfo.stIpv6PdnInfo.ulBitPrefixLen = IMSA_IPV6_ADDR_PREFIX_BIT_LEN;
            IMSA_MEM_CPY_S(   pstRnicPdnModifyInd->stPdnInfo.stIpv6PdnInfo.aucIpV6Addr,
                            IMSA_RNIC_IPV6_ADDR_LENGTH,			
                            pstPdpInfo->stPdpAddr.aucIpV6Addr,
                            IMSA_IPV6_ADDR_LEN);
    
            /* 配置IPV6 DNS地址 */
            pstRnicPdnModifyInd->stPdnInfo.stIpv6PdnInfo.bitOpDnsPrim = pstPdpInfo->stPdpIpv6Dns.bitOpPriDns;
            IMSA_MEM_CPY_S(   pstRnicPdnModifyInd->stPdnInfo.stIpv6PdnInfo.aucDnsPrimAddr,
                            IMSA_RNIC_IPV6_ADDR_LENGTH,
                            pstPdpInfo->stPdpIpv6Dns.aucPriDns,
                            IMSA_IPV6_ADDR_LEN);
    
            pstRnicPdnModifyInd->stPdnInfo.stIpv6PdnInfo.bitOpDnsSec = pstPdpInfo->stPdpIpv6Dns.bitOpSecDns;
            IMSA_MEM_CPY_S(   pstRnicPdnModifyInd->stPdnInfo.stIpv6PdnInfo.aucDnsSecAddr,
                            IMSA_RNIC_IPV6_ADDR_LENGTH,
                            pstPdpInfo->stPdpIpv6Dns.aucSecDns,
                            IMSA_IPV6_ADDR_LEN);
        }
    }
    
    /*调用消息发送函数 */
    IMSA_SND_MSG(pstRnicPdnModifyInd);
    return;

}



VOS_VOID IMSA_SndMsgRnicWifiPdnActInd(const IMSA_WIFI_PDP_CNTXT_INFO_STRU     *pstPdpInfo)
{

    IMSA_RNIC_PDN_ACT_IND_STRU          *pstRnicPdnActInd;
    VOS_UINT8                           aucTemp[IMSA_IPV6_ADDR_PREFIX_LEN]   = {0};

    IMSA_NORM_LOG("IMSA_SndMsgRnicPdnActInd enter!");
    TLPS_PRINT2LAYER_INFO(IMSA_SndMsgRnicPdnActInd_ENUM, LNAS_ENTRY);

    /*分配消息空间*/
    pstRnicPdnActInd = (VOS_VOID*)IMSA_ALLOC_MSG(sizeof(IMSA_RNIC_PDN_ACT_IND_STRU));

    /*检测是否分配成功*/
    if (VOS_NULL_PTR == pstRnicPdnActInd)
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_SndMsgRnicPdnActInd:ERROR:Alloc Msg fail!");
        TLPS_PRINT2LAYER_ERROR(IMSA_SndMsgRnicPdnActInd_ENUM, LNAS_NULL_PTR);
        return ;
    }

    IMSA_MEM_SET_S( IMSA_GET_MSG_ENTITY(pstRnicPdnActInd),IMSA_GET_MSG_LENGTH(pstRnicPdnActInd), 0, IMSA_GET_MSG_LENGTH(pstRnicPdnActInd));

    IMSA_WRITE_RNIC_MSG_HEAD(pstRnicPdnActInd,ID_IMSA_RNIC_PDN_ACT_IND);

    /*填写消息内容*/
    /* wifi下没有承载ID，暂时填0 */
    pstRnicPdnActInd->stPdnInfo.ucRabId = 0;
    pstRnicPdnActInd->stPdnInfo.enModemId = MODEM_ID_0;

    pstRnicPdnActInd->stPdnInfo.enRatType = IMSA_RNIC_IMS_RAT_TYPE_WIFI;

    if ((IMSA_IP_TYPE_IPV4 == pstPdpInfo->stPdpAddr.enIpType)
        || (IMSA_IP_TYPE_IPV4V6 == pstPdpInfo->stPdpAddr.enIpType))
    {
        /* 配置IPV4地址 */
        pstRnicPdnActInd->stPdnInfo.bitOpIpv4PdnInfo    = IMSA_OP_TRUE;
        IMSA_MEM_CPY_S( pstRnicPdnActInd->stPdnInfo.stIpv4PdnInfo.aucIpV4Addr,
                        IMSA_RNIC_IPV4_ADDR_LENGTH,
                        pstPdpInfo->stPdpAddr.aucIpV4Addr,
                        IMSA_IPV4_ADDR_LEN);

        /* 配置IPV4 DNS地址 */
        if (IMSA_TRUE == pstPdpInfo->stPdpIpv4Dns.cOpPriDns)
        {
            pstRnicPdnActInd->stPdnInfo.stIpv4PdnInfo.bitOpDnsPrim = IMSA_OP_TRUE;
            IMSA_MEM_CPY_S(   pstRnicPdnActInd->stPdnInfo.stIpv4PdnInfo.aucDnsPrimAddr,
                            IMSA_RNIC_IPV4_ADDR_LENGTH,
                            pstPdpInfo->stPdpIpv4Dns.aucPriDns,
                            IMSA_IPV4_ADDR_LEN);
        }


        if (IMSA_TRUE == pstPdpInfo->stPdpIpv4Dns.cOpSecDns)
        {
            pstRnicPdnActInd->stPdnInfo.stIpv4PdnInfo.bitOpDnsSec = IMSA_OP_TRUE;
            IMSA_MEM_CPY_S(   pstRnicPdnActInd->stPdnInfo.stIpv4PdnInfo.aucDnsSecAddr,
                            IMSA_RNIC_IPV4_ADDR_LENGTH,
                            pstPdpInfo->stPdpIpv4Dns.aucSecDns,
                            IMSA_IPV4_ADDR_LEN);
        }

    }

    if ((IMSA_IP_TYPE_IPV6 == pstPdpInfo->stPdpAddr.enIpType)
        || (IMSA_IP_TYPE_IPV4V6 == pstPdpInfo->stPdpAddr.enIpType))
    {
        if (0 != IMSA_MEM_CMP(aucTemp, pstPdpInfo->stPdpAddr.aucIpV6Addr, IMSA_IPV6_ADDR_PREFIX_LEN))
        {
            /* 配置IPV6地址 */
            pstRnicPdnActInd->stPdnInfo.bitOpIpv6PdnInfo    = IMSA_OP_TRUE;
            pstRnicPdnActInd->stPdnInfo.stIpv6PdnInfo.ulBitPrefixLen = IMSA_IPV6_ADDR_PREFIX_BIT_LEN;
            IMSA_MEM_CPY_S(   pstRnicPdnActInd->stPdnInfo.stIpv6PdnInfo.aucIpV6Addr,
                            IMSA_IPV6_ADDR_LEN,
                            pstPdpInfo->stPdpAddr.aucIpV6Addr,
                            IMSA_IPV6_ADDR_LEN);

            /* 配置IPV6 DNS地址 */
            if (IMSA_TRUE == pstPdpInfo->stPdpIpv6Dns.cOpPriDns)
            {
                pstRnicPdnActInd->stPdnInfo.stIpv6PdnInfo.bitOpDnsPrim = IMSA_OP_TRUE;
                IMSA_MEM_CPY_S(   pstRnicPdnActInd->stPdnInfo.stIpv6PdnInfo.aucDnsPrimAddr,
                                IMSA_IPV6_ADDR_LEN,
                                pstPdpInfo->stPdpIpv6Dns.aucPriDns,
                                IMSA_IPV6_ADDR_LEN);
            }


            if (IMSA_TRUE == pstPdpInfo->stPdpIpv6Dns.cOpSecDns)
            {
                pstRnicPdnActInd->stPdnInfo.stIpv6PdnInfo.bitOpDnsSec = IMSA_OP_TRUE;
                IMSA_MEM_CPY_S(   pstRnicPdnActInd->stPdnInfo.stIpv6PdnInfo.aucDnsSecAddr,
                                IMSA_IPV6_ADDR_LEN,
                                pstPdpInfo->stPdpIpv6Dns.aucSecDns,
                                IMSA_IPV6_ADDR_LEN);
            }

        }
    }

    /*调用消息发送函数 */
    IMSA_SND_MSG(pstRnicPdnActInd);
    return;
}


VOS_VOID IMSA_SndMsgRnicWifiPdnModifyInd(const IMSA_WIFI_PDP_CNTXT_INFO_STRU     *pstPdpInfo)
{

    IMSA_RNIC_PDN_MODIFY_IND_STRU      *pstRnicPdnModifyInd;
    VOS_UINT8                           aucTemp[IMSA_IPV6_ADDR_PREFIX_LEN]   = {0};

    IMSA_NORM_LOG("IMSA_SndMsgRnicPdnActInd enter!");
    TLPS_PRINT2LAYER_INFO(IMSA_SndMsgRnicPdnActInd_ENUM, LNAS_ENTRY);

    /*分配消息空间*/
    pstRnicPdnModifyInd = (VOS_VOID*)IMSA_ALLOC_MSG(sizeof(IMSA_RNIC_PDN_MODIFY_IND_STRU));

    /*检测是否分配成功*/
    if (VOS_NULL_PTR == pstRnicPdnModifyInd)
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_SndMsgRnicPdnActInd:ERROR:Alloc Msg fail!");
        TLPS_PRINT2LAYER_ERROR(IMSA_SndMsgRnicPdnActInd_ENUM, LNAS_NULL_PTR);
        return ;
    }

    IMSA_MEM_SET_S( IMSA_GET_MSG_ENTITY(pstRnicPdnModifyInd),
                    IMSA_GET_MSG_LENGTH(pstRnicPdnModifyInd),
                    0,
                    IMSA_GET_MSG_LENGTH(pstRnicPdnModifyInd));

    IMSA_WRITE_RNIC_MSG_HEAD(pstRnicPdnModifyInd,ID_IMSA_RNIC_PDN_MODIFY_IND);

    /*填写消息内容*/
    /* wifi下没有承载ID，暂时填0 */
    pstRnicPdnModifyInd->stPdnInfo.ucRabId = 0;
    pstRnicPdnModifyInd->stPdnInfo.enModemId = MODEM_ID_0;

    pstRnicPdnModifyInd->stPdnInfo.enRatType = IMSA_RNIC_IMS_RAT_TYPE_WIFI;

    if ((IMSA_IP_TYPE_IPV4 == pstPdpInfo->stPdpAddr.enIpType)
        || (IMSA_IP_TYPE_IPV4V6 == pstPdpInfo->stPdpAddr.enIpType))
    {
        /* 配置IPV4地址 */
        pstRnicPdnModifyInd->stPdnInfo.bitOpIpv4PdnInfo    = IMSA_OP_TRUE;
        IMSA_MEM_CPY_S(   pstRnicPdnModifyInd->stPdnInfo.stIpv4PdnInfo.aucIpV4Addr,
                        IMSA_RNIC_IPV4_ADDR_LENGTH,
						pstPdpInfo->stPdpAddr.aucIpV4Addr,
                        IMSA_IPV4_ADDR_LEN);

        /* 配置IPV4 DNS地址 */
        if (IMSA_TRUE == pstPdpInfo->stPdpIpv4Dns.cOpPriDns)
        {
            pstRnicPdnModifyInd->stPdnInfo.stIpv4PdnInfo.bitOpDnsPrim = IMSA_OP_TRUE;
            IMSA_MEM_CPY_S(   pstRnicPdnModifyInd->stPdnInfo.stIpv4PdnInfo.aucDnsPrimAddr,
                            IMSA_RNIC_IPV4_ADDR_LENGTH,			
                            pstPdpInfo->stPdpIpv4Dns.aucPriDns,
                            IMSA_IPV4_ADDR_LEN);
        }


        if (IMSA_TRUE == pstPdpInfo->stPdpIpv4Dns.cOpSecDns)
        {
            pstRnicPdnModifyInd->stPdnInfo.stIpv4PdnInfo.bitOpDnsSec = IMSA_OP_TRUE;
            IMSA_MEM_CPY_S(   pstRnicPdnModifyInd->stPdnInfo.stIpv4PdnInfo.aucDnsSecAddr,
                            IMSA_RNIC_IPV4_ADDR_LENGTH,			
                            pstPdpInfo->stPdpIpv4Dns.aucSecDns,
                            IMSA_IPV4_ADDR_LEN);
        }

    }

    if ((IMSA_IP_TYPE_IPV6 == pstPdpInfo->stPdpAddr.enIpType)
        || (IMSA_IP_TYPE_IPV4V6 == pstPdpInfo->stPdpAddr.enIpType))
    {
        if (0 != IMSA_MEM_CMP(aucTemp, pstPdpInfo->stPdpAddr.aucIpV6Addr, IMSA_IPV6_ADDR_PREFIX_LEN))
        {
            /* 配置IPV6地址 */
            pstRnicPdnModifyInd->stPdnInfo.bitOpIpv6PdnInfo    = IMSA_OP_TRUE;
            pstRnicPdnModifyInd->stPdnInfo.stIpv6PdnInfo.ulBitPrefixLen = IMSA_IPV6_ADDR_PREFIX_BIT_LEN;
            IMSA_MEM_CPY_S(   pstRnicPdnModifyInd->stPdnInfo.stIpv6PdnInfo.aucIpV6Addr,
                            IMSA_RNIC_IPV6_ADDR_LENGTH,
                            pstPdpInfo->stPdpAddr.aucIpV6Addr,
                            IMSA_IPV6_ADDR_LEN);

            /* 配置IPV6 DNS地址 */
            if (IMSA_TRUE == pstPdpInfo->stPdpIpv6Dns.cOpPriDns)
            {
                pstRnicPdnModifyInd->stPdnInfo.stIpv6PdnInfo.bitOpDnsPrim = IMSA_OP_TRUE;
                IMSA_MEM_CPY_S(   pstRnicPdnModifyInd->stPdnInfo.stIpv6PdnInfo.aucDnsPrimAddr,
                                IMSA_RNIC_IPV6_ADDR_LENGTH,
                                pstPdpInfo->stPdpIpv6Dns.aucPriDns,
                                IMSA_IPV6_ADDR_LEN);
            }


            if (IMSA_TRUE == pstPdpInfo->stPdpIpv6Dns.cOpSecDns)
            {
                pstRnicPdnModifyInd->stPdnInfo.stIpv6PdnInfo.bitOpDnsSec = IMSA_OP_TRUE;
                IMSA_MEM_CPY_S(   pstRnicPdnModifyInd->stPdnInfo.stIpv6PdnInfo.aucDnsSecAddr,
                                IMSA_RNIC_IPV6_ADDR_LENGTH,
                                pstPdpInfo->stPdpIpv6Dns.aucSecDns,
                                IMSA_IPV6_ADDR_LEN);
            }

        }
    }

    /*调用消息发送函数 */
    IMSA_SND_MSG(pstRnicPdnModifyInd);
    return;
}




#endif

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif
/* end of ImsaProcRnicMsg.c */




