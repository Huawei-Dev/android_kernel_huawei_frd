

#ifndef __OMCOMMON_H__
#define __OMCOMMON_H__

/*****************************************************************************
  1 Include Headfile
*****************************************************************************/

#include "vos.h"
#include "LPSCommon.h"
#include "AppL2Interface.h"
#include "AppRrcInterface.h"
#include  "LPsFsmState.h"
#include  "LPsTimerState.h"

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

/*key event id的取值范围在CP V100是0x10500~0x11099，这个和测试UE的key event id的定义
  typedef VOS_UINT8 OM_PS_KEY_EVENT_ENUM_UINT8  取值长度矛盾。
 解决方法：增加key evnet id 的偏移量*/
/* OM融合，工具EVENT ID只使用16位 */
#define OM_PS_KEY_EVENT_BASE_ID         0x4FF




/*PS 可维可测*/
/* 小区可带的最多PLMN个数 */
#define LPS_RRC_CSEL_MAX_FOUND_PLMN_COUNT           (6)

/* UE支持的同频或异频小区最大数目 */
#define LPS_LRRC_LPHY_MAX_FREQ_CELL_NUM                        33

/* 主模非LTE模时，UE支持的异频的最大数目是4，
   主模为LTE模时，UE支持的异频的最大数目是3 */
#define LPS_LRRC_LPHY_MAX_ADDITIONAL_CARRIER_NUM              4

/*最大承载数*/
#define LPS_NAS_ESM_MAX_EPSB_NUM            (11)
/* TA 列表最大个数 */

#define LPS_NAS_MM_MAX_TA_NUM     64

#define LPS_APP_MAX_IPV4_ADDR_LEN                               4
#define LPS_APP_MAX_IPV6_ADDR_LEN                               16

#define LPS_NAS_MM_MAX_UEID_BUF_SIZE        10                  /* UNIT: BYTE  */

#define MSP_L3_PID_DT  MSP_PID_DIAG_APP_AGENT
#define L3_MSP_PID_DT  MSP_PID_DIAG_APP_AGENT
#define L3_MSP_PID_DT_IND  MSP_PID_DIAG_AGENT



/*****************************************************************************
  3 Massage Declare
*****************************************************************************/

/* Msg ID包括4部分:
-----------------------------------------------------------------
  字节高位                                      字节低位
-----------------------------------------------------------------
| BYTE1  |  BYTE2    |   BYTE3    |    BYTE4   |
-----------------------------------------------------------------
BYTE1 消息类型:
                取值参考UE_MSG_TYPE枚举的定义

BYTE2 源模块号:
                取值范围是 UE_FUNC_MODULE_ID_ENUM 和 UE_APP_MODULE_ID_ENUM的定义


BYTE3 目的模块号:
                取值范围同BYTE2

BYTE4  消息号:0~255
*/

/*****************************************************************************
 结构名    : OM_PS_COMM_CMD_ENUM
 结构说明  : OM和协议栈间的公共功能的接口消息定义
*****************************************************************************/
enum OM_PS_COMM_CMD_ENUM
{
    /*------------------- 协议栈和OM间公共功能的接口的命令 -------------------*/
    /*空口消息上报*/
    PS_OMT_AIR_MSG_REPORT_IND       =
            DEF_MSG(ENUM_AIR_MSG, UE_MODULE_PS_ID, UE_APP_SUBSYS_ID, 0x01),

    /*关键事件上报MM(SM/RRC/L2)->OM->OMT */
    PS_OMT_KEY_EVT_REPORT_IND       =
            DEF_MSG(ENUM_KEY_EVENT_MSG, UE_MODULE_PS_ID, UE_APP_SUBSYS_ID, 0x01),

    /*透明命令,OM发送给PS各个模块的统一的透明消息的定义*/
    OM_PS_TRANSPARENT_CMD_REQ       =
            DEF_MSG(ENUM_TRANSPARENT_MSG, UE_APP_SUBSYS_ID, UE_MODULE_PS_ID, 0x01),

    OM_PS_TRANSPARENT_CMD_CNF       =
            DEF_MSG(ENUM_TRANSPARENT_MSG, UE_MODULE_PS_ID, UE_APP_SUBSYS_ID, 0x01)

};
typedef VOS_UINT32 OM_PS_COMM_CMD_ENUM_UINT32;

/*****************************************************************************
 结构名    : OM_CMD_ENUM
 结构说明  : OMT和OM的各个模块间的消息定义
*****************************************************************************/
enum OM_CMD_ENUM
{
    /*---------------------------- OM和OMT的ulMsgId --------------------------*/
    /*OMAGENT 与OM之间的握手消息原语*/
    OMAGENT_OM_HANDSHAKE_REQ        =
                DEF_MSG(ENUM_GENERAL_CMD_MSG, UE_APP_SUBSYS_ID, UE_MODULE_OM_ID, 00),

    OM_OMAGENT_HANDSHAKE_CNF        =
                DEF_MSG(ENUM_GENERAL_CMD_MSG, UE_MODULE_OM_ID, UE_APP_SUBSYS_ID, 00),
    /*OMAGENT 与OM之间的心跳消息原语*/
    OMAGENT_OM_HEART_REQ =
                DEF_MSG(ENUM_GENERAL_CMD_MSG, UE_MODULE_OM_AGENT_ID, UE_MODULE_OM_ID, 0x01),
    OM_OMAGENT_HEART_CNF =
                DEF_MSG(ENUM_GENERAL_CMD_MSG, UE_MODULE_OM_ID, UE_MODULE_OM_AGENT_ID, 0x01),

    /*OMT配置层间消息开启关闭  OMT<->OM 普通命令*/
    OMT_OM_HOOKTRACE_CONFIG_REQ     =
                DEF_MSG(ENUM_GENERAL_CMD_MSG, UE_APP_SUBSYS_ID, UE_MODULE_OM_ID, 01),

    OM_OMT_HOOKTRACE_CONFIG_CNF     =
                DEF_MSG(ENUM_GENERAL_CMD_MSG, UE_MODULE_OM_ID, UE_APP_SUBSYS_ID, 01),

    /*OMT配置所有模块log级别  OMT<->OM 普通命令*/
    OMT_OM_LOG_CONFIG_REQ           =
                DEF_MSG(ENUM_GENERAL_CMD_MSG, UE_APP_SUBSYS_ID, UE_MODULE_OM_ID, 02),

    OM_OMT_LOG_CONFIG_CNF           =
                DEF_MSG(ENUM_GENERAL_CMD_MSG, UE_MODULE_OM_ID, UE_APP_SUBSYS_ID, 02),

    /*OMT对个别模块log级别的配置  OMT<->OM 普通命令*/
    OMT_OM_SET_PS_LOGPRINT_LEV_REQ  =
                DEF_MSG(ENUM_GENERAL_CMD_MSG, UE_APP_SUBSYS_ID, UE_MODULE_OM_ID, 03),

    OM_OMT_SET_PS_LOGPRINT_LEV_CNF  =
                DEF_MSG(ENUM_GENERAL_CMD_MSG, UE_MODULE_OM_ID, UE_APP_SUBSYS_ID, 03),

    /*OMT配置PS具体模块的空口消息跟踪标志 OMT<-> 普通命令*/
    OMT_OM_SET_AIR_MSG_TRACE_REQ    =
                DEF_MSG(ENUM_GENERAL_CMD_MSG, UE_APP_SUBSYS_ID, UE_MODULE_OM_ID, 04),

    OM_OMT_SET_AIR_MSG_TRACE_CNF    =
                DEF_MSG(ENUM_GENERAL_CMD_MSG, UE_MODULE_OM_ID, UE_APP_SUBSYS_ID, 04),


    /*L2 LOG配置接口 OMT->MASTER OM->HPA->L2 OM Agent*/
    OMT_L2_LOG_CONFIG_REQ           =
                DEF_MSG(ENUM_GENERAL_CMD_MSG, UE_MODULE_OM_ID, UE_MODULE_OM_SLAVE_ID, 00),

    /*L2 LOG消息配置结果 L2 OM Agent->HPA->MASTER OM->OMT*/
    L2_OMT_LOG_CONFIG_CNF           =
                DEF_MSG(ENUM_GENERAL_CMD_MSG, UE_MODULE_OM_SLAVE_ID, UE_MODULE_OM_ID, 00)

};
typedef VOS_UINT32 OM_CMD_ENUM_UINT32;


/*****************************************************************************
 结构名    : PS_OM_KEY_EVENT_ENUM
 结构说明  : 关键事件的取值的定义
*****************************************************************************/
enum OM_PS_KEY_EVENT_ENUM
{
    /*--------------------RRC报告的key event--------------------*/
    RRC_OM_KEY_EVENT_CELL_SEARCH_SPEC_PLMN       = 1,
    RRC_OM_KEY_EVENT_CELL_SEARCH_PLMN_LIST,
    RRC_OM_KEY_EVENT_CELL_SEARCH_SUIT,
    RRC_OM_KEY_EVENT_CELL_SEARCH_ANYCELL,
    RRC_OM_KEY_EVENT_SIB_RCV,
    RRC_OM_KEY_EVENT_CELL_CAMPED_ON,
    RRC_OM_KEY_EVENT_CELL_RESEL,
    RRC_OM_KEY_EVENT_RRC_CONN_SETUP,
    RRC_OM_KEY_EVENT_RRC_CONN_RELEASE,
    RRC_OM_KEY_EVENT_RB_SETUP,
    RRC_OM_KEY_EVENT_HANDOVER_START,
    RRC_OM_KEY_EVENT_RRC_CONN_REEST,
    RRC_OM_KEY_EVENT_SMC,
    RRC_OM_KEY_EVENT_RRC_CONN_REQ,
    RRC_OM_KEY_EVENT_RRC_CONN_SETUP_CMPL,
    RRC_OM_KEY_EVENT_HANDOVER_SUCC,
    RRC_OM_KEY_EVENT_RRC_UE_CAP_ENQUIRE,
    RRC_OM_KEY_EVENT_W2L_RESEL_STRAT,
    RRC_OM_KEY_EVENT_W2L_RESEL_SUCC,
    RRC_OM_KEY_EVENT_W2L_RESEL_FAIL,
    RRC_OM_KEY_EVENT_W2L_REDIRECTED_STRAT,
    RRC_OM_KEY_EVENT_W2L_REDIRECTED_SUCC,
    RRC_OM_KEY_EVENT_W2L_REDIRECTED_FAIL,
    RRC_OM_KEY_EVENT_W2L_HANDOVER_STRAT,
    RRC_OM_KEY_EVENT_W2L_HANDOVER_SUCC,
    RRC_OM_KEY_EVENT_W2L_HANDOVER_FAIL,
    RRC_OM_KEY_EVENT_W2L_GETUECAPINFO_STRAT,
    RRC_OM_KEY_EVENT_W2L_GETUECAPINFO_SUCC,
    RRC_OM_KEY_EVENT_W2L_GETUECAPINFO_FAIL,
    RRC_OM_KEY_EVENT_W2L_PLMN_SEARCH_STRAT,
    RRC_OM_KEY_EVENT_W2L_PLMN_SEARCH_SUCC,
    RRC_OM_KEY_EVENT_W2L_PLMN_SEARCH_FAIL,
    RRC_OM_KEY_EVENT_L2W_RESEL_START,
    RRC_OM_KEY_EVENT_L2W_RESEL_SUCC,
    RRC_OM_KEY_EVENT_L2W_RESEL_FAIL,
    RRC_OM_KEY_EVENT_L2W_REDIRECTED_START,
    RRC_OM_KEY_EVENT_L2W_REDIRECTED_SUCC,
    RRC_OM_KEY_EVENT_L2W_REDIRECTED_FAIL,
    RRC_OM_KEY_EVENT_L2W_HANDOVER_START,
    RRC_OM_KEY_EVENT_L2W_HANDOVER_SUCC,
    RRC_OM_KEY_EVENT_L2W_HANDOVER_FAIL,
    RRC_OM_KEY_EVENT_L2W_GETUECAPINFO_START,
    RRC_OM_KEY_EVENT_L2W_GETUECAPINFO_SUCC,
    RRC_OM_KEY_EVENT_L2W_GETUECAPINFO_FAIL,
    RRC_OM_KEY_EVENT_G2L_RESEL_STRAT,
    RRC_OM_KEY_EVENT_G2L_RESEL_SUCC,
    RRC_OM_KEY_EVENT_G2L_RESEL_FAIL,
    RRC_OM_KEY_EVENT_G2L_REDIRECTED_STRAT,
    RRC_OM_KEY_EVENT_G2L_REDIRECTED_SUCC,
    RRC_OM_KEY_EVENT_G2L_REDIRECTED_FAIL,
    RRC_OM_KEY_EVENT_G2L_HANDOVER_STRAT,
    RRC_OM_KEY_EVENT_G2L_HANDOVER_SUCC,
    RRC_OM_KEY_EVENT_G2L_HANDOVER_FAIL,
    RRC_OM_KEY_EVENT_G2L_CCO_STRAT,
    RRC_OM_KEY_EVENT_G2L_CCO_SUCC,
    RRC_OM_KEY_EVENT_G2L_CCO_FAIL,
    RRC_OM_KEY_EVENT_L2G_RESEL_START,
    RRC_OM_KEY_EVENT_L2G_RESEL_SUCC,
    RRC_OM_KEY_EVENT_L2G_RESEL_FAIL,
    RRC_OM_KEY_EVENT_L2G_REDIRECTED_START,
    RRC_OM_KEY_EVENT_L2G_REDIRECTED_SUCC,
    RRC_OM_KEY_EVENT_L2G_REDIRECTED_FAIL,
    RRC_OM_KEY_EVENT_L2G_HANDOVER_START,
    RRC_OM_KEY_EVENT_L2G_HANDOVER_SUCC,
    RRC_OM_KEY_EVENT_L2G_HANDOVER_FAIL,
    RRC_OM_KEY_EVENT_L2G_CCO_START,
    RRC_OM_KEY_EVENT_L2G_CCO_SUCC,
    RRC_OM_KEY_EVENT_L2G_CCO_FAIL,
    RRC_OM_KEY_EVENT_L2G_GETUECAPINFO_START,
    RRC_OM_KEY_EVENT_L2G_GETUECAPINFO_SUCC,
    RRC_OM_KEY_EVENT_L2G_GETUECAPINFO_FAIL,
    RRC_OM_KEY_EVENT_L2C_RESEL_FAIL,
    RRC_OM_KEY_EVENT_L2C_REDIR_FAIL,
    RRC_OM_KEY_EVENT_L2C_RESEL_START,
    RRC_OM_KEY_EVENT_L2C_RESEL_SUCC,
    RRC_OM_KEY_EVENT_C2L_RESEL_STRAT,
    RRC_OM_KEY_EVENT_C2L_RESEL_SUCC,
    RRC_OM_KEY_EVENT_C2L_RESEL_FAIL,

    /*--------------------NAS报告的key event--------------------*/
    MM_KEY_EVENT_PLMN_SPEC_SEARCH_START ,
    MM_KEY_EVENT_PLMN_LIST_SEARCH_START,
    MM_KEY_EVENT_PLMN_SEARCH_SUCC,
    MM_KEY_EVENT_PLMN_SEARCH_FAIL,
    MM_KEY_EVENT_START_AUTO_RESEL,
    MM_KEY_EVENT_START_MANUAL_RESEL,
    MM_KEY_EVENT_PLMN_RESEL_CNF,
    MM_KEY_EVENT_COVERAGE_LOST,

    MM_KEY_EVENT_ATTACH_REQ,
    MM_KEY_EVENT_ATTACH_ACP,
    MM_KEY_EVENT_ATTACH_CMP,
    MM_KEY_EVENT_ATTACH_REJ,

    MM_KEY_EVENT_DETACH_REQ_MT,
    MM_KEY_EVENT_DETACH_ACP_MT,
    MM_KEY_EVENT_DETACH_REQ_MO,
    MM_KEY_EVENT_DETACH_ACP_MO,

    MM_KEY_EVENT_TAU_REQ,
    MM_KEY_EVENT_TAU_ACP,
    MM_KEY_EVENT_TAU_CMP,
    MM_KEY_EVENT_TAU_REJ,

    MM_KEY_EVENT_SER_REQ,
    MM_KEY_EVENT_SER_REJ,
    MM_KEY_EVENT_SER_ACP,

    MM_KEY_EVENT_GUTI_CMD,
    MM_KEY_EVENT_GUTI_CMP,

    MM_KEY_EVENT_IDEN_REQ,
    MM_KEY_EVENT_IDEN_RSP,

    MM_KEY_EVENT_AUTH_REQ,
    MM_KEY_EVENT_AUTH_FAIL,
    MM_KEY_EVENT_AUTH_RSP,
    MM_KEY_EVENT_AUTH_REJ,

    MM_KEY_EVENT_SECU_CMD,
    MM_KEY_EVENT_SECU_CMP,
    MM_KEY_EVENT_SECU_REJ,

    MM_KEY_EVENT_EMM_STATUS,
    MM_KEY_EVENT_EMM_INFO,

    MM_KEY_EVENT_EMM_UPLINK_NAS_TRANSPORT,
    MM_KEY_EVENT_EMM_DOWNLINK_NAS_TRANSPORT,

    MM_KEY_EVENT_EXTENDED_SER_REQ,              /* 2012-02-24 */
    MM_KEY_EVENT_CS_SER_NOTIFICATION,           /* 2012-02-24 */

    ESM_KEY_EVENT_BEARER_5_SETUP,
    ESM_KEY_EVENT_BEARER_5_MODIFY,
    ESM_KEY_EVENT_BEARER_5_RELEASE,

    ESM_KEY_EVENT_BEARER_6_SETUP,
    ESM_KEY_EVENT_BEARER_6_MODIFY,
    ESM_KEY_EVENT_BEARER_6_RELEASE,

    ESM_KEY_EVENT_BEARER_7_SETUP,
    ESM_KEY_EVENT_BEARER_7_MODIFY,
    ESM_KEY_EVENT_BEARER_7_RELEASE,

    ESM_KEY_EVENT_BEARER_8_SETUP,
    ESM_KEY_EVENT_BEARER_8_MODIFY,
    ESM_KEY_EVENT_BEARER_8_RELEASE,

    ESM_KEY_EVENT_BEARER_9_SETUP,
    ESM_KEY_EVENT_BEARER_9_MODIFY,
    ESM_KEY_EVENT_BEARER_9_RELEASE,

    ESM_KEY_EVENT_BEARER_10_SETUP,
    ESM_KEY_EVENT_BEARER_10_MODIFY,
    ESM_KEY_EVENT_BEARER_10_RELEASE,

    ESM_KEY_EVENT_BEARER_11_SETUP,
    ESM_KEY_EVENT_BEARER_11_MODIFY,
    ESM_KEY_EVENT_BEARER_11_RELEASE,

    ESM_KEY_EVENT_BEARER_12_SETUP,
    ESM_KEY_EVENT_BEARER_12_MODIFY,
    ESM_KEY_EVENT_BEARER_12_RELEASE,

    ESM_KEY_EVENT_BEARER_13_SETUP,
    ESM_KEY_EVENT_BEARER_13_MODIFY,
    ESM_KEY_EVENT_BEARER_13_RELEASE,

    ESM_KEY_EVENT_BEARER_14_SETUP,
    ESM_KEY_EVENT_BEARER_14_MODIFY,
    ESM_KEY_EVENT_BEARER_14_RELEASE,

    ESM_KEY_EVENT_BEARER_15_SETUP,
    ESM_KEY_EVENT_BEARER_15_MODIFY,
    ESM_KEY_EVENT_BEARER_15_RELEASE,

    /*--------------------L2报告的key event--------------------*/
    /*L2_KEY_EVENT_XYZ       = 41,*/
    L2_KEY_EVENT_PDCP_SETUP_SUCC,           /* PDCP实体建立成功 */
    L2_KEY_EVENT_PDCP_SETUP_FAIL,           /* PDCP实体建立失败 */
    L2_KEY_EVENT_PDCP_REEST_START,          /* PDCP重建立开始 */
    L2_KEY_EVENT_PDCP_REEST_SUCC,           /* PDCP重建立成功 */
    L2_KEY_EVENT_PDCP_REEST_FAIL,           /* PDCP重建立失败 */
    L2_KEY_EVENT_PDCP_ACTIVE_CIPHER,        /* PDCP激活安全特性 */
    L2_KEY_EVENT_PDCP_ACTIVE_CIPHER_FAIL,   /* PDCP激活安全特性失败 */
    L2_KEY_EVENT_PDCP_CHANGE_KEY,           /* PDCP安全特性更换密钥 */
    L2_KEY_EVENT_PDCP_CHANGE_ALG,           /* PDCP安全特性更换算法 */
    L2_KEY_EVENT_PDCP_RELEASE,              /* PDCP实体释放 */
    L2_KEY_EVENT_PDCP_ULBUF_FULL,           /* PDCP上行缓存满 */
    L2_KEY_EVENT_PDCP_DLBUF_FULL,           /* PDCP下行缓存满 */
    L2_KEY_EVENT_PDCP_INTE_VERIFY_FAIL,     /* PDCP完整性检查失败 */

    L2_KEY_EVENT_RLC_STABLISH_SUCC,  /*92*/
    L2_KEY_EVENT_RLC_STABLISH_FAIL,
    L2_KEY_EVENT_RLC_RECFG_SUCC,
    L2_KEY_EVENT_RLC_RECFG_FAIL,
    L2_KEY_EVENT_RLC_RESTABLISH_SUCC,
    L2_KEY_EVENT_RLC_RESTABLISH_FAIL,
    L2_KEY_EVENT_RLC_RELEASE_SUCC,
    L2_KEY_EVENT_RLC_RELEASE_FAIL,
    L2_KEY_EVENT_RLC_RETX_TIMES_FULL,   /*RLC AM PDU达到最大重传次数*/
    L2_KEY_EVENT_RLC_CTRL_PDU_ERROR,    /*RLC AM 接收到的状态PDU错误*/
    L2_KEY_EVENT_RLC_PDU_LI_ERROR,      /*RLC下行接到的PDU LI字段填写异常*/


    L2_KEY_EVENT_MAC_SETUP_SUCC,
    L2_KEY_EVENT_MAC_SETUP_FAIL,
    L2_KEY_EVENT_MAC_RESET_SUCC,
    L2_KEY_EVENT_MAC_RESET_FAIL,
    L2_KEY_EVENT_MAC_RELEASE_SUCC,
    L2_KEY_EVENT_MAC_RELEASE_FAIL,
    L2_KEY_EVENT_MAC_RA_SUCC,
    L2_KEY_EVENT_MAC_RA_FAIL,
    L2_KEY_EVENT_MAC_RACH_SUCC,
    L2_KEY_EVENT_MAC_RACH_FAIL,
    L2_KEY_EVENT_MAC_RANDOM_ACCESS_SUCC,
    L2_KEY_EVENT_MAC_RANDOM_ACCESS_FAIL,
    L2_KEY_EVENT_HO_MAC_RX_L3_RA_IND,
    L2_KEY_EVENT_HO_MAC_TX_PREAMBLE,
    L2_KEY_EVENT_HO_MAC_RANDACCESS_SUCC,
    L2_KEY_EVENT_HO_MAC_RX_LAST_PDU,
    L2_KEY_EVENT_HO_MAC_RX_FIRST_PDU,
    L2_KEY_EVENT_HO_MAC_TX_LAST_PDU,
    L2_KEY_EVENT_HO_MAC_TX_FIRST_PDU,
    L2_KEY_EVENT_HO_MAC_RX_LAST_ULGRANT,
    L2_KEY_EVENT_HO_MAC_RX_FIRST_ULGRANT,
    L2_KEY_EVENT_HO_RLC_RX_LAST_PDU,
    L2_KEY_EVENT_HO_RLC_RX_FIRST_PDU,
    L2_KEY_EVENT_HO_RLC_TX_LAST_PDU,
    L2_KEY_EVENT_HO_RLC_TX_FIRST_PDU,
    L2_KEY_EVENT_HO_PDCP_DL_STOP_TIME,
    L2_KEY_EVENT_HO_PDCP_DL_CONTINUE_TIME,
    L2_KEY_EVENT_HO_PDCP_UL_STOP_TIME,
    L2_KEY_EVENT_HO_PDCP_UL_CONTINUE_TIME,

    /* 由于影响路测，下面两条关键事件放到最后面*/
    MM_KEY_EVENT_EMM_DOWNLINK_GENERIC_NAS_TRANSPORT,
    MM_KEY_EVENT_EMM_UPLINK_GENERIC_NAS_TRANSPORT,

    L2_MCE_DEACTIVE_SCELL1,
    L2_MCE_DEACTIVE_SCELL2,
    L2_MCE_DEACTIVE_SCELL3,
    L2_MCE_DEACTIVE_SCELL4,
    L2_MCE_DEACTIVE_SCELL5,
    L2_MCE_DEACTIVE_SCELL6,
    L2_MCE_DEACTIVE_SCELL7,

    L2_MCE_ACTIVE_SCELL1,
    L2_MCE_ACTIVE_SCELL2,
    L2_MCE_ACTIVE_SCELL3,
    L2_MCE_ACTIVE_SCELL4,
    L2_MCE_ACTIVE_SCELL5,
    L2_MCE_ACTIVE_SCELL6,
    L2_MCE_ACTIVE_SCELL7,

    L2_DEACTIVATIONTIMER_TIMEOUT_SCELL1,
    L2_DEACTIVATIONTIMER_TIMEOUT_SCELL2,
    L2_DEACTIVATIONTIMER_TIMEOUT_SCELL3,
    L2_DEACTIVATIONTIMER_TIMEOUT_SCELL4,
    L2_DEACTIVATIONTIMER_TIMEOUT_SCELL5,
    L2_DEACTIVATIONTIMER_TIMEOUT_SCELL6,
    L2_DEACTIVATIONTIMER_TIMEOUT_SCELL7,

