

#include "bsp_applog.h"
#include "bsp_slice.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef CONFIG_APPLOG

int mdrv_applog_cfg(unsigned int enable,unsigned int level)
{
    return bsp_applog_cfg(enable,level);
}



int mdrv_applog_conn(void)
{
    return bsp_applog_conn();
}

#else

int mdrv_applog_conn(void)
{
    return 0;
}

int mdrv_applog_cfg(unsigned int enable,unsigned int level)
{
    return 0;
}
#endif


#ifdef __cplusplus
}
#endif


