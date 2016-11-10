/** ****************************************************************************

                    Huawei Technologies Sweden AB (C), 2001-2015

 ********************************************************************************
 * @author
 * @version
 * @date      2016-01-30
 * @file
 * @brief
 *
 * @copyright Huawei Technologies Sweden AB
 *******************************************************************************/
#ifndef XSMS_FLAC_PIF_H
#define XSMS_FLAC_PIF_H

/*******************************************************************************
 1. Other files included
*******************************************************************************/

#include "vos.h"
#include "VosPidDef.h"

#ifdef __cplusplus
#if __cplusplus
    extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */
#pragma pack(4)

/*******************************************************************************
 2. Macro definitions
*******************************************************************************/

/*******************************************************************************
 3. Enumerations declarations
*******************************************************************************/

/** ****************************************************************************
 * Name        : XSMS_FLAC_PIF_MSG_TYPE_ENUM_UINT16
 *
 * Description :
 *******************************************************************************/
enum XSMS_FLAC_PIF_MSG_TYPE_ENUM
{
    ID_XSMS_FLAC_QRY_RCV_STATUS_REQ         =  0x0000, /* _H2ASN_MsgChoice XSMS_FLAC_QRY_RCV_STATUS_REQ_STRU *//**< @sa XSMS_FLAC_QRY_RCV_STATUS_REQ_STRU */
    ID_FLAC_XSMS_QRY_RCV_STATUS_CNF         =  0x0001, /* _H2ASN_MsgChoice FLAC_XSMS_QRY_RCV_STATUS_CNF_STRU *//**< @sa FLAC_XSMS_QRY_RCV_STATUS_CNF_STRU */

    ID_XSMS_FLAC_PIF_MSG_TYPE_ENUM_BUTT,
};

typedef VOS_UINT16 XSMS_FLAC_PIF_MSG_TYPE_ENUM_UINT16;

/** ****************************************************************************
 * Name        : FLAC_RCV_STATUS_ENUM_UINT32
 *
 * Description :
 *******************************************************************************/
enum FLAC_RCV_STATUS_ENUM
{
    FLAC_RCV_STATUS_DEALING_DBM     = 0,
    FLAC_RCV_STATUS_IDLE            = 1,
    FLAC_RCV_STATUS_ENUM_BUTT
};
typedef VOS_UINT32 FLAC_RCV_STATUS_ENUM_UINT32;

/*******************************************************************************
 4. Message Header declaration
*******************************************************************************/

/*******************************************************************************
 5. Message declaration
*******************************************************************************/

/*******************************************************************************
 6. STRUCT and UNION declaration
*******************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    XSMS_FLAC_PIF_MSG_TYPE_ENUM_UINT16  enMsgType;
    VOS_UINT16                          usOpId;
} XSMS_FLAC_QRY_RCV_STATUS_REQ_STRU;


typedef struct
{
    VOS_MSG_HEADER
    XSMS_FLAC_PIF_MSG_TYPE_ENUM_UINT16  enMsgType;
    VOS_UINT16                          usOpId;
    FLAC_RCV_STATUS_ENUM_UINT32         enFlacRcvStatus;
} FLAC_XSMS_QRY_RCV_STATUS_CNF_STRU;


/*****************************************************************************
  H2ASN顶级消息结构定义
*****************************************************************************/
typedef struct
{
    XSMS_FLAC_PIF_MSG_TYPE_ENUM_UINT16  ulMsgId;                                /*_H2ASN_MsgChoice_Export XSMS_FLAC_PIF_MSG_TYPE_ENUM_UINT16*/
    VOS_UINT8                           aucMsgBlock[2];
    /***************************************************************************
        _H2ASN_MsgChoice_When_Comment          XSMS_FLAC_PIF_MSG_TYPE_ENUM_UINT16
    ****************************************************************************/
}XSMS_FLAC_PIF_MSG_TYPE_DATA;
/*_H2ASN_Length UINT32*/

typedef struct
{
    VOS_MSG_HEADER
    XSMS_FLAC_PIF_MSG_TYPE_DATA          stMsgReq;
}XSMS_FLAC_pif_MSG;


/*******************************************************************************
 8. Global  declaration
*******************************************************************************/

/*******************************************************************************
 9. Function declarations
*******************************************************************************/

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
