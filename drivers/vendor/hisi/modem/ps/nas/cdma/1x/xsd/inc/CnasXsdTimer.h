

#ifndef __CNAS_XSD_TIMER_H__
#define __CNAS_XSD_TIMER_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include  "vos.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#pragma pack(4)

/*****************************************************************************
  2 宏定义
*****************************************************************************/
/* XSD中定时器的时长 */
#define TI_CNAS_XSD_WAIT_CARD_FILE_CNF_LEN                  (30*1000)
#define TI_CNAS_XSD_WAIT_XCC_START_CNF_LEN                  (5*1000)
#define TI_CNAS_XSD_WAIT_XREG_START_CNF_LEN                 (5*1000)
#define TI_CNAS_XSD_WAIT_CAS_START_CNF_LEN                  (10*1000)
#define TI_CNAS_XSD_WAIT_XCC_POWEROFF_CNF_LEN               (5*1000)
#define TI_CNAS_XSD_WAIT_XREG_POWEROFF_CNF_LEN              (5*1000)
#define TI_CNAS_XSD_WAIT_CAS_POWEROFF_CNF_LEN               (5*1000)

#define TI_CNAS_XSD_SRCH_ONE_CHAN_TIMER_LEN                 (330)       /* 单位:ms 物理层搜索单个频点时长 */
#define TI_CNAS_XSD_ONE_CHAN_SRCH_TIMES                     (4)         /* 单个频点搜索的次数 */
#define TI_CNAS_XSD_SVLTE_CAS_SYNC_ONE_CHAN_TIMER_LEN       (1*1000)   /* 单位:ms SVLTE下 CAS SYNC一个chan的时长*/
#define TI_CNAS_XSD_SRLTE_CAS_SYNC_ONE_CHAN_ADD_TIMER_LEN   (10*1000)  /* 单位:ms SRLTE下 cas sync一个chan的额外增加的时间，一般从读取cas的nv来获取*/
#define TI_CNAS_XSD_SVLTE_CAS_SYNC_APPEND_TIME_LEN          (60*1000)  /* 单位:ms SVLTE下 cas申请资源保护时长60S */
#define TI_CNAS_XSD_WAIT_CAS_SYNC_CNF_APPEND_TIME_LEN       (5*1000)   /* 单位:ms 等待CAS同步结果 NAS 延时5s */


#define TI_CNAS_XSD_WAIT_CAS_OHM_IND_LEN                    (30*1000)
#define TI_CNAS_XSD_WAIT_CAS_STOP_SYSTEM_SYNC_CNF_LEN       (5*1000)

#define TI_CNAS_XSD_AVAILABLE_LEN                           (5*1000)

/* Reference to CDG 143 Table A-1 Configurable Parameters */
#define TI_CNAS_XSD_T_BSR_DIG_LEN                           (180*1000)
#define TI_CNAS_XSD_T_BSR_CALL_LEN                          (5*1000)
#define TI_CNAS_XSD_T_BSR_NEWSYS_LEN                        (1*1000)

#define TI_CNAS_XSD_SLICE_REVERSE_PROTECT_TIMER_LEN         (15*3600*1000)  /* 15 hours */

#define TI_CNAS_XSD_WAIT_CAS_SYSTEM_DETERMIN_IND_LEN         (3*1000)

#define TI_CNAS_XSD_POWEROFF_CAMP_ON_PROTECT_TIMER_LEN       (1*1000)

#define TI_CNAS_XSD_WAIT_CAS_SUSPEND_CNF_LEN                 (6*1000)

#define TI_CNAS_XSD_EMC_CALLBACK_NETWORK_SRCH_PROTECT_TIMER_LEN     (15*1000)   /* 紧急呼CallBack模式搜网15s保护定时器 */
#define TI_CNAS_XSD_EMC_CALLBACK_NETWORK_SRCH_BREAK_TIMER_LEN       (500)       /* 紧急呼CallBack模式搜网500毫秒保护定时器 */

#define TI_CNAS_XSD_SYS_ACQ_NO_RF_PROTECT_TIMER_LEN                 (5)         /* 单位:分钟 SRLTE1x搜网不申请资源 5分钟 保护定时器 */
#define TI_CNAS_XSD_REDIR_NO_RF_PROTECT_TIMER_LEN                   (5)         /* 单位:分钟 SRLTE1x搜网重定向不申请资源 5分钟 保护定时器 */

#define TI_CNAS_XSD_SYS_ACQ_SYNC_DELAY_TIMER_LEN                    (500)       /* HRPD连接态或者SRLTE下LTE连接态搜1x网络，两次同步之间的缓冲时长 */

/*****************************************************************************
  3 枚举定义
*****************************************************************************/

enum CNAS_XSD_TIMER_ID_ENUM
{
    TI_CNAS_XSD_WAIT_CARD_FILE_CNF,
    TI_CNAS_XSD_WAIT_XCC_START_CNF,
    TI_CNAS_XSD_WAIT_XREG_START_CNF,
    TI_CNAS_XSD_WAIT_CAS_START_CNF,
    TI_CNAS_XSD_WAIT_XCC_POWEROFF_CNF,
    TI_CNAS_XSD_WAIT_XREG_POWEROFF_CNF,
    TI_CNAS_XSD_WAIT_CAS_POWEROFF_CNF,
    TI_CNAS_XSD_WAIT_CAS_SYSTEM_SYNC_CNF,
    TI_CNAS_XSD_WAIT_CAS_OHM_IND,
    TI_CNAS_XSD_WAIT_CAS_STOP_SYSTEM_SYNC_CNF,
    TI_CNAS_XSD_AVAILABLE_TIMER,
    /* Reference to CDG 143 Table A-1 Configurable Parameters */
    TI_CNAS_XSD_T_BSR_DIG,
    TI_CNAS_XSD_T_BSR_CALL,
    TI_CNAS_XSD_T_BSR_NEWSYS,
    TI_CNAS_XSD_WAIT_CAS_SYSTEM_DETERMIN_IND,
    TI_CNAS_XSD_POWEROFF_CAMP_ON_PROTECT_TIMER,
    TI_CNAS_XSD_SLICE_REVERSE_PROTECT_TIMER,
    TI_CNAS_XSD_AVAILABLE_MRU0_TIMER,
    TI_CNAS_XSD_WAIT_CAS_SUSPEND_CNF,
    TI_CNAS_XSD_EMC_CALLBACK_NETWORK_SRCH_PROTECT_TIMER,    /* 紧急呼CALLBACK模式15s搜网保护定时器 */
    TI_CNAS_XSD_EMC_CALLBACK_MODE_PROTECT_TIMER,            /* 紧急呼CALLBACK模式保护定时器 */
    TI_CNAS_XSD_EMC_CALLBACK_NETWORK_SRCH_BREAK_TIMER,      /* 紧急呼CaLLBACK模式的间隔定时器 */
    TI_CNAS_XSD_OOS_SYS_ACQ_CURRENT_PHASE_TOTAL_TIMER,      /* OOS场景系统捕获当前阶段总时长定时器,由于当前只存在一个阶段,因此复用一个每阶段的总时长定时器 */

    TI_CNAS_XSD_SYS_ACQ_NO_RF_PROTECT_TIMER,                /* SRLTE下1x 搜网不申请资源，由于可能会有NO RF，导致搜网时间变长，新增保护定时器 */
    TI_CNAS_XSD_REDIR_NO_RF_PROTECT_TIMER,                  /* SRLTE下1x 重定向不申请资源，由于可能会有NO RF，导致搜网时间变长，新增保护定时器 */

    TI_CNAS_XSD_SYS_ACQ_WHEN_LTE_OR_DO_CONN_SYNC_DELAY_TIMER,                    /* 数据连接态下搜1x网络，两次同步请求之间增加缓冲时间 */
    TI_CNAS_XSD_TIMER_BUTT
};
typedef VOS_UINT32  CNAS_XSD_TIMER_ID_ENUM_UINT32;

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


/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/




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

#endif /* end of CnasXsdTimerMgmt.h */
