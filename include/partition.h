#ifndef _PARTITION_H_
#define _PARTITION_H_

#include "hisi_partition.h"

typedef struct partition {
	char name[PART_NAMELEN];
	unsigned long long start;
	unsigned long long length;
	unsigned int flags;
}PARTITION;

#if defined(CONFIG_HISI_PARTITION_HI3650)
static const struct partition partition_table_emmc[] =
{
    {PART_XLOADER,           0,         256,      EMMC_BOOT_MAJOR_PART},
    {PART_PTABLE,            0,         512,      EMMC_USER_PART}, /* ptable           512K */
    {PART_VRL,               512,       256,      EMMC_USER_PART}, /* VRL              256K p1*/
    {PART_VRL_BACKUP,        768,       256,      EMMC_USER_PART}, /* VRL backup       256K p2*/
    {PART_FW_LPM3,           1024,      256,      EMMC_USER_PART}, /* mcuimage         256K p3*/
    {PART_FRP,               1280,      768,      EMMC_USER_PART}, /* frp              768K p4*/
    {PART_FASTBOOT,          2*1024,    4*1024,   EMMC_USER_PART}, /* fastboot         4M   p5*/
    {PART_MODEMNVM_FACTORY,  6*1024,    4*1024,   EMMC_USER_PART}, /* modemnvm factory 4M   p6*/
    {PART_NVME,              10*1024,   6*1024,   EMMC_USER_PART}, /* nvme             6M   p7*/
    {PART_OEMINFO,           16*1024,   64*1024,  EMMC_USER_PART}, /* oeminfo          64M  p8*/
    {PART_SPLASH,            80*1024,   8*1024,   EMMC_USER_PART}, /* splash           8M   p9*/
    {PART_MODEMNVM_BACKUP,   88*1024,   4*1024,   EMMC_USER_PART}, /* modemnvm backup  4M   p10*/
    {PART_MODEMNVM_IMG,      92*1024,   8*1024,   EMMC_USER_PART}, /* modemnvm img     8M   p11*/
    {PART_MODEMNVM_SYSTEM,   100*1024,  4*1024,   EMMC_USER_PART}, /* modemnvm system  4M   p12*/
    {PART_SECURE_STORAGE,    104*1024,  32*1024,  EMMC_USER_PART}, /* secure storage   32M  p13*/
    {PART_3RDMODEMNVM,       136*1024,  16*1024,  EMMC_USER_PART}, /* 3rdmodemnvm      16M  p14*/
    {PART_3RDMODEMNVMBKP,    152*1024,  16*1024,  EMMC_USER_PART}, /* 3rdmodemnvmback  16M  p15*/
    {PART_PERSIST,           168*1024,  2*1024,   EMMC_USER_PART}, /* persist          2M   p16*/
    {PART_RESERVED1,         170*1024,  14*1024,  EMMC_USER_PART}, /* reserved1        14M  p17*/
    {PART_MODEM_OM,          184*1024,  32*1024,  EMMC_USER_PART}, /* modem om         32M  p18*/
    {PART_SPLASH2,           216*1024,  64*1024,  EMMC_USER_PART}, /* splash2          64M  p19*/
    {PART_MISC,              280*1024,  2*1024,   EMMC_USER_PART}, /* misc             2M   p20*/
    {PART_MODEMNVM_UPDATE,   282*1024,  24*1024,  EMMC_USER_PART}, /* modemnvm update  24M  p21*/
    {PART_RECOVERY2,         306*1024,  64*1024,  EMMC_USER_PART}, /* recovery2        64M  p22*/
    {PART_RESERVED2,         370*1024,  64*1024,  EMMC_USER_PART}, /* reserved2        64M  p23*/
    {PART_TEEOS,             434*1024,  4*1024,   EMMC_USER_PART}, /* teeos            4M   p24*/
    {PART_TRUSTFIRMWARE,     438*1024,  2*1024,   EMMC_USER_PART}, /* trustfirmware    2M   p25*/
    {PART_SENSORHUB,         440*1024,  16*1024,  EMMC_USER_PART}, /* sensorhub        16M  p26*/
    {PART_FW_HIFI,           456*1024,  12*1024,  EMMC_USER_PART}, /* hifi             12M  p27*/
    {PART_BOOT,              468*1024,  32*1024,  EMMC_USER_PART}, /* boot             32M  p28*/
    {PART_RECOVERY,          500*1024,  64*1024,  EMMC_USER_PART}, /* recovery         64M  p29*/
    {PART_DTS,               564*1024,  64*1024,  EMMC_USER_PART}, /* dtimage          64M  p30*/
    {PART_MODEM,             628*1024,  64*1024,  EMMC_USER_PART}, /* modem            64M  p31*/
    {PART_MODEM_DSP,         692*1024,  16*1024,  EMMC_USER_PART}, /* modem_dsp        16M  p32*/
    {PART_MODEM_DTB,         708*1024,  8*1024,   EMMC_USER_PART}, /* modem_dtb        8M   p33*/
    {PART_DFX,               716*1024,  16*1024,  EMMC_USER_PART}, /* dfx              16M  p34*/
    {PART_3RDMODEM,          732*1024,  64*1024,  EMMC_USER_PART}, /* 3rdmodem         64M  p35*/
    {PART_CACHE,             796*1024,  256*1024, EMMC_USER_PART}, /* cache            256M p36*/
    {PART_HISITEST0,         1052*1024, 2*1024,   EMMC_USER_PART}, /* hisitest0        2M   p37*/
    {PART_HISITEST1,         1054*1024, 2*1024,   EMMC_USER_PART}, /* hisitest1        2M   p38*/
#ifdef CONFIG_MARKET_OVERSEA
    {PART_SYSTEM,            1056*1024, 3584*1024,EMMC_USER_PART}, /* system           3584M p39*/
    {PART_CUST,              4640*1024, 512*1024, EMMC_USER_PART}, /* cust             512M  p40*/
    {PART_HISITEST2,         5152*1024, 4*1024,   EMMC_USER_PART}, /* hisitest2        4M    p41*/
    {PART_USERDATA,          5156*1024, 4096*1024,EMMC_USER_PART}, /* userdata         4096M p42*/
#elif defined CONFIG_MARKET_INTERNAL
    {PART_SYSTEM,            1056*1024, 3264*1024,EMMC_USER_PART}, /* system           3264M p39*/
    {PART_CUST,              4320*1024, 512*1024, EMMC_USER_PART}, /* cust             512M  p40*/
    {PART_HISITEST2,         4832*1024, 4*1024,   EMMC_USER_PART}, /* hisitest2        4M    p41*/
    {PART_USERDATA,          4836*1024, 4096*1024,EMMC_USER_PART}, /* userdata         4096M p42*/
#else
    {PART_SYSTEM,            1056*1024, 2560*1024,EMMC_USER_PART}, /* system           2560M p39*/
    {PART_CUST,              3616*1024, 512*1024, EMMC_USER_PART}, /* cust             512M  p40*/
    {PART_HISITEST2,         4128*1024, 4*1024,   EMMC_USER_PART}, /* hisitest2        4M    p41*/
    {PART_USERDATA,          4132*1024, 4096*1024,EMMC_USER_PART}, /* userdata         4096M p42*/
#endif
    {"0", 0, 0, 0},                                        /* total 8228M*/
};

