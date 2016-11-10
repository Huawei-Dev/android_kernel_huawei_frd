

#ifndef __HISI__ONEIMAGE_H__
#define __HISI__ONEIMAGE_H__

#include <linux/version.h>
#ifdef CONFIG_HWCONNECTIVITY
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4,1,0))
#include <linux/huawei/hw_connectivity.h>
#else
#include <huawei_platform/connectivity/hw_connectivity.h>
#endif
#endif

#endif