    RRC_DEACTIVE_SCELL1,
    RRC_DEACTIVE_SCELL2,
    RRC_DEACTIVE_SCELL3,
    RRC_DEACTIVE_SCELL4,
    RRC_DEACTIVE_SCELL5,
    RRC_DEACTIVE_SCELL6,
    RRC_DEACTIVE_SCELL7
};
typedef VOS_UINT8 OM_PS_KEY_EVENT_ENUM_UINT8;

/*****************************************************************************
 结构名    : OM_PS_AIR_MSG_ENUM
 结构说明  : 空口消息的取值的定义
*****************************************************************************/
enum OM_PS_AIR_MSG_ENUM
{
    /*RRC的空口消息定义*/
    RRC_OM_CDMA2000_CSFB_PARAM_REQ     = 0,
    RRC_OM_CDMA2000_CSFB_PARAM_RSP     = 1,
    RRC_OM_CNT_CHK                     = 2,
    RRC_OM_CNT_CHK_RSP                 = 3,
    RRC_OM_DL_INFO_TRANSF              = 4,
    RRC_OM_HO_FROM_EUTRA_PREP_REQ      = 5,
    RRC_OM_MASTER_INFO_BLOCK           = 6,
    RRC_OM_MEAS_RPRT                   = 7,
    RRC_OM_MOBIL_FROM_EUTRA_CMD        = 8,
    RRC_OM_PAGING                      = 9,
    RRC_OM_CONN_RECFG                  = 10,
    RRC_OM_CONN_RECFG_CMP              = 11,
    RRC_OM_CONN_REESTAB                = 12,
    RRC_OM_CONN_REESTAB_CMP            = 13,
    RRC_OM_CONN_REESTAB_REJ            = 14,
    RRC_OM_CONN_REESTAB_REQ            = 15,
    RRC_OM_CONN_REJ                    = 16,
    RRC_OM_CONN_REL                    = 17,
    RRC_OM_CONN_REQ                    = 18,
    RRC_OM_CONN_SETUP                  = 19,
    RRC_OM_CONN_SETUP_CMP              = 20,
    RRC_OM_SECUR_MODE_CMD              = 21,
    RRC_OM_SECUR_MODE_CMP              = 22,
    RRC_OM_SECUR_MODE_FAIL             = 23,
    RRC_OM_SYS_INFO                    = 24,
    RRC_OM_SIB_TYPE1                   = 25,
    RRC_OM_UE_CAP_ENQUIRY              = 26,
    RRC_OM_UE_CAP_INFO                 = 27,
    RRC_OM_UL_HO_PREP_TRANSF           = 28,
    RRC_OM_UL_INFO_TRANSF              = 29,

    RRC_OM_INTER_RAT_MSG               = 30,
    RRC_OM_HO_CMD                      = 31,
    RRC_OM_HO_PREP_INFO                = 32,
    RRC_OM_UE_RADIO_ACCESS_CAP_INFO    = 33,
    RRC_OM_CONN_RECFG_MOBILE_CONTROL   = 34,

    RRC_OM_HO_CMD_RECFG                = 35,

    RRC_OM_UE_INFO_REQ               = 36,
    RRC_OM_UE_INFO_RSP               = 37,
    RRC_OM_PROXIMITY_IND             = 38,

    /* MBMS Begin */
    RRC_OM_MCCH_MSG                  = 39,
    /* MBMS End */


    /* v7r2 begin */
    RRC_OM_SIB1_V8h0_IES_MSG         = 40,
    RRC_OM_SIB2_V8h0_IES_MSG         = 41,
    RRC_OM_SIB5_V8h0_IES_MSG         = 42,
    RRC_OM_SIB6_V8h0_IES_MSG         = 43,

    RRC_0M_LOGGED_MEASUREMENT_CFG    = 44,

    /* v7r2 end */
    RRC_OM_EUTRA_CAP                 = 45,

    RRC_OM_MBMS_COUNTING_RSP         = 46,
    RRC_OM_CONN_REL_V9e0_IES_MSG     = 47,
    RRC_OM_UE_ASSIS_INFO             = 48,
    RRC_OM_UE_IDC_IND                = 49,
    RRC_OM_MBMS_INTEREST_IND         = 50,
    RRC_OM_INTER_FREQ_RSTD_MEAS_IND  = 51,
    RRC_OM_CONN_RCG_SIB1_IES_MSG     = 52,
    RRC_OM_UE_CAP_V9A0_IES_MSG       = 53,
    /* begin: 2016/1/4 cr develop */
    RRC_OM_CONN_RECFG_V8M0_IES_MSG   = 54,
    /* End: 2016/1/4 cr develop */
    RRC_AIR_MSG_END                  = 55,

    /*LPP AIR MSG ID*/
    LNAS_LPP_REQ_CAP                 = 80,
    LNAS_LPP_PROV_CAP                = 81,
    LNAS_LPP_REQ_ASSIST_DATA         = 82,
    LNAS_LPP_PROV_ASSIST_DATA        = 83,
    LNAS_LPP_REQ_LOC_INFO            = 84,
    LNAS_LPP_PROV_LOC_INFO           = 85,
    LNAS_LPP_ABORT_UP                = 86,
    LNAS_LPP_ABORT_DN                = 87,
    LNAS_LPP_ERROR_UP                = 88,
    LNAS_LPP_ERROR_DN                = 89,
    LNAS_LPP_ACK_UP                  = 90,
    LNAS_LPP_ACK_DN                  = 91,
    LNAS_LPP_DN_MSG_PARA_ERR         = 92,

    /* NAS组空口消息定义*/
    /* LCS空口消息 */
    LCS_REGISTER                    = 70,
    LCS_FACILITY                    = 71,
    LCS_RELEASE_COMPLTE             = 72,

    /* ESM 空口消息 */
    ESM_ACT_DEFLT_EPS_BEARER_CNTXT_REQ = 100,
    ESM_ACT_DEFLT_EPS_BEARER_CNTXT_ACP,
    ESM_ACT_DEFLT_EPS_BEARER_CNTXT_REJ,

    ESM_ACT_DEDICT_EPS_BEARER_CNTXT_REQ,
    ESM_ACT_DEDICT_EPS_BEARER_CNTXT_ACP,
    ESM_ACT_DEDICT_EPS_BEARER_CNTXT_REJ,

    ESM_MOD_EPS_BEARER_CNTXT_REQ,
    ESM_MOD_EPS_BEARER_CNTXT_ACP,
    ESM_MOD_EPS_BEARER_CNTXT_REJ,

    ESM_DEACT_EPS_BEARER_CNTXT_REQ,
    ESM_DEACT_EPS_BEARER_CNTXT_ACP,

    ESM_PDN_CONNECT_REQ,
    ESM_PDN_CONNECT_REJ,

    ESM_PDN_DISCONNECT_REQ,
    ESM_PDN_DISCONNECT_REJ,

    ESM_BEARER_RESOUCE_ALLOC_REQ,
    ESM_BEARER_RESOUCE_ALLOC_REJ,

    ESM_BEARER_RESOUCE_MOD_REQ,
    ESM_BEARER_RESOUCE_MOD_REJ,

    ESM_INFO_REQ,
    ESM_INFO_RSP,

    ESM_STUS_MT,
    ESM_STUS_MO,

    ESM_NOTIFICATION_MT,

    /* EMM 空口消息 */
    MM_ATTACH_REQ,
    MM_ATTACH_ACP,
    MM_ATTACH_CMP,
    MM_ATTACH_REJ,

    MM_DETACH_REQ_MT,
    MM_DETACH_ACP_MT,
    MM_DETACH_REQ_MO,
    MM_DETACH_ACP_MO,

    MM_TAU_REQ,
    MM_TAU_ACP,
    MM_TAU_CMP,
    MM_TAU_REJ,

    MM_SER_REQ,
    MM_SER_REJ,
    MM_SER_ACP,

    MM_GUTI_CMD,
    MM_GUTI_CMP,

    MM_IDEN_REQ,
    MM_IDEN_RSP,

    MM_AUTH_REQ,
    MM_AUTH_FAIL,
    MM_AUTH_RSP,
    MM_AUTH_REJ,

    MM_SECU_CMD,
    MM_SECU_CMP,
    MM_SECU_REJ,

    MM_EMM_STATUS,

    TC_ACT_TEST_MODE,
    TC_ACT_TEST_MODE_CMP,

    TC_DEACT_TEST_MODE,
    TC_DEACT_TEST_MODE_CMP,

    TC_CLOSE_UE_TEST_LOOP,
    TC_CLOSE_UE_TEST_LOOP_CMP,

    TC_OPEN_UE_TEST_LOOP,
    TC_OPEN_UE_TEST_LOOP_CMP,

    MM_EMM_INFO,

    MM_EMM_DOWNLINK_NAS_TRANSPORT,
    MM_EMM_UPLINK_NAS_TRANSPORT,

    MM_EXTENDED_SER_REQ,                /* 2012-02-24 */
    MM_CS_SER_NOTIFICATION,             /* 2012-02-24 */

    MM_EMM_DOWNLINK_GENERIC_NAS_TRANSPORT,
    MM_EMM_UPLINK_GENERIC_NAS_TRANSPORT,

    TC_RESET_POSITION_INFO

    /*L2组的空口消息定义*/
    /*PDCP_RRC_DATA_REQ      = 200,
    PDCP_RRC_DATA_IND,

    PDCP_RABM_DATA_REQ,
    PDCP_RABM_DATA_IND,

    PDCP_RLC_DATA_REQ,
    PDCP_RLC_DATA_IND,

    MAC_PHY_UL_FEEDBACK_IND,
    MAC_PHY_DCI_IND,
    MAC_PHY_CQI_IND,
    MAC_PHY_LOOP_IND,

    LMAC_DLCOM_PBCH,
    LMAC_DLCOM_PDSCH,
    LMAC_DLCOM_PRNTI,
    LMAC_DLCOM_SIRNTI,*/


};
typedef VOS_UINT8 OM_PS_AIR_MSG_ENUM_UINT8;

/*****************************************************************************
 结构名    : DT_FLAG_ENUM
 结构说明  : DT有效标识
*****************************************************************************/
enum DT_FLAG_ENUM
{
    DT_FLAG_INVALID,                 /* 无效*/
    DT_FLAG_VALID,                   /* 有效 */
    DT_FLAG_ENUM_BUTT
};
typedef VOS_UINT8 DT_FLAG_ENUM_UINT8;

/*****************************************************************************
 结构名    : APP_PS_MSG_TYPE_ENUM
 协议表格  :
 ASN.1描述 :
 结构说明  : 协议栈和APP之间的消息ID定义
*****************************************************************************/

enum APP_PS_MSG_TYPE_ENUM
{

    /*可维可测-camped cell information上报*/
    ID_OM_PS_CAMPEDCELL_INFO_CMD_REQ           = (PS_MSG_ID_APP_TO_PS_BASE + 0x01),/*0x1336*/
    ID_PS_OM_CAMPEDCELL_INFO_CMD_CNF           = (PS_MSG_ID_PS_TO_APP_BASE + 0x01),/*0x1316*/

    /*可维可测-handover上报*/
    ID_OM_PS_HANDOVER_INFO_CMD_REQ             = (PS_MSG_ID_APP_TO_PS_BASE + 0x02),/*0x1337*/
    ID_PS_OM_HANDOVER_INFO_CMD_CNF             = (PS_MSG_ID_PS_TO_APP_BASE + 0x02),/*0x1317*/

    /*可维可测-prach\sr上报*/
    ID_OM_PS_PRACH_SR_INFO_CMD_REQ             = (PS_MSG_ID_APP_TO_PS_BASE + 0x03),/*0x1338*/
    ID_PS_OM_PRACH_SR_INFOCMD_CNF              = (PS_MSG_ID_PS_TO_APP_BASE + 0x03),/*0x1318*/

    /*可维可测-bsr上报*/
    ID_OM_PS_BSR_INFO_CMD_REQ                  = (PS_MSG_ID_APP_TO_PS_BASE + 0x04),/*0x1339*/
    ID_PS_OM_BSR_INFO_CMD_CNF                  = (PS_MSG_ID_PS_TO_APP_BASE + 0x04)/*0x1319*/



};
typedef VOS_UINT32   APP_PS_MSG_TYPE_ENUM_UINT32;

/*****************************************************************************
 枚举名    : TDS_L2_UP_EVENT_ENUM
 协议表格  :
 ASN.1描述 :
 枚举说明  : TL用户面事件枚举定义，用于记录影响TL流量的各种事件
*****************************************************************************/
enum    TL_UP_EVENT_ENUM
{
    /*CDS事件*/
    TL_UP_EVENT_CDS_UL_IPF_ERR                  = 0,
    TL_UP_EVENT_CDS_UL_SOFT_IPF_ERR,
    TL_UP_EVENT_CDS_UL_FLOW_CTRL,
    TL_UP_EVENT_CDS_DL_FLOW_CTRL,
    TL_UP_EVENT_CDS_DL_SDU_QUE_FULL,

    /*TDS事件*/
    TL_UP_EVENT_TPDCP_UL_DISCARD_SDU            = 50,
    TL_UP_EVENT_TPDCP_DL_DISCARD_SDU,

    TL_UP_EVENT_TRLC_UL_RETX_PDU                = 100,
    TL_UP_EVENT_TRLC_UL_RETX_PDU_MAX_NUM,
    TL_UP_EVENT_TRLC_UL_TX_NACK,
    TL_UP_EVENT_TRLC_UL_TX_RTS_PDU,
    TL_UP_EVENT_TRLC_UL_TX_RTS_ACK_PDU,
    TL_UP_EVENT_TRLC_UL_SDU_QUE_FULL,
    TL_UP_EVENT_TRLC_UL_RL_FAIL,
    TL_UP_EVENT_TRLC_UL_DISCARD_TIMER_EXPIRED,
    TL_UP_EVENT_TRLC_DL_RX_NACK,
    TL_UP_EVENT_TRLC_DL_RX_MRW,
    TL_UP_EVENT_TRLC_DL_RX_MRW_ACK,
    TL_UP_EVENT_TRLC_DL_RX_RTS_PDU,
    TL_UP_EVENT_TRLC_DL_RX_RTS_ACK_PDU,
    TL_UP_EVENT_TRLC_DL_RX_SN_OUT_OF_WIN,

    TL_UP_EVENT_TMAC_UL_HARQ_RETX               = 150,
    TL_UP_EVENT_TMAC_UL_HARQ_RETX_MAX,
    TL_UP_EVENT_TMAC_UL_HARQ_RESET,
    TL_UP_EVENT_TMAC_UL_PDU_FULL,
    TL_UP_EVENT_TMAC_UL_RX_NACK,
    TL_UP_EVENT_TMAC_UL_SFN_ERR,
    TL_UP_EVENT_TMAC_DL_RX_DPCH_CRC_ERR,
    TL_UP_EVENT_TMAC_DL_RX_DPA_CRC_ERR,
    TL_UP_EVENT_TMAC_DL_REORDER_TIMER_EXPIRED,
    TL_UP_EVENT_TMAC_DL_T312_EXPIRED,
    TL_UP_EVENT_TMAC_DL_T313_EXPIRED,
    TL_UP_EVENT_TMAC_DL_T2_EXPIRED,
    TL_UP_EVENT_TMAC_DL_OUT_OF_SYNC,
    TL_UP_EVENT_TMAC_UPA_ACTIVE,
    TL_UP_EVENT_TMAC_UPA_DEACTIVE,
    TL_UP_EVENT_TMAC_DPA_ACTIVE,
    TL_UP_EVENT_TMAC_DPA_DEACTIVE,
    TL_UP_EVENT_TMAC_UL_TTF_MEM_FULL,
    TL_UP_EVENT_TMAC_DL_TTF_MEM_FULL,

    /*LTE事件*/
    TL_UP_EVENT_LPDCP_UL_SDU_BUFFER_FULL            = 200,
    TL_UP_EVENT_LPDCP_UL_DISCARD_SDU_HIGH_PRI,
    TL_UP_EVENT_LPDCP_UL_DISCARD_SDU_LOW_PRI,
    TL_UP_EVENT_LPDCP_UL_FLOW_CTRL_STOP_TX,
    TL_UP_EVENT_LPDCP_DL_SN_NOT_CONSECUTIVE,
    TL_UP_EVENT_LPDCP_DL_SN_OUT_OF_WIN,
    TL_UP_EVENT_LPDCP_DL_CIFIFO_FULL,
    TL_UP_EVENT_LPDCP_DL_SDU_LEN_ERR,
    TL_UP_EVENT_LPDCP_DL_DISCARD_PDU_TOTAL,
    TL_UP_EVENT_LPDCP_STOP,
    TL_UP_EVENT_LPDCP_CONTINUE,
    TL_UP_EVENT_LPDCP_REL,

    TL_UP_EVENT_LRLC_UL_RETX_PDU                = 250,
    TL_UP_EVENT_LRLC_UL_RETX_PDU_SEGMENT,
    TL_UP_EVENT_LRLC_UL_RETX_PDU_MAX_NUM,
    TL_UP_EVENT_LRLC_UL_RX_NACK,
    TL_UP_EVENT_LRLC_UL_SDU_QUE_FULL,
    TL_UP_EVENT_LRLC_UL_DISCARD_TIMER_EXPIRED,
    TL_UP_EVENT_LRLC_DL_DISCARD_PDU_TOTAL,
    TL_UP_EVENT_LRLC_DL_DISCARD_PDU_MEM_FULL,
    TL_UP_EVENT_LRLC_DL_START_REORDER_TIMER,
    TL_UP_EVENT_LRLC_DL_SN_OUT_OF_WIN,
    TL_UP_EVENT_LRLC_DL_DISCARD_SDU_MEM_FULL,
    TL_UP_EVENT_LRLC_DL_FEEDBACK_NACK,
    TL_UP_EVENT_LRLC_REEST,
    TL_UP_EVENT_LRLC_REL,

    TL_UP_EVENT_LMAC_UL_HARQ_RETX               = 300,
    TL_UP_EVENT_LMAC_UL_HARQ_RETX_MAX_NUM,
    TL_UP_EVENT_LMAC_UL_SCHED_INVALID,
    TL_UP_EVENT_LMAC_UL_SCHED_PDU_FAIL,
    TL_UP_EVENT_LMAC_UL_SCHED_TIMEOUT,
    TL_UP_EVENT_LMAC_UL_PDU_ERROR,
    TL_UP_EVENT_LMAC_UL_TWO_SCHED_IND_IN_1MS,
    TL_UP_EVENT_LMAC_UL_SCHED_IND_NOT_CONSECUTIVE,
    TL_UP_EVENT_LMAC_UL_SCHED_ERROR_TOTAL,
    TL_UP_EVENT_LMAC_DL_RX_TB_CRC_ERR,
    TL_UP_EVENT_LMAC_DL_RX_TB_REPEAT,
    TL_UP_EVENT_LMAC_SR,
    TL_UP_EVENT_LMAC_SR_MAX_NUM,
    TL_UP_EVENT_LMAC_RESET,
    TL_UP_EVENT_LMAC_RA_MAC_TRIGGERED,
    TL_UP_EVENT_LMAC_RA_RRC_TRIGGERED,
    TL_UP_EVENT_LMAC_RA_PDCCH_TRIGGERED,
    TL_UP_EVENT_LMAC_REL,

    TL_UP_EVENT_BUTT                            =  350
};
typedef VOS_UINT16 TL_UP_EVENT_ENUM_UINT16;

/*****************************************************************************
 枚举名    : LSCELL_STATUS_ENUM
 协议表格  :
 ASN.1描述 :
 枚举说明  : LTE CA 辅小区状态
*****************************************************************************/
enum    LSCELL_STATUS_ENUM
{
    LSCELL_NOT_CONFIGED                  = 0,
    LSCELL_CONFIGED_NOT_ACTIVE,
    LSCELL_ACTIVE,
    LSCELL_STATUS_BUTT
};
typedef VOS_UINT8 LSCELL_STATUS_ENUM_UINT8;

/*****************************************************************************
 枚举名    : LMAC_SCELL_CFG_STATUS_ENUM
 协议表格  :
 ASN.1描述 :
 枚举说明  : LTE CA 辅小区配置状态
*****************************************************************************/
enum    LMAC_SCELL_CFG_STATUS_ENUM
{
    LMAC_SCELL_NOT_CONFIG                  = 0,
    LMAC_SCELL_CONFIG,
    LMAC_SCELL_CFG_STATUS_BUTT
};
typedef VOS_UINT8 LMAC_SCELL_CFG_STATUS_ENUM_UINT8;

/*****************************************************************************
 枚举名    : LSCELL_DEACT_TIMER_VIEW_ENUM
 协议表格  :
 ASN.1描述 :
 枚举说明  : LTE CA 去激活定时器OM显示
*****************************************************************************/
enum    LSCELL_DEACT_TIMER_VIEW_ENUM
{
    LSCELL_DEACT_TIMER_INFINITE                  = 0,
    LSCELL_DEACT_TIMER_NOTCONFIG
};
typedef VOS_UINT32 LSCELL_DEACT_TIMER_VIEW_ENUM_UINT32;


/*****************************************************************************
 枚举名    : LRRC_FLOW_CTRL_TYPE_ENUM
 协议表格  :
 ASN.1描述 : 
 枚举说明  : 流程控制类型枚举
             修改枚举时需要注意，失败类型取值 = 启动类型取值+1
             用于外部显示，类型和LRRC_FLOW_CTRL_TYPE_ENUM保持一致
 比如: LRRC_FLOW_CTRL_TYPE_G2L_RESEL_FAIL = LRRC_FLOW_CTRL_TYPE_G2L_START_RESEL + 1
*****************************************************************************/
enum LRRC_FLOW_CTRL_ENUM
{
    LRRC_FLOW_CTRL_L2L_NORMAL            = 0,     /* LTE内流程 */
    LRRC_FLOW_CTRL_G2L_START_RESEL,               /* G->L开始重选 */
    LRRC_FLOW_CTRL_G2L_RESEL_FAIL,                /* G->L重选失败 */
    LRRC_FLOW_CTRL_G2L_STOP_RESEL,                /* G->L停止重选 */
    LRRC_FLOW_CTRL_G2L_START_REDIR,               /* G->L开始重定向 */
    LRRC_FLOW_CTRL_G2L_REDIR_FAIL,                /* G->L重定向失败 */
    LRRC_FLOW_CTRL_G2L_STOP_REDIR,                /* G->L停止重定向 */
    LRRC_FLOW_CTRL_G2L_START_CCO,                 /* G->L开始CCO */
    LRRC_FLOW_CTRL_G2L_CCO_FAIL,                  /* G->L CCO失败 */
    LRRC_FLOW_CTRL_G2L_STOP_CCO,                  /* G->L停止CCO */
    LRRC_FLOW_CTRL_G2L_START_HO,                  /* G->L开始HO */
    LRRC_FLOW_CTRL_G2L_HO_FAIL,                   /* G->L HO失败 */
    LRRC_FLOW_CTRL_G2L_STOP_HO,                   /* G->L停止HO */
    LRRC_FLOW_CTRL_W2L_START_RESEL,               /* W->L开始重选 */
    LRRC_FLOW_CTRL_W2L_RESEL_FAIL,                /* W->L重选失败 */
    LRRC_FLOW_CTRL_W2L_STOP_RESEL,                /* W->L停止重选 */
    LRRC_FLOW_CTRL_W2L_START_REDIR,               /* W->L开始重定向 */
    LRRC_FLOW_CTRL_W2L_REDIR_FAIL,                /* W->L重定向失败 */
    LRRC_FLOW_CTRL_W2L_STOP_REDIR,                /* W->L停止重定向 */
    LRRC_FLOW_CTRL_W2L_START_HO,                  /* W->L开始HO */
    LRRC_FLOW_CTRL_W2L_HO_FAIL,                   /* W->L HO失败 */
    LRRC_FLOW_CTRL_W2L_STOP_HO,                   /* W->L停止HO */
    LRRC_FLOW_CTRL_W2L_START_PLMN,                /* W->L开始PLMN搜索 */
    LRRC_FLOW_CTRL_W2L_DONE_PLMN,                 /* W->L PLMN搜索完成 */
    LRRC_FLOW_CTRL_W2L_STOP_PLMN,                 /* W->L停止PLMN搜索 */
    LRRC_FLOW_CTRL_L2G_START_RESEL,               /* L->G开始重选 */
    LRRC_FLOW_CTRL_L2G_RESEL_FAIL,                /* L->G重选失败 */
    LRRC_FLOW_CTRL_L2G_START_REDIR,               /* L->G重定向 */
    LRRC_FLOW_CTRL_L2G_REDIR_FAIL,                /* L->G重定向失败 */
    LRRC_FLOW_CTRL_L2G_START_CCO,                 /* L->G开始CCO */
    LRRC_FLOW_CTRL_L2G_CCO_FAIL,                  /* L->G CCO失败 */
    LRRC_FLOW_CTRL_L2G_START_HO,                  /* L->G开始切换 */
    LRRC_FLOW_CTRL_L2G_HO_FAIL,                   /* L->G切换失败 */
    LRRC_FLOW_CTRL_L2W_START_RESEL,               /* L->W开始重选 */
    LRRC_FLOW_CTRL_L2W_RESEL_FAIL,                /* L->W重选失败 */
    LRRC_FLOW_CTRL_L2W_START_REDIR,               /* L->W重定向 */
    LRRC_FLOW_CTRL_L2W_REDIR_FAIL,                /* L->W重定向失败 */
    LRRC_FLOW_CTRL_L2W_START_HO,                  /* L->W开始切换 */
    LRRC_FLOW_CTRL_L2W_HO_FAIL,                   /* L->W切换失败 */
    LRRC_FLOW_CTRL_G2L_IDLE_MEAS,                 /* G->L IDLE测量配置 */
    LRRC_FLOW_CTRL_W2L_IDLE_MEAS,                 /* W->L IDLE测量配置 */
    LRRC_FLOW_CTRL_G2L_CONN_MEAS,                 /* G->L CONN测量配置 */
    LRRC_FLOW_CTRL_W2L_CONN_MEAS,                 /* W->L CONN测量配置 */
    LRRC_FLOW_CTRL_W2L_RELALL,                    /* W->L RELALL */
    LRRC_FLOW_CTRL_L2W_START_CELL_SRCH,           /* L->W 搜网 */
    LRRC_FLOW_CTRL_L2W_STOP_CELL_SRCH,            /* L->W 停止搜网 */
    LRRC_FLOW_CTRL_L2G_START_CELL_SRCH,           /* L->G 搜网 */
    LRRC_FLOW_CTRL_L2G_STOP_CELL_SRCH,            /* L->G 停止搜网 */
    LRRC_FLOW_CTRL_T2L_RELALL,                    /* T->L RELALL */
    LRRC_FLOW_CTRL_G2L_RELALL,                    /* G->L RELALL */
    LRRC_FLOW_CTRL_L2T_START_REDIR,               /* L->T重定向 */
    LRRC_FLOW_CTRL_L2T_START_HO,                  /* L->T开始切换 */
    LRRC_FLOW_CTRL_L2T_START_RESEL,               /* L->T开始重选 */
    LRRC_FLOW_CTRL_T2L_IDLE_MEAS,                 /* T->L IDLE测量配置 */
    LRRC_FLOW_CTRL_T2L_CONN_MEAS,                 /* G->L CONN测量配置 */
    LRRC_FLOW_CTRL_C2L_IDLE_MEAS,                 /* C->L IDLE测量配置 */
    LRRC_FLOW_CTRL_L2C_START_RESEL,               /* L->C开始重选 */
    LRRC_FLOW_CTRL_L2C_RESEL_FAIL,                /* L->C重选失败 */
    LRRC_FLOW_CTRL_L2C_START_REDIR,               /* L->C重定向 */
    LRRC_FLOW_CTRL_L2C_REDIR_FAIL,                /* L->C重定向失败 */
    LRRC_FLOW_CTRL_C2L_START_RESEL,               /* C->L开始重选 */
    LRRC_FLOW_CTRL_C2L_RESEL_FAIL,                /* C->L重选失败 */
    LRRC_FLOW_CTRL_C2L_STOP_RESEL,                /* C->L停止重选 */
    LRRC_FLOW_CTRL_C2L_RELALL,                    /* C->L RELALL */
    LRRC_FLOW_CTRL_BUTT
};
typedef VOS_UINT32 LRRC_FLOW_CTRL_ENUM_UINT32;