#elif defined (CONFIG_HISI_PARTITION_HI6250)
static const struct partition partition_table_emmc[] =
{
    {PART_XLOADER,           0,         256,      EMMC_BOOT_MAJOR_PART},
    {PART_PTABLE,            0,         512,      EMMC_USER_PART}, /* ptable           512K */
    {PART_VRL,               512,       256,      EMMC_USER_PART}, /* VRL              256K p1*/
    {PART_VRL_BACKUP,        768,       256,      EMMC_USER_PART}, /* VRL backup       256K p2*/
    {PART_FW_LPM3,           1024,      256,      EMMC_USER_PART}, /* mcuimage         256K p3*/
    {PART_FRP,               1280,      768,      EMMC_USER_PART}, /* frp              768K p4*/
    {PART_FASTBOOT,          2*1024,    4*1024,   EMMC_USER_PART}, /* fastboot         4M   p5*/
    {PART_MODEMNVM_FACTORY,  6*1024,    4*1024,   EMMC_USER_PART}, /* modemnvm factory 4M   p6*/
    {PART_NVME,              10*1024,   6*1024,   EMMC_USER_PART}, /* nvme             6M   p7*/
    {PART_OEMINFO,           16*1024,   64*1024,  EMMC_USER_PART}, /* oeminfo          64M  p8*/
    {PART_RESERVED3,         80*1024,   4*1024,   EMMC_USER_PART}, /* reserved3        4M   p9*/
    {PART_MODEMNVM_BACKUP,   84*1024,   4*1024,   EMMC_USER_PART}, /* modemnvm backup  4M   p10*/
    {PART_MODEMNVM_IMG,      88*1024,   8*1024,   EMMC_USER_PART}, /* modemnvm img     8M   p11*/
    {PART_MODEMNVM_SYSTEM,   96*1024,   4*1024,   EMMC_USER_PART}, /* modemnvm system  4M   p12*/
    {PART_SECURE_STORAGE,    100*1024,  32*1024,  EMMC_USER_PART}, /* secure storage   32M  p13*/
    {PART_RESERVED4,         132*1024,  2*1024,   EMMC_USER_PART}, /* reserved4        2M   p14*/
    {PART_RESERVED5,         134*1024,  2*1024,   EMMC_USER_PART}, /* reserved5        2M   p15*/
    {PART_PERSIST,           136*1024,  2*1024,   EMMC_USER_PART}, /* persist          2M   p16*/
    {PART_RESERVED1,         138*1024,  14*1024,  EMMC_USER_PART}, /* reserved1        14M  p17*/
    {PART_MODEM_OM,          152*1024,  32*1024,  EMMC_USER_PART}, /* modem om         32M  p18*/
    {PART_SPLASH2,           184*1024,  64*1024,  EMMC_USER_PART}, /* splash2          64M  p19*/
    {PART_MISC,              248*1024,  2*1024,   EMMC_USER_PART}, /* misc             2M   p20*/
    {PART_MODEMNVM_UPDATE,   250*1024,  24*1024,  EMMC_USER_PART}, /* modemnvm update  24M  p21*/
    {PART_RECOVERY2,         274*1024,  64*1024,  EMMC_USER_PART}, /* recovery2        64M  p22*/
    {PART_RESERVED2,         338*1024,  60*1024,  EMMC_USER_PART}, /* reserved2        60M  p23*/
    {PART_TEEOS,             398*1024,  4*1024,   EMMC_USER_PART}, /* teeos            4M   p24*/
    {PART_TRUSTFIRMWARE,     402*1024,  2*1024,   EMMC_USER_PART}, /* trustfirmware    2M   p25*/
    {PART_SENSORHUB,         404*1024,  16*1024,  EMMC_USER_PART}, /* sensorhub        16M  p26*/
    {PART_FW_HIFI,           420*1024,  12*1024,  EMMC_USER_PART}, /* hifi             12M  p27*/
    {PART_BOOT,              432*1024,  32*1024,  EMMC_USER_PART}, /* boot             32M  p28*/
    {PART_RECOVERY,          464*1024,  64*1024,  EMMC_USER_PART}, /* recovery         64M  p29*/
    {PART_DTS,               528*1024,  32*1024,  EMMC_USER_PART}, /* dtimage          32M  p30*/
    {PART_MODEM_FW,          560*1024,  96*1024,  EMMC_USER_PART}, /* modem_fw         96M  p31*/
    {PART_RESERVED7,         656*1024,  2*1024,   EMMC_USER_PART}, /* reserved7        2M   p32*/
    {PART_RESERVED8,         658*1024,  2*1024,   EMMC_USER_PART}, /* reserved8        2M   p33*/
    {PART_DFX,               660*1024,  16*1024,  EMMC_USER_PART}, /* dfx              16M  p34*/
    {PART_RESERVED6,         676*1024,  4*1024,   EMMC_USER_PART}, /* reserved6        4M   p35*/
    {PART_CACHE,             680*1024,  256*1024, EMMC_USER_PART}, /* cache            256M p36*/
    {PART_HISITEST0,         936*1024,  2*1024,   EMMC_USER_PART}, /* hisitest0        2M   p37*/
    {PART_HISITEST1,         938*1024,  2*1024,   EMMC_USER_PART}, /* hisitest1        2M   p38*/
    {PART_HISITEST2,         940*1024,  4*1024,   EMMC_USER_PART}, /* hisitest2        4M   p39*/
#ifdef CONFIG_MARKET_OVERSEA
    {PART_SYSTEM,            944*1024,  3584*1024,EMMC_USER_PART}, /* system           3584M p40*/
    {PART_CUST,              4528*1024, 512*1024, EMMC_USER_PART}, /* cust             512M  p41*/
    {PART_USERDATA,          5040*1024, 4096*1024,EMMC_USER_PART}, /* userdata         4096M p42*/
#elif defined CONFIG_MARKET_INTERNAL
    {PART_SYSTEM,            944*1024,  3264*1024,EMMC_USER_PART}, /* system           3264M p40*/
    {PART_CUST,              4208*1024, 512*1024, EMMC_USER_PART}, /* cust             512M  p41*/
    {PART_USERDATA,          4720*1024, 4096*1024,EMMC_USER_PART}, /* userdata         4096M p42*/
#else
    {PART_SYSTEM,            944*1024,  2560*1024,EMMC_USER_PART}, /* system           2560M p40*/
    {PART_CUST,              3504*1024, 512*1024, EMMC_USER_PART}, /* cust             512M  p41*/
    {PART_USERDATA,          4016*1024, 4096*1024,EMMC_USER_PART}, /* userdata         4096M p42*/
#endif
    {"0", 0, 0, 0},                                       /* */
};

