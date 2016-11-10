

#ifndef __NV_BOOT_H__
#define __NV_BOOT_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#include <product_config.h>
/*lint -save -e537*/
#ifndef FEATURE_NV_FLASH_ON
#include "types.h"
#else
#include <types.h>
#include <ptable_com.h>
#include <fdrv.h>
#include <bsp_nandc.h>
#endif
#include <bsp_shared_ddr.h>
#include <nv_xml_dec.h>
#include <nv_file.h>
#include <bsp_nvim.h>
/*lint -restore +e537*/



#define NV_FLASH_NULL                        (0xffffffff)
#define NV_FLASH_VALID_DATA                  (0x0)
#define NV_FLASH_FILL                        (0xff)    /*mtd write one page,need to fill invality data*/


#define  printf              cprintf

void *alloc(unsigned sz);
void free(void * addr);
#define  nv_malloc(a)        alloc(a)
#define  nv_free(p)          (p = p)

#define NV_SUPPORT_MULTI_CARD (nv_boot_support_multi_card())


#ifndef NAND_OK
#define NAND_OK    0
#endif

#ifndef EMMC_BLOCK_SIZE
#define EMMC_BLOCK_SIZE       512
#endif


#ifndef FSZ
#define FSZ                             long unsigned int
#endif


enum
{
    NV_GOOD_SECTION,
    NV_BAD_SECTION
};

/*flash file exist flag*/
#define   NV_FILE_EXIST              NV_FILE_USING

typedef struct _nv_rd_req
{
    u32  itemid;
    u32  modemid;
    u32  offset;
    u8 * pdata;
    u32  size;
}nv_rd_req;

typedef struct _nv_wr_req
{
    u32  itemid;
    u32  modemid;
    u32  offset;
    u8 * pdata;
    u32  size;
}nv_wr_req;

struct nv_flash_global_ctrl_stru
{
    struct nv_dload_packet_head_stru nv_dload;  /*data info in dload flag*/
    STRU_XNV_MAP_FILE_INFO *other_card_info;  /*除卡1 2外的其他卡的NV信息*/
    struct nv_file_info_stru sys_nv; /*img all section info*/
    struct nv_file_info_stru bak_info;
    struct nand_spec  nand_info;
    struct nv_global_ddr_info_stru*    shared_addr;
};

extern void cprintf(const char *fmt, ...);
extern  void *memcpy(void *_dst, const void *_src, unsigned len);
extern int strlen(const char *s);
extern void *memset(void *_p, unsigned v, unsigned count);
FILE* nv_nand_open(const s8* path,const s8* mode);
s32 nv_nand_read(u8* ptr, u32 size, u32 count, FILE* fp);
s32 nv_nand_close(FILE* fp);
s32 nv_nand_seek(FILE* fp,s32 offset,s32 whence);
s32 nv_nand_ftell(FILE* fp);
s32 nv_nand_access(const s8* path,s32 mode);
s32 nv_file_read(u8* ptr, u32 size, u32 count, FILE* fp);
s32 nv_file_seek(FILE* fp,s32 offset,s32 whence);
s32 nv_file_close(FILE* fp);
s32 nv_file_ftell(FILE* fp);
s32 nv_file_access(const s8* path,s32 mode);
u32 nv_boot_get_file_len(FILE* fp);
FILE* nv_file_open(const s8* path,const s8* mode);
u32 nv_boot_write_to_mem(nv_wr_req *wreq, nv_item_info_s *item_info);
u32 nv_boot_support_multi_card(void);
u32 is_nvbin_burned(void);
u32 nv_get_item_offset(nv_item_info_s *item_info, nv_global_info_s *global_info);
u32 nv_get_item_mdmoffset(nv_item_info_s *item_info, u32 modem_id, nv_global_info_s *global_info, nv_ctrl_info_s *ctrl_info);
u8* nv_get_item_base(nv_item_info_s *item_info);
u8* nv_get_item_mdmbase(nv_item_info_s *item_info, u32 modem_id, nv_ctrl_info_s *ctrl_info);

#define NV_TAG(a, b, c) \
    printf("[nv]:%s %d 0x%x 0x%x 0x%x\n", __FUNCTION__, __LINE__, a, b, c);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif





