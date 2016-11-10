

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#include "oal_net.h"
#include "oal_cfg80211.h"
#include "oal_ext_if.h"
#include "frw_ext_if.h"
#include "wal_linux_cfgvendor.h"
#include "wal_linux_ioctl.h"

#undef  THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_WAL_LINUX_CFGVENDOR_C

#define OUI_GOOGLE  0x001A11
#define OUI_HISI    0x001018

#if (defined(_PRE_PRODUCT_ID_HI110X_HOST) || (LINUX_VERSION_CODE >= KERNEL_VERSION(3,14,0))) && (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)


OAL_STATIC oal_void wal_cfgvendor_del_radar_channel(mac_vendor_cmd_channel_list_stru *pst_channel_list)
{
    oal_uint8 uc_loops;
    oal_uint8 uc_idx;
    oal_uint8 uc_channel_num;
    oal_uint8 uc_channel_idx;
    oal_uint8 auc_tmp_channel_list[MAC_CHANNEL_FREQ_5_BUTT];

    if (pst_channel_list->uc_channel_num_5g == 0)
    {
        return;
    }

    uc_channel_idx = 0;
    uc_channel_num = OAL_MIN(MAC_CHANNEL_FREQ_5_BUTT, pst_channel_list->uc_channel_num_5g);

    for (uc_loops = 0; uc_loops < uc_channel_num; uc_loops++)
    {
        if (OAL_SUCC != mac_get_channel_idx_from_num(MAC_RC_START_FREQ_5, pst_channel_list->auc_channel_list_5g[uc_loops], &uc_idx))
        {
            continue;
        }
        if (OAL_TRUE == mac_is_ch_in_radar_band(MAC_RC_START_FREQ_5, uc_idx))
        {
            continue;
        }

        auc_tmp_channel_list[uc_channel_idx++] = pst_channel_list->auc_channel_list_5g[uc_loops];
    }
    pst_channel_list->uc_channel_num_5g = uc_channel_idx;
    oal_memcopy(pst_channel_list->auc_channel_list_5g, auc_tmp_channel_list, uc_channel_idx);
}


OAL_STATIC oal_uint32 wal_cfgvendor_copy_channel_list(mac_vendor_cmd_channel_list_stru *pst_channel_list,
                                                            oal_uint32   band,
                                                            oal_int32  * pl_channel_list,
                                                            oal_uint32 * pul_num_channels)
{
    oal_uint8  *puc_chanel_list;
    oal_uint32  ul_loop;

    *pul_num_channels = (band == WLAN_BAND_2G)?pst_channel_list->uc_channel_num_2g:pst_channel_list->uc_channel_num_5g;
    if (*pul_num_channels == 0)
    {
        return OAL_FAIL;
    }

    *pul_num_channels = OAL_MIN(*pul_num_channels, (band == WLAN_BAND_2G)?MAC_CHANNEL_FREQ_2_BUTT:MAC_CHANNEL_FREQ_5_BUTT);

    puc_chanel_list = (band == WLAN_BAND_2G)?pst_channel_list->auc_channel_list_2g:pst_channel_list->auc_channel_list_5g;
    for (ul_loop = 0; ul_loop < *pul_num_channels; ul_loop++)
    {
        pl_channel_list[ul_loop] = oal_ieee80211_channel_to_frequency(puc_chanel_list[ul_loop],
                                                                    (band == WLAN_BAND_2G)?IEEE80211_BAND_2GHZ:IEEE80211_BAND_5GHZ);
    }

    return OAL_SUCC;
}