#elif defined CONFIG_HISI_PARTITION_HI3660
static const struct partition partition_table_emmc[] =
{
    {PART_XLOADER,          0,          256,      EMMC_BOOT_MAJOR_PART},
    {PART_PTABLE,           0,          512,      EMMC_USER_PART}, /* ptable           512K */
    {PART_VRL,              512,        256,      EMMC_USER_PART}, /* VRL              256K p1*/
    {PART_VRL_BACKUP,       768,        256,      EMMC_USER_PART}, /* VRL backup       256K p2*/
    {PART_FW_LPM3,          1024,       256,      EMMC_USER_PART}, /* mcuimage         256K p3*/
    {PART_FRP,              1280,       768,      EMMC_USER_PART}, /* frp              768K p4*/
    {PART_FASTBOOT,         2*1024,     4*1024,   EMMC_USER_PART}, /* fastboot         4M   p5*/
    {PART_MODEMNVM_FACTORY, 6*1024,     4*1024,   EMMC_USER_PART}, /* modemnvm factory 4M   p6*/
    {PART_NVME,             10*1024,    6*1024,   EMMC_USER_PART}, /* nvme             6M   p7*/
    {PART_OEMINFO,          16*1024,    64*1024,  EMMC_USER_PART}, /* oeminfo          64M  p8*/
    {PART_VECTOR,           80*1024,    4*1024,   EMMC_USER_PART}, /* avs vector       4M   p9*/
    {PART_MODEMNVM_BACKUP,  84*1024,    4*1024,   EMMC_USER_PART}, /* modemnvm backup  4M   p10*/
    {PART_MODEMNVM_IMG,     88*1024,    12*1024,  EMMC_USER_PART}, /* modemnvm img     12M   p11*/
    {PART_MODEMNVM_SYSTEM,  100*1024,    4*1024,   EMMC_USER_PART}, /* modemnvm system  4M   p12*/
    {PART_SECURE_STORAGE,   104*1024,   32*1024,  EMMC_USER_PART}, /* secure storage   32M  p13*/
    {PART_HISEE_FS,         136*1024,   10*1024,  EMMC_USER_PART}, /* hisee_fs         10M  p14*/
    {PART_ISP_BOOT,         146*1024,   2*1024,   EMMC_USER_PART}, /* isp_boot         2M   p15*/
    {PART_PERSIST,          148*1024,   2*1024,   EMMC_USER_PART}, /* persist          2M   p16*/
    {PART_ISP_FIRMWARE,     150*1024,   14*1024,  EMMC_USER_PART}, /* isp_firmware     14M  p17*/
    {PART_MODEM_OM,         164*1024,   32*1024,  EMMC_USER_PART}, /* modem om         32M  p18*/
    {PART_SPLASH2,          196*1024,   80*1024,  EMMC_USER_PART}, /* splash2          80M  p19*/
    {PART_MISC,             276*1024,   2*1024,   EMMC_USER_PART}, /* misc             2M   p20*/
    {PART_MODEMNVM_UPDATE,  278*1024,   24*1024,  EMMC_USER_PART}, /* modemnvm update  24M  p21*/
    {PART_RECOVERY2,        302*1024,   64*1024,  EMMC_USER_PART}, /* recovery2        64M  p22*/
    {PART_RESERVED2,        366*1024,   60*1024,  EMMC_USER_PART}, /* reserved2        60M  p23*/
    {PART_TEEOS,            426*1024,   8*1024,   EMMC_USER_PART}, /* teeos            8M   p24*/
    {PART_TRUSTFIRMWARE,    434*1024,   2*1024,   EMMC_USER_PART}, /* trustfirmware    2M   p25*/
    {PART_SENSORHUB,        436*1024,   16*1024,  EMMC_USER_PART}, /* sensorhub        16M  p26*/
    {PART_FW_HIFI,          452*1024,   12*1024,  EMMC_USER_PART}, /* hifi             12M  p27*/
    {PART_BOOT,             464*1024,   32*1024,  EMMC_USER_PART}, /* boot             32M  p28*/
    {PART_RECOVERY,         496*1024,   64*1024,  EMMC_USER_PART}, /* recovery         64M  p29*/
    {PART_DTS,              560*1024,   32*1024,  EMMC_USER_PART}, /* dtimage          32M  p30*/
    {PART_MODEM_FW,         592*1024,   96*1024,  EMMC_USER_PART}, /* modemi_fw        96M  p31*/
    {PART_ISP_DTS,          688*1024,   32*1024,  EMMC_USER_PART}, /* isp_dts          32M  p32*/
    {PART_HISEE_IMG,        720*1024,   4*1024,   EMMC_USER_PART}, /* hisee_img        4M   p33*/
    {PART_DFX,              724*1024,   16*1024,  EMMC_USER_PART}, /* dfx              16M  p34*/
    {PART_RESERVED1,        740*1024,   4*1024,   EMMC_USER_PART}, /* reserved1        4M   p35*/
    {PART_CACHE,            744*1024,   256*1024, EMMC_USER_PART}, /* cache            256M p36*/
    {PART_HISITEST0,        1000*1024,  2*1024,   EMMC_USER_PART}, /* hisitest0        2M   p37*/
    {PART_HISITEST1,        1002*1024,  2*1024,   EMMC_USER_PART}, /* hisitest1        2M   p38*/
    {PART_HISITEST2,        1004*1024,  4*1024,   EMMC_USER_PART}, /* hisitest2        4M   p39*/
    {PART_SYSTEM,           1008*1024,  2560*1024,EMMC_USER_PART}, /* system           2560M p40*/
    {PART_CUST,             3568*1024,  512*1024, EMMC_USER_PART}, /* cust             512M  p41*/
    {PART_USERDATA,         4080*1024,  4096*1024,EMMC_USER_PART}, /* userdata         4096M p42*/
    {"0", 0, 0, 0},                                        /* total 8176M*/
};
#ifdef CONFIG_HISI_STORAGE_UFS_PARTITION
static const struct partition partition_table_ufs[] =
{
    {PART_XLOADER,          0,          256,      UFS_BOOT_MAJOR_PART},
    {PART_PTABLE,           0,          512,      UFS_NORMAL_PART}, /* ptable           512K */
    {PART_VRL,              512,        256,      UFS_NORMAL_PART}, /* VRL              256K p1*/
    {PART_VRL_BACKUP,       768,        256,      UFS_NORMAL_PART}, /* VRL backup       256K p2*/
    {PART_FW_LPM3,          1024,       256,      UFS_NORMAL_PART}, /* mcuimage         256K p3*/
    {PART_FRP,              1280,       768,      UFS_NORMAL_PART}, /* frp              768K p4*/
    {PART_FASTBOOT,         2*1024,     4*1024,   UFS_NORMAL_PART}, /* fastboot         4M   p5*/
    {PART_MODEMNVM_FACTORY, 6*1024,     4*1024,   UFS_NORMAL_PART}, /* modemnvm factory 4M   p6*/
    {PART_NVME,             10*1024,    6*1024,   UFS_NORMAL_PART}, /* nvme             6M   p7*/
    {PART_OEMINFO,          16*1024,    64*1024,  UFS_NORMAL_PART}, /* oeminfo          64M  p8*/
    {PART_VECTOR,           80*1024,    4*1024,   UFS_NORMAL_PART}, /* avs vector       4M   p9*/
    {PART_MODEMNVM_BACKUP,  84*1024,    4*1024,   UFS_NORMAL_PART}, /* modemnvm backup  4M   p10*/
    {PART_MODEMNVM_IMG,     88*1024,    12*1024,  UFS_NORMAL_PART}, /* modemnvm img     12M  p11*/
    {PART_MODEMNVM_SYSTEM,  100*1024,    4*1024,   UFS_NORMAL_PART}, /* modemnvm system  4M  p12*/
    {PART_SECURE_STORAGE,   104*1024,   32*1024,  UFS_NORMAL_PART}, /* secure storage   32M  p13*/
    {PART_HISEE_FS,         136*1024,   10*1024,  UFS_NORMAL_PART}, /* hisee_fs         10M  p14*/
    {PART_ISP_BOOT,         146*1024,   2*1024,   UFS_NORMAL_PART}, /* isp_boot         2M   p15*/
    {PART_PERSIST,          148*1024,   2*1024,   UFS_NORMAL_PART}, /* persist          2M   p16*/
    {PART_ISP_FIRMWARE,     150*1024,   14*1024,  UFS_NORMAL_PART}, /* isp_firmware     14M  p17*/
    {PART_MODEM_OM,         164*1024,   32*1024,  UFS_NORMAL_PART}, /* modem om         32M  p18*/
    {PART_SPLASH2,          196*1024,   80*1024,  UFS_NORMAL_PART}, /* splash2          80M  p19*/
    {PART_MISC,             276*1024,   2*1024,   UFS_NORMAL_PART}, /* misc             2M   p20*/
    {PART_MODEMNVM_UPDATE,  278*1024,   24*1024,  UFS_NORMAL_PART}, /* modemnvm update  24M  p21*/
    {PART_RECOVERY2,        302*1024,   64*1024,  UFS_NORMAL_PART}, /* recovery2        64M  p22*/
    {PART_RESERVED2,        366*1024,   60*1024,  UFS_NORMAL_PART}, /* reserved2        60M  p23*/
    {PART_TEEOS,            426*1024,   8*1024,   UFS_NORMAL_PART}, /* teeos            8M   p24*/
    {PART_TRUSTFIRMWARE,    434*1024,   2*1024,   UFS_NORMAL_PART}, /* trustfirmware    2M   p25*/
    {PART_SENSORHUB,        436*1024,   16*1024,  UFS_NORMAL_PART}, /* sensorhub        16M  p26*/
    {PART_FW_HIFI,          452*1024,   12*1024,  UFS_NORMAL_PART}, /* hifi             12M  p27*/
    {PART_BOOT,             464*1024,   32*1024,  UFS_NORMAL_PART}, /* boot             32M  p28*/
    {PART_RECOVERY,         496*1024,   64*1024,  UFS_NORMAL_PART}, /* recovery         64M  p29*/
    {PART_DTS,              560*1024,   32*1024,  UFS_NORMAL_PART}, /* dtimage          32M  p30*/
    {PART_MODEM_FW,         592*1024,   96*1024,  UFS_NORMAL_PART}, /* modem_fw         96M  p31*/
    {PART_ISP_DTS,          688 *1024,  32*1024,  UFS_NORMAL_PART}, /* isp_dts          32M  p32*/
    {PART_HISEE_IMG,        720*1024,   4*1024,   UFS_NORMAL_PART}, /* hisee_img        4M   p33*/
    {PART_DFX,              724*1024,   16*1024,  UFS_NORMAL_PART}, /* dfx              16M  p34*/
    {PART_RESERVED1,        740*1024,   4*1024,   UFS_NORMAL_PART}, /* reserved1        4M   p35*/
    {PART_CACHE,            744*1024,   256*1024, UFS_NORMAL_PART}, /* cache            256M p36*/
    {PART_HISITEST0,        1000*1024,   2*1024,   UFS_NORMAL_PART}, /* hisitest0        2M   p37*/
    {PART_HISITEST1,        1002*1024,   2*1024,   UFS_NORMAL_PART}, /* hisitest1        2M   p38*/
    {PART_HISITEST2,        1004*1024,   4*1024,   UFS_NORMAL_PART}, /* hisitest2        4M   p39*/
    {PART_SYSTEM,           1008*1024,   2560*1024,UFS_NORMAL_PART}, /* system           2560M p40*/
    {PART_CUST,             3568*1024,  512*1024, UFS_NORMAL_PART}, /* cust             512M  p41*/
    {PART_USERDATA,         4080*1024,  4096*1024,UFS_NORMAL_PART}, /* userdata         4096M p42*/
    {"0", 0, 0, 0},                                       /* Total  8176M*/
};
#endif

#endif

#endif