/*begin for RRC_APP_DEBUG_INFO_STRU*/
/*****************************************************************************
 枚举名    : RRC_FILE_NAME_ID_ENUM
 枚举说明  : RRC文件名枚举
*****************************************************************************/
enum RRC_FILE_NAME_ID_ENUM
{
    RrcCmmComm_c,
    RrcCmmDebug_c,
    RrcCmmFsm_c,
    RrcCmmGlobal_c,
    RrcCmmHandover_c,
    RrcCmmL3Ie_c,
    RrcCmmL3msg_c,
    RrcCmmMeasCtrl_c,
    RrcCmmMeasEvaluate_c,
    RrcCmmNcellMeasCfg_c,
    RrcCmmPhy_c,
    RrcCmmSpeedState_c,
    RrcCmmState_c,
    RrcCmmTimer_c,
    RrcCselCellSelect_c,
    RrcCselComm_c,
    RrcCselDebug_c,
    RrcCselEvaluate_c,
    RrcCselFsm_c,
    RrcCselGlobal_c,
    RrcCselInit_c,
    RrcCselLimitCell_c,
    RrcCselMeasure_c,
    RrcCselMobility_c,
    RrcCselMsgSend_c,
    RrcCselPlmnSearch_c,
    RrcCselReselect_c,
    RrcCselSysInfo_c,
    RrcCselTimer_c,
    RrcCselPcCellResel_c,
    RrcCselpcCellSearch_c,
    RrcCselPcComm_c,
    RrcCselpcEntry_c,
    RrcCselPcFsm_c,
    RrcCselpcGlobal_c,
    RrcCselpcPlmnSearch_c,
    RrcCselPcStaChange_c,
    RrcCselpcSysInfoUpdate_c,
    RrcItf_c,
    RrcItfFsm_c,
    RrcItfL2Func_c,
    RrcItfNasSndMsg_c,
    RrcItfPhyFunc_c,
    RrcCommPrint_c,
    RrcMainAsn1_c,
    RrcMainEntry_c,
    RrcMainErmmFsm_c,
    RrcMainErrcFsm_c,
    RrcMainFsmComm_c,
    RrcMainGlobal_c,
    RrcMainPrint_c,
    RrcMainTimer_c,
    RrcOmItf_c,
    RrcRbCfgRcvConfigCnf_c,
    RrcRbEntry_c,
    RrcRbFsm_c,
    RrcRbGlobal_c,
    RrcRbHoRcvConfigCnf_c,
    RrcRbRecfgRcvConfigCnf_c,
    RrcRbReconfigMsgProc_c,
    RrcRbSndMsg_c,
    RrcRbUpdateCtx_c,
    RrcReestConnReest_c,
    RrcReestFsm_c,
    RrcReestGlobal_c,
    RrcReestProcConnReestMsg_c,
    RrcReestSndConfigMsg_c,
    RrcReestTimerHandle_c,
    RrcCcb_c,
    RrcComm_c,
    RrcCommSaveIe_c,
    RrcCommSaveSibIe_c,
    RrcRrcConnAccessCtrl_c,
    RrcRrcConnEst_c,
    RrcRrcConnRel_c,
    RrcRrcConnRelComm_c,
    RrcRrcDebug_c,
    RrcRrcErrorProc_c,
    RrcRrcFsm_c,
    RrcRrcGlobal_c,
    RrcRrcNasCmd_c,
    RrcRrcPaging_c,
    RrcRrcProcRrcConnMsg_c,
    RrcRrcPtlTimerHandle_c,
    RrcRrcSndMsg_c,
    RrcRrcUeCapEnquiry_c,
    RrcRrcUeInfoEnquiry_c,
    RrcSndMsgComm_c,
    RrcSibComm_c,
    RrcSibFsm_c,
    RrcSibGlobal_c,
    RrcSibProc_c,
    RrcSmcFsm_c,
    RrcSmcGlobal_c,
    RrcSmcKeyProc_c,
    RrcSmcMsgProc_c,
    LRrcLRrcIratCommProc_c,
    LRrcLRrcIratReselProc_c,
    LRrcLRrcIratRedirProc_c,
    LRrcLRrcIratCcoProc_c,
    LRrcLRrcIratHoProc_c,
    LRrcLRrcIratPlmnSearchProc_c,
    /* MBMS Begin */
    LRrcMbmsComm_c,
    LRrcMbmsProc_c,
    LRrcMbmsGlobal_c,
    LRrcMbmsSndMsg_c,
    /* MBMS End */

    /* bgs begin */
    LRrcLCselBgFsm_c,
    LRrcLCselBgScan_c,
    LRrcLCselBgGlobal_c,
    /* bgs end */
    LRrcLRrcLppCmd_c,
    RRC_FILE_NAME_ID_BUTT
};

typedef VOS_UINT32 RRC_FILE_NAME_ID_ENUM_UINT32;

/*****************************************************************************
 结构名    : LRRC_APP_DBG_INFO_TYPE_ENUM
 结构说明  : DBG上报的枚举
*****************************************************************************/
enum LRRC_APP_DBG_INFO_TYPE_ENUM
{
    LRRC_APP_BAR_CELL_INFO,    /* _H2ASN_MsgChoice LRRC_APP_LIMITED_INFO_STRU */
    LRRC_APP_DBG_COMM_INFO,    /* _H2ASN_MsgChoice LRRC_APP_DBG_COMM_INFO_STRU */
    LRRC_APP_DBG_RESEL_INFO,    /* _H2ASN_MsgChoice LRRC_APP_DBG_RESEL_INFO_STRU */
    LRRC_APP_DBG_INFO_TYPE_BUTT
};
typedef VOS_UINT32 LRRC_APP_DBG_INFO_TYPE_ENUM_UNIT32;

/*****************************************************************************
 结构名    : LRRC_LCSEL_FORBIDDEN_CAUSE_ENUM
 协议表格  :
 ASN.1描述 :
 结构说明  : FORBIDDEN情况的两种细分原因值，L模forbidden列表中使用，bar列表中无用
 *****************************************************************************/
enum LRRC_APP_FORBIDDEN_CAUSE_ENUM
{
    LRRC_APP_FORBIDDEN_CAUSE_NOT_EPLMN  = 0,                                  /* 由于非EPLMN原因而被forbidden */
    LRRC_APP_FORBIDDEN_CAUSE_FORBIDDEN,                                       /* 其他原因被forbidden */
    LRRC_APP_FORBIDDEN_CAUSE_BUTT
};
typedef VOS_UINT16  LRRC_APP_FORBIDDEN_CAUSE_ENUM_UINT16;

/*****************************************************************************
 结构名    : LRRC_APP_DBG_INFO_TYPE_ENUM
 结构说明  : DBG上报的枚举
*****************************************************************************/
enum LRRC_APP_DBG_FUN_NAME_ENUM
{
    LRRC_APP_DBG_NO_FUN_NAME = 0,
    SUBFUN_NULL_MARK_ONE_E,
    SUBFUN_NULL_MARK_TWO_E,
    SUBFUN_NULL_MARK_THREE_E,
    SUBFUN_NULL_MARK_FOUR_E,
    SUBFUN_NULL_MARK_FIVE_E,
    SUBFUN_NULL_MARK_SIX_E,
    SUBFUN_NULL_MARK_SEVEN_E,
    SUBFUN_NULL_MARK_EIGHT_E,
    SUBFUN_NULL_MARK_NINE_E,
    SUBFUN_NULL_MARK_TEN_E = 10,
    SUBFUN_NULL_MARK_ELEVEN_E,
    SUBFUN_NULL_MARK_TWELVE_E,
    SUBFUN_NULL_MARK_THIRTEEN_E,
    SUBFUN_NULL_MARK_FOURTEEN_E,
    SUBFUN_NULL_MARK_FIFTEEN_E = 15,
    LRRC_LCSEL_GetSysCfgSuspendFlag_E,
    RRC_CSEL_GetNcellNoEplmnSrchFlg_E,
    RRC_CSEL_CheckIsEPlmn_E,
    RRC_SIB_CheckIsStopRcvOtherSysInfoOrNot_E,
    RRC_CSEL_CheckTcellMultiBandInd_E = 20,
    LRRC_CSEL_CheckIsEmPlmn_E,
    RRC_CheckIsChinaMcc_E,
    LRRC_CSEL_CheckIsCellSearching_E,
    LRRC_CSEL_CheckIsInteroperability_E,
    RRC_CSEL_CheckCellIsBarBySIB1_E,
    RRC_CSEL_CheckIsEplmnBySIB1_E,
    RRC_CSEL_CheckIsForbiddenTacBySIB1_E,
    RRC_CSEL_CheckIsSatifySCritorBySIB1_E,
    RRC_CSEL_CheckTcellFreqBandInd_E,
    RRC_CSEL_CheckTaiNotInForbTa_E = 30,
    RRC_CSEL_CheckTCellBarredForReserved_E,
    RRC_CSEL_ProcSibGetSysinfoCnf_E,
    RRC_CSEL_ProcSibGetSysinfoCnfRcvSucc_E,
    RRC_CSEL_CheckTCellLimitStatus_E,
    RRC_CSEL_CheckTCellLimit_E,
    LRRC_LCSEL_CheckTCellBarred_E,
    RRC_CSEL_CheckCellAllFreqIsSupport_E,
    RRC_CSEL_CheckTCellSuitable_E,
    RRC_CSEL_SearchMovGetSysInfoBarOrForbidenInd_E,
    RRC_CSEL_ProcSibSysInfoSucLimitCell_E = 40,
    RRC_CSEL_CheckBandWidthIsSupport_E,
    LRRC_CSEL_CheckPartBandInCustBand_E,
    RRC_COMM_CheckPartBandInSpecBand_E,
    PS_IE_ABSENT_E,
    RRC_COMM_CalculateBandIndByDlEarfcn_E,
    RRC_COMM_CheckFreqIsInUeCapBand_E,
    RRC_COMM_CheckBandInfo_E,
    RRC_COMM_CalculateDlFreq_E,
    RRC_RB_CheckUuRrcConnRecfgMsgIe_E,
    RRC_RB_CheckTtiBundlingPara_E = 50,
    RRC_RB_CheckTtiBundlingAndSps_E,
    RRC_RRC_CheckTtiBundlingAndSps_E,
    RRC_RRC_CheckTtiBundlingAndSubframeAssign_E,
    RRC_RB_CheckRadioResourceCfgOfRrcConnRecfgMsg_E,
    RRC_RB_ReturnResultOfCheckRadioResDedic_E,
    RRC_RB_CheckPhyCfgDedicatedInfoOfRrcConnRecfgMsg_E,
    RRC_RB_CheckSpsCfgInfoOfRrcConnRecfgMsg_E,
    RRC_RRC_CheckDlSpsCfgInfoOfRrcConnSetupMsg_E,
    RRC_RRC_CheckUlSpsCfgInfoOfRrcConnSetupMsg_E,
    RRC_RB_CheckMacCfgInfoOfRrcConnRecfgMsg_E = 60,
    RRC_RRC_CheckMacMainCfgR11_E,
    RRC_RRC_CheckMacMainCfgR10_E,
    RRC_RRC_CheckConnectedDrxCfgOfRrcConnSetupMsg_E,
    RRC_RRC_CheckUlSchCfgOfRrcConnSetupMsg_E,
    UU_SPARE2_RETX_BSR_TIMER_E,
    RRC_RB_CheckDrbInfoOfRrcConnRecfgMsg_E,
    RRC_RB_CheckSrbInfoOfRrcConnRecfgMsg_E,
    RRC_RB_CheckRadioRsrcCfgPara_E,
    UU_MAX_NUM_OF_DRB_E,
    UU_C1_CRITI_EXT_RRC_CONN_RECFG_CHOSEN_E = 70,
    UU_RRC_CONN_RECFG_R8_C1_RRC_CONN_RECFG_CHOSEN_E,
    RRC_RB_CheckMobilityCtrlInfoOfRrcConnRecfgMsg_E,
    RRC_SMC_INACTIVE_E,
    RRC_COMM_CheckFreqIsInSpecBand_E,
    RRC_RB_CheckEutraCarrierFreqInfoOfMobilityCtrlInfo_E,
    RRC_RB_CheckLocalTtiBundlingAndSpsAfterCheckUu_E,
    RRC_RB_CheckSecurityCfgOfRrcConnRecfgMsg_E,
    RRC_RB_CheckIntraLteHoSecurityCfg_E,
    RRC_SMC_CheckSecurAlgorPara_E,
    RRC_RB_CheckBandWidthOfMobilityCtrlInfo_E = 80,
    UU_SPARE10_DL_BANDWIDTH_E,
    UU_SPARE10_UL_BANDWIDTH_E,
    RRC_RB_CheckAllFreqInMobilityCtrlInfoValid_E,
    RRC_CMM_UuMeasCfgCheck_E,
    RRC_CMM_UuMeasObjCfgCheck_E,
    RRC_CMM_UuMeasRptCfgCheck_E,
    RRC_CMM_UuMeasQuantCfgCheck_E,
    RRC_CMM_UuUtraMeasCfgCheck_E,
    RRC_CMM_UuMeasSpeedCfgCheck_E,
    RRC_CMM_UuUtraMeasCfgThrshldChk_E = 90,
    RRC_RB_CheckFullConfigOfRrcConnRecfgMsg_E,
    RRC_RB_CheckRadioResCfgCommInfoOfMobilityCtrlInfo_E,
    RRC_RB_CheckSrsUlCfgCommInfo_E,
    RRC_RB_CheckAntennaCfgInfoOfMobilityCtrlInfo_E,
    RRC_RB_CheckUpPwrCtrlCommR10OfMobilityCtrlInfo_E,
    RRC_RB_CheckMobilCtrlInfoOfGu2LHo_E,
    LPS_OM_GetTmode_E,
    RRC_CSEL_DecodeAccessSysInfoWhenSearch_E,
    RRC_COMM_CheckIsEqulCell_E,
    RRC_CSEL_GetCampState_E = 100,
    RRC_CSEL_GetSearchCampSate_E,
    LRRC_LCSEL_GetDeleteFddBandMCC_E,
    LRRC_LCSEL_GetDeleteFddBandFlg_E,
    RRC_SIB_GetRcvRslt_E,
    RRC_SIB_IsReceivedAll_E,
    RRC_CSEL_DecodeReselParamInSib3_E,
    RRC_CSEL_DecodeReselParamInSib4_E,
    RRC_CSEL_DecodeReselParamInSib5_E,
    RRC_CSEL_DecodeReselParamInSib6_E,
    LRRC_LCSEL_SaveUtraTddFreqListSib6_E = 110,
    LRRC_LCSEL_SaveUtraFddFreqListSib6_E,
    RRC_CSEL_DecodeReselParamInSib7_E,
	RRC_RRC_JudgeAccessRestrict_E,
    RRC_RRC_JudgeMoSignallingAccessRestrict_E,
    RRC_RRC_JudgeUsimAcIsValid_E,
    RRC_RRC_JudgeCellAccessIsBarredByRandom_E,
    RRC_RRC_JudgeEmergencyAndMoCallAcIsValid_E,
    RRC_COMM_CheckBandInUeCap_E,
    LRRC_COMM_SetMFBISupportFlag_E,
    LRRC_COMM_SetMFBICampOnWiderBandFlag_E = 120,
    LRRC_COMM_SaveCapBandFreqInfoTable_E,
    RRC_COMM_SortAndSaveSubBandInfo_E,
    LRRC_COMM_SrchSupportBandByFreq_E,
    RRC_COMM_DelMFBIBarredCellFromBarList_E,
    RRC_CSEL_DelCellFromBarredList_E,
    LRRC_LRRC_SaveW2LRedirReqMsg_E,
    LRRC_LRRC_SaveG2LRedirReqMsg_E,
    RRC_RRC_ConfigUuUlTransMsg_E,
    RRC_ITFL2_SndUlTransAmDataReq_E,
    RRC_RRC_ProcUlInfoTransMsg_E = 130,
    RRC_COMM_CheckNeedHookDlMsg_E,
    LRRC_COMM_SetANRSupportFlag_E,
    RRC_RRC_JudgeMtCallAccessRestrict_E,
    RRC_RRC_JudgeEmergencyCallAccessRestrict_E,
    RRC_RRC_JudgeMoCallAccessRestrict_E,
    RRC_RRC_AccessBarringCheck_E,
    LRRC_APP_DBG_FUN_NAME_ENUM_BUTT
};
typedef VOS_UINT32 LRRC_APP_DBG_FUN_NAME_ENUM_UNIT32;

typedef LRRC_APP_DBG_FUN_NAME_ENUM_UNIT32 LRRC_APP_DBG_SUBFUN_NAME_ENUM_UNIT32;
typedef LRRC_APP_DBG_FUN_NAME_ENUM_UNIT32 LRRC_APP_DBG_PARA_ENUM_UNIT32;
/*****************************************************************************
 结构名    : LRRC_APP_DBG_MSG_TYPE_ENUM
 结构说明  : DBG上报的类型枚举
*****************************************************************************/
enum LRRC_APP_DBG_MSG_TYPE_ENUM
{
    FUNCTION_ENTRY_INFO = 0,                /* 函数入口参数打印 ,不再使用*/
    CELL_BARED_BY_SIB,                      /* SIB消息指示小区被bar */
    CELL_FORBIDENED_BY_SIB,                 /* SIB消息指示小区被forbidden */
    CELL_FORBIDENED_BY_TAI,                 /* 所有tai都在forbidden ta list中 */
    CELL_BARED_BY_EPLMN_AC,                 /* 等效PLMN list为空 或 AC数据异常 */
    CELL_BARED_BY_FIRST_PLMN_MCC,           /* SIB1中PlmnIdList包含的第一个PLMN的MCC不存在 */
    CELL_BAR_FORBIDEN,                      /* 小区被bar或forbidden */
    CELL_FREQ_BARED,                        /* 频率被bar */
    CELL_NO_LIMIT,                          /* 小区不受限 ，不再使用*/
    DECODE_SYS_INFO_ERROR,                  /* 系统消息解码错误 */
    INVALID_S_VALUE,                        /* 10,不满足S准则 */
    INVALID_FREQ_CELLID,                    /* 频点错误 */
    INVALID_BAND,                           /* 不支持的band */
    BAND_OUT_CUSTOM,                        /* band范围不在定制范围内 */
    BAND_CUSTOM,                            /* band范围在定制范围内 */
    BAND_FREQ_SUPPORT,                      /* band和频率都在支持范围内 ,不再使用 */
    BANDWIDTH_OUT_CUSTOM,                   /* 带宽不在定制范围内 */
    BAND_NOT_64,                            /* 当前band不是band64 */
    CAMPED_ON_ANY_CELL,                     /* any cell驻留状态 */
    MEASCFG_MSG_INVALID,                    /* 测量配置消息错误 */
    MSG_NULL,                               /* 20,空消息 */
    GU2L_LIMIT_NOTEPLMN_ACESS,              /* GU->L重选时，非等效PLMN或推出被bar */
    GU2L_LIMIT_FORBID_BAR,                  /* GU->L重选时，被forbidden或bar */
    TAI_IN_FORBIDEN_TA,                     /* 存在TAI在被禁止TA列表中 */
    TAI_NOT_IN_FORBIDEN_TA,                 /* TAI都不在被禁止TA列表中 */
    CELL_CSG_INDICATION_TRUE,               /* 小区的csg-Indication指示为TRUE */
    CELL_NOT_SUITABLE,                      /* 非suitable小区 */
    MAIN_STATUS_INVALID,                    /* 主状态错误 */
    RSRP_INVALID,                           /* RSRP不满足S准则 */
    RSRQ_INVALID,                           /* RSRQ的S值小于0 */
    HO_FAIL_CONF_UNACCEPTABLE,              /* 30,切换失败原因值为CONF_UNACCEPTABLE */
    HO_FAIL_ARFCN_NOT_SUPPORT,              /* 切换失败原因值为ARFCN_NOT_SUPPORT */
    BANDWIDTH_INVALID,                      /* 带宽不可用 */
    PUCCH_INVALID,                          /* PUCCH不可用 */
    PARA_VAL_INVALID,                       /* 参数取值非法 */
    MOBILOTY_AS_SECURITY_INACTIVE,          /* 安全未激活 */
    MOBILOTY_INCLUDE_NAS_INFO,              /* 存在NAS消息 */
    TMODE_ERROR,                            /* 模式错误 */
    CONFIG_UNSUPPORT,                       /* 配置参数错误 */
    ARRAY_OVERFLOW,                         /* 数组越界 */
    NOT_CHINA_MCC,                          /* 40,非国内MCC */
    JP_MCC_MNC_NOT_EPLMN,                   /* 软银定制，MNC1不是EPLMN */
    NOT_JP_MCC,                             /* 非日本MCC */
    CHINA_MODE_DEL_FDD_BAND,                /* 国内模式删除FDDband */
    ONLY_NO_SIB12,                          /* 仅没收到sib12 */
    USIM_ABSENT_OR_INVALID,                 /* USIM卡不存在或不可用 */
    T302_RUNNING,                           /* T302运行中 */
    T305_RUNNING,                           /* T305运行中 */
    MFBI_FLAG_OFF,                          /* MFBI开关关闭 ，不再使用*/
    MFBI_FLAG_ON,                           /* 50,MFBI开关打开 */
    WIDER_BAND_FLAG,                        /* 驻留到协议物理带宽更宽Band开关 */
    CHECK_ALL_BAND,                         /* 检查UE支持的所有band */
    ULTRANSMSG_MASK,                        /* 被屏蔽的上行直传消息 */
    DLTRANSMSG_MASK,                        /* 被屏蔽的下行直传消息 */
    LMM_DATA_REQ,                           /* LMM_LRRC_DATA_REQ */
    AM_DATA_IND,                            /* AM DTAT IND */
    AM_DATA_REQ,                            /* AM DTAT REQ */
    ANR_FLAG,                               /* ANR开关 */
    EST_BAR_FOR_AC_CLASS,                   /* 接入bar---接入等级不合法 */
    EST_BAR_RHPLMN_ERR,                     /* 60，接入bar---MO CALL检查失败 */
    EST_BAR_RPLMN_ERR,                      /* 接入bar---RPLMN不为Home PLMN,EHPLMN,Home country之一 */
    EST_BAR_BY_RANDOM,                      /* 接入bar---被随机值bar */
    T303_RUNNING,                           /* 接入bar---T303运行中 */
    T306_RUNNING,                           /* 接入bar---T306运行中 */
    LRRC_APP_DBG_MSG_TYPE_ENUM_BUTT
};
typedef VOS_UINT32 LRRC_APP_DBG_MSG_TYPE_ENUM_UINT32;

enum LRRC_APP_CONNREQ_TIMER_STATUS_ENUM
{
    LRRC_APP_CONNREQ_TIMER_STATUS_NOT_USED = 0 ,
    LRRC_APP_CONNREQ_TIMER_STATUS_USED = 1,

    LRRC_APP_CONNREQ_TIMER_STATUS_BUTT
};
typedef VOS_UINT8 LRRC_APP_CONNREQ_TIMER_STATUS_ENUM_UINT8;

enum LRRC_APP_TIMER_STATUS_ENUM
{
    LRRC_APP_TIMER_STATUS_RUNNING = 1,
    LRRC_APP_TIMER_STATUS_STOP = 2,
    LRRC_APP_TIMER_STATUS_BUTT
};
typedef VOS_UINT8 LRRC_APP_TIMER_STATUS_ENUM_UINT8;

/*****************************************************************************
   5 STRUCT
*****************************************************************************/