OAL_STATIC oal_uint32 wal_cfgvendor_get_current_channel_list(oal_net_device_stru *pst_netdev,
                                                    oal_uint32   band,
                                                    oal_int32  * pl_channel_list,
                                                    oal_uint32 * pul_num_channels)
{
    oal_int32                     l_ret;
    wal_msg_query_stru            st_query_msg;
    wal_msg_stru                 *pst_rsp_msg = OAL_PTR_NULL;
    wal_msg_rsp_stru             *pst_query_rsp_msg;
    mac_vendor_cmd_channel_list_stru *pst_channel_list = OAL_PTR_NULL;

    if (OAL_PTR_NULL == pst_netdev || OAL_PTR_NULL == pl_channel_list || OAL_PTR_NULL == pul_num_channels)
    {
        OAM_ERROR_LOG3(0, OAM_SF_ANY, "{wal_get_current_channel_list::channel_list or num_channel is NULL!"
                                        "netdev %p, channel_list %p, num_channels %p.}",
                                        pst_netdev, pl_channel_list, pul_num_channels);
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* 上层在任何时候都可能下发此命令，需要先判断当前netdev的状态并及时返回 */
    if (OAL_UNLIKELY(OAL_PTR_NULL == OAL_NET_DEV_PRIV(pst_netdev)))
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{wal_get_current_channel_list::NET_DEV_PRIV is NULL.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* 消息参数准备 */
    st_query_msg.en_wid = WLAN_CFGID_VENDOR_CMD_GET_CHANNEL_LIST;

    /***************************************************************************
        抛事件到wal层处理
    ***************************************************************************/
    /* 发送消息 */
    l_ret = wal_send_cfg_event(pst_netdev,
                               WAL_MSG_TYPE_QUERY,
                               WAL_MSG_WID_LENGTH,
                               (oal_uint8 *)&st_query_msg,
                               OAL_TRUE,
                               &pst_rsp_msg);

    if (OAL_SUCC != l_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_get_current_channel_list:: wal_send_cfg_event return err code %d!}\r\n", l_ret);
        if (OAL_PTR_NULL != pst_rsp_msg)
        {
            oal_free(pst_rsp_msg);
        }
        return l_ret;
    }

    pst_query_rsp_msg = (wal_msg_rsp_stru *)(pst_rsp_msg->auc_msg_data);
    pst_channel_list  = (mac_vendor_cmd_channel_list_stru *)(pst_query_rsp_msg->auc_value);

    /* 从信道列表中删除DFS 信道不上报 */
    wal_cfgvendor_del_radar_channel(pst_channel_list);

    if (wal_cfgvendor_copy_channel_list(pst_channel_list, band, pl_channel_list, pul_num_channels) != OAL_SUCC)
    {
        oal_free(pst_rsp_msg);
        return OAL_FAIL;
    }

    oal_free(pst_rsp_msg);
    return OAL_SUCC;
}


OAL_STATIC oal_int32 wal_cfgvendor_get_channel_list(oal_wiphy_stru *wiphy,
    oal_wireless_dev_stru *wdev, OAL_CONST oal_void  *data, oal_int32 len)
{
    oal_int32  err = 0;
    oal_int32  type;
    oal_int32  al_channel_list[MAC_MAX_SUPP_CHANNEL] = {0};
    oal_uint32 band;
    oal_uint32 mem_needed;
    oal_uint32 num_channels = 0;
    oal_uint32 reply_len = 0;
    oal_uint32 ul_ret;
    oal_netbuf_stru *skb;

    if (wdev == OAL_PTR_NULL || wdev->netdev == OAL_PTR_NULL)
    {
        OAM_ERROR_LOG1(0, OAM_SF_ANY, "{wal_cfgvendor_get_channel_list::wdev or netdev is NULL! wdev %p.}", wdev);
        return -OAL_EFAIL;
    }

    type = oal_nla_type(data);
    if (type == GSCAN_ATTRIBUTE_BAND)
    {
        band = oal_nla_get_u32(data);
    }
    else
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_cfgvendor_get_channel_list::invalid nla type! type %d.}", type);
        return -OAL_EFAIL;
    }

    /* band = 0,      返回2.4GHz 信道
     * band = others, 返回5GHz 信道
    */
    ul_ret = wal_cfgvendor_get_current_channel_list(wdev->netdev, band, al_channel_list, &num_channels);
    if (ul_ret != OAL_SUCC)
    {
        OAM_WARNING_LOG3(0, OAM_SF_ANY, "{wal_cfgvendor_get_channel_list::fail. ret_value %d, band %d, num_channels %d.}",
                        ul_ret, band, num_channels);
        return -OAL_EFAIL;
    }

    reply_len = OAL_SIZEOF(al_channel_list[0])*(num_channels);
    mem_needed = reply_len + VENDOR_REPLY_OVERHEAD + (ATTRIBUTE_U32_LEN * 2);

    /* 申请SKB 上报信道列表 */
    skb = oal_cfg80211_vendor_cmd_alloc_reply_skb(wiphy, mem_needed);
    if (OAL_UNLIKELY(!skb))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "wal_cfgvendor_get_channel_list::skb alloc failed.len %d\r\n",  mem_needed);
        return -OAL_ENOMEM;
    }

    oal_nla_put_u32(skb, GSCAN_ATTRIBUTE_NUM_CHANNELS, num_channels);
    oal_nla_put(skb, GSCAN_ATTRIBUTE_CHANNEL_LIST, reply_len, al_channel_list);

    err = oal_cfg80211_vendor_cmd_reply(skb);
    if (OAL_UNLIKELY(err))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "wal_cfgvendor_get_channel_list::Vendor Command reply failed ret:%d.\r\n", err);
    }

    OAM_WARNING_LOG3(0, OAM_SF_ANY, "{wal_cfgvendor_get_channel_list::ret_value %d, band %d, num_channels %d.}",
                    err, band, num_channels);

    return err;
}


OAL_STATIC oal_int32 wal_cfgvendor_set_country(oal_wiphy_stru *wiphy,
    oal_wireless_dev_stru *wdev, OAL_CONST oal_void  *data, oal_int32 len)
{
    oal_int32 rem;
    oal_int32 type;
    oal_int32 l_ret = -OAL_EFAIL;
    oal_int8  auc_country_code[WLAN_COUNTRY_STR_LEN] = {0};
    OAL_CONST oal_nlattr_stru *iter;

    OAL_NLA_FOR_EACH_ATTR(iter, data, len, rem) 
    {
        OAL_MEMZERO(auc_country_code, WLAN_COUNTRY_STR_LEN);
        type = oal_nla_type(iter);
        switch (type) 
        {
            case ANDR_WIFI_ATTRIBUTE_COUNTRY:
#ifdef _PRE_WLAN_FEATURE_11D
                oal_memcopy(auc_country_code, oal_nla_data(iter),
                            OAL_MIN(oal_nla_len(iter), OAL_SIZEOF(auc_country_code)));
                OAM_WARNING_LOG4(0, OAM_SF_ANY, "{wal_cfgvendor_set_country::country code:0x%X 0x%X 0x%X, len = %d!}\r\n",
                                 auc_country_code[0], auc_country_code[1], auc_country_code[2], oal_nla_len(iter));
                /* 设置国家码到wifi 驱动 */
                l_ret = wal_regdomain_update_country_code(wdev->netdev, auc_country_code);
#else
                OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_cfgvendor_set_country::_PRE_WLAN_FEATURE_11D is not define!}\r\n");
#endif
                break;
            default:
                OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_cfgvendor_set_country::unknow type:%d!}\r\n", type);
                break;
        }
    }

    return l_ret;
}


OAL_STATIC OAL_CONST oal_wiphy_vendor_command_stru wal_vendor_cmds[] =
{
    {
        {
            .vendor_id = OUI_GOOGLE,
            .subcmd = GSCAN_SUBCMD_GET_CHANNEL_LIST
        },
        .flags = WIPHY_VENDOR_CMD_NEED_WDEV | WIPHY_VENDOR_CMD_NEED_NETDEV,
        .doit = wal_cfgvendor_get_channel_list
    },

    {
        {
            .vendor_id = OUI_GOOGLE,
            .subcmd = ANDR_WIFI_SET_COUNTRY
        },
        .flags = WIPHY_VENDOR_CMD_NEED_WDEV | WIPHY_VENDOR_CMD_NEED_NETDEV,
        .doit = wal_cfgvendor_set_country
    }
};

OAL_STATIC OAL_CONST oal_nl80211_vendor_cmd_info_stru wal_vendor_events[] =
{
    { OUI_HISI, HISI_VENDOR_EVENT_UNSPEC },
    { OUI_HISI, HISI_VENDOR_EVENT_PRIV_STR }
};



oal_void wal_cfgvendor_init(oal_wiphy_stru *wiphy)
{
    wiphy->vendor_commands  = wal_vendor_cmds;
    wiphy->n_vendor_commands = OAL_ARRAY_SIZE(wal_vendor_cmds);
    wiphy->vendor_events    = wal_vendor_events;
    wiphy->n_vendor_events  = OAL_ARRAY_SIZE(wal_vendor_events);
}

oal_void wal_cfgvendor_deinit(oal_wiphy_stru *wiphy)
{
    wiphy->vendor_commands  = NULL;
    wiphy->vendor_events    = NULL;
    wiphy->n_vendor_commands = 0;
    wiphy->n_vendor_events  = 0;
}

#endif  /* (defined(_PRE_PRODUCT_ID_HI110X_HOST) || (LINUX_VERSION_CODE >= KERNEL_VERSION(3,14,0))) && (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) */

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

