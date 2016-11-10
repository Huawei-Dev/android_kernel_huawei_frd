/**

                   Huawei Technologies (C), 2001-2016,

 ******************************************************************************
 * @file
 *
 * @author
 * @version
 * @date    2016/02/27
 * @copyright Huawei Technologies
******************************************************************************/

/*****************************************************************************
  1 Other files included
*****************************************************************************/
#ifndef INCLUSION_GUARD_FWK_LOG_EVENT_H
#define INCLUSION_GUARD_FWK_LOG_EVENT_H

#include "vos.h"

#ifdef __cplusplus
#if __cplusplus
    extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#pragma pack(4)

/*******************************************************************************
 2. Macro definitions
*******************************************************************************/

/* Event header for elf explain */
#define CPROC_EVENT_HEADER CPROC_OM_MNTN_EVENT_ID_ENUM_UINT16 usEventId;     \
                           CPROC_MODULE_ID_ENUM_UINT16 usModuleId;

#define TIMER_CALLBACK_LOG_NUM      (20)
#define CPROC_LOG_COMMON_EVENT_NUM  (12)
#define TIMER_CALLBACK_LOG_GAP      (200)
#define CPROC_CALLBACK_FUNC_NUM    (10)

/*****************************************************************************
  3 Enumeration declaration
*****************************************************************************/

/** ****************************************************************************
 * Name        : CPROC_1X_CHR_EVENT_ID_ENUM
 *
 * Description :
 * Please add new 1x ENUM before 0x007F
 * Please add new hrpd ENUM between 0x0080 and 0x00ff
 *******************************************************************************/
enum CPROC_CHR_EVENT_ID_ENUM
{
    CPROC_1X_ERR_LOG_EVENT_NO_RESOURCE        = 0x0001,
    CPROC_1X_ERR_LOG_EVENT_FSM_ABNORMAL       = 0x0002,
    CPROC_1X_ERR_LOG_EVENT_PILOT_SEARCH_FAIL  = 0x0003,
    CPROC_1X_ERR_LOG_EVENT_TIME_ABNORMAL      = 0x0004,
    CPROC_1X_ERR_LOG_EVENT_TCH_ABNORMAL       = 0x0005,
    CPROC_1X_ERR_LOG_EVENT_PAGING_RESULT      = 0x0006,
    CPROC_1X_ERR_LOG_EVENT_ALL_DATA           = 0x0007,
    CPROC_1X_ERR_LOG_EVENT_MEASURE_INFO       = 0x0008,

    CPROC_1X_ERR_LOG_EVENT_BUTT               = 0x007F,

    CPROC_HRPD_ERR_LOG_EVENT_NO_RESOURCE      = 0x0081,
    CPROC_HRPD_ERR_LOG_EVENT_FSM_ABNORMAL     = 0x0082,
    CPROC_HRPD_ERR_LOG_EVENT_PILOT_SEARCH_FAIL= 0x0083,
    CPROC_HRPD_ERR_LOG_EVENT_TIME_ABNORMAL    = 0x0084,
    CPROC_HRPD_ERR_LOG_EVENT_PAGING_RESULT    = 0x0085,
    CPROC_HRPD_ERR_LOG_EVENT_SLAVE_MEAS_INFO  = 0x0086,
    CPROC_HRPD_ERR_LOG_EVENT_ERROR_IND_COUNT  = 0X0087,

    CPROC_HRPD_ERR_LOG_EVENT_BUTT             = 0x00FF
};
typedef VOS_UINT16 CPROC_CHR_EVENT_ID_ENUM_UINT16;

/** ****************************************************************************
 * Name        : CPROC_OM_MNTN_EVENT_ID_ENUM
 *
 * Description :
 *******************************************************************************/
enum CPROC_OM_MNTN_EVENT_ID_ENUM
{
    CPROC_1X_EVENT_TIMER_REGISTER           = 0x0100,
    CPROC_1X_EVENT_TIMER_CALLBACK           = 0x0101,
    CPROC_1X_EVENT_TIMER_RELEASE            = 0x0102,
    CPROC_COMMON_EVENT_LIST                 = 0x0103,
    CPROC_1X_EVENT_INTERRUPT_PRINT_10MS     = 0x0104,
    CPROC_HRPD_EVENT_INTERRUPT_PRINT_1_67MS = 0x0105,
    CPROC_DRX_WAKEUP_MNTN                   = 0x0106,
    CPROC_1X_DIVERSITY_RXON_TYPE            = 0x0107,
    CPROC_1X_DIVERSITY_RFA_SET_DIV_REQ      = 0x0108,
    CPROC_1X_DIVERSITY_SM_SET_DIV_REQ       = 0x0109,
    CPROC_1X_DIVERSITY_SM_START_MPS_REQ     = 0x010A,
    CPROC_1X_DIVERSITY_TEMPERATURE          = 0x010B,
    CPROC_1X_DIVERSITY_VOTE_RESULT          = 0x010C,
    CPROC_1X_DIVERSITY_START_TEMPRATURE     = 0x010D,
    CPROC_1X_DIVERSITY_STOP_TEMPRATURE      = 0x010E,
    CPROC_EVENT_BUTT
};
typedef VOS_UINT16 CPROC_OM_MNTN_EVENT_ID_ENUM_UINT16;

/** ****************************************************************************
 * Name        : CPROC_COMMON_EVENT_ID_ENUM
 *
 * Description : Explain event ID in common event
 *******************************************************************************/
enum CPROC_COMMON_EVENT_ID_ENUM
{
    CPROC_COMMON_EVENT_INTERRUPT_PRINT      = 0x0000,
    CPROC_COMMON_EVENT_INTERRUPT_HANDLER    = 0x0001,
    CPROC_COMMON_EVENT_TICK_MAINTENANCE     = 0x0002,
    CPROC_COMMON_EVENT_BUTT
};
typedef VOS_UINT16 CPROC_COMMON_EVENT_ID_ENUM_UINT16;

/** ****************************************************************************
 * Name        : CPROC_MODULE_ID_ENUM
 *
 * Description :
 *******************************************************************************/
enum CPROC_MODULE_ID_ENUM
{
    CPROC_MODULE_1X                         = 0x0000,
    CPROC_MODULE_HRPD                       = 0x0001,
    CPROC_MODULE_RM                         = 0x0002,
    CPROC_MODULE_BUTT
};
typedef VOS_UINT16 CPROC_MODULE_ID_ENUM_UINT16;

/** ****************************************************************************
 * Name        : CPROC_LOG_EVENT_LEVEL_ENUM
 *
 * Description :
 *******************************************************************************/
enum CPROC_LOG_EVENT_LEVEL_ENUM
{
    CPROC_LOG_EVENT_LEVEL_INFO               = 0x0000,
    CPROC_LOG_EVENT_LEVEL_WARNING            = 0x0001,
    CPROC_LOG_EVENT_LEVEL_ERROR              = 0x0002,
    CPROC_LOG_EVENT_LEVEL_BUTT
};
typedef VOS_UINT16 CPROC_LOG_EVENT_LEVEL_ENUM_UINT16;

/*****************************************************************************
  4 Message Header declaration
*****************************************************************************/

/*****************************************************************************
  5 Message declarations
*****************************************************************************/

/*****************************************************************************
  6 STRUCT declarations
*****************************************************************************/

/** ****************************************************************************
 * Name        : CPROC_1X_TIMER_INFO_STRU
 *
 * Description :
 *******************************************************************************/
typedef struct
{
    VOS_UINT32                         ulTimeStamp;
    VOS_UINT32                         pfCallBack;
}CPROC_1X_TIMER_INFO_STRU;

/** ****************************************************************************
 * Name        : CPROC_LOG_COMMON_EVENT_INFO_STRU
 *
 * Description : Common event struct
 *******************************************************************************/
typedef struct
{
    CPROC_COMMON_EVENT_ID_ENUM_UINT16  enEventId;
    CPROC_LOG_EVENT_LEVEL_ENUM_UINT16  enEventLevel;
    VOS_UINT32                         ulFileNo;
    VOS_UINT32                         ulLineNo;
    VOS_UINT32                         ulTimeStamp;
    VOS_UINT32                         usVarCnt;
    VOS_UINT32                         ulValue[5];
}CPROC_LOG_COMMON_EVENT_INFO_STRU;

/** ****************************************************************************
 * Name        : CPROC_LOG_COMMON_EVENT_STRU
 *
 * Description :
 *******************************************************************************/
typedef struct
{
    CPROC_EVENT_HEADER
    CPROC_LOG_COMMON_EVENT_INFO_STRU   stEventInfo;
}CPROC_LOG_COMMON_EVENT_STRU;

/** ****************************************************************************
 * Name        : CPROC_LOG_COMMON_EVENT_LIST_STRU
 *
 * Description :
 *******************************************************************************/
typedef struct
{
    CPROC_EVENT_HEADER
    VOS_UINT32                         usLogNum;
    CPROC_LOG_COMMON_EVENT_INFO_STRU   astEventInfo[CPROC_LOG_COMMON_EVENT_NUM];
}CPROC_LOG_COMMON_EVENT_LIST_STRU;

/** ****************************************************************************
 * Name        : CPROC_1X_EVENT_TIMER_REGISTER_STRU
 *
 * Description :
 *******************************************************************************/
typedef struct
{
    CPROC_EVENT_HEADER
    VOS_UINT32                         ulTimeStamp;
    VOS_UINT16                         enCdmaMode;
    VOS_UINT16                         enPeriod;
    VOS_INT32                          swTimeShift;
    VOS_UINT32                         pfCallBack;
}CPROC_1X_EVENT_TIMER_REGISTER_STRU;

/** ****************************************************************************
 * Name        : CPROC_1X_EVENT_TIMER_CALLBACK_STRU
 *
 * Description :
 *******************************************************************************/
typedef struct
{
    CPROC_EVENT_HEADER
    VOS_UINT32               usLogNum;
    CPROC_1X_TIMER_INFO_STRU astCproc1xTimerInfo[TIMER_CALLBACK_LOG_NUM];
}CPROC_1X_EVENT_TIMER_CALLBACK_STRU;

/** ****************************************************************************
 * Name        : CPROC_1X_EVENT_TIMER_RELEASE_STRU
 *
 * Description :
 *******************************************************************************/
typedef struct
{
    CPROC_EVENT_HEADER
    CPROC_1X_TIMER_INFO_STRU stCproc1xTimerInfo;
}CPROC_1X_EVENT_TIMER_RELEASE_STRU;

typedef struct
{
    CPROC_1X_EVENT_TIMER_CALLBACK_STRU stTimerCallBack;
    CPROC_LOG_COMMON_EVENT_LIST_STRU   stLogCommonEvent;
    CPROC_LOG_COMMON_EVENT_LIST_STRU   stCprocHrpdTimerPrintEvent;
}CPROC_OM_MNTN_EVENT_CONTEXT_STRU;
/*****************************************************************************
  7 UNION declarations
*****************************************************************************/

/*****************************************************************************
  8 OTHER declarations
*****************************************************************************/

/*****************************************************************************
  9 Global variables
*****************************************************************************/

/*****************************************************************************
  10 Function declarations
*****************************************************************************/


#if ((VOS_OS_VER == VOS_WIN32) || (VOS_OS_VER == VOS_NUCLEUS))
#pragma pack()
#else
#pragma pack(0)
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#endif