/*****************************************************************************
 结构名    : PS_MSG_STRU
 结构说明  : 协议栈各模块消息结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT32          ulMsgId;        /* 消息原语ID */
    VOS_UINT8           aucPara[4];     /* 参数内容 */
} PS_MSG_STRU;


/*****************************************************************************
 结构名    : PS_OM_AIR_MSG_REPORT_IND_STRU
 结构说明  : 协议栈所有模块上报空口消息的结构体
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT32                  ulMsgId;        /*原语类型,*/
    APP_MSG_HEADER

    VOS_UINT8                   ucUpDown;       /*上下行标识：   0-上行，1-下行  */
    OM_PS_AIR_MSG_ENUM_UINT8    enAirMsgId;     /*空中接口消息的Id               */
    VOS_UINT8                   aucReserved[2]; /*保留                           */
    VOS_UINT32                  ulAirLength;    /*空中消息数据长度               */
    VOS_UINT8                   aucData[4];     /*空中消息数据                   */
}PS_OM_AIR_MSG_REPORT_IND_STRU;

/*****************************************************************************
 结构名    : PS_OMT_KEY_EVENT_REPORT_IND_STRU
 结构说明  : PS所有模块->OMT的关键事件结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT32                          ulMsgId;        /*原语类型,*/
    APP_MSG_HEADER

    OM_PS_KEY_EVENT_ENUM_UINT8          ucKeyEvent;     /*关键事件类型，为枚举值 */
    VOS_UINT8                           aucReserved[3];
}PS_OMT_KEY_EVENT_REPORT_IND_STRU;


/*****************************************************************************
 结构名    : OM_PS_TRANSPARENT_CMD_REQ_STRU
 结构说明  : OMT->PS所有模块的透明命令下发结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT32          ulMsgId;        /*原语类型*/
    APP_MSG_HEADER

    VOS_UINT8           aucTransCmd[4]; /*透明码流，可变*/
}OM_PS_TRANSPARENT_CMD_REQ_STRU;

/*****************************************************************************
 结构名    : PS_OM_TRANSPARENT_CMD_CNF_STRU
 结构说明  : PS->OMT的透明命令执行结果数据结构(
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT32          ulMsgId;            /*原语类型*/
    APP_MSG_HEADER

    VOS_UINT8           aucTransCmdCnf[4];  /*透明命令结果码流，可变*/
}PS_OM_TRANSPARENT_CMD_CNF_STRU;
/*****************************************************************************
 结构名    : PS_OM_TRANSPARENT_CMD_IND_STRU
 结构说明  : PS->OMT的透明命令执行IND数据结构(
*****************************************************************************/
typedef  PS_OM_TRANSPARENT_CMD_CNF_STRU PS_OM_TRANSPARENT_CMD_IND_STRU;


/*PS可维可测 BEGIN */

/*****************************************************************************
 结构名    : OM_PS_CAMPEDCELL_INFO_REQ_STRU
 结构说明  : MSP查询驻留小区信息的REQ/CNF命令数据结构
*****************************************************************************/
typedef  APP_OM_MSG_STRU OM_PS_CAMPEDCELL_INFO_REQ_STRU;
typedef  APP_OM_MSG_STRU PS_OM_CAMPEDCELL_INFO_CNF_STRU;

/*****************************************************************************
 结构名    : OM_PS_HANDOVER_INFO_REQ_STRU
 结构说明  : MSP查询小区切换信息的REQ/CNF命令数据结构
*****************************************************************************/
typedef  APP_OM_MSG_STRU OM_PS_HANDOVER_INFO_REQ_STRU;
typedef  APP_OM_MSG_STRU PS_OM_HANDOVER_INFO_CNF_STRU;

/*****************************************************************************
 结构名    : OM_PS_PRACH_SR_INFO_REQ_STRU
 结构说明  : MSP查询小区PRACH\SR的REQ/CNF命令数据结构
*****************************************************************************/
typedef  APP_OM_MSG_STRU OM_PS_PRACH_SR_INFO_REQ_STRU;
typedef  APP_OM_MSG_STRU PS_OM_PRACH_SR_INFO_CNF_STRU;

/*****************************************************************************
 结构名    : OM_PS_BSR_INFO_REQ_STRU
 结构说明  : MSP查询BSR的REQ/CNF命令数据结构
*****************************************************************************/
typedef  APP_OM_MSG_STRU OM_PS_BSR_INFO_REQ_STRU;
typedef  APP_OM_MSG_STRU PS_OM_BSR_INFO_CNF_STRU;

/*****************************************************************************
 结构名    : OM_L2_THROUGHPUT_REQ_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : APP向L2请求APP层Tx与Rx统计信息结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT32                          ulMsgId;
    APP_MSG_HEADER
    VOS_UINT32                          ulOpId;
    DT_CMD_ENUM_UINT8                   enCmd;
    VOS_UINT8                           aucRsv[3];
    VOS_UINT32                          ulRptPeriod;
}OM_L2_THROUGHPUT_REQ_STRU;

/*****************************************************************************
 结构名    : L2_OM_THROUGHPUT_CNF_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : OM向L2请求APP 层Tx与Rx统计信息的回应结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT32                          ulMsgId;
    APP_MSG_HEADER
    VOS_UINT32                          ulOpId;
    DT_RESULT_ENUM_UINT32               enResult;
} L2_OM_THROUGHPUT_CNF_STRU;

/*****************************************************************************
 结构名    : OM_L2_SWITCH_CTRL_REQ_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : APP向L2请求修改可维可测控制开关
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT32                          ulMsgId;
    APP_MSG_HEADER
    VOS_UINT32                          ulOpId;
    VOS_UINT32                          ulTLPduRptFlg;
    VOS_UINT32                          ulCdsUlIpPktRptFlg;
    VOS_UINT32                          ulCdsDlIpPktRptFlg;
    VOS_UINT32                          ulLPdcpUlPduRptFlg;
    VOS_UINT32                          ulLPdcpDlPduRptFlg;
    VOS_UINT32                          ulLPdcpDlSduRptFlg;
    VOS_UINT32                          ulLRlcUlPduRptFlg;
    VOS_UINT32                          ulLRlcDlPduRptFlg;
    VOS_UINT32                          ulLRlcAmCtrlPduRptFlg;
    VOS_UINT32                          ulLMacUlPduRptFlg;
    VOS_UINT32                          ulLMacDlPduRptFlg;
    VOS_UINT32                          ulCdsUlIpHookFlg;
    VOS_UINT32                          ulCdsDlIpHookFlg;
    VOS_UINT32                          ulCdsDlLteDbgHookFlg;
    VOS_UINT32                          ulCdsImsHookFlg;
    VOS_UINT32                          ulCdsBastetHookFlg;
    VOS_UINT32                          ulLteLatencyRptFlg;
    VOS_UINT32                          ulLMacRegBsrandSRInfoFlg;
    VOS_UINT32                          ulLMacBsrRptFlg;
    VOS_UINT32                          ulLteOverviewFlg;
    VOS_UINT8                           ucSwitch[20];
    VOS_UINT32                          ulLMacUlGrantFlg;
    VOS_UINT32                          ulLMacDlAssignFlg;
    VOS_UINT32                          ulLMacSrReqRptFlg;
    VOS_UINT32                          ulLMacLPhyPhrCfgReqRptFlg;
    VOS_UINT32                          ulLMacLPhySrCnfRptFlg;
    VOS_UINT32                          ulLMacLPhyPucchIndRptFlg;
    VOS_UINT32                          ulLMacLPhyTaReqRptFlg;
    VOS_UINT32                          ulLMacLPhyScellActReqRptFlg;


    /*
    L2 开关说明
    ucSwitch[0]:g_ulTlUpEventFlag      TDS&LTE L2事件上报开关
    ucSwitch[1]:g_ulRlcPrintFlag       LTE L2数传打印开关
    ucSwitch[2]:g_ulLMacRebootTmpFlag  LTE RAR接收超时手动触发复位开关
    ucSwitch[3]:g_ulL2UlGrantIndFlag   LTE MAC 上行授权消息上报开关
    ucSwitch[4]:g_ulL2UlSrDataFlag     LTE MAC 上行SR发起消息上报开关
    ucSwitch[5]:g_ulRohcPrintFlag      LTE ROHC 可维可测打印信息上报开关
    ucSwitch[6]:g_ulTcpPrintFlag       LTE TCP 信息打印上报开关,已删除，做占位用
    ucSwitch[7]:g_ulTcpDlFlg           LTE TCP 下行打印上报开关,已删除，做占位用
    ucSwitch[8]:g_ulTcpUlFlg           LTE TCP 上行打印上报开关,已删除，做占位用
    ucSwitch[9]:g_ulTcpRbID            LTE TCP 监测特定rb上数据,已删除，做占位用
    */
}OM_L2_SWITCH_CTRL_REQ_STRU;

/*****************************************************************************
 结构名    : L2_OM_SWITCH_CTRL_CNF_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : OM向L2请求修改可维可测控制开关的回应结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT32                          ulMsgId;
    APP_MSG_HEADER
    VOS_UINT32                          ulOpId;
    DT_RESULT_ENUM_UINT32               enResult;
} L2_OM_SWITCH_CTRL_CNF_STRU;

/*DCM logger*/
/*****************************************************************************
 结构名    : OM_L2_MAC_PDU_RPT_REQ_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : OM向L2指示打开或者关闭MAC PDU信息的上报
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT32                          ulMsgId;
    APP_MSG_HEADER
    VOS_UINT32                          ulOpId;
    VOS_UINT8                           ucCmd;  /*值为0，表示关闭上报，值为1，表示打开上报*/
    VOS_UINT8                           aucRsv[3]; /*预留字节*/
}OM_L2_MAC_PDU_RPT_REQ_STRU;

/*****************************************************************************
 结构名    : L2_OM_MAC_PDU_RPT_CNF_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : L2向OM回复打开或者关闭MAC PDU信息的上报是否成功
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT32                          ulMsgId;
    APP_MSG_HEADER
    VOS_UINT32                          ulOpId;
    VOS_UINT8                           ucResult;  /*值为0，表示成功，其他值，表示失败*/
    VOS_UINT8                           aucRsv[3]; /*预留字节*/
} L2_OM_MAC_PDU_RPT_CNF_STRU;

/*****************************************************************************
 结构名    : OM_L2_BSR_RPT_REQ_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : OM向L2指示打开或者关闭BSR信息的上报
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT32                          ulMsgId;
    APP_MSG_HEADER
    VOS_UINT32                          ulOpId;
    VOS_UINT8                           ucCmd;  /*值为0，表示关闭上报，值为1，表示打开上报*/
    VOS_UINT8                           aucRsv[3]; /*预留字节*/
}OM_L2_BSR_RPT_REQ_STRU;

/*****************************************************************************
 结构名    : L2_OM_BSR_RPT_CNF_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : L2向OM回复打开或者关闭BSR信息的上报是否成功
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT32                          ulMsgId;
    APP_MSG_HEADER
    VOS_UINT32                          ulOpId;
    VOS_UINT8                           ucResult;  /*值为0，表示成功，其他值，表示失败*/
    VOS_UINT8                           aucRsv[3]; /*预留字节*/
} L2_OM_BSR_RPT_CNF_STRU;
/*end DCM*/
/*****************************************************************************
 结构名    : OM_L2_SDU_COUNT_REQ_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : APP向L2请求各层SDU统计信息
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT32                          ulMsgId;
    APP_MSG_HEADER
    VOS_UINT32                          ulOpId;
    DT_CMD_ENUM_UINT8                   enCmd;
    VOS_UINT8                           aucRsv[3];
    VOS_UINT32                          ulRptPeriod;
}OM_L2_SDU_COUNT_REQ_STRU;

/*****************************************************************************
 结构名    : L2_OM_APP_THROUGHPUT_CNF_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : APP向L2请求各层SDU统计信息的回应结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT32                          ulMsgId;
    APP_MSG_HEADER
    VOS_UINT32                          ulOpId;
    DT_RESULT_ENUM_UINT32               enResult;
} L2_OM_SDU_COUNT_CNF_STRU;


/*****************************************************************************
 结构名    : APP_LPS_PRACH_SR_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : PS向MSP上报PRACH/SR相关信息结构体
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulTotalPblNum;         /*Preamble Number*/
    VOS_UINT32                          ulGrpAPblNum;          /*Group A Preamble Number*/
    VOS_UINT32                          ulGrpBPblNum;          /*Group B Preamble Number*/
    VOS_UINT32                          ulContentionPblNum;    /*Contention Preamble Number*/
    VOS_UINT32                          ulNoneContentionPblNum;/*None Contention Preamble Number*/
    VOS_UINT32                          ulRarNum;              /*RAR Number*/
    VOS_UINT32                          ulCompetitionNum;      /*Competition Number*/
    VOS_UINT32                          ulSrNum;               /*SR Number*/
}APP_LPS_PRACH_SR_INFO_STRU;

/*****************************************************************************
 结构名    : APP_LPS_BSR_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : PS向MSP上报BSR相关信息结构体
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulShortBsrGrpId;          /*short bsr对应的GROUP ID*/
    VOS_UINT32                          ulShortBsrValue;          /*Short BSR*/
    VOS_UINT32                          ulLongBsr0Value;          /*Long BSR 0*/
    VOS_UINT32                          ulLongBsr1Value;          /*Long BSR 1*/
    VOS_UINT32                          ulLongBsr2Value;          /*Long BSR 2*/
    VOS_UINT32                          ulLongBsr3Value;          /*Long BSR 3*/
}APP_LPS_BSR_INFO_STRU;

/*****************************************************************************
 结构名    : LPS_PLMN_ITEM_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : PLMN条目
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           aucPlmnId[3];
    VOS_UINT8                           ucReserved;
} LPS_PLMN_ITEM_STRU;

/*****************************************************************************
 结构名    : LPS_PLMN_ITEM_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : PLMN LIST
*****************************************************************************/
typedef struct
{
    VOS_UINT16                          usPlmnCount;
    VOS_UINT8                           aucReservered[2];
    LPS_PLMN_ITEM_STRU                  astPlmnItems[LPS_RRC_CSEL_MAX_FOUND_PLMN_COUNT];
} LPS_CSEL_PLMN_LIST_CTX_STRU;

/*****************************************************************************
 结构名    : LPS_NAS_MM_TAC_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : TAC 信息
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucTac;
    VOS_UINT8                           ucTacCnt;
    VOS_UINT8                           aucRsv[2];
}LPS_NAS_MM_TAC_STRU;

/*****************************************************************************
 结构名    : LPS_NAS_MM_TA_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : TA 信息
*****************************************************************************/
typedef struct
{
    LPS_PLMN_ITEM_STRU                 stPlmnId;
    LPS_NAS_MM_TAC_STRU                stTac;
}LPS_NAS_MM_TA_STRU;

/*****************************************************************************
 结构名    : LPS_NAS_MM_TA_LIST_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : TA LIST
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulTaNum;            /* TA的个数:为0表示无相关信息*/
    LPS_NAS_MM_TA_STRU                  astTa[LPS_NAS_MM_MAX_TA_NUM];
}LPS_NAS_MM_TA_LIST_STRU;

/*****************************************************************************
 结构名    : LPS_NAS_MM_GUTI_HEAD_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : GUTI Head
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucLenth;
    VOS_UINT8                           ucOeToi;
    VOS_UINT8                           aucRsv[2];
}LPS_NAS_MM_GUTI_HEAD_STRU;

/*****************************************************************************
 结构名    : LPS_NAS_MM_GUTI_HEAD_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : GUTI Head
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucGroupId;
    VOS_UINT8                           ucGroupIdCnt;
    VOS_UINT8                           aucRsv[2];
}LPS_NAS_MM_MME_GROUPID_STRU;

/*****************************************************************************
 结构名    : LPS_NAS_MM_MME_CODE_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : MME code
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucMmeCode;
    VOS_UINT8                           aucRsv[3];
}LPS_NAS_MM_MME_CODE_STRU;

/*****************************************************************************
 结构名    : LPS_NAS_MM_MME_CODE_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : MTIMSI
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucMTmsi;
    VOS_UINT8                           ucMTmsiCnt1;
    VOS_UINT8                           ucMTmsiCnt2;
    VOS_UINT8                           ucMTmsiCnt3;
}LPS_NAS_MM_MTMSI_STRU;

/*****************************************************************************
 结构名    : LPS_NAS_MM_MME_CODE_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : GUTI 信息
*****************************************************************************/
typedef struct
{
    LPS_NAS_MM_GUTI_HEAD_STRU               stGutiHead;
    LPS_PLMN_ITEM_STRU                      stPlmnId;
    LPS_NAS_MM_MME_GROUPID_STRU             stMmeGroupId;
    LPS_NAS_MM_MME_CODE_STRU                stMmeCode;
    LPS_NAS_MM_MTMSI_STRU                   stMTmsi;
}LPS_NAS_MM_GUTI_STRU;

/*****************************************************************************
 结构名    : LPS_NAS_ESM_IP_ADDR_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : IP地址信息
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucIpType;
    VOS_UINT8                           ucReserved[3];
    VOS_UINT8                           aucIpV4Addr[LPS_APP_MAX_IPV4_ADDR_LEN];
    VOS_UINT8                           aucIpV6Addr[LPS_APP_MAX_IPV6_ADDR_LEN];
}LPS_NAS_ESM_IP_ADDR_STRU;

/*****************************************************************************
 结构名    : LPS_NAS_ESM_EPSB_CNTXT_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 每个EPS承载对应的IP 地址
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulEpsbId;           /*对应EPS承载ID*/
    LPS_NAS_ESM_IP_ADDR_STRU            stPdnAddrInfo;
}LPS_NAS_ESM_EPSB_CNTXT_INFO_STRU;


/*****************************************************************************
 结构名    : LPS_NAS_IMSI_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : IMSI信息
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           aucImsi[LPS_NAS_MM_MAX_UEID_BUF_SIZE];    /*IMSI*/
    VOS_UINT8                           ucResvd[2];
}LPS_NAS_IMSI_STRU;


/*****************************************************************************
 结构名    : APP_LPS_CAMPED_CELL_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : PS向MSP上报驻留小区的相关信息结构体
*****************************************************************************/
typedef struct
{
    VOS_UINT16                          usPhyCellId;          /*servering cell id*/
    VOS_UINT16                          usSyncFlag;           /*1:同步 0:失步 显示"Sync"or"Out of Sync"*/
    LPS_CSEL_PLMN_LIST_CTX_STRU         stPlmnListInfo;      /*PLMN LIST*/
    LPS_NAS_MM_TA_LIST_STRU             stRegPlmnTaList;     /*TA LIST*/

    LPS_NAS_IMSI_STRU                   stImsiInfo;          /*IMSI*/
    LPS_NAS_MM_GUTI_STRU                stGutiInfo;          /*GUTI*/


    LPS_NAS_ESM_EPSB_CNTXT_INFO_STRU    astEpsbCntxtInfo[LPS_NAS_ESM_MAX_EPSB_NUM]; /* EPS承载IP地址信息  */

    VOS_UINT16                          usUlEarfcn;          /*UL EARFCN */
    VOS_UINT16                          usDlEarfcn;          /*DL EARFCN*/
    VOS_UINT16                          usUlFreq;            /*UL Frequency 单位:Mhz*/
    VOS_UINT16                          usDlFreq;            /*DL Frequency 单位:Mhz*/
    VOS_UINT16                          usUlBandwidth;       /*单位:RB个数 */
    VOS_UINT16                          usDlBandwidth;       /*单位:RB个数 */
    VOS_UINT32                          ulBand;              /*BAND ID*/

    VOS_UINT16                          usCRnti;             /*C-RNTI*/
    VOS_UINT16                          usSpsRnti;           /*SPS-RNTI*/
    VOS_UINT16                          usPucchRnti;         /*TPC-PUCCH-RNTI*/
    VOS_UINT16                          usPuschRnti;         /*TPC-PUSCH-RNTI*/

    VOS_UINT8                           ucSbuFrmAssignmentType;
    VOS_UINT8                           ucSpecialSubFrmPatterns;
    VOS_UINT8                           ucUlCpLength;
    VOS_UINT8                           ucDlCpLength;

    VOS_UINT8                           ucEnbTxAntennaNum;
    VOS_UINT8                           ucTransmissionMode;
    VOS_UINT8                           aucResv[2];


}APP_LPS_CAMPED_CELL_INFO_STRU;
/*****************************************************************************
 结构名    : APP_PS_HO_STAT_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 切换次数统计
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulHOTotalCnt;      /*切换总次数*/
    VOS_UINT32                          ulHOSuccCnt;       /*切换成功的次数*/
    VOS_UINT32                          ulHOFailCnt;       /*切换失败的次数*/
}LPS_HO_STAT_INFO_STRU;


/*****************************************************************************
 结构名    : APP_PS_HANDOVER_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : PS向MSP上报切换的相关信息结构体
*****************************************************************************/
typedef struct
{
    VOS_UINT16                          usLatestHOLatency;      /*最近一次切换时延*/
    VOS_UINT16                          usAverageHOLatency;     /*平均切换时延*/
    LPS_HO_STAT_INFO_STRU                astIntraFreqHO;
    LPS_HO_STAT_INFO_STRU                astInterFreqHO;        /*异频切换统计*/
    LPS_HO_STAT_INFO_STRU                astInterRatHO;         /*异系统切换次数统计*/
}APP_LPS_HANDOVER_INFO_STRU;

/*PS可维可测 END */


/*****************************************************************************
 结构名    :TL_OM_COMM_HEAD_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :TDS L2单个事件上报结构体
*****************************************************************************/
typedef struct
{
    VOS_UINT16                          usPid;
    VOS_UINT16                          usRbId;
    VOS_UINT16                          usFrameNum;
    VOS_UINT16                          usSubFrameNum;
} TL_OM_COMM_HEAD_STRU;

/*****************************************************************************
 结构名    :TDS_L2_SINGLE_EVENT_RPT_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :TDS L2单个事件上报结构体
*****************************************************************************/
typedef struct
{
    TL_OM_COMM_HEAD_STRU                stCommHead;
    TL_UP_EVENT_ENUM_UINT16             usEvent;
    VOS_UINT16                          usRsv;
} TL_OM_SINGLE_UP_EVENT_RPT_STRU;

/*****************************************************************************
 结构名    :TDS_L2_AlL_EVENT_RPT_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :TDS L2所有事件上报结构体
*****************************************************************************/
typedef struct
{
    TL_OM_COMM_HEAD_STRU                stCommHead;
    VOS_UINT32                          aulEventStat[TL_UP_EVENT_BUTT];
} TL_OM_AlL_UP_EVENT_RPT_STRU;

/*****************************************************************************
 结构名    : TL_OM_CDS_IPF_PROC_STAT_IND_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : CDS IPF相关处理统计量
*****************************************************************************/
typedef struct
{
    TL_OM_COMM_HEAD_STRU    stCommHead;

    /*IPF事件*/
    VOS_UINT32              ulIpfULIntNum;
    VOS_UINT32              ulULProcIpfIntNum;
    VOS_UINT32              ulUlAdqEmtypIntNum;
    VOS_UINT32              ulUlProcAdqEmptyIntNum;

    /*上行RD统计信息*/
    VOS_UINT32              ulULIpfRxRDNum;
    VOS_UINT32              ulULIpfTransMemFailNum;
    VOS_UINT32              ulULIpfModemIdErrNum;
    VOS_UINT32              ulULIpfFreePktInLB;
    VOS_UINT32              ulULIpfRxNdisPktNum;
    VOS_UINT32              ulULIpfRxIpPktNum;
    VOS_UINT32              ulULIpfRxIpSegNum;
    VOS_UINT32              ulULIpfFilterErrNum;

    /*上行AD信息*/
    VOS_UINT32              ulULADQCfgAdNum;
    VOS_UINT32              ulULADQCfgAd0Num;
    VOS_UINT32              ulULADQCfgAd1Num;
    VOS_UINT32              ulULADQAllocMemNum;
    VOS_UINT32              ulULADQFreeMemNum;
    VOS_UINT32              ulULADQCfgAdSucc;
    VOS_UINT32              ulULADQCfgAdFail;
    VOS_UINT32              ulULADQStartEmptyTmrNum;
    VOS_UINT32              ulULADQStopEmptyTmrNum;
    VOS_UINT32              ulULADQEmptyTmrTimeoutNum;
    VOS_UINT32              ulULADQAd0EmptyIntNum;
    VOS_UINT32              ulULADQAd1EmptyIntNum;


    /*下行IPF信息*/
    VOS_UINT32              ulDLIpfSaveSrcMemFail;
    VOS_UINT32              ulDLIpfSaveSrcMemSucc;
    VOS_UINT32              ulDLIpfFreeSrcMemNum;
    VOS_UINT32              ulDLIpfFreeSrcMemErr;

    VOS_UINT32              ulDLIpfCfgHaveNoBD;
    VOS_UINT32              ulDLIpfCfgHaveNoCD;
    VOS_UINT32              ulDLIpfCfgCDNotEnough;
    VOS_UINT32              ulDLIpfCfgBDSucc;
    VOS_UINT32              ulDLIpfCfgBDFail;
    VOS_UINT32              ulDLIpfCfgBDAllocMemFail;
    VOS_UINT32              ulDLIpfCfgBDSduLenErr;
}TL_OM_CDS_IPF_PROC_STAT_IND_STRU;

/*****************************************************************************
 结构名    : TL_OM_CDS_UL_IP_PKT_STAT_IND_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : CDS 上行IP包相关处理统计量
*****************************************************************************/
typedef struct
{
    TL_OM_COMM_HEAD_STRU    stCommHead;

    /*上行IP包处理*/
    VOS_UINT32              ulULRecvNdPktNum;
    VOS_UINT32              ulULASRxIpPktNum;
    VOS_UINT32              ulULDirectFreePktNum;
    VOS_UINT32              ulULLTERxPktNum;
    VOS_UINT32              ulULGURxPktNum;
    VOS_UINT32              ulULNULLRxPktNum;
    VOS_UINT32              ulULRanModeErrFreeNum;

    VOS_UINT32              ulULLTEFcFreePktNum;
    VOS_UINT32              ulULLTEProcIpSegFail;
    VOS_UINT32              ulULLTESendToLPdcpNum;
    VOS_UINT32              ulULLTESavePktNum;
    VOS_UINT32              ulULLTEERabmStateErr;

    VOS_UINT32              ulULGUFcFreePktNum;
    VOS_UINT32              ulULGUSendToRabmNum;
    VOS_UINT32              ulULGUSavePktNum;
    VOS_UINT32              ulULGURabmStateErr;

    VOS_UINT32              ulULBuffEnQueSucc;
    VOS_UINT32              ulULBuffEnQueFail;
    VOS_UINT32              ulULBuffSoftFilterFail;
    VOS_UINT32              ulULBuffSendPktSuccNum;
    VOS_UINT32              ulULBuffFreePktNum;

    /*软过滤相关统计量*/
    VOS_UINT32              ulULSoftFilterPktNum;
    VOS_UINT32              ulULSoftFilterIPv4Num;
    VOS_UINT32              ulULSoftFilterIPv6Num;
    VOS_UINT32              ulULSoftFilterFailNum;
    VOS_UINT32              ulULSoftFilterIPSegNum;
    VOS_UINT32              ulULSoftFilterIPSegSucc;
    VOS_UINT32              ulULSoftFilterIPSegFail;
}TL_OM_CDS_UL_IP_PKT_STAT_IND_STRU;

/*****************************************************************************
 结构名    : TL_OM_CDS_DL_SDU_STAT_IND_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : CDS 下行SDU相关处理统计量
*****************************************************************************/
typedef struct
{
    TL_OM_COMM_HEAD_STRU    stCommHead;

    /*下行事件统计*/
    VOS_UINT32              ulDL10msTmrTrigEvent;
    VOS_UINT32              ulDL1msIntTrigEvent;
    VOS_UINT32              ulDLUmtsIntTrigEvent;
    VOS_UINT32              ulDLProcEventNum;

    /*下行SDU信息*/
    VOS_UINT32              ulDLGSMRxSduNum;
    VOS_UINT32              ulDLGSMRxImsNum;
    VOS_UINT32              ulDLGSMFcFreeSduNum;
    VOS_UINT32              ulDLGSMGetDeftRabIdFail;
    VOS_UINT32              ulDLGSMEnQueSucc;
    VOS_UINT32              ulDLGSMEnQueFail;

    VOS_UINT32              ulDLUMTSRxSduNum;
    VOS_UINT32              ulDLUMTSRxImsNum;
    VOS_UINT32              ulDLUMTSFcFreeSduNum;
    VOS_UINT32              ulDLUMTSDirectFreeSduNum;
    VOS_UINT32              ulDLUMTSGetDeftRabIdFail;
    VOS_UINT32              ulDLUMTSEnQueSucc;
    VOS_UINT32              ulDLUMTSEnQueFail;

    VOS_UINT32              ulDLLteRxSduNum;
    VOS_UINT32              ulDLLteRxDbgPkt;
    VOS_UINT32              ulDLLteRxMbmsSdu;
    VOS_UINT32              ulDLLteRxImsNum;
    VOS_UINT32              ulDLLteFcFreeSduNum;
    VOS_UINT32              ulDLLteDirectFreeSduNum;
    VOS_UINT32              ulDLLteGetEpsbIDFail;
    VOS_UINT32              ulDLLteGetDeftEpsbIDFail;
    VOS_UINT32              ulDLLteEnQueSucc;
    VOS_UINT32              ulDLLteEnQueFail;

    VOS_UINT32              ulDLEnSduQueSucc;
    VOS_UINT32              ulDLEnSduQueFail;
    VOS_UINT32              ulDLSduTrigEvent;
    VOS_UINT32              ulDLLteRxSduLenErrNum;
}TL_OM_CDS_DL_SDU_STAT_IND_STRU;

/*****************************************************************************
 结构名    : TL_OM_CDS_LOOP_BACK_STAT_IND_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : CDS 环回相关处理统计量
*****************************************************************************/
typedef struct
{
    TL_OM_COMM_HEAD_STRU    stCommHead;

    VOS_UINT32              ulLBDLRxSduInGSM;
    VOS_UINT32              ulLBDLRxSduInUMTS;
    VOS_UINT32              ulLBDLRxSduInLte;
    VOS_UINT32              ulLBDLRxSduInNonModeB;
    VOS_UINT32              ulLBDLAllocMemFail;
    VOS_UINT32              ulLBDDLEnQueSucc;
    VOS_UINT32              ulLBDLEnQueFail;
    VOS_UINT32              ulLBULSoftFilterFail;
    VOS_UINT32              ulLBUlSendPktNum;
    VOS_UINT32              ulLBUlClearPktNum;

}TL_OM_CDS_LOOP_BACK_STAT_IND_STRU;

/*****************************************************************************
 结构名    : TL_OM_CDS_IMS_PROC_STAT_IND_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : CDS IMS相关处理统计量
*****************************************************************************/
typedef struct
{
    TL_OM_COMM_HEAD_STRU    stCommHead;

    VOS_UINT32              ulImsULRxFunCallNum;
    VOS_UINT32              ulImsULRxFunParaErr;
    VOS_UINT32              ulImsULRxNicPktNum;
    VOS_UINT32              ulImsULAllocMemFail;
    VOS_UINT32              ulImsULEnQueFail;
    VOS_UINT32              ulImsULEnQueSucc;
    VOS_UINT32              ulImsULTrigImsEvent;
    VOS_UINT32              ulImsULProcImsEvent;
    VOS_UINT32              ulImsULSoftFilterFail;
    VOS_UINT32              ulImsULSendToRan;

    VOS_UINT32              ulImsDLRxImsPkt;
    VOS_UINT32              ulImsDLNdPktNum;
    VOS_UINT32              ulImsDLAllocMemFail;
    VOS_UINT32              ulImsDLSendToNicFail;
    VOS_UINT32              ulImsDLSendToNicSucc;

}TL_OM_CDS_IMS_PROC_STAT_IND_STRU;



/*****************************************************************************
 结构名    : TL_OM_LTE_ROHC_UL_STAT_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : LTE  ROHC 上行统计信息上报结构体
*****************************************************************************/
typedef struct
{
    TL_OM_COMM_HEAD_STRU                stCommHead;

    VOS_UINT32                          ulCidNo;
    VOS_UINT32                          ulUlCompTotalStat;
    VOS_UINT32                          ulUlCompSuccStat;
    VOS_UINT32                          ulUlCompFailStat;

    VOS_UINT32                          ulUlRecvFeedbackCnt;
    VOS_UINT32                          ulUlRecvFeedbackSuccCnt;
    VOS_UINT32                          ulUlRecvFeedbackeErrCnt;

    VOS_UINT32                          ulUlProfile0Stat;
    VOS_UINT32                          ulUlProfile1Stat;
    VOS_UINT32                          ulUlProfile2Stat;
    VOS_UINT32                          ulUlProfile3Stat;
    VOS_UINT32                          ulUlProfile4Stat;

    VOS_UINT32                          ulUlIRStatePktsCnt;
    VOS_UINT32                          ulUlFOStatePktsCnt;
    VOS_UINT32                          ulUlSOStatePktsCnt;

    VOS_UINT32                          ulUlUModePktsCnt;
    VOS_UINT32                          ulUlOModePktsCnt;
    VOS_UINT32                          ulUlRModePktsCnt;

    VOS_UINT32                          ulUlIRPktsCnt;
    VOS_UINT32                          ulUlIRDynPktsCnt;
    VOS_UINT32                          ulUlUO0PktsCnt;
    VOS_UINT32                          ulUlUO1PktsCnt;

    VOS_UINT32                          ulUlUO1_IdPktsCnt;
    VOS_UINT32                          ulUlUO1_TsPktsCnt;
    VOS_UINT32                          ulUlR0PktsCnt;
    VOS_UINT32                          ulUlR0CrcPktsCnt;

    VOS_UINT32                          ulUlUO1_Id_Ex0PktsCnt;
    VOS_UINT32                          ulUlUO1_Id_Ex1PktsCnt;
    VOS_UINT32                          ulUlUO1_Id_Ex2PktsCnt;
    VOS_UINT32                          ulUlUO1_Id_Ex3PktsCnt;

    VOS_UINT32                          ulUlR1PktsCnt;
    VOS_UINT32                          ulUlR1_Ex0PktsCnt;
    VOS_UINT32                          ulUlR1_Ex1PktsCnt;
    VOS_UINT32                          ulUlR1_Ex2PktsCnt;
    VOS_UINT32                          ulUlR1_Ex3PktsCnt;

    VOS_UINT32                          ulUlR1_IdPktsCnt;
    VOS_UINT32                          ulUlR1_Id_Ex0PktsCnt;
    VOS_UINT32                          ulUlR1_Id_Ex1PktsCnt;
    VOS_UINT32                          ulUlR1_Id_Ex2PktsCnt;
    VOS_UINT32                          ulUlR1_Id_Ex3PktsCnt;

    VOS_UINT32                          ulUlR1_TsPktsCnt;
    VOS_UINT32                          ulUlR1_Ts_Ex0PktsCnt;
    VOS_UINT32                          ulUlR1_Ts_Ex1PktsCnt;
    VOS_UINT32                          ulUlR1_Ts_Ex2PktsCnt;
    VOS_UINT32                          ulUlR1_Ts_Ex3PktsCnt;

    VOS_UINT32                          ulUlUOR2PktsCnt;
    VOS_UINT32                          ulUlUOR2_Ex0PktsCnt;
    VOS_UINT32                          ulUlUOR2_Ex1PktsCnt;
    VOS_UINT32                          ulUlUOR2_Ex2PktsCnt;
    VOS_UINT32                          ulUlUOR2_Ex3PktsCnt;

    VOS_UINT32                          ulUlUOR2_IdPktsCnt;
    VOS_UINT32                          ulUlUOR2_Id_Ex0PktsCnt;
    VOS_UINT32                          ulUlUOR2_Id_Ex1PktsCnt;
    VOS_UINT32                          ulUlUOR2_Id_Ex2PktsCnt;
    VOS_UINT32                          ulUlUOR2_Id_Ex3PktsCnt;

    VOS_UINT32                          ulUlUOR2_Ts_PktsCnt;
    VOS_UINT32                          ulUlUOR2_Ts_Ex0PktsCnt;
    VOS_UINT32                          ulUlUOR2_Ts_Ex1PktsCnt;
    VOS_UINT32                          ulUlUOR2_Ts_Ex2PktsCnt;
    VOS_UINT32                          ulUlUOR2_Ts_Ex3PktsCnt;

    VOS_UINT32                          ulUlPrf0IRPktsCnt;
    VOS_UINT32                          ulUlNormalPktsCnt;

    VOS_UINT32                          ulUlIR2FOCnt       ;                /* 上行状态迁移:IR2FO计数:压缩, 类型: ROHC实例统计项 | 分组: ROHC_ENTITY. */
    VOS_UINT32                          ulUlIR2SOCnt         ;              /* 上行状态迁移:IR2SO计数:压缩, 类型: ROHC实例统计项 | 分组: ROHC_ENTITY. */
    VOS_UINT32                          ulUlFO2SOCnt       ;                /* 上行状态迁移:FO2SO计数:压缩, 类型: ROHC实例统计项 | 分组: ROHC_ENTITY. */
    VOS_UINT32                          ulUlFO2IRCnt         ;               /* 上行状态迁移:FO2IR计数:压缩, 类型: ROHC实例统计项 | 分组: ROHC_ENTITY. */
    VOS_UINT32                          ulUlSO2IRCnt           ;           /* 上行状态迁移:SO2IR计数:压缩, 类型: ROHC实例统计项 | 分组: ROHC_ENTITY. */
    VOS_UINT32                          ulUlSO2FOCnt         ;              /* 上行状态迁移:SO2FO计数:压缩, 类型: ROHC实例统计项 | 分组: ROHC_ENTITY. */

}TL_OM_LTE_ROHC_UL_STAT_INFO_STRU;


/*****************************************************************************
 结构名    : TL_OM_LTE_ROHC_DL_STAT_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : LTE  ROHC 下行统计信息上报结构体
*****************************************************************************/
typedef struct
{
    TL_OM_COMM_HEAD_STRU                stCommHead;

    VOS_UINT32                          ulCidNo;
    VOS_UINT32                          ulDlProfile0Stat;
    VOS_UINT32                          ulDlProfile1Stat;
    VOS_UINT32                          ulDlProfile2Stat;
    VOS_UINT32                          ulDlProfile3Stat;
    VOS_UINT32                          ulDlProfile4Stat;

    VOS_UINT32                          ulDlRcvPktsCnt;
    VOS_UINT32                          ulDlRcvCrcOkPktsCnt;
    VOS_UINT32                          ulDlRcvCrcErrPktTypeCnt;
    VOS_UINT32                          ulDlRcvPktTypeErrCnt;

    VOS_UINT32                          ulDlNCStatePktsCnt;
    VOS_UINT32                          ulDlSCStatePktsCnt;
    VOS_UINT32                          ulDlFCStatePktsCnt;

    VOS_UINT32                          ulDlUModePktsCnt;
    VOS_UINT32                          ulDlOModePktsCnt;
    VOS_UINT32                          ulDlRModePktsCnt;

    VOS_UINT32                          ulDlSendFeedbackCnt;
    VOS_UINT32                          ulDlUMOdeAckCnt;
    VOS_UINT32                          ulDlUMOdeNackCnt;
    VOS_UINT32                          ulDlUMOdeStcNackCnt;
    VOS_UINT32                          ulDlOMOdeAckCnt;
    VOS_UINT32                          ulDlOMOdeNackCnt;


    VOS_UINT32                          ulDlOMOdeStcNackCnt;
    VOS_UINT32                          ulDlRMOdeAckCnt;
    VOS_UINT32                          ulDlRMOdeNackCnt;
    VOS_UINT32                          ulDlRMOdeStcNackCnt;
    VOS_UINT32                          ulDlFdbk1Cnt;

    VOS_UINT32                          ulDlPkt0Cnt;
    VOS_UINT32                          ulDlPkt1Cnt;
    VOS_UINT32                          ulDlPkt2Cnt;

    VOS_UINT32                          ulDlIRPktsCnt;
    VOS_UINT32                          ulDlIRDynPktsCnt;


    VOS_UINT32                          ulDlUO0PktsCnt;
    VOS_UINT32                          ulDlUO1PktsCnt;
    VOS_UINT32                          ulDlUO1_IDPktsCnt;


    VOS_UINT32                          ulDlUO1_TsPktsCnt;
    VOS_UINT32                          ulDlR0PktsCnt;
    VOS_UINT32                          ulDlR0CrcPktsCnt;

    VOS_UINT32                          ulDlUO1_Id_Ex0PktsCnt;
    VOS_UINT32                          ulDlUO1_Id_Ex1PktsCnt;
    VOS_UINT32                          ulDlUO1_Id_Ex2PktsCnt;
    VOS_UINT32                          ulDlUO1_Id_Ex3PktsCnt;

    VOS_UINT32                          ulDlR1PktsCnt;
    VOS_UINT32                          ulDlR1_Ex0PktsCnt;
    VOS_UINT32                          ulDlR1_Ex1PktsCnt;
    VOS_UINT32                          ulDlR1_Ex2PktsCnt;
    VOS_UINT32                          ulDlR1_Ex3PktsCnt;

    VOS_UINT32                          ulDlR1_IdPktsCnt;
    VOS_UINT32                          ulDlR1_Id_Ex0PktsCnt;
    VOS_UINT32                          ulDlR1_Id_Ex1PktsCnt;
    VOS_UINT32                          ulDlR1_Id_Ex2PktsCnt;
    VOS_UINT32                          ulDlR1_Id_Ex3PktsCnt;

    VOS_UINT32                          ulDlR1_TsPktsCnt;
    VOS_UINT32                          ulDlR1_Ts_Ex0PktsCnt;
    VOS_UINT32                          ulDlR1_Ts_Ex1PktsCnt;
    VOS_UINT32                          ulDlR1_Ts_Ex2PktsCnt;
    VOS_UINT32                          ulDlR1_Ts_Ex3PktsCnt;

    VOS_UINT32                          ulDlUOR2PktsCnt;
    VOS_UINT32                          ulDlUOR2_Ex0PktsCnt;
    VOS_UINT32                          ulDlUOR2_Ex1PktsCnt;
    VOS_UINT32                          ulDlUOR2_Ex2PktsCnt;
    VOS_UINT32                          ulDlUOR2_Ex3PktsCnt;

    VOS_UINT32                          ulDlUOR2_IdPktsCnt;
    VOS_UINT32                          ulDlUOR2_Id_Ex0PktsCnt;
    VOS_UINT32                          ulDlUOR2_Id_Ex1PktsCnt;
    VOS_UINT32                          ulDlUOR2_Id_Ex2PktsCnt;
    VOS_UINT32                          ulDlUOR2_Id_Ex3PktsCnt;

    VOS_UINT32                          ulDlUOR2_Ts_PktsCnt;
    VOS_UINT32                          ulDlUOR2_Ts_Ex0PktsCnt;
    VOS_UINT32                          ulDlUOR2_Ts_Ex1PktsCnt;
    VOS_UINT32                          ulDlUOR2_Ts_Ex2PktsCnt;
    VOS_UINT32                          ulDlUOR2_Ts_Ex3PktsCnt;

}TL_OM_LTE_ROHC_DL_STAT_INFO_STRU;



/*****************************************************************************
 结构名    : TL_OM_LTE_ROHC_COMPRESS_CONTEXT_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : LTE  ROHC 上行上下文信息上报结构体
*****************************************************************************/
typedef struct
{
    TL_OM_COMM_HEAD_STRU                stCommHead;

    VOS_UINT8                          ucCidNo;
    VOS_UINT8                          ucCMode;
    VOS_UINT8                          ucState;
    VOS_UINT8                          ucRsv;

    VOS_UINT8                          ucCTrans;
    VOS_UINT8                          ucHdrChainType;
    VOS_UINT8                          ucProfileVal;
    VOS_UINT8                          ucPktType;

    VOS_UINT8                          ucIPIDRndFlag;
    VOS_UINT8                          ucIPIDRnd2Flag;
    VOS_UINT8                          ucIPIDNboFlag;
    VOS_UINT8                          ucIPIDNbo2Flag;

    VOS_UINT8                           ucKforSn;
    VOS_UINT8                           ucKforIPID;
    VOS_UINT8                           ucKforIPID2;
    VOS_UINT8                           ucKforTS;

    VOS_UINT8                           ucRmodePktWithoutCRC;
    VOS_UINT8                           ucXM;                    /* 最低位表M，第四位表X */
    VOS_UINT8                           ucIrSntPktCount;  /*IR状态下连续发包的个数，初始化为0*/
    VOS_UINT8                           ucSntDynPktCount;  /*动态信息变化后连续发包的个数，初始化为0*/

    VOS_UINT16                          usIpIdOffset;
    VOS_UINT16                          usIpIdOffset2;
    VOS_UINT16                          usUdpSnValue;

    VOS_UINT32                          ulEspSnValue;
    VOS_UINT32                          ulLastOperTime;
    VOS_UINT32                          ulTimeStamp;
    VOS_UINT32                          ulTS_Stride;
    VOS_UINT32                          ulTS_Scaled;
    VOS_UINT32                          ulLastAckSn;

    VOS_UINT32                          ufCompressRatio;



}TL_OM_LTE_ROHC_COMPRESS_CONTEXT_INFO_STRU;


/*****************************************************************************
 结构名    : TL_OM_LTE_ROHC_DECOMPRESS_CONTEXT_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : LTE  ROHC 下行上下文信息上报结构体
*****************************************************************************/
typedef struct
{
    TL_OM_COMM_HEAD_STRU                stCommHead;

    VOS_UINT8                          ucCidNo;
    VOS_UINT8                          ucDMode;
    VOS_UINT8                          ucState;

    VOS_UINT8                          ucDf1Flag;
    VOS_UINT8                          ucDf2Flag;
    VOS_UINT8                          ucProfileVal;
    VOS_UINT8                          ucRtpM;

    VOS_UINT8                          ucPktType;
    VOS_UINT8                           ucFcToScPacketNum;    /* FC->SC最近发送的N1个包，个数代表后面窗口使用的位数，解压成功对应位置为1，失败置为0，先低位，后高位 */
    VOS_UINT8                           ucScToNcPacketNum;    /* SC->NC最近发送的N2个包 */
    VOS_UINT8                           ucFcToScErrPacketNum; /* FC->SC错包个数，如果最近N1个包中错包达到此值，则状态向下迁移 */

    VOS_UINT8                           ucScToNcErrPacketNum; /* SC->NC错包个数 */
    VOS_UINT8                           ucSendUToOPktCnt;     /* 用来发送U2O模式迁移的反馈包的计数器 */
    VOS_UINT8                           ucRtpPt;
    VOS_UINT8                           ucRtpV;

    VOS_UINT8                           ucRtpP;
    VOS_UINT8                           ucRtpX;
    VOS_UINT8                           ucRtpCC;
    VOS_UINT8                           ucIPIDRndFlag;

    VOS_UINT8                          ucIPIDRnd2Flag;
    VOS_UINT8                          ucIPIDNboFlag;
    VOS_UINT8                          ucIPIDNbo2Flag;
    VOS_UINT8                          ucRsv[2];

    VOS_UINT8                           ucUdpChksumFlg;
    VOS_UINT8                           ucDTrans;
    VOS_UINT16                          usSN;

    VOS_UINT32                          ufCompressRatio;

    VOS_UINT8                           ucCtxtDyn[40];


}TL_OM_LTE_ROHC_DECOMPRESS_CONTEXT_INFO_STRU;


/*****************************************************************************
 结构名    : TL_OM_LTE_ROHC_COMPRESS_PKT_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : LTE  ROHC 上行压缩报文头部内容上报
*****************************************************************************/
typedef struct
{
    TL_OM_COMM_HEAD_STRU                stCommHead;

    VOS_UINT8                          ucCidNo;
    VOS_UINT8                          ucLen;
    VOS_UINT8                          ucType; /*0-上行压缩后的压缩包;1-下行恢复前的压缩包;2-上行发送的反馈包;3-下行接收的反馈包*/
    VOS_UINT8                          ucRealLen;


    VOS_UINT8                          ucState;
    VOS_UINT8                          ucMode;
    VOS_UINT8                          ucProfile;
    VOS_UINT8                          ucRohcPktType;/*压缩报文包类型*/

    VOS_UINT8                          ucData[64];

}TL_OM_LTE_ROHC_CAPTURE_PKT_INFO_STRU;


/*****************************************************************************
 结构名    : TL_OM_TDS_PDCP_STAT_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : TDS PDCP统计信息上报结构体
*****************************************************************************/
typedef struct
{
    TL_OM_COMM_HEAD_STRU                stCommHead;

    VOS_UINT32                          ulPdcpUlRecvSduNum;         /*PDCP上行从RABM接收到的SDU数目*/
    VOS_UINT32                          ulPdcpUlDiscardSduNum;      /*PDCP上行丢弃的SDU数目*/
    VOS_UINT32                          ulPdcpUlSendPduToRlcNum;    /*PDCP上行发送给RLC 的PDU数目*/
    VOS_UINT32                          ulPdcpDlRecvPduNum;         /*PDCP下行从RLC接收到的PDU数目*/
    VOS_UINT32                          ulPdcpDlDiscardPduNum;      /*PDCP下行丢弃的PDU数目*/
    VOS_UINT32                          ulPdcpDlSendSduToRabmNum;   /*PDCP下行发送给RABM的SDU数目*/
}TL_OM_TDS_PDCP_STAT_INFO_STRU;

/*****************************************************************************
 结构名    : TL_OM_TDS_RLC_STAT_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : TDS RLC统计信息上报结构体
*****************************************************************************/
typedef struct
{
    TL_OM_COMM_HEAD_STRU                stCommHead;

    VOS_UINT32                          ulRlcRbSetupNum;            /*RLC实体建立数目*/
    VOS_UINT32                          ulRlcRbReleaseNum;          /*RLC实体释放数目*/
    VOS_UINT32                          ulRlcRbReCfgNum;            /*RLC实体重配数目*/
    VOS_UINT32                          ulRlcRbResetNum;            /*RLC实体Reset操作数目*/

    VOS_UINT32                          ulRlcUlRecvSduNum;          /*RLC上行从PDCP处接收到的SDU数目*/
    VOS_UINT32                          ulRlcUlDiscardSduNum;       /*RLC上行丢弃的PDCP SDU数目*/
    VOS_UINT32                          ulRlcUlSendPduToMacNum;     /*RLC上行发送给MAC的PDU数目*/
    VOS_UINT32                          ulRlcUlNewTxPduNum;         /*RLC上行发送新传PDU数目*/
    VOS_UINT32                          ulRlcUlReTxPduNum;          /*RLC上行发送重传PDU数目*/
    VOS_UINT32                          ulRlcUlReTxMaxCntNum;       /*RLC上行发送重传达到最大次数的数目*/
    VOS_UINT32                          ulRlcUlSendPollNum;         /*RLC上行发送Poll数目*/
    VOS_UINT32                          ulRlcUlSendResetNum;        /*RLC上行发送Reset数目*/
    VOS_UINT32                          ulRlcUlSendPiggybackPduNum;          /*RLC下行向网侧反馈ACK数目*/

    VOS_UINT32                          ulRlcDlRecvPduNum;          /*RLC下行从MAC收到的 PDU数目*/
    VOS_UINT32                          ulRlcDlDiscardPdu;          /*RLC下行丢弃的PDU数目*/
    VOS_UINT32                          ulRlcDlSendSduToPdcpNum;    /*RLC下行发送给PDCP的SDU数目*/
    VOS_UINT32                          ulRlcDlRecvPollNum;         /*RLC下行收到Poll数目*/

    VOS_UINT32                          ulRlcDlRcvResetPduNum;         /*RLC下行向网侧反馈NACK数目*/
    VOS_UINT32                          ulRlcDlRcvResetAckPduNum;         /*RLC下行收到Poll数目*/
    VOS_UINT32                          ulRlcDlRcvErrPduNum;        /*RLC下行收到Reset数目*/

    VOS_UINT32                          ulRlcDlRcvCtrlPduNum;        /*RLC下行收到Reset数目*/
}TL_OM_TDS_RLC_STAT_INFO_STRU;

