
#ifndef __TL_PHY_INTERFACE_H__
#define __TL_PHY_INTERFACE_H__

/************************************************************
                     包含其它模块的头文件
************************************************************/
#include "product_config.h"

/************************************************************
                               宏定义
************************************************************/

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif


#if (defined(TL_PHY_ASIC_V760))
    #include "comm/hi6960/TLPhyInterfaceLower.h"
#else
    #include "comm/hi6950/TLPhyInterfaceLower.h"
#endif


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif


#endif /* __MEM_MAP_BASE_H__ */