#define TDS_MAC_LOSS_MSG_MAX_NUM 10
/*****************************************************************************
 结构名    : TL_OM_TDS_MAC_STAT_INFO_STRU
 ASN.1描述 :
 结构说明  : TDS MAC统计信息上报结构体
*****************************************************************************/
typedef struct
{
    TL_OM_COMM_HEAD_STRU                stCommHead;

    VOS_UINT32                          ulMacUlRecvSduNum;          /*MAC上行接收到的RLC PDU数目*/
    VOS_UINT32                          ulMacUlSendPduToPhyNum;     /*MAC上行发送给PHY的PDU数目*/
    VOS_UINT32                          ulMacDlRecvPduNum;          /*MAC下行从PHY接收到的PDU数目*/
    VOS_UINT32                          ulMacDlSendSduToRlcNum;     /*MAC下行发送给RLC 的PDU数目*/
    VOS_UINT32                          ulStatusIndNum;          /*MAC上报status ind给RRC的次数*/

    VOS_UINT32                          ulUlSendToDspMsgTotal;
    VOS_UINT32                          ulUlSendToDspMsgSuccess;
    VOS_UINT32                          ulUlSendToDspMsgFail;
    VOS_UINT32                          ulUlSendToDspImmediateMsgTotal;
    VOS_UINT32                          ulUlSendToDspImmediateMsgSuccess;
    VOS_UINT32                          ulUlSendToDspImmediateMsgFail;
    VOS_UINT32                          ulUlSendToDspImmediateMsgError;
    VOS_UINT32                          ulUlSendDpchTotal;
    VOS_UINT32                          ulUlSendPrachTotal;
    VOS_UINT32                          ulUlSendSi;
    VOS_UINT32                          ulUlRecvGrant;
    VOS_UINT32                          ulUlSendEpuchTotal;
    VOS_UINT32                          ulUlSendEpuchNewTx;
    VOS_UINT32                          ulUlSendEpuchReTx;
    VOS_UINT32                          ulDlRecvFpachTotal;
    VOS_UINT32                          ulDlRecvFpachCrcOk;
    VOS_UINT32                          ulDlRecvFpachCrcError;
    VOS_UINT32                          ulDlRecvSccpchTotal;
    VOS_UINT32                          ulDlRecvSccpchCrcOk;
    VOS_UINT32                          ulDlRecvSccpchCrcError;
    VOS_UINT32                          ulDlRecvPccpchTotal;
    VOS_UINT32                          ulDlRecvPccpchCrcOk;
    VOS_UINT32                          ulDlRecvPccpchCrcError;
    VOS_UINT32                          ulDlRecvDpchTotal;
    VOS_UINT32                          ulDlRecvDpchCrcOk;
    VOS_UINT32                          ulDlRecvDpchCrcError;
    VOS_UINT32                          ulDlRecvHspdschTotal;
    VOS_UINT32                          ulDlRecvHspdschCrcOk;
    VOS_UINT32                          ulDlRecvHspdschCrcError;
    VOS_UINT32                          ulDlRecvHiSichNum;

    VOS_UINT32                          ulSyncLoss;
    VOS_UINT32                          ulLossMsgID[TDS_MAC_LOSS_MSG_MAX_NUM];
    VOS_UINT32                          ulLossMsgIndex;

   VOS_UINT32                          ulAsSearchRfReqNum;
   VOS_UINT32                          ulAsSearchRfRelNum;
   VOS_UINT32                          ulAsSearchRfCnfNum;
   VOS_UINT32                          ulSndSigProIndNum;
   VOS_UINT32                          ulSndSigDeProIndNum;
   VOS_UINT32                          ulFchNoRfIndNum;
   VOS_UINT32                          ulFchHaveRfIndNum;
   VOS_UINT32                          ulDchNoRfIndNum;
   VOS_UINT32                          ulDchHaveRfIndNum;
}TL_OM_TDS_MAC_STAT_INFO_STRU;

/*****************************************************************************
 结构名    : TL_OM_TDS_L2_OVERVIEW_STAT_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : TDS L2概略统计信息上报结构体
*****************************************************************************/
typedef struct
{
    TL_OM_COMM_HEAD_STRU                stCommHead;

    /*More to add*/

}TL_OM_TDS_L2_OVERVIEW_STAT_INFO_STRU;

/*****************************************************************************
 结构名    : TL_OM_LTE_PDCP_STAT_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : LTE PDCP统计信息上报结构体
*****************************************************************************/
typedef struct
{
    TL_OM_COMM_HEAD_STRU                stCommHead;

    /*More to add*/
    /*APP*/
    VOS_UINT32                          ulAppRxIpPktCnt;
    VOS_UINT32                          ulAppRxIpPktLen;
    VOS_UINT32                          ulMemTransFailNum;
    VOS_UINT32                          ulEnAppReqQueSucc;
    VOS_UINT32                          ulEnAppReqQueFail;
    VOS_UINT32                          ulDeAppReqQueSucc;
    VOS_UINT32                          ulRelAppReqQueNum;

    /*UL*/
    VOS_UINT32                          ulULTotlRxDrbSduCnt;
    VOS_UINT32                          ulULTotlTxDrbPduCnt;
    VOS_UINT32                          ulULTotlRxSrbSduCnt;
    VOS_UINT32                          ulULTotlTxSrbPduCnt;
    VOS_UINT32                          usULTotlFreeSduCnt;
    VOS_UINT32                          ulULTotlRbidErrCnt;

    VOS_UINT32                          ulULTotlRxDrbSduLen;
    VOS_UINT32                          ulULTotlTxDrbPduLen;
    VOS_UINT32                          ulULTotlRxSrbSduLen;
    VOS_UINT32                          ulULTotlTxSrbPduLen;

    /*DL*/
    VOS_UINT32                          ulDLTotlRxDrbPduCnt;
    VOS_UINT32                          ulDLTotlTxDrbSduCnt;
    VOS_UINT32                          ulDLTotlRxSrbPduCnt;
    VOS_UINT32                          ulDLTotlTxSrbSduCnt;
    VOS_UINT32                          ulDLTotlRbidErrCnt;

    VOS_UINT32                          ulDLTotlRxDrbPduLen;
    VOS_UINT32                          ulDLTotlTxDrbSduLen;
    VOS_UINT32                          ulDLTotlRxSrbPduLen;
    VOS_UINT32                          ulDLTotlTxSrbSduLen;

    /*Secu Stat*/
    VOS_UINT32                          ulCh1ChkErrIntCnt;
    VOS_UINT32                          ulCh1CmpIntCnt;
    VOS_UINT32                          ulCh1ChkErrEventCnt;
    VOS_UINT32                          ulCh1CmpEventCnt;
    VOS_UINT32                          ulCh2OneCmpIntCnt;
    VOS_UINT32                          ulCh2CmpEventCnt;
    VOS_UINT32                          ulCh3OneCmpIntCnt;
    VOS_UINT32                          ulCh3CmpEventCnt;
    VOS_UINT32                          ulCh3BdFullCnt;

}TL_OM_LTE_PDCP_ALL_RB_STAT_INFO_STRU;

/*****************************************************************************
 结构名    : TL_OM_LTE_PDCP_UL_RB_STAT_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : LTE PDCP 上行RB统计信息上报结构体
*****************************************************************************/
typedef struct
{
    TL_OM_COMM_HEAD_STRU                stCommHead;

    /*新增*/
    VOS_UINT32                          ulRxSduNum;
    VOS_UINT32                          ulRxSduLen;
    VOS_UINT32                          ulDiscardSduNum;
    VOS_UINT32                          ulDiscardSduLEn;
    VOS_UINT32                          ulAckedSduNum;
    VOS_UINT32                          ulEnSduQueFailNum;
    VOS_UINT32                          ulPduFullDiscardNum;

    VOS_UINT32                          ulTxDataPduNum;
    VOS_UINT32                          ulTxDataPduLen;
    VOS_UINT32                          ulTxCtrlPduNum;
    VOS_UINT32                          ulTxCtrlPduLen;
    VOS_UINT32                          ulHoRetxPduNum;
    VOS_UINT32                          ulHoRetxPduLen;
    VOS_UINT32                          ulRxEnbCtrlPduNum;
    VOS_UINT32                          ulTxRohcFeedBackNum;

    VOS_UINT32                          ulRohcSduNum;
    VOS_UINT32                          ulRohcFailNum;
    VOS_UINT32                          ulRohcSduLen;
    VOS_UINT32                          ulRohcComredSduLen;

    /*More to add,根据新增情况进行删除*/
    VOS_UINT32                          ulUlRxSduCnt;
    VOS_UINT32                          ulUlTxPduCnt;
    VOS_UINT32                          ulUlAckedSduCnt;

    VOS_UINT32                          ulUlDiscardMaxCnt;
    VOS_UINT32                          ulUlDisSduCnt;
    VOS_UINT32                          ulUlBufFullDisSduCnt;
    VOS_UINT32                          ulUlReestDiscardCnt;

    VOS_UINT32                          ulUlInvalidSduCnt;
    VOS_UINT32                          ulUlTxCtrlPduNum;
    VOS_UINT32                          ulUlAckEndSnError;

    VOS_UINT32                          ulCompSuccCnt;
    VOS_UINT32                          ulCompFailCnt;
    VOS_UINT32                          ulRohcCompCnt;

    VOS_UINT32                          ulSduEnQueFailCnt;
    VOS_UINT32                          ulUlDisFirstSduCnt;
    VOS_UINT32                          ulHighSduEnQueCnt;
    VOS_UINT32                          ulLowSduEnQueCnt;

}TL_OM_LTE_PDCP_UL_RB_STAT_INFO_STRU;

/*****************************************************************************
 结构名    : TL_OM_LTE_PDCP_DL_RB_STAT_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : LTE PDCP下行RB统计信息上报结构体
*****************************************************************************/
typedef struct
{
    TL_OM_COMM_HEAD_STRU                stCommHead;

    /*新增*/
    VOS_UINT32                          ulRxDataPduNum;
    VOS_UINT32                          ulRxDataPduLen;
    VOS_UINT32                          ulRxStatusPduNum;
    VOS_UINT32                          ulRxRohcCtrlPduNum;

    VOS_UINT32                          ulTxSduNum;
    VOS_UINT32                          ulTxSduLen;
    VOS_UINT32                          ulTxCtrlPduNum;
    VOS_UINT32                          ulTxCtrlPduLen;

    VOS_UINT32                          ulPduDupNum;
    VOS_UINT32                          ulPduOutOfWinNum;
    VOS_UINT32                          ulPduInvalidNum;
    VOS_UINT32                          ulPduExceptFreeNum;
    VOS_UINT32                          ulPduDeCipherErr;
    VOS_UINT32                          ulRohcFailNum;


    /*More to add,根据新增情况进行删除*/
    VOS_UINT32                          ulDlRxPduCnt;
    VOS_UINT32                          ulDlRxCtlPduCnt;
    VOS_UINT32                          ulDlTxStrpPduCnt;
    VOS_UINT32                          ulDlTxRohcPduCnt;
    VOS_UINT32                          ulDlErrPduRlcIndCnt;

    VOS_UINT32                          ulDlExecReordCnt;
    VOS_UINT32                          ulDlCipherBufFullCnt;
    VOS_UINT32                          ulDlAmRlcDataErrCnt;
    VOS_UINT32                          ulDlRlcDataErrCnt;

    VOS_UINT32                          ulDlUnCompFailCnt;
    VOS_UINT32                          ulHoUnCompFailCnt;
    VOS_UINT32                          ulDeCompSuccCnt;
    VOS_UINT32                          ulDeCompFailCnt;
    VOS_UINT32                          ulRohcDeCompCnt;
    VOS_UINT32                          ulDlRecvFeedbackCnt;
    VOS_UINT32                          ulDlRecvFeedbackSuccCnt;
    VOS_UINT32                          ulDecompFdbkUnkown;

}TL_OM_LTE_PDCP_DL_RB_STAT_INFO_STRU;

/*****************************************************************************
 结构名    : TL_OM_LPDCP_CIPHER_CH_STAT_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : LTE PDCP Cipher通道统计信息
*****************************************************************************/
typedef struct
{
    TL_OM_COMM_HEAD_STRU                stCommHead;

    VOS_UINT32                          ulRxSduCnt;
    VOS_UINT32                          ulRxLenErrSdu;

    /*直通通道*/
    VOS_UINT32                          ulThrChRxSduCnt;
    VOS_UINT32                          ulThrChDirDiscard;
    VOS_UINT32                          ulThrChGetDeftEpsFail;
    VOS_UINT32                          ulThrChEnQueFail;
    VOS_UINT32                          ulThrChEnQueSucc;
    VOS_UINT32                          ulThrChHaveNoBDCnt;

    VOS_UINT32                          ulThrChCfgBdFailCnt;
    VOS_UINT32                          ulThrChCfgBdSuccCnt;
    VOS_UINT32                          ulThrChCfgBdNum;
    VOS_UINT32                          ulThrChFreeBdNum;

    VOS_UINT32                          ulThrChSrcMemSaveSucc;
    VOS_UINT32                          ulThrChSrcMemSaveFail;
    VOS_UINT32                          ulThrChSrcMemFreeCnt;
    VOS_UINT32                          ulThrChFreeDeCipherSdu;

    /*非直通通道*/
    VOS_UINT32                          ulNonThrChRxSduCnt;
    VOS_UINT32                          ulNonThrChEnQueFail;
    VOS_UINT32                          ulNonThrChEnQueSucc;

    VOS_UINT32                          ulNonThrChHaveNOBdCnt;
    VOS_UINT32                          ulNonThrChCfgBdFailCnt;
    VOS_UINT32                          ulNonThrChCfgBdSuccCnt;
    VOS_UINT32                          ulNonThrChCfgBdNum;
    VOS_UINT32                          ulNonThrChFreeBdNum;

    VOS_UINT32                          ulNonThrChSrcMemSaveSucc;
    VOS_UINT32                          ulNonThrChSrcMemSaveFail;
    VOS_UINT32                          ulNonThrChSrcMemFreeCnt;
    VOS_UINT32                          ulNonThrChFreeDeCipherSdu;

}TL_OM_LPDCP_CIPHER_CH_STAT_INFO_STRU;


/*****************************************************************************
 结构名    : TL_OM_LTE_RLC_UL_RB_STAT_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : LTE RLC上行统计信息上报结构体
*****************************************************************************/
typedef struct
{
    TL_OM_COMM_HEAD_STRU    stCommHead;

    /*新增*/
    VOS_UINT32              ulRxSduNum;
    VOS_UINT32              ulRxSduLen;
    VOS_UINT32              ulDiscardSduNum;
    VOS_UINT32              ulDiscardSduLen;
    VOS_UINT32              ulTxSduSuccNum;
    VOS_UINT32              ulTxSduFailNum;

    VOS_UINT32              ulLMacNtfSchedNum;
    VOS_UINT32              ulTxNullPduNum;
    VOS_UINT32              ulRlcNoSduNum;
    VOS_UINT32              ulTxRlcPduNum;
    VOS_UINT32              ulTxRlcPduLen;

    VOS_UINT32              ulTxNewDataPduNum;
    VOS_UINT32              ulTxNewDataPduLen;
    VOS_UINT32              ulTxCtrlPduNum;
    VOS_UINT32              ulTxCtrlPduLen;
    VOS_UINT32              ulRetxPduNum;
    VOS_UINT32              ulRetxPduLen;
    VOS_UINT32              ulRetxSegNum;
    VOS_UINT32              ulTxPollPduNum;
    VOS_UINT32              ulPollRetxTMrExpiry;

    VOS_UINT32              ulEnbCtrlPduNum;                /*收到ENB状态报告个数*/
    VOS_UINT32              ulInvalidEnbCtrlPduNum;         /*无效状态报告个数*/
    VOS_UINT32              ulNackIndPduNum;                /*NACK指示重传PDU个数*/
    VOS_UINT32              ulNackIndSegNum;                /*NACK指示重传Segment PDU个数*/


    /*More to add,根据新增内容进行删除*/
    VOS_UINT32              ulRlcDropSdu;
    VOS_UINT32              ulQueFullDropSdu;
    VOS_UINT32              ulDropSduNoRsn;
    VOS_UINT32              ulResNotEnough;
}TL_OM_LTE_RLC_UL_RB_STAT_INFO_STRU;


/*****************************************************************************
 结构名    : TL_OM_LTE_RLC_DL_RB_STAT_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : LTE RLC 下行统计信息上报结构体
*****************************************************************************/
typedef struct
{
    TL_OM_COMM_HEAD_STRU    stCommHead;

    /*新增*/
    VOS_UINT32              ulRxPduNum;
    VOS_UINT32              ulRxPduLen;
    VOS_UINT32              ulRxDataPduNum;
    VOS_UINT32              ulRxDataPduLen;
    VOS_UINT32              ulRxCtrlPduNum;
    VOS_UINT32              ulRxCtrlPduLen;
    VOS_UINT32              ulMissedUmPduNum;
    VOS_UINT32              ulInvalidPduNum;
    VOS_UINT32              ulDiscardPduNum;
    VOS_UINT32              ulDupPduNum;
    VOS_UINT32              ulProcPduFail;

    VOS_UINT32              ulTxSduNum;
    VOS_UINT32              ulTxSduLen;
    VOS_UINT32              ulTxDlCtrlPduNum;
    VOS_UINT32              ulReorderTmrStartNum;
    VOS_UINT32              ulReorderTmrExpiryNum;

    VOS_UINT32              ulDiscardHeadSgmnt;
    VOS_UINT32              ulDiscardTailSgmnt;
    VOS_UINT32              ulResmPduFail;

    /*More to add,根据新增内容进行删除*/
    VOS_UINT32              ulReRecvSnDrop;
    VOS_UINT32              ulRecePduSeg;

}TL_OM_LTE_RLC_DL_RB_STAT_INFO_STRU;

/*****************************************************************************
 结构名    : TL_OM_LTE_MAC_RA_STAT_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : LTE MAC 随机接入统计信息上报结构体
*****************************************************************************/
typedef struct
{

    TL_OM_COMM_HEAD_STRU                      stCommHead;

    /*More to add*/
    VOS_UINT32                                ulMacRaReq;
    VOS_UINT32                                ulPdcchNonContentRaReq;
    VOS_UINT32                                ulPdcchContentRaReq;
    VOS_UINT32                                ulRrcNonContentRaReq;
    VOS_UINT32                                ulRrcContentRaReq;
    VOS_UINT32                                ulSelectGroupA;
    VOS_UINT32                                ulSelectGroupB;

    VOS_UINT32                                ulRecvPhyRaCnf;
    VOS_UINT32                                ulRepeatRaReq;
    VOS_UINT32                                ulMaxRaReq;

    VOS_UINT32                                ulRarTimerOut;
    VOS_UINT32                                ulContentTimerOut;
    VOS_UINT32                                ulRaCnfTimerOut;

    VOS_UINT32                                ulRaCauseFail;
    VOS_UINT32                                ulSelectGroupABFail;

    VOS_UINT32                                ulRecvRarNum;
    VOS_UINT32                                ulRecvRarSucc;
    VOS_UINT32                                ulRecvRarFail;
    VOS_UINT32                                ulRarBiNum;
    VOS_UINT32                                ulRarProcSucc;
    VOS_UINT32                                ulNonContentSucc;

    VOS_UINT32                                ulRecvContentionNum;
    VOS_UINT32                                ulWaitContentionProcFail;

    VOS_UINT32                                ulContentTcrntiSucc;
    VOS_UINT32                                ulContentCrntiSucc;
    VOS_UINT32                                ulContentNoMatch;

    VOS_UINT32                                ulSchMsg3Num;
    VOS_UINT32                                ulSchMsg3ReTransNum;
    VOS_UINT32                                ulGetMsg3FromRlcFail;

    VOS_UINT32                                ulMacNewMsg3Rrctrig;
    VOS_UINT32                                ulMacNewMsg3Mactrig;
    VOS_UINT32                                ulMacNewMsg3MacPdu;
    VOS_UINT32                                ulHoMsg3NoRecvCmp;
    VOS_UINT32                                ulMacRetranMsg3Num;
    VOS_UINT32                                ulMacActiveRetranMsg3Num;
    VOS_UINT32                                ulMacOverMsg3MaxRetran;

    VOS_UINT32                                ulStateErrorDrop;
    VOS_UINT32                                ulSendRlcSetupOkNum;
    VOS_UINT32                                ulRarSchedNotFinishNum;
    VOS_UINT32                                ulNoRarCfgCnfNum;

} TL_OM_LTE_MAC_RA_STAT_INFO_STRU;

/*****************************************************************************
 结构名    : TL_OM_LTE_MAC_UL_STAT_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : LTE MAC 上行统计信息上报结构体
*****************************************************************************/
typedef struct
{
    TL_OM_COMM_HEAD_STRU                      stCommHead;
    /*新增*/
    VOS_UINT32                                ulRxSduNum;
    VOS_UINT32                                ulRxSduLen;
    VOS_UINT32                                ulRxSchFrmInd;
    VOS_UINT32                                ulRxUlGrantNum;
    VOS_UINT32                                ulUlGrantNotSchNum;

    VOS_UINT32                                ulSchedPduNum;
    VOS_UINT32                                ulSchedPduLen;
    VOS_UINT32                                ulSchNotFinishNum;
    VOS_UINT32                                ulStartAccNum;

    VOS_UINT32                                ulTxPreambleNum;
    VOS_UINT32                                ulTxCRntiNum;
    VOS_UINT32                                ulTxCcchNum;
    VOS_UINT32                                ulTxSrNum;
    VOS_UINT32                                ulPhyCancelSrNum;
    VOS_UINT32                                ulMacCancelSrNum;
    VOS_UINT32                                ulTxPhrNum;
    VOS_UINT32                                ulTxShortBsrNum;
    VOS_UINT32                                ulTxLongBsrNum;
    VOS_UINT32                                ulTxTruncatedBsrNum;
    VOS_UINT32                                ulTxPadShortBsrNum;
    VOS_UINT32                                ulTxPadLongBsrNum;
    VOS_UINT32                                ulCancelBsrNum;
    VOS_UINT32                                ulTxPaddingNum;

    VOS_UINT32                                ulTaTmrExpiryNum;
    VOS_UINT32                                ulScellDeactTmrExpiryNum;
    VOS_UINT32                                ulMacRelUlSpsNum;
    VOS_UINT32                                ulPhyRelUlSpsNum;
    VOS_UINT32                                ulForbidDspSleepErrNum;
    VOS_UINT32                                ulWriteDeiMailErrNum;

    /*More to add,根据情况进行删除*/
    VOS_UINT32                                ulGetRlcNuLLBuffNum;
    VOS_UINT32                                ulGetCcchInActiveNum;
    VOS_UINT32                                ulGetLchPriFromRlcFail;
    VOS_UINT32                                ulSendAbnormBsr;

    /*上行ACC统计量*/
    VOS_UINT32                                  ulAccEnableNoIdleCh;            /*使能ACC时刻发现通道不空闲*/
    VOS_UINT32                                  ulAccEnableSucc;                /*使能ACC成功统计*/
    VOS_UINT32                                  ulAccEnableFail;                /*使能ACC失败统计*/
    VOS_UINT32                                  ulAccEnableMissTime;            /*错过发送时机的统计*/
    VOS_UINT32                                  ulAccNotEnable;                 /*ACC没有使能统计*/
    VOS_UINT32                                  ulAccNotFinish;                 /*ACC没有搬移完成统计*/

    VOS_UINT32                                  ulAccCfgGetBdQFail;             /*ACC配置获取BDQ失败次数*/
    VOS_UINT32                                  ulAccCfgCipherNodeSucc;         /*ACC配置Cipher节点成功次数*/
    VOS_UINT32                                  ulAccCfgCipherNodeFail;         /*ACC配置Cipher节点失败次数*/
    VOS_UINT32                                  ulAccCfgDmaNodeSucc;            /*ACC配置DMA节点成功次数*/
    VOS_UINT32                                  ulAccCfgDmaNodeFail;            /*ACC配置DMA节点失败次数*/
    VOS_UINT32                                  ulAccCfgDataTypeErr;            /*ACC配置类型错误统计*/
    VOS_UINT32                                  ulAccCfgOffsetErr;              /*ACC配置偏移错误*/
    VOS_UINT32                                  ulAccCfgOverMaxNode;            /*ACC超过最大搬移的节点数统计*/
    VOS_UINT32                                  ulBbpMacPduValidFlgNotClr;      /*BBP MAC PDU有效标志未及时清除次数*/

    /*L2投票状态记录，最后一次投票的状态*/
    VOS_UINT32                                  ulLHpaState;
    VOS_UINT32                                  ulTotalBuffSize;
    VOS_UINT32                                  ulAmBuffSize;
    VOS_UINT32                                  ulLRlcUlSleepFlg;
    VOS_UINT32                                  ulLRlcDlSleepFlg;
    VOS_UINT32                                  ulLPsSleepFlg;

}TL_OM_LTE_MAC_UL_STAT_INFO_STRU;

/*****************************************************************************
 结构名    : TL_OM_LTE_MAC_DL_STAT_INFO_STRUE
 协议表格  :
 ASN.1描述 :
 结构说明  : LTE MAC 下行统计信息上报结构体
*****************************************************************************/
typedef struct
{
    TL_OM_COMM_HEAD_STRU                      stCommHead;

    /*新增*/
    VOS_UINT32                                ulRxDlTbIntNum;
    VOS_UINT32                                ulAllocDlTbNum;
    VOS_UINT32                                ulGetDlTbNum;

    VOS_UINT32                                ulRxBcchTbNum;
    VOS_UINT32                                ulRxPbchTbNum;
    VOS_UINT32                                ulRxPchTbNum;
    VOS_UINT32                                ulRxPduNum;
    VOS_UINT32                                ulRxPduLen;
    VOS_UINT32                                ulPduDemutiSuccNum;
    VOS_UINT32                                ulPduDemutiFailNum;
    VOS_UINT32                                ulRxTaMceNum;
    VOS_UINT32                                ulRxScellMceNum;
    VOS_UINT32                                ulRxDrxMceNum;
    VOS_UINT32                                ulRxPaddingNum;
    VOS_UINT32                                ulTxSduToRlcNum;
    VOS_UINT32                                ulTxSduToRlcLen;

    /*More to add，根据情况进行删除*/

    VOS_UINT32                                ulDlTbRecvIntErr;                 /*V7R2*/
    VOS_UINT32                                ulDlForbidDspSleepFail;           /*V7R2*/

    VOS_UINT32                                ulActSendRlcErrorNum;
    VOS_UINT32                                ulActDemulOneLchErr;
    VOS_UINT32                                ulActRecvErrPkt;


    VOS_UINT32                                ulDlTbCheckErr;

    VOS_UINT32                                ulRadnomStateRecvTb;
    VOS_UINT32                                ulRadnomStateRecvTbErr;

    VOS_UINT32                                ulMacPdcchRecvTb;

    VOS_UINT32                                ulDlTbIntAllocMemFail;
    VOS_UINT32                                ulDlLteMasterGetMemFail;
    VOS_UINT32                                ulDlLteSlaveGetMemFail;
    VOS_UINT32                                ulDlTbRecvQueFull;
    VOS_UINT32                                ulDlTbAllocMemNum;
    VOS_UINT32                                ulDlTbMemMacFreeNum;
    VOS_UINT32                                ulDlMacGetDlTbNum;
    VOS_UINT32                                ulDlTbAddrNull;
    VOS_UINT32                                ulDlTbMemCheckErr;
    VOS_UINT32                                ulDlTbCtrlInfoErr;
    VOS_UINT32                                ulDlRecvTbSegErr;
    VOS_UINT32                                ulDlRecvTbSegOk;

}TL_OM_LTE_MAC_DL_STAT_INFO_STRU;

#define TL_OM_LMAC_CA_MAX_RPT_CELL_NUM      (8)
/*****************************************************************************
 结构名    : TL_OM_LTE_MAC_CA_STAT_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : LTE MAC CA 统计信息上报结构体.Index =0 为主小区.
*****************************************************************************/
typedef struct
{
    TL_OM_COMM_HEAD_STRU                      stCommHead;

    /*More to add*/
    LSCELL_STATUS_ENUM_UINT8                  ucScellStatusType[TL_OM_LMAC_CA_MAX_RPT_CELL_NUM];
    LSCELL_DEACT_TIMER_VIEW_ENUM_UINT32       ulScellDeactTimer[TL_OM_LMAC_CA_MAX_RPT_CELL_NUM];
    VOS_UINT32                                ulConfigedNum[TL_OM_LMAC_CA_MAX_RPT_CELL_NUM];           /*配置对应SCell个数*/
    VOS_UINT32                                ulDeletedNum[TL_OM_LMAC_CA_MAX_RPT_CELL_NUM];            /*删除对应SCell个数*/
    VOS_UINT32                                ulRecvActiveMceNum[TL_OM_LMAC_CA_MAX_RPT_CELL_NUM];      /*接收到对应SCell激活控制元个数*/
    VOS_UINT32                                ulSendActiveToPhyNum[TL_OM_LMAC_CA_MAX_RPT_CELL_NUM];    /*发送对应SCell的激活命令给DSP的个数*/
    VOS_UINT32                                ulRecvDeactiveMceNum[TL_OM_LMAC_CA_MAX_RPT_CELL_NUM];    /*接收到对应SCell去激活控制元个数*/
    VOS_UINT32                                ulSendDeactiveToPhyNum[TL_OM_LMAC_CA_MAX_RPT_CELL_NUM];  /*发送对应SCell的去激活命令给DSP的个数*/
    VOS_UINT32                                ulRecvDlAssignmentNum[TL_OM_LMAC_CA_MAX_RPT_CELL_NUM];   /*接收到对应SCell的下行指派的个数*/
    VOS_UINT32                                ulDeactiveTimerOutNum[TL_OM_LMAC_CA_MAX_RPT_CELL_NUM];   /*对应SCell去激活定时器超时的次数*/
    VOS_UINT32                                ulRecvDlTbNum[TL_OM_LMAC_CA_MAX_RPT_CELL_NUM];           /*对应SCell接收到下行TB块的个数*/
    VOS_UINT32                                ulRecvDlTbByte[TL_OM_LMAC_CA_MAX_RPT_CELL_NUM];          /*对应SCell接收到下行TB字节数*/
    LMAC_SCELL_CFG_STATUS_ENUM_UINT8          ucUlCaCfgStatus[TL_OM_LMAC_CA_MAX_RPT_CELL_NUM];         /*配置对应上行CA的状态*/
    LMAC_SCELL_CFG_STATUS_ENUM_UINT8          ucDlCaCfgStatus[TL_OM_LMAC_CA_MAX_RPT_CELL_NUM];         /*配置对应下行CA的状态*/

}TL_OM_LTE_MAC_CA_STAT_INFO_STRU;

/*****************************************************************************
 结构名    : TL_OM_LTE_MAC_MBMS_STAT_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : LTE MAC MBMS 统计信息上报结构体
*****************************************************************************/
typedef struct
{
    TL_OM_COMM_HEAD_STRU                      stCommHead;

    /*More to add*/
    VOS_UINT32                                ulMchRecvSucc;                    /*MCH解析成功个数*/
    VOS_UINT32                                ulMchRecvFail;                    /*MCH解析失败个数*/
    VOS_UINT32                                ulMchRecvPadding;
    VOS_UINT32                                ulMchMsiNum;
    VOS_UINT32                                ulPmchInValidCnt;
    VOS_UINT32                                ulMcchRecvCnt;
    VOS_UINT32                                ulMtchRecvCnt;

}TL_OM_LTE_MAC_MBMS_STAT_INFO_STRU;

/*****************************************************************************
 结构名    : TL_OM_LTE_L2_OVERVIEW_STAT_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : LTE L2概略统计信息上报结构体
*****************************************************************************/
typedef struct
{
    TL_OM_COMM_HEAD_STRU                stCommHead;

    /*More to add*/

}TL_OM_LTE_L2_OVERVIEW_STAT_INFO_STRU;


/*需要总和考虑性能和LOG量*/
#define TL_OM_LTE_MAX_RPT_NUM                   (200)
#define TL_OM_LTE_IP_RPT_MAX_NUM                (200)
#define TL_OM_LTE_UL_PDU_RPT_MAX_NUM            (100)
#define TL_OM_LTE_DL_PDU_RPT_MAX_NUM            (160)
#define TL_OM_LTE_PDCP_DL_SDU_RPT_MAX_NUM       (140)
#define TL_OM_LTE_RLC_AM_CTRL_PDU_RPT_MAX_NUM   (50)
#define TL_OM_LTE_RLC_MAX_NACK_RPT_NUM          (4)
#define TL_OM_LTE_MAC_UL_PDU_DATA_LEN           (16)
#define TL_OM_LTE_MAC_DL_PDU_DATA_LEN           (8)

/*最大逻辑信道个数*/
#define TL_OM_LTE_MAC_MAX_LCH_NUM               (11)
/*上报BUFFER状态的个数*/
#define TL_OM_LTE_MAC_BUF_STATUS_RPT_NUM        (10)

/*****************************************************************************
 枚举名    : TL_OM_LTE_MAC_PDU_TYPE_ENUM
 协议表格  :
 ASN.1描述 :
 枚举说明  : LTE MAC PDU类型
*****************************************************************************/
typedef enum
{
    TL_OM_LTE_MAC_PDU_TYPE_DATA                     = 0x01,
    TL_OM_LTE_MAC_PDU_TYPE_SR_TX                    = 0x02,
    TL_OM_LTE_MAC_PDU_TYPE_PREAMBLE                 = 0x03,
    TL_OM_LTE_MAC_PDU_TYPE_SR_REQ                   = 0x04,
    TL_OM_LTE_MAC_PDU_TYPE_MAC_CANCEL_SR            = 0x05,
    TL_OM_LTE_MAC_PDU_TYPE_PHY_CANCEL_SR            = 0x06,
    TL_OM_LTE_MAC_PDU_TYPE_SCHED_FAIL               = 0x07,/*调度失败*/
    TL_OM_LTE_MAC_PDU_TYPE_MSG3_RETX                = 0x08,
    TL_OM_LTE_MAC_PDU_TYPE_LMAC_SPS_REL             = 0x09,
    TL_OM_LTE_MAC_PDU_TYPE_PDCCH_ULSPS_REL          = 0x0A,
    TL_OM_LTE_MAC_PDU_TYPE_RETX_BSR_TMROUT          = 0x0B,
    TL_OM_LTE_MAC_PDU_TYPE_PERI_BSR_TMROUT          = 0x0C,
    TL_OM_LTE_MAC_PDU_TYPE_PERI_PHR_TMROUT          = 0x0D,
    TL_OM_LTE_MAC_PDU_TYPE_SCELL_DEACTTIMER_TMROUT  = 0x0E,
    TL_OM_LTE_MAC_PDU_TYPE_TAG_TMROUT               = 0x0F,
    TL_OM_LTE_MAC_PDU_TYPE_BUTT
} TL_OM_LTE_MAC_PDU_TYPE_ENUM;

/*****************************************************************************
 枚举名    : TL_OM_LTE_PDCP_PDU_TYPE_ENUM
 协议表格  :
 ASN.1描述 :
 枚举说明  : LTE PDCP PDU类型
*****************************************************************************/
typedef enum
{
    TL_OM_LTE_PDCP_PDU_TYPE_DATA                    = 0x00,
    TL_OM_LTE_PDCP_PDU_TYPE_CTRL                    = 0x01,
    TL_OM_LTE_PDCP_PDU_TYPE_PDU_DISCARD             = 0x02,
    TL_OM_LTE_PDCP_PDU_TYPE_BUTT
} TL_OM_LTE_PDCP_PDU_TYPE_ENUM;

/*****************************************************************************
 枚举名    : TL_OM_LTE_RLC_TIMER_TYPE_ENUM
 协议表格  :
 ASN.1描述 :
 枚举说明  : LTE RLC PDU类型
*****************************************************************************/
typedef enum
{
    TL_OM_LTE_RLC_UM_PDU_TYPE_DISCARD            = 0x01,
    TL_OM_LTE_RLC_PDU_TYPE_BUTT
} TL_OM_LTE_RLC_PDU_TYPE_ENUM;

/*****************************************************************************
 枚举名    : TL_OM_LTE_RLC_TIMER_TYPE_ENUM
 协议表格  :
 ASN.1描述 :
 枚举说明  : LTE RLC TIMER相关定时器类型
*****************************************************************************/
typedef enum
{
    TL_OM_LTE_RLC_TIMER_TYPE_DL_REORDER             = 0x00,
    TL_OM_LTE_RLC_TIMER_TYPE_AM_DL_STATUS_PROHIBIT  = 0x01,
    TL_OM_LTE_RLC_TIMER_TYPE_BUTT
} TL_OM_LTE_RLC_TIMER_TYPE_ENUM;


/*****************************************************************************
 结构名    : TL_OM_IP_PKT_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 保存IP关键信息
*****************************************************************************/
typedef struct
{
    VOS_UINT8                   ucIpVer;
    VOS_UINT8                   ucProtocol;
    VOS_UINT16                  usIpId;

    VOS_UINT32                  ulTcpSn;
    VOS_UINT32                  ulTcpAckSn;
} TL_OM_IP_PKT_INFO_STRU;

/*****************************************************************************
 结构名    : TL_OM_CDS_IP_PKT_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 保存CDS IP关键信息
*****************************************************************************/
typedef struct
{
    VOS_UINT32                  ulTimeStamp;
    VOS_UINT8                   ucDir;/*0-上行,1-下行*/
    VOS_UINT8                   ucType;
    VOS_UINT8                   ucRabId;
    VOS_UINT8                   ucModemId;
    TL_OM_IP_PKT_INFO_STRU      stIpPktInfo;
} TL_OM_CDS_IP_PKT_INFO_STRU;

/*****************************************************************************
 结构名    : TL_OM_CDS_IP_PKT_INFO_ARRAY_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : CDS IP信息数组
*****************************************************************************/
typedef struct
{
    VOS_UINT32                      ulRptNum;
    TL_OM_CDS_IP_PKT_INFO_STRU      astCdsIpPktInfo[TL_OM_LTE_IP_RPT_MAX_NUM];
} TL_OM_CDS_IP_PKT_INFO_ARRAY_STRU;

/*****************************************************************************
 结构名    : TL_OM_LPDCP_UL_PDU_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 保存LPDCP UL PDU&IP关键信息
*****************************************************************************/
typedef struct
{
    VOS_UINT16                usFrame       :10;
    VOS_UINT16                usSubFrame    :6;
    VOS_UINT16                usPdcpSn;

    VOS_UINT8                 ucRbId;
    VOS_UINT8                 ucRlcMode     :2;
    VOS_UINT8                 ucPduType     :2;
    VOS_UINT8                 ucSnBitNum    :4;
    VOS_UINT16                usStampH16Bit;

    VOS_UINT32                ulStampL32Bit;

    VOS_UINT16                usNextAckSn;
    VOS_UINT16                usPduLen;

    VOS_UINT32                ulCount;
    VOS_UINT32                ulBuffTime;   /*在PDCP缓存的时间*/
    TL_OM_IP_PKT_INFO_STRU    stIpPktInfo;
} TL_OM_LPDCP_UL_PDU_INFO_STRU;

/*****************************************************************************
 结构名    : TL_OM_LPDCP_UL_PDU_INFO_ARRAY_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : LPDCP UL PDU信息缓存数组
*****************************************************************************/
typedef struct
{
    VOS_UINT32                      ulRptNum;
    TL_OM_LPDCP_UL_PDU_INFO_STRU    astLPdcpUlPduInfo[TL_OM_LTE_UL_PDU_RPT_MAX_NUM];
} TL_OM_LPDCP_UL_PDU_INFO_ARRAY_STRU;

/*****************************************************************************
 结构名    : TL_OM_LPDCP_DL_PDU_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 保存LPDCP DL PDU&LRLC DL PDU SN关键信息
*****************************************************************************/
typedef struct
{
    VOS_UINT32                  bitSysFrame     :10;
    VOS_UINT32                  bitSubFrame     :4;
    VOS_UINT32                  bitRbId         :8;
    VOS_UINT32                  bitRlcMode      :4;
    VOS_UINT32                  bitSnBitNum     :4;
    VOS_UINT32                  ulPduType       :2;

    VOS_UINT16                  usPdcpSn;
    VOS_UINT16                  usStampH16Bit;

    VOS_UINT32                  ulStampL32Bit;

    VOS_UINT16                  usPduLen;
    VOS_UINT16                  usRlcSn;

    VOS_UINT16                  usNextRxSn;
    VOS_UINT16                  usLastSubmitSn;

    VOS_UINT32                  ulRxHfn;
} TL_OM_LPDCP_DL_PDU_INFO_STRU;

/*****************************************************************************
 结构名    : TL_OM_LPDCP_DL_PDU_INFO_ARRAY_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : LPDCP DL PDU上报结构体
*****************************************************************************/
typedef struct
{
    VOS_UINT32                      ulRptNum;
    TL_OM_LPDCP_DL_PDU_INFO_STRU    astLPdcpDlPduInfo[TL_OM_LTE_DL_PDU_RPT_MAX_NUM];
} TL_OM_LPDCP_DL_PDU_INFO_ARRAY_STRU;

/*****************************************************************************
 结构名    : TL_OM_LPDCP_DL_SDU_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 保存LPDCP DL SDU&IP关键信息
*****************************************************************************/
typedef struct
{
    VOS_UINT32                  bitSysFrame     :10;
    VOS_UINT32                  bitSubFrame     :4;
    VOS_UINT32                  bitRbId         :8;
    VOS_UINT32                  bitDiscardFlg   :1;
    VOS_UINT32                  bitResv         :9;

    VOS_UINT32                  ulStampL32Bit;
    VOS_UINT16                  usStampH16Bit;
    VOS_UINT16                  usPdcpSn;

    VOS_UINT16                  usSduLen;
    VOS_UINT16                  usRlcSn;
    VOS_UINT32                  ulCount;

    TL_OM_IP_PKT_INFO_STRU      stIpPktInfo;
} TL_OM_LPDCP_DL_SDU_INFO_STRU;

/*****************************************************************************
 结构名    : TL_OM_LPDCP_DL_SDU_INFO_ARRAY_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : LPDCP DL SDU信息数组
*****************************************************************************/
typedef struct
{
    VOS_UINT32                      ulRptNum;
    TL_OM_LPDCP_DL_SDU_INFO_STRU    astLPdcpDlSduInfo[TL_OM_LTE_PDCP_DL_SDU_RPT_MAX_NUM];
} TL_OM_LPDCP_DL_SDU_INFO_ARRAY_STRU;

/*****************************************************************************
 结构名    : TL_OM_LRLC_UL_PDU_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 保存LRLC UL PDU&LPDCP UL PDU SN关键信息
*****************************************************************************/
typedef struct
{
    VOS_UINT16                  usProcFrame     :10;/*处理时刻*/
    VOS_UINT16                  usProcSubFrame  :6;
    VOS_UINT16                  usAirFrame      :10;/*空口发送时刻*/
    VOS_UINT16                  usAirSubFrame   :6;

    VOS_UINT16                  usRlcSn;
    VOS_UINT8                   ucRbId;
    VOS_UINT8                   ucRetxNum;

    VOS_UINT16                  usPduLen;
    VOS_UINT16                  usPduType       :1;
    VOS_UINT16                  usRF            :1;
    VOS_UINT16                  usPolling       :1;
    VOS_UINT16                  usFi            :2;
    VOS_UINT16                  usAckSn         :10;
    VOS_UINT16                  usRptType       :1;

    VOS_UINT16                  usEndPdcpSn;
    VOS_UINT16                  usVTs_VTus;

    VOS_UINT16                  usVTa;
    VOS_UINT16                  usVTms;
} TL_OM_LRLC_UL_PDU_INFO_STRU;

/*****************************************************************************
 结构名    : TL_OM_LRLC_UL_PDU_INFO_ARRAY_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : LRLC UL PDU上报结构体
*****************************************************************************/
typedef struct
{
    VOS_UINT32                      ulRptNum;
    TL_OM_LRLC_UL_PDU_INFO_STRU     astLRlcUlPduInfo[TL_OM_LTE_DL_PDU_RPT_MAX_NUM];
} TL_OM_LRLC_UL_PDU_INFO_ARRAY_STRU;

/*****************************************************************************
 结构名    : TL_OM_LRLC_DL_PDU_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 保存LRLC DL PDU关键信息
*****************************************************************************/
typedef struct
{
    VOS_UINT16                usProcFrame       :10; /*处理时刻*/
    VOS_UINT16                usProcSubFrame    :6;
    VOS_UINT16                usAirFrame        :10; /*空口时刻*/
    VOS_UINT16                usAirSubFrame     :6;

    VOS_UINT16                usRlcSn;
    VOS_UINT8                 ucRbId;
    VOS_UINT8                 ucPduType       : 1;
    VOS_UINT8                 ucPolling       : 1;
    VOS_UINT8                 ucFi            : 2;
    VOS_UINT8                 ucRf            : 2;
    VOS_UINT8                 ucTmrType       : 2;

    VOS_UINT16                usAckSn;
    VOS_UINT16                usPduLen;
    VOS_UINT16                usVRr_VRur;
    VOS_UINT16                usVRh_VRuh;

    VOS_UINT16                usVRx_VRux;
    VOS_UINT16                usVRms;
} TL_OM_LRLC_DL_PDU_INFO_STRU;

/*****************************************************************************
 结构名    : TL_OM_LRLC_DL_PDU_INFO_ARRAY_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : LRLC DL PDU上报结构体
*****************************************************************************/
typedef struct
{
    VOS_UINT32                      ulRptNum;
    TL_OM_LRLC_DL_PDU_INFO_STRU     astLRlcDlPduInfo[TL_OM_LTE_DL_PDU_RPT_MAX_NUM];
} TL_OM_LRLC_DL_PDU_INFO_ARRAY_STRU;


/*****************************************************************************
 结构名    : TL_OM_LRLC_AM_CTRL_PDU_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 保存LRLC AM状态报告
*****************************************************************************/
typedef struct
{
    VOS_UINT16                  usFrame;
    VOS_UINT8                   ucSubFrame;
    VOS_UINT8                   ucDirection     :4;
    VOS_UINT8                   ucNackNum       :4;

    VOS_UINT8                   ucRbId;
    VOS_UINT8                   ucLchId;
    VOS_UINT16                  usAckSn;

    VOS_UINT16                  ausNackSn[TL_OM_LTE_RLC_MAX_NACK_RPT_NUM];
} TL_OM_LRLC_AM_CTRL_PDU_INFO_STRU;

/*****************************************************************************
 结构名    : TL_OM_LRLC_AM_CTRL_PDU_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 保存LRLC AM状态报告
*****************************************************************************/
typedef struct
{
    VOS_UINT32                              ulRptNum;
    TL_OM_LRLC_AM_CTRL_PDU_INFO_STRU        astLRlcAmCtrlPduInfo[TL_OM_LTE_RLC_AM_CTRL_PDU_RPT_MAX_NUM];
} TL_OM_LRLC_AM_CTRL_PDU_INFO_ARRAY_STRU;

/*****************************************************************************
 结构名    : TL_OM_LMAC_UL_PDU_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 保存LMAC UL PDU关键信息
*****************************************************************************/
typedef struct
{
    VOS_UINT32                  ulStampL32Bit;

    VOS_UINT32                  bitStampH16Bit  :16;
    VOS_UINT32                  bitAirFrame     :10;
    VOS_UINT32                  bitAirSubFrame  :4;
    VOS_UINT32                  bitTbIndex      :2;

    VOS_UINT32                  bitPduType      :4;
    VOS_UINT32                  bitHarqId       :4;
    VOS_UINT32                  bitCellIndex    :4;
    VOS_UINT32                  bitUlGrantType  :4;
    VOS_UINT32                  bitTbSize       :16;

    VOS_UINT8                   aucMacPdu[TL_OM_LTE_MAC_UL_PDU_DATA_LEN];
} TL_OM_LMAC_UL_PDU_INFO_STRU;

/*****************************************************************************
 结构名    : TL_OM_LMAC_UL_PDU_INFO_ARRAY_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : LMAC UL PDU上报结构体
*****************************************************************************/
typedef struct
{
    VOS_UINT32                      ulRptNum;
    TL_OM_LMAC_UL_PDU_INFO_STRU     astLMacUlPduInfo[TL_OM_LTE_UL_PDU_RPT_MAX_NUM];
} TL_OM_LMAC_UL_PDU_INFO_ARRAY_STRU;

/*****************************************************************************
 结构名    : TL_OM_LMAC_DL_PDU_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 保存LMAC DL PDU关键信息
*****************************************************************************/
typedef struct
{
    VOS_UINT32                  bitProcFrame    :10;  /*处理时刻*/
    VOS_UINT32                  bitProcSubFrame :4;
    VOS_UINT32                  bitAirFrame     :10;  /*空口时刻*/
    VOS_UINT32                  bitAirSubFrame  :4;
    VOS_UINT32                  bitFrameType    :4;

    VOS_UINT32                  ulStampL32Bit;        /*处理完PDU子帧对应的时间戳*/
    VOS_UINT16                  usStampH16Bit;
    VOS_UINT16                  bitRntiType     :4;
    VOS_UINT16                  bitHarqId       :4;
    VOS_UINT16                  bitCellIndex    :4;
    VOS_UINT16                  bitErrIndFlg    :2;
    VOS_UINT16                  bitCrcFlg       :2;

    VOS_UINT32                  bitAreaId       :8;
    VOS_UINT32                  bitPmchIndex    :6;
    VOS_UINT32                  bitTbSize       :18;

    VOS_UINT8                   aucMacPdu[TL_OM_LTE_MAC_DL_PDU_DATA_LEN];
} TL_OM_LMAC_DL_PDU_INFO_STRU;

/*****************************************************************************
 结构名    : TL_OM_LMAC_DL_PDU_INFO_ARRAY_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : LMAC DL PDU上报结构体
*****************************************************************************/
typedef struct
{
    VOS_UINT32                      ulRptNum;
    TL_OM_LMAC_DL_PDU_INFO_STRU     astLMacDlPduInfo[TL_OM_LTE_DL_PDU_RPT_MAX_NUM];
} TL_OM_LMAC_DL_PDU_INFO_ARRAY_STRU;

/*****************************************************************************
 结构名    : TL_OM_LTE_UP_LATENCY_RPT_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : LTE用户面时延上报
*****************************************************************************/
typedef struct
{
    VOS_UINT32                  ulLatencyType;      /*上报时延类型*/
    VOS_UINT32                  ulResult;           /*当前操作处理结果*/
    VOS_UINT32                  ulLatencyTime;      /*时延长度*/
    VOS_UINT32                  ulStartTime;        /*起始时间*/
    VOS_UINT32                  ulEndTime;          /*结束时间*/
} TL_OM_LTE_UP_LATENCY_RPT_STRU;

/*****************************************************************************
 结构名    : TL_OM_LMAC_REG_BSR_SR_INFO_RPT_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : LTE MAC 规则BSR和SR情况上报
*****************************************************************************/
typedef struct
{
    VOS_UINT16                  usCurSysFrame;
    VOS_UINT16                  usCurSubFrame;
    VOS_UINT8                   ucSrState;              /*SR状态*/
    VOS_UINT8                   ucSpsGrantValid;        /*SPS授权有效标志*/
    VOS_UINT8                   ucSrMaskLchTrigFlg;     /*srMask逻辑信道触发规则BSR*/
    VOS_UINT8                   ucHigherLchTrigFlg;     /*高优先级逻辑信道触发规则BSR*/

    VOS_UINT8                   ucNewLcgTrigFlg;        /*新逻辑信道组触发规则BSR*/
    VOS_UINT8                   ucSrProhibitTmrFlg;     /*SR禁止定时器状态*/
    VOS_UINT8                   ucLchMaxPri;            /*有数据要发的逻辑信道的最高优先级*/
    VOS_UINT8                   ucMaxPriLcgId;          /*具有最高优先级的逻辑信道组ID*/
} TL_OM_LMAC_REG_BSR_SR_INFO_RPT_STRU;

/*****************************************************************************
 结构名    : TL_OM_LMAC_SR_TX_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : LTE MAC SR发送信息上报
*****************************************************************************/
typedef struct
{
    VOS_UINT16                  usCurSysFrame;
    VOS_UINT8                   ucCurSubFrame;
    VOS_UINT8                   ucSrTxResult;           /*SR发送结果,0-发送，1-取消*/

    VOS_UINT16                  usSrReqFrame;           /*SR请求帧号*/
    VOS_UINT16                  usSrReqSubFrame;
    VOS_UINT16                  usSrReqCount;           /*SR请求计数*/
    VOS_UINT16                  usSrReqNotCnfCnt;       /*SR请求没有收到CNF计数*/

    VOS_UINT8                   ucPhyCancelSrFlg;       /*SR发送结果为取消时有效，PS_TRUE为LPHY取消，PS_FALSE为LMAC取消*/
    VOS_UINT8                   ucSrTxSubFrame;         /*SR发送子帧号*/
    VOS_UINT16                  usSrTxFrame;

    VOS_UINT8                   ucSrPeriod;
    VOS_UINT8                   ucSrOffset;
    VOS_UINT16                  usSrMaxTxNum;

} TL_OM_LMAC_SR_TX_INFO_STRU;

/*****************************************************************************
 枚举名    : TL_OM_LMAC_PHY_TRIG_ENUM
 协议表格  :
 ASN.1描述 :
 枚举说明  : LTE MAC PHR触发类型
*****************************************************************************/
enum TL_OM_LMAC_PHR_TRIG_ENUM
{
    TL_OM_LMAC_PHR_TRIG_PATHLOSS        = 0x00,
    TL_OM_LMAC_PHR_TRIG_PERIODIC        = 0x01,
    TL_OM_LMAC_PHR_TRIG_CFG             = 0x02,
    TL_OM_LMAC_PHR_TRIG_ULSCELL_ACT     = 0x03,
    TL_OM_LMAC_PHR_TRIG_PWR_BACKOFF     = 0x04,
    TL_OM_LMAC_PHR_TRIG_BUTT
} ;

typedef VOS_UINT32  TL_OM_LMAC_PHR_TRIG_ENUM_UINT32;

/*****************************************************************************
 结构名    : TL_OM_LMAC_PHR_TRIGGER_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : LTE MAC PHR触发条件信息
*****************************************************************************/
typedef struct
{
    TL_OM_LMAC_PHR_TRIG_ENUM_UINT32     enTrigType;

    VOS_UINT16                          usTrigSysFrame;
    VOS_UINT8                           ucTrigSubFrame;
    VOS_UINT8                           ucDlPathLossChange;

    VOS_UINT8                           ucSimulPUCCH_PUSCH;
    VOS_UINT8                           ucExtPhrEnable;
    VOS_UINT8                           ucPeriPhrFlag;
    VOS_UINT8                           ucProhibitPhrFlag;

    VOS_UINT16                          usCurPCellPathLoss;
    VOS_UINT16                          usLastPCellPathLoss;
    VOS_UINT16                          usCurSCellPathLoss;
    VOS_UINT16                          usLastSCellPathLoss;
    VOS_UINT16                          usCurPCellPwrBackOff;
    VOS_UINT16                          usLastPCellPwrBackOff;
    VOS_UINT16                          usCurSCellPwrBackOff;
    VOS_UINT16                          usLastSCellPwrBackOff;
} TL_OM_LMAC_PHR_TRIGGER_INFO_STRU;

/*****************************************************************************
 结构名    : TL_OM_LMAC_UL_GRANT_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : LTE MAC 上行授权信息
*****************************************************************************/
typedef struct
{
    VOS_UINT16                  usCurSysFrame   : 10;
    VOS_UINT16                  usCurSubFrame   : 6;
    VOS_UINT16                  usStampH16Bit;
    VOS_UINT32                  ulStampL32Bit;

    VOS_UINT16                  usTxSysFrame    :10;
    VOS_UINT16                  usTxSubFrame    :6;
    VOS_UINT8                   bitSchedMode    :4;
    VOS_UINT8                   bitHarqId       :4;
    VOS_UINT8                   bitCellIndex    :4;
    VOS_UINT8                   bitTbIndex      :4;

    VOS_UINT8                   bitUlGrantInd   :4;
    VOS_UINT8                   bitHarqFeedInd  :4;
    VOS_UINT8                   bitNewDataInd   :4;
    VOS_UINT8                   bitGapInd       :4;
    VOS_UINT16                  usRnti;

    VOS_UINT32                  ulTbSize;
} TL_OM_LMAC_UL_GRANT_INFO_STRU;

/*****************************************************************************
 结构名    : TL_OM_LMAC_UL_GRANT_INFO_ARRAY_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : LTE MAC 上行授权上报结构信息
*****************************************************************************/
typedef struct
{
    VOS_UINT32                      ulRptNum;
    TL_OM_LMAC_UL_GRANT_INFO_STRU   astUlGrant[TL_OM_LTE_MAX_RPT_NUM];
} TL_OM_LMAC_UL_GRANT_INFO_ARRAY_STRU;

/*****************************************************************************
 结构名    : TL_OM_LMAC_DL_ASSIGNMENT_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : LTE MAC下行指派信息
*****************************************************************************/
typedef struct
{
    VOS_UINT16                  usCurFrame          :10;
    VOS_UINT16                  usCurSubFrame       :6;
    VOS_UINT16                  usTimeStampH16Bit;
    VOS_UINT32                  ulTimeStampL32Bit;

    VOS_UINT16                  usRxFrame           :10;
    VOS_UINT16                  usRxSubFrame        :6;
    VOS_UINT8                   ucCellIndex;
    VOS_UINT8                   ucTb0Valid          :1;
    VOS_UINT8                   ucTb0NewDataInd     :1;
    VOS_UINT8                   ucTb1Valid          :1;
    VOS_UINT8                   ucTb1NewDataInd     :1;
    VOS_UINT8                   ucRsv               :4;

} TL_OM_LMAC_DL_ASSIGNMENT_INFO_STRU;

/*****************************************************************************
 结构名    : TL_OM_LMAC_DL_ASSIGNMENT_INFO_ARRAY_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : LTE MAC 下行指派上报结构信息
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulRptNum;
    TL_OM_LMAC_DL_ASSIGNMENT_INFO_STRU  astDlAssign[TL_OM_LTE_MAX_RPT_NUM];
} TL_OM_LMAC_DL_ASSIGNMENT_INFO_ARRAY_STRU;

/*****************************************************************************
 结构名    : TL_OM_LMAC_ONE_LCH_BUFFER_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : LTE MAC逻辑信道缓存情况
*****************************************************************************/
typedef struct
{
    VOS_UINT8                   ucLchId;
    VOS_UINT8                   ucLcgId;
    VOS_UINT8                   ucRbId;
    VOS_UINT8                   ucLchPrio;

    VOS_UINT32                  ulSduLen;
    VOS_UINT32                  ulRetxPduLen;
    VOS_UINT32                  ulCtrlPduLen;
    VOS_UINT32                  ulPdcpBuf;

} TL_OM_LMAC_LCH_BUFF_INFO_STRU;

/*****************************************************************************
 结构名    : TL_OM_LMAC_BUFFER_STATUS_INFO_RPT_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : LTE MAC 缓存情况上报
*****************************************************************************/
typedef struct
{
    VOS_UINT16                              usFrame;
    VOS_UINT8                               ucSubFrame;
    VOS_UINT8                               ucLchNum;   /*激活逻辑信道个数*/
    TL_OM_LMAC_LCH_BUFF_INFO_STRU           astLchBuff[TL_OM_LTE_MAC_MAX_LCH_NUM];
} TL_OM_LMAC_BUFFER_STATUS_INFO_STRU;

/*****************************************************************************
 结构名    : TL_OM_LMAC_BUFFER_STATUS_INFO_RPT_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : LTE MAC 缓存情况上报
*****************************************************************************/
typedef struct
{
    VOS_UINT32                              ulRptNum;
    TL_OM_LMAC_BUFFER_STATUS_INFO_STRU      astLMacBuff[TL_OM_LTE_MAC_BUF_STATUS_RPT_NUM];
} TL_OM_LMAC_BUFFER_STATUS_RPT_STRU;

/*****************************************************************************
 结构名    : TL_OM_LTE_L2_OVERVIEW_STAT_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : LTE MAC逻辑信道缓存情况
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulAppUlRate;
    VOS_UINT32                          ulAppDlRate;
    VOS_UINT32                          ulPdcpUlRate;
    VOS_UINT32                          ulPdcpDlRate;
    VOS_UINT32                          ulRlcUlRate;
    VOS_UINT32                          ulRlcDlRate;
    VOS_UINT32                          ulMacUlRate;
    VOS_UINT32                          ulMacDlRate;

    VOS_UINT32                          ulCpuProfile;
    VOS_UINT32                          ulDdrProfile;

} TL_OM_LTE_L2_THROUGHPUT_STAT_STRU;


/*****************************************************************************
 结构名    : TL_OM_LTE_USER_PLANE_OVERVIEW_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : LTE 用户面整体数传情况
*****************************************************************************/
typedef struct
{
    /*速率统计*/
    VOS_UINT32                          ulAppUlRate;
    VOS_UINT32                          ulAppDlRate;
    VOS_UINT32                          ulLPdcpUlRate;
    VOS_UINT32                          ulLPdcpDlRate;
    VOS_UINT32                          ulLRlcUlRate;
    VOS_UINT32                          ulLRlcDlRate;
    VOS_UINT32                          ulLMacUlRate;
    VOS_UINT32                          ulLMacDlRate;

    /*DFS*/
    //VOS_UINT32                          ulCpuProfile;
    //VOS_UINT32                          ulDdrProfile;
    VOS_UINT32                          ulDfsProfile;
    /*内存统计*/
    VOS_UINT32                          ulTtfUlIpMemFree;           /*下行TTF 上行*/
    VOS_UINT32                          ulTtfDlIpMemFree;           /*下行TTF DL PDU池空闲块数*/
    VOS_UINT32                          ulDlTbFreeNum;              /*下行TB内存空闲块数*/

    /*队列缓存情况*/
    VOS_UINT32                          ulUlAppQueCnt;              /*上行APP队列缓存IP包个数*/
    VOS_UINT32                          ulUlLPdcpBuffHighSdu;       /*上行PDCP缓存SDU个数*/
    VOS_UINT32                          ulUlLPdcpBuffLowSdu;       /*上行PDCP缓存SDU个数*/
    VOS_UINT32                          ulDlLMacTbQueCnt;           /*下行缓存TB个数*/
    VOS_UINT32                          ulDlNonThrSduQueCnt;        /*下行非直通通道缓存SDU个数*/
    VOS_UINT32                          ulDlThrSduQueCnt;           /*下行直通通道缓存SDU个数*/
    VOS_UINT32                          ulDlCdsSduQueCnt;           /*CDS缓存SDU个数*/

    /*上行统计量*/
    VOS_UINT32                          ulUlCdsRxRd;
    VOS_UINT32                          ulUlFcDiscardPkt;
    VOS_UINT32                          ulUlCdsTxtoLte;
    VOS_UINT32                          ulUlLPdcpRxSdu;
    VOS_UINT32                          ulUlLPdcpTxPdu;
    VOS_UINT32                          ulUlLRlcRxSdu;
    VOS_UINT32                          ulUlLRlcDropSdu;
    VOS_UINT32                          ulUlLRlcTxSduSucc;
    VOS_UINT32                          ulUlLRlcTxSduFail;
    VOS_UINT32                          ulUlLRlcRxCtrlPdu;
    VOS_UINT32                          ulUlLRlcTxPdu;
    VOS_UINT32                          ulUlLMacUlGrant;
    VOS_UINT32                          ulUlLMacSchedRlcPdu;
    VOS_UINT32                          ulUlLMacTxPdu;

    /*下行统计量*/
    VOS_UINT32                          ulDlLMacAllocTbMem;
    VOS_UINT32                          ulDlLMacGetTbMem;
    VOS_UINT32                          ulDlLRlcRxPdu;
    VOS_UINT32                          ulDlLRlcDropPdu;
    VOS_UINT32                          ulDlLRlcTxSduSucc;
    VOS_UINT32                          ulDlLRlcTxSduFail;
    VOS_UINT32                          ulDlLPdcpRxPdu;
    VOS_UINT32                          ulDlLPdcpTxSdu;
    VOS_UINT32                          ulDlCdsRxLteSdu;
    VOS_UINT32                          ulDlFcDiscardPkt;
    VOS_UINT32                          ulDlCdsTxImsNic;
    VOS_UINT32                          ulDlCdsCfgBd;

} TL_OM_LTE_USER_PLANE_OVERVIEW_INFO_STRU;

/*****************************************************************************
 结构名    : LRRC_APP_DAM_STATUS_DBG_INFO_IND_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                          /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgId;            /*_H2ASN_Skip*/
    PS_BOOL_ENUM_UINT8                  enDAMSwitchStatus; /* RRC DAM功能状态,0:关闭,1:打开 */
    VOS_UINT8                           ucDAMCRRCCurrentCnt; /* DAM CRRC计数器计数值 */
    VOS_UINT8                           ucDAMJudgeMaxCRRCCnt; /* NV设置最大CRRC计数器值 */
    LRRC_APP_CONNREQ_TIMER_STATUS_ENUM_UINT8         enDAMConnReqTimerStatus; /* DAM连续建链失败判断定时器状态 */
    VOS_UINT8                           ucDAMCellBarTimerLenth; /* TRRC定时器时长 */
    LRRC_APP_TIMER_STATUS_ENUM_UINT8    enDAMCellBarTimerStatus; /* TRRC定时器状态 */
    VOS_UINT16                          usSrvCellId; /* 出现建链失败的服务小区id */
    VOS_UINT16                          usSrvFreq; /* 出现建链失败的服务小区频点 */
    VOS_UINT16                          usRev; /* 保留 */
    VOS_UINT32                          ulTimeOutInMs; /* 连续建链判决定时器超时时间ms */
    VOS_UINT32                          ulCurrentTimeInMs; /* 当前时间ms */
}LRRC_APP_DAM_STATUS_DBG_INFO_IND_STRU;


/*****************************************************************************
 结构名    : RRC_CSEL_LIMITED_ITEM_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 受限(Barred/Forbidden)小区的资源池,包括了受限节点的信息,超时的时间点
*****************************************************************************/
typedef struct
{
    VOS_UINT16                             usFreqInfo;                             /* 该条目的频率信息 */
    VOS_UINT16                             usCellId;                               /* 该条目的小区信息,若无效值表示整个频率受限 */
    LRRC_APP_FORBIDDEN_CAUSE_ENUM_UINT16   enForbCause;                         /* forbidden列表中forbidden的原因，此域只在forbidden列表中使用，bar列表中无效 */
    VOS_UINT8                              ucBandInd;                              /* 受限列表中需要增加，band指示 */
    VOS_UINT8                              aucReserved[1];
    VOS_UINT32                             ulTimeOutTimeInMs;                      /* 该条目实效的时刻 */
    VOS_UINT32                             ulRemainTimerLenth;                     /* 距离超时剩余时间 */
} LRRC_APP_LIMITED_ITEM_STRU;

typedef struct
{
    VOS_UINT32                             ulCnt;
    LRRC_APP_LIMITED_ITEM_STRU             astLimitedItems[64];
}LRRC_APP_LIMITED_INFO_STRU;


/*****************************************************************************
 结构名    : RRC_DEBUG_PRINT_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    LRRC_APP_DBG_MSG_TYPE_ENUM_UINT32       enMsgType;
    LRRC_APP_DBG_FUN_NAME_ENUM_UNIT32       enFunName;
    LRRC_APP_DBG_SUBFUN_NAME_ENUM_UNIT32    enSubFunName;
    LRRC_APP_DBG_PARA_ENUM_UNIT32           enPara1;
    LRRC_APP_DBG_PARA_ENUM_UNIT32           enPara2;
    VOS_INT32                               lPara3;
    VOS_INT32                               lPara4;
    VOS_INT32                               lPara5;
    VOS_INT32                               lPara6;
    VOS_INT32                               lPara7;
    VOS_INT32                               lPara8;
}LRRC_APP_DBG_ERROR_INFO_STRU;

/*****************************************************************************
 结构名    : LRRC_APP_DBG_RESEL_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 0xFFFFFFFF 为无效值
*****************************************************************************/
typedef struct
{
    LRRC_APP_DBG_FUN_NAME_ENUM_UNIT32           enFunName;
    VOS_UINT32                                  ulFreq;
    VOS_UINT32                                  ulSsc_Switch;
    VOS_UINT32                                  ulAdrx_Switch;
    VOS_INT32                                   lq_RxLevMin;
    VOS_INT32                                   lthreshServingLowP;
    VOS_INT32                                   lthreshServingLowQ;
    VOS_INT32                                   lT_Reselection;
    VOS_INT32                                   lscell_priority;
    VOS_INT32                                   lncell_priority;
    VOS_INT32                                   lthreshX_HighP;
    VOS_INT32                                   lthreshX_LowP;
    VOS_INT32                                   lthreshX_HighQ;
    VOS_INT32                                   lthreshX_LowQ;
    VOS_INT32                                   lSCell_sValue;
    VOS_INT32                                   lNCell_sValue;
    VOS_INT32                                   lSCell_qValue;
    VOS_INT32                                   lNCell_qValue;
    VOS_INT32                                   ls_IntraSerchP;
    VOS_INT32                                   ls_IntraSerchQ;
    VOS_INT32                                   ls_NonIntraSerchP;
    VOS_INT32                                   ls_NonIntraSerchQ;
    VOS_INT32                                   lQ_offset;
    VOS_INT32                                   lQ_Hyst;
    VOS_INT32                                   lStart_Meas_SCell_Rsrp;
    VOS_INT32                                   lStart_Meas_SCell_Rsrq;
    VOS_INT32                                   lHigh_pri_NCell_Rsrp;
    VOS_INT32                                   lHigh_pri_NCell_Rsrq;
    VOS_INT32                                   lLow_pri_SCell_Rsrp;
    VOS_INT32                                   lLow_pri_SCell_Rsrq;
    VOS_INT32                                   lLow_pri_NCell_Rsrp;
    VOS_INT32                                   lLow_pri_NCell_Rsrq;
    VOS_INT32                                   lIntra_Freq_SCell_Rsrp;
    VOS_INT32                                   lIntra_Freq_NCell_Rsrp;
}LRRC_APP_DBG_RESEL_INFO_STRU;

/*****************************************************************************
 结构名    : LRRC_APP_DBG_INFO_IND_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : Debug上报类型结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                           /*_H2ASN_Skip*/
    VOS_UINT32                               ulMsgId;        /*_H2ASN_Skip*/
    LRRC_APP_DBG_INFO_TYPE_ENUM_UNIT32       enReportType;
    union  /* _H2ASN_Skip */
    {      /* _H2ASN_Skip */
        LRRC_APP_LIMITED_INFO_STRU          stLimitedInfo;   /* _H2ASN_Skip */
        LRRC_APP_DBG_ERROR_INFO_STRU        stCommDbgInfo;   /* _H2ASN_Skip */
        LRRC_APP_DBG_RESEL_INFO_STRU        stReselDbgInfo;
        /******************************************************************************************************
            _H2ASN_IeChoice_When        LRRC_APP_DBG_INFO_TYPE_ENUM_UNIT16
        ******************************************************************************************************/
    }u;  /* _H2ASN_Skip */
}LRRC_APP_DBG_INFO_IND_STRU;

/*****************************************************************************
 结构名    : RRC_DEBUG_PRINT_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    RRC_FILE_NAME_ID_ENUM_UINT32        enFileName;
    VOS_UINT32                          ulLineNo;
    RRC_MS_ID_ENUM_UINT16               enMainState;
    RRC_SS_ID_ENUM_UINT16               enSubState;
    RRC_PTL_TI_ENUM_UINT16              enPtlTi;
    RRC_STATE_TI_ENUM_UINT16            enStaTi;
    VOS_UINT32                          ulTick;
    VOS_INT32                           lPara1;
    VOS_INT32                           lPara2;
    VOS_INT32                           lPara3;
    VOS_INT32                           lPara4;
}RRC_DEBUG_PRINT_INFO_STRU;

/*****************************************************************************
 结构名    : RRC_APP_DEBUG_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                          /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgId;            /*_H2ASN_Skip*/
    RRC_DEBUG_PRINT_INFO_STRU           stRrcDbgInfo;
}RRC_APP_DEBUG_INFO_STRU;

/*****************************************************************************
 枚举名    : LRRC_CMM_EVENT_TYPE_ENUM
 协议表格  :
 ASN.1描述 : 
 枚举说明  : 事件类型

*****************************************************************************/
enum LRRC_CMM_EVENT_TYPE_ENUM  
{ 
    LRRC_CMM_EVENT_A1 = 1,
    LRRC_CMM_EVENT_A2,
    LRRC_CMM_EVENT_A3,
    LRRC_CMM_EVENT_A4,
    LRRC_CMM_EVENT_A5,
    LRRC_CMM_EVENT_A6,
    LRRC_CMM_EVENT_B1,
    LRRC_CMM_EVENT_B2,
    LRRC_CMM_EVENT_BUTT
};
typedef VOS_UINT32 LRRC_CMM_EVENT_TYPE_ENUM_UINT32;


/*begin for RRC_APP_DEBUG_MEAS_STRU*/
/*****************************************************************************
 结构名    : RRC_DEBUG_PRINT_MEAS_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    RRC_FILE_NAME_ID_ENUM_UINT32        enFileName;
    VOS_UINT32                          ulLineNo;
    RRC_MS_ID_ENUM_UINT16               enMainState;
    RRC_SS_ID_ENUM_UINT16               enSubState;
    RRC_PTL_TI_ENUM_UINT16              enPtlTi;
    RRC_STATE_TI_ENUM_UINT16            enStaTi;
    VOS_UINT32                          ulTick;
    RRC_TIME_STATUS_ENUM_UINT32         lTimerOp;
    VOS_INT32                           timerLen;
    LRRC_CMM_EVENT_TYPE_ENUM_UINT32     timerOpEvent;
    LRRC_FLOW_CTRL_ENUM_UINT32     timerFlowCtrlFlg;
}RRC_DEBUG_PRINT_MEAS_STRU;

/*****************************************************************************
 结构名    : RRC_APP_DEBUG_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                          /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgId;            /*_H2ASN_Skip*/
    RRC_DEBUG_PRINT_MEAS_STRU           stRrcDbgMeasInfo;
}RRC_APP_DEBUG_MEAS_STRU;
/*end for RRC_APP_DEBUG_MEAS_STRU*/


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
  9 OTHERS
*****************************************************************************/
extern VOS_VOID LPS_OM_LOG( const VOS_CHAR  *pcFileName,  VOS_UINT32  ulLineNum,
                                  VOS_UINT32      ulModuleId,   VOS_UINT32 ulLevel,
                                  const VOS_CHAR  *pcString );
extern VOS_VOID LPS_OM_LOG1( const VOS_CHAR   *pcFileName, VOS_UINT32  ulLineNum,
                                   VOS_UINT32  ulModuleId,       VOS_UINT32 ulLevel,
                                   const VOS_CHAR    *pcString,  VOS_INT32  lPara1);
extern VOS_VOID LPS_OM_LOG2( const VOS_CHAR   *pcFileName, VOS_UINT32  ulLineNum,
                                   VOS_UINT32 ulModuleId,        VOS_UINT32 ulLevel,
                                   const VOS_CHAR   *pcString,   VOS_INT32  lPara1,
                                   VOS_INT32  lPara2);
extern VOS_VOID LPS_OM_LOG3( const VOS_CHAR  *pcFileName,  VOS_UINT32  ulLineNum,
                                   VOS_UINT32 ulModuleId,        VOS_UINT32 ulLevel,
                                   const VOS_CHAR   *pcString,   VOS_INT32  lPara1,
                                   VOS_INT32  lPara2,            VOS_INT32  lPara3);
extern VOS_VOID LPS_OM_LOG4( const VOS_CHAR   *pcFileName, VOS_UINT32  ulLineNum,
                                   VOS_UINT32 ulModuleId,        VOS_UINT32 ulLevel,
                                   const VOS_CHAR   *pcString,   VOS_INT32  lPara1,
                                   VOS_INT32  lPara2,            VOS_INT32  lPara3,
                                   VOS_INT32  lPara4);
extern VOS_UINT32  OM_GetTimeStamp(void);/*获取精度为0.1毫秒的时间戳*/

extern VOS_VOID TL_OM_ReportSingleUpEvent(
                                                TL_UP_EVENT_ENUM_UINT16 usEvent,
                                                VOS_UINT16 usPid,
                                                VOS_UINT16 usRbId,
                                                VOS_UINT16 usFrameNum,
                                                VOS_UINT16 usSubFrameNum,
                                                VOS_UINT32 ulModeType);

extern VOS_VOID TL_OM_ReportAllUpEvent(VOS_UINT32 ulModeType);

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

#endif /* end of OmCommon.h */
