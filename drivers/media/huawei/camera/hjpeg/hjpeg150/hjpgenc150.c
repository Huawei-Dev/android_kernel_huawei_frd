

#include <linux/of.h>
#include <linux/module.h>
#include <linux/workqueue.h>
#include <linux/delay.h>
#include <linux/types.h>
#include <linux/ioctl.h>
#include <linux/proc_fs.h>
#include <linux/wait.h>
#include <linux/videodev2.h>
#include <linux/iommu.h>
#include <linux/platform_device.h>
#include <media/v4l2-fh.h>
#include <linux/interrupt.h>
#include <linux/clk.h>
#include <linux/regulator/consumer.h>
#include <linux/regulator/driver.h>
#include <linux/regulator/machine.h>
#include <linux/fs.h>
#include <linux/dma-mapping.h>
#include <media/huawei/hjpeg_cfg.h>
#include <asm/io.h>
#include <linux/hisi/hisi_ion.h>
#include <asm/uaccess.h>
#include <linux/rpmsg.h>

#include "hjpgenc150.h"
#include "hjpeg_intf.h"
#include "hjpg150_reg_offset.h"
#include "hjpg150_reg_offset_field.h"
#include "hjpg150_table.h"
#include "cam_log.h"

/*TODO:use kernel dump mem space do JPEG IP system verify*/
#define JPEG_INPUT_PHYADDR 0x5D000000
#define JPEG_OUTPUT_PHYADDR 0x65000000
#define IRQ_MANAGER_BASE 0xE842C000

#define SMMU 1
#define CHIP_VERIFICATION 1
/* #define DUMP_FILE 1 */

static void hjpeg150_isr_do_tasklet(unsigned long data);
DECLARE_TASKLET(hjpeg150_isr_tasklet, hjpeg150_isr_do_tasklet, (unsigned long)0);

typedef struct _tag_hjpeg150_hw_ctl
{
    void __iomem *                              base_viraddr;
    phys_addr_t                                 phyaddr;
    u32                                         mem_size;
    u32                                         irq_no;

    void __iomem *                              cvdr_viraddr;
    phys_addr_t                                 cvdr_phyaddr;
    u32                                         cvdr_mem_size;
}hjpeg150_hw_ctl_t;

typedef struct _tag_hjpeg150
{
    struct platform_device*                     pdev;
    hjpeg_intf_t                                intf;
    char const*                                 name;

    void __iomem *                              viraddr;
    phys_addr_t                                 phyaddr;
    u32                                         mem_size;
    u32                                         irq_no;
    struct semaphore                            buff_done;
    hjpeg150_hw_ctl_t                           hw_ctl;
    struct ion_client*                          ion_client;
    bool                                        power_on_state;
} hjpeg150_t;

#define I2HJPEG150(i) container_of(i, hjpeg150_t, intf)

static int hjpeg150_encode_process(hjpeg_intf_t *i, void *cfg);
static int hjpeg150_power_on(hjpeg_intf_t *i);
static int hjpeg150_power_off(hjpeg_intf_t *i);
static int hjpeg150_get_reg(hjpeg_intf_t *i, void* cfg);
static int hjpeg150_set_reg(hjpeg_intf_t *i, void* cfg);

static hjpeg_vtbl_t
s_vtbl_hjpeg150 =
{
    .encode_process = hjpeg150_encode_process,
    .power_on = hjpeg150_power_on,
    .power_down = hjpeg150_power_off,
    .get_reg = hjpeg150_get_reg,
    .set_reg = hjpeg150_set_reg,
};

static hjpeg150_t
s_hjpeg150 =
{
    .intf = { .vtbl = &s_vtbl_hjpeg150, },
    .name = "hjpeg150",
};

static const struct of_device_id
s_hjpeg150_dt_match[] =
{
    {
        .compatible = "huawei,hjpeg150",
        .data = &s_hjpeg150.intf,
    },
    {
    },
};

MODULE_DEVICE_TABLE(of, s_hjpeg150_dt_match);

static void hjpeg150_isr_do_tasklet(unsigned long data)
{
    up(&s_hjpeg150.buff_done);
}

#define ENCODE_FINISH (1<<4)
static irqreturn_t hjpeg150_irq_handler(int irq, void *dev_id)
{
    void __iomem *base = s_hjpeg150.viraddr;
    u32 value = 0;

    value = REG_GET(base + JPGENC_JPE_STATUS_RIS_REG );
    if(value & ENCODE_FINISH) {
        tasklet_schedule(&hjpeg150_isr_tasklet);
    } else {
        cam_err("%s err irq status 0x%x ",__func__, value);
    }

    /*clr jpeg irq*/
    REG_SET(base + JPGENC_JPE_STATUS_ICR_REG, 0x30);
    return IRQ_HANDLED;
}

void hjpeg150_dump_file(char *filename, unsigned long addr, u32 size)
{
#if DUMP_FILE
    struct file *file1 = NULL;
    mm_segment_t old_fs;
    memset(&old_fs, 0x0, sizeof(old_fs));

    cam_info("dumpfile %s with size %u", filename, size);
    if (filename == NULL) {
        printk("dump file with NULL file name!");
        return;
    }
    file1 = filp_open(filename, O_CREAT | O_RDWR, 0644);
    if (IS_ERR(file1)) {
        cam_err("error occured while opening file %s, exiting...\n", filename);
        return;
    } else {
        old_fs = get_fs();
        set_fs(KERNEL_DS);
        file1->f_op->write(file1, (char *)addr, size, &file1->f_pos);
        set_fs(old_fs);
        filp_close(file1, NULL);
        return;
    }
#endif
}

static int hjpeg150_res_init(struct device *pdev )
{
    struct device_node *of_node = pdev->of_node;
    uint32_t base_array[2] = {0};
    uint32_t count = 0;
    int ret = -1;


    /* property(hisi,isp-base) = <address, size>, so count is 2 */
    count = 2;
    if (of_node) {
        ret = of_property_read_u32_array(of_node, "huawei,hjpeg150-base",
                base_array, count);
        if (ret < 0) {
            cam_err("%s failed line %d", __func__, __LINE__);
            return ret;
        }
    } else {
        cam_err("%s hjpeg150 of_node is NULL.%d", __func__, __LINE__);
        return -ENXIO;
    }

    s_hjpeg150.phyaddr = base_array[0];
    s_hjpeg150.mem_size = base_array[1];

    s_hjpeg150.viraddr = ioremap_nocache(s_hjpeg150.phyaddr, s_hjpeg150.mem_size);

    if (IS_ERR_OR_NULL(s_hjpeg150.viraddr)) {
        cam_err("%s ioremap fail", __func__);
        return -ENXIO;
    }

    s_hjpeg150.irq_no = irq_of_parse_and_map(of_node, 0);
    if (s_hjpeg150.irq_no  <= 0) {
        cam_err("%s failed line %d\n", __func__, __LINE__);
        goto fail;
    }

    cam_info("%s hijpeg150 base address = %p. hjpeg150-base size = 0x%x. hjpeg150-irq = %d viraddr = %p",
            __func__,(void *)(s_hjpeg150.phyaddr), s_hjpeg150.mem_size, s_hjpeg150.irq_no, s_hjpeg150.viraddr);


    /*request irq*/
    ret = request_irq(s_hjpeg150.irq_no, hjpeg150_irq_handler, 0, "hjpeg150_irq", 0);

    if (ret != 0) {
        cam_err("fail to request irq [%d], error: %d", s_hjpeg150.irq_no, ret);
        ret = -ENXIO;
        goto fail;
    }

    s_hjpeg150.hw_ctl.cvdr_viraddr = ioremap_nocache(CVDR_SRT_BASE, 4096);
    if (IS_ERR_OR_NULL(s_hjpeg150.hw_ctl.cvdr_viraddr)){
        cam_err("%s (%d) remap cvdr viraddr failed",__func__, __LINE__);
        ret = -ENXIO;
        goto fail;
    }

    return 0;

fail:
    if (s_hjpeg150.viraddr){
        iounmap((void*)s_hjpeg150.viraddr);
        s_hjpeg150.viraddr = NULL;
    }

    if (s_hjpeg150.irq_no) {
        free_irq(s_hjpeg150.irq_no, 0);
        s_hjpeg150.irq_no = 0;
    }

    return ret;
}

static int hjpeg150_res_deinit(struct device *pdev)
{
    if (s_hjpeg150.viraddr){
        iounmap((void*)s_hjpeg150.viraddr);
        s_hjpeg150.viraddr = NULL;
    }

    if (s_hjpeg150.irq_no) {
        free_irq(s_hjpeg150.irq_no, 0);
        s_hjpeg150.irq_no = 0;
    }

    if(s_hjpeg150.hw_ctl.cvdr_viraddr)
        iounmap((void *)s_hjpeg150.hw_ctl.cvdr_viraddr);

    return 0;
}

static int hjpeg150_enable_autogating(void)
{
    void __iomem*  base_addr = s_hjpeg150.viraddr;

    cam_info("%s enter ",__func__);
    REG_SET(base_addr + JPGENC_FORCE_CLK_ON_CFG_REG, 0x0);

    return 0;
}

static int hjpeg150_disable_autogating(void)
{
    void __iomem*  base_addr = s_hjpeg150.viraddr;

    cam_info("%s enter ",__func__);
    REG_SET(base_addr + JPGENC_FORCE_CLK_ON_CFG_REG, 0x1);

    return 0;
}


static void hjpeg150_hufftable_init(void)
{
    int tmp;
    uint32_t i;
    uint32_t length_bit,length_value,length;
    void __iomem* base_addr = s_hjpeg150.viraddr;


    cam_info("%s enter ",__func__);
    /*DC 0 */
    length_bit = ARRAY_SIZE(luma_dc_bits);
    length_value = ARRAY_SIZE(luma_dc_value);
    length = length_bit + length_value;

    REG_SET(base_addr + JPGENC_JPE_TDC0_LEN_REG,length);
    REG_SET(base_addr + JPGENC_JPE_TABLE_ID_REG,4);
    for(i = 1;i < length_bit;i = i + 2){
        tmp = 0;
        REG_SET_FIELD(tmp,JPGENC_TABLE_WDATA_H,luma_dc_bits[i - 1]);
        REG_SET_FIELD(tmp,JPGENC_TABLE_WDATA_L,luma_dc_bits[i]);
        REG_SET(base_addr + JPGENC_JPE_TABLE_DATA_REG,tmp);
    }
    for(i = 1;i < length_value;i += 2){
        tmp = 0;
        REG_SET_FIELD(tmp,JPGENC_TABLE_WDATA_H,luma_dc_value[i - 1]);
        REG_SET_FIELD(tmp,JPGENC_TABLE_WDATA_L,luma_dc_value[i]);
        REG_SET(base_addr + JPGENC_JPE_TABLE_DATA_REG,tmp);
    }

    /*AC 0 */
    length_bit = ARRAY_SIZE(luma_ac_bits);
    length_value = ARRAY_SIZE(luma_ac_value);
    length = length_bit + length_value;
    REG_SET(base_addr + JPGENC_JPE_TAC0_LEN_REG,length);
    REG_SET(base_addr + JPGENC_JPE_TABLE_ID_REG,5);
    for(i = 1;i < length_bit;i = i + 2){
        tmp = 0;
        REG_SET_FIELD(tmp,JPGENC_TABLE_WDATA_H,luma_ac_bits[i - 1]);
        REG_SET_FIELD(tmp,JPGENC_TABLE_WDATA_L,luma_ac_bits[i]);
        REG_SET(base_addr + JPGENC_JPE_TABLE_DATA_REG,tmp);
    }
    for(i = 1;i < length_value;i = i + 2){
        tmp = 0;
        REG_SET_FIELD(tmp,JPGENC_TABLE_WDATA_H,luma_ac_value[i - 1]);
        REG_SET_FIELD(tmp,JPGENC_TABLE_WDATA_L,luma_ac_value[i]);
        REG_SET(base_addr + JPGENC_JPE_TABLE_DATA_REG,tmp);
    }

    /*DC 1 */
    length_bit = ARRAY_SIZE(chroma_dc_bits);
    length_value = ARRAY_SIZE(chroma_dc_value);
    length = length_bit + length_value;
    REG_SET(base_addr + JPGENC_JPE_TDC1_LEN_REG,length);
    REG_SET(base_addr + JPGENC_JPE_TABLE_ID_REG,6);
    for(i = 1;i < length_bit;i = i + 2){
        tmp = 0;
        REG_SET_FIELD(tmp,JPGENC_TABLE_WDATA_H,chroma_dc_bits[i - 1]);
        REG_SET_FIELD(tmp,JPGENC_TABLE_WDATA_L,chroma_dc_bits[i]);
        REG_SET(base_addr + JPGENC_JPE_TABLE_DATA_REG,tmp);
    }
    for(i = 1;i < length_value;i = i + 2){
        tmp = 0;
        REG_SET_FIELD(tmp,JPGENC_TABLE_WDATA_H,chroma_dc_value[i - 1]);
        REG_SET_FIELD(tmp,JPGENC_TABLE_WDATA_L,chroma_dc_value[i]);
        REG_SET(base_addr + JPGENC_JPE_TABLE_DATA_REG,tmp);
    }

    /*AC 1 */
    length_bit = ARRAY_SIZE(chroma_ac_bits);
    length_value = ARRAY_SIZE(chroma_ac_value);
    length = length_bit + length_value;
    REG_SET(base_addr + JPGENC_JPE_TAC1_LEN_REG,length);
    REG_SET(base_addr + JPGENC_JPE_TABLE_ID_REG,7);
    for(i = 1;i < length_bit;i = i + 2){
        tmp = 0;
        REG_SET_FIELD(tmp,JPGENC_TABLE_WDATA_H,chroma_ac_bits[i - 1]);
        REG_SET_FIELD(tmp,JPGENC_TABLE_WDATA_L,chroma_ac_bits[i]);
        REG_SET(base_addr + JPGENC_JPE_TABLE_DATA_REG,tmp);
    }
    for(i = 1;i < length_value;i = i + 2){
        tmp = 0;
        REG_SET_FIELD(tmp,JPGENC_TABLE_WDATA_H,chroma_ac_value[i - 1]);
        REG_SET_FIELD(tmp,JPGENC_TABLE_WDATA_L,chroma_ac_value[i]);
        REG_SET(base_addr + JPGENC_JPE_TABLE_DATA_REG,tmp);
    }
    /*end*/
}

static void hjpeg150_qtable_init(void)
{
    int length;
    int i;
    unsigned int tmp;
    void __iomem*  base_addr = s_hjpeg150.viraddr;


    cam_info("%s enter ",__func__);
    /*q-table 0*/
    length = ARRAY_SIZE(luma_qtable1);
    REG_SET(base_addr + JPGENC_JPE_TABLE_ID_REG,0);
    for(i = 1;i < length;i = i + 2)
    {
        tmp = 0;
        REG_SET_FIELD(tmp,JPGENC_TABLE_WDATA_H,luma_qtable1[i - 1]);
        REG_SET_FIELD(tmp,JPGENC_TABLE_WDATA_L,luma_qtable1[i]);
        REG_SET(base_addr + JPGENC_JPE_TABLE_DATA_REG,tmp);
    }

    /*q-table 1*/
    length = ARRAY_SIZE(chroma_qtable1);
    REG_SET(base_addr + JPGENC_JPE_TABLE_ID_REG,1);
    for(i = 1;i < length;i = i + 2)
    {
        tmp = 0;
        REG_SET_FIELD(tmp,JPGENC_TABLE_WDATA_H,chroma_qtable1[i - 1]);
        REG_SET_FIELD(tmp,JPGENC_TABLE_WDATA_L,chroma_qtable1[i]);
        REG_SET(base_addr + JPGENC_JPE_TABLE_DATA_REG,tmp);
    }

    /*q-table 2*/
    length = ARRAY_SIZE(luma_qtable2);
    REG_SET(base_addr + JPGENC_JPE_TABLE_ID_REG,2);
    for(i = 0;i < length;i = i + 2)
    {
        tmp = 0;
        REG_SET_FIELD(tmp,JPGENC_TABLE_WDATA_H,luma_qtable2[i - 1]);
        REG_SET_FIELD(tmp,JPGENC_TABLE_WDATA_L,luma_qtable2[i]);
        REG_SET(base_addr + JPGENC_JPE_TABLE_DATA_REG,tmp);
    }

    /*q-table 3*/
    length = ARRAY_SIZE(chroma_qtable2);
    REG_SET(base_addr + JPGENC_JPE_TABLE_ID_REG,3);
    for(i = 0;i < length;i = i + 2)
    {
        tmp = 0;
        REG_SET_FIELD(tmp,JPGENC_TABLE_WDATA_H,chroma_qtable2[i - 1]);
        REG_SET_FIELD(tmp,JPGENC_TABLE_WDATA_L,chroma_qtable2[i]);
        REG_SET(base_addr + JPGENC_JPE_TABLE_DATA_REG,tmp);
    }
    /*end*/
}

static void hjpeg150_prefetch_init(void)
{
    uint32_t tmp = 0;
    void __iomem* base_addr = s_hjpeg150.viraddr;


    cam_info("%s enter ",__func__);
#ifdef SMMU
    REG_SET_FIELD(tmp,JPGENC_PREFETCH_EN,1);
#else
    REG_SET_FIELD(tmp,JPGENC_PREFETCH_EN,0);
#endif

    REG_SET_FIELD(tmp,JPGENC_PREFETCH_DELAY,1210);
    REG_SET(base_addr + JPGENC_PREFETCH_REG,tmp);

    tmp = 0;
    REG_SET_FIELD(tmp,JPGENC_ID0_Y,21);
    REG_SET_FIELD(tmp,JPGENC_ID1_Y,22);
    REG_SET_FIELD(tmp,JPGENC_ID2_Y,23);
    REG_SET_FIELD(tmp,JPGENC_ID3_Y,24);
    REG_SET(base_addr + JPGENC_PREFETCH_IDY0_REG,tmp);

    tmp = 0;
    REG_SET_FIELD(tmp,JPGENC_ID4_Y,25);
    REG_SET_FIELD(tmp,JPGENC_ID5_Y,26);
    REG_SET_FIELD(tmp,JPGENC_ID6_Y,27);
    REG_SET_FIELD(tmp,JPGENC_ID7_Y,28);
    REG_SET(base_addr + JPGENC_PREFETCH_IDY1_REG,tmp);

    tmp = 0;
    REG_SET_FIELD(tmp,JPGENC_ID0_UV,30);
    REG_SET_FIELD(tmp,JPGENC_ID1_UV,31);
    REG_SET_FIELD(tmp,JPGENC_ID2_UV,32);
    REG_SET_FIELD(tmp,JPGENC_ID3_UV,33);
    REG_SET(base_addr + JPGENC_PREFETCH_IDUV0_REG,tmp);

    tmp = 0;
    REG_SET_FIELD(tmp,JPGENC_ID8_Y,29);
    REG_SET_FIELD(tmp,JPGENC_ID4_UV,34);
    REG_SET(base_addr + JPGENC_PREFETCH_IDUVY_REG,tmp);
}

static void hjpeg150_rstmarker_init(void)
{
    void __iomem*  base_addr = s_hjpeg150.viraddr;

    cam_info("%s enter ",__func__);
    REG_SET(base_addr + JPGENC_JPE_RESTART_INTERVAL_REG,JPGENC_RESTART_INTERVAL);
}

static void hjpeg150_rstmarker(unsigned int rst)
{
    void __iomem*  base_addr = s_hjpeg150.viraddr;

    cam_info("%s enter , rst = %d",__func__, rst);
    REG_SET(base_addr + JPGENC_JPE_RESTART_INTERVAL_REG,rst);
}

static void hjpeg150_synccfg_init(void)
{
    void __iomem* base_addr = s_hjpeg150.viraddr;
    uint32_t tmp       = 0;

    cam_info("%s enter ",__func__);
    REG_SET_FIELD(tmp,JPGENC_SOURCE, 1);
    REG_SET_FIELD(tmp,JPGENC_SRAM_NOOPT, 1);
    REG_SET(base_addr + JPGENC_SYNCCFG_REG,tmp);
}

static void hjpeg150_add_header(jpgenc_config_t* config)
{
    uint32_t i;
    int scaler;
    long temp;

    uint32_t length        = 0;
    uint32_t width         = config->buffer.width;
    uint32_t height        = config->buffer.height;
    uint32_t quality       = config->buffer.quality;
    encode_format_e format = config->buffer.format;
    unsigned int rst = config->buffer.rst;

#ifdef SMMU
    unsigned char* jpgenc_addr = (unsigned char*)config->buffer.ion_vaddr;
#else
    unsigned char* jpgenc_addr = ioremap_nocache(JPEG_OUTPUT_PHYADDR, JPGENC_HEAD_SIZE);
    if(0 == jpgenc_addr){
        cam_err("%s(%d) remap fail", __func__, __LINE__);
        return;
    }
#endif

    unsigned char* org_jpeg_addr = jpgenc_addr;

    *jpgenc_addr = JPGENC_HEAD_OFFSET;
    jpgenc_addr += JPGENC_HEAD_OFFSET;

    length = ARRAY_SIZE(header_soi);
    for(i = 0;i < length;i ++){
        *jpgenc_addr = header_soi[i];
        jpgenc_addr ++;
    }

    length = ARRAY_SIZE(header_app0);
    for(i = 0;i < length;i ++){
        *jpgenc_addr = header_app0[i];
        jpgenc_addr ++;
    }

    length = ARRAY_SIZE(header_qtable0);
    for(i = 0;i < length;i ++){
        *jpgenc_addr = header_qtable0[i];
        jpgenc_addr ++;
    }

    if(quality == 0){
        length = ARRAY_SIZE(luma_qtable1);
        for(i = 0;i < length;i ++) {
            *jpgenc_addr = luma_qtable1[i];
            jpgenc_addr ++;
        }
    }else{
        if(quality < 50)
            scaler = 5000/quality;
        else
            scaler = 200 - quality*2;

        length = ARRAY_SIZE(luma_qtable2);
        for(i = 0; i < length; i ++){
            temp = (luma_qtable2[i] * scaler + 50L)/100L;
            if(temp <= 0L)
                temp = 1L;
            if(temp >255L)
                temp = 255L;
            *jpgenc_addr = temp;
            jpgenc_addr ++;
        }
    }

    length = ARRAY_SIZE(header_qtable1);
    for(i = 0; i < length; i ++){
        *jpgenc_addr = header_qtable1[i];
        jpgenc_addr ++;
    }

    if(quality == 0){
        length = ARRAY_SIZE(chroma_qtable1);
        for(i = 0;i < length;i ++){
            *jpgenc_addr = chroma_qtable1[i];
            jpgenc_addr ++;
        }
    }else{
        if(quality < 50)
            scaler = 5000/quality;
        else
            scaler = 200 - quality*2;

        length = ARRAY_SIZE(chroma_qtable2);
        for(i = 0;i < length;i ++) {
            temp = (chroma_qtable2[i]*scaler + 50L)/100L;
            if(temp <= 0L)
                temp = 1L;
            if(temp >255L)
                temp = 255L;
            *jpgenc_addr = temp;
            jpgenc_addr ++;
        }
    }

    *jpgenc_addr = 0xff;
    jpgenc_addr ++;
    *jpgenc_addr = 0xc0;
    jpgenc_addr ++;
    *jpgenc_addr = 0x00;
    jpgenc_addr ++;
    *jpgenc_addr = 0x11;
    jpgenc_addr ++;
    *jpgenc_addr = 0x08;
    jpgenc_addr ++;
    *jpgenc_addr = height/256;
    jpgenc_addr ++;
    *jpgenc_addr = height%256;
    jpgenc_addr ++;
    *jpgenc_addr = width/256;
    jpgenc_addr ++;
    *jpgenc_addr = width%256;
    jpgenc_addr ++;
    *jpgenc_addr = 0x03;
    jpgenc_addr ++;
    *jpgenc_addr = 0x01;
    jpgenc_addr ++;
    if(JPGENC_FORMAT_YUV422 == (format & JPGENC_FORMAT_BIT))
        *jpgenc_addr = 0x21;
    else
        *jpgenc_addr = 0x22;
    jpgenc_addr ++;
    *jpgenc_addr = 0x00;
    jpgenc_addr ++;

    *jpgenc_addr = 0x02;//0x02:nv12 0x03:nv21
    jpgenc_addr ++;
    *jpgenc_addr = 0x11;
    jpgenc_addr ++;
    *jpgenc_addr = 0x01;
    jpgenc_addr ++;
    *jpgenc_addr = 0x03;//0x03:nv12 0x02:nv21
    jpgenc_addr ++;
    *jpgenc_addr = 0x11;
    jpgenc_addr ++;
    *jpgenc_addr = 0x01;
    jpgenc_addr ++;

    length = ARRAY_SIZE(header_hufftable_dc0);
    for(i = 0;i < length;i ++){
        *jpgenc_addr = header_hufftable_dc0[i];
        jpgenc_addr ++;
    }

    length = ARRAY_SIZE(luma_dc_bits);
    for(i = 0;i < length;i ++){
        *jpgenc_addr = luma_dc_bits[i];
        jpgenc_addr ++;
    }

    length = ARRAY_SIZE(luma_dc_value);
    for(i = 0;i < length;i ++){
        *jpgenc_addr = luma_dc_value[i];
        jpgenc_addr ++;
    }

    length = ARRAY_SIZE(header_hufftable_ac0);
    for(i = 0;i < length;i ++){
        *jpgenc_addr = header_hufftable_ac0[i];
        jpgenc_addr ++;
    }

    length = ARRAY_SIZE(luma_ac_bits);
    for(i = 0;i < length;i ++){
        *jpgenc_addr = luma_ac_bits[i];
        jpgenc_addr ++;
    }

    length = ARRAY_SIZE(luma_ac_value);
    for(i = 0;i < length;i ++){
        *jpgenc_addr = luma_ac_value[i];
        jpgenc_addr ++;
    }

    length = ARRAY_SIZE(header_hufftable_dc1);
    for(i = 0;i < length;i ++){
        *jpgenc_addr = header_hufftable_dc1[i];
        jpgenc_addr++;
    }

    length = ARRAY_SIZE(chroma_dc_bits);
    for(i = 0;i < length;i ++){
        *jpgenc_addr = chroma_dc_bits[i];
        jpgenc_addr ++;
    }

    length = ARRAY_SIZE(chroma_dc_value);
    for(i = 0;i < length;i ++){
        *jpgenc_addr = chroma_dc_value[i];
        jpgenc_addr ++;
    }

    length = ARRAY_SIZE(header_hufftable_ac1);
    for(i = 0;i < length;i ++){
        *jpgenc_addr = header_hufftable_ac1[i];
        jpgenc_addr ++;
    }

    length = ARRAY_SIZE(chroma_ac_bits);
    for(i = 0;i < length;i ++){
        *jpgenc_addr = chroma_ac_bits[i];
        jpgenc_addr ++;
    }

    length = ARRAY_SIZE(chroma_ac_value);
    for(i = 0;i < length;i ++){
        *jpgenc_addr = chroma_ac_value[i];
        jpgenc_addr ++;
    }

    /* add DRI */
    *jpgenc_addr = 0xff;
    jpgenc_addr ++;
    *jpgenc_addr = 0xdd;
    jpgenc_addr ++;
    *jpgenc_addr = 0x00;
    jpgenc_addr ++;
    *jpgenc_addr = 0x04;
    jpgenc_addr ++;
    *jpgenc_addr = rst/256;
    jpgenc_addr ++;
    *jpgenc_addr = rst%256;
    jpgenc_addr ++;

    *jpgenc_addr = 0xff;
    jpgenc_addr ++;
    *jpgenc_addr = 0xda;
    jpgenc_addr ++;
    *jpgenc_addr = 0X00;
    jpgenc_addr ++;
    *jpgenc_addr = 0X0C;
    jpgenc_addr ++;
    *jpgenc_addr = 0x03;
    jpgenc_addr ++;
    *jpgenc_addr = 0x01;
    jpgenc_addr ++;
    *jpgenc_addr = 0x00;
    jpgenc_addr ++;
    *jpgenc_addr = 0x02;
    jpgenc_addr ++;
    *jpgenc_addr = 0x11;
    jpgenc_addr ++;
    *jpgenc_addr = 0x03;
    jpgenc_addr ++;
    *jpgenc_addr = 0x11;
    jpgenc_addr ++;
    *jpgenc_addr = 0x00;
    jpgenc_addr ++;
    *jpgenc_addr = 0x3F;
    jpgenc_addr ++;
    *jpgenc_addr = 0x00;

#ifdef SMMU
    if(org_jpeg_addr)
        org_jpeg_addr = NULL;
#else
    if(org_jpeg_addr)
        iounmap((void *)org_jpeg_addr);
#endif

    return;
}

static int check_rst(jpgenc_config_t *config)
{
    cam_debug("enter %s(%d)",__func__, __LINE__);
    int ret = 0;

    if (NULL == config) {
        cam_err(" config is null! %s(%d)",__func__, __LINE__);
        return -1;
    }

    switch(config->buffer.rst) {
        case JPGENC_RESTART_INTERVAL:
            break;
        case JPGENC_RESTART_INTERVAL_ON:
            hjpeg150_rstmarker(config->buffer.rst);
            cam_info("JPEG restart interval is on %s(%d)", __func__, __LINE__);
            break;
        default:
            cam_err("invalid value of \"rst\". %s(%d)", __func__, __LINE__);
            ret = -1;
            break;
    }

    return ret;
}

static int check_config(jpgenc_config_t* config)
{
    cam_info("enter %s(%d)",__func__, __LINE__);
    if(config == NULL){
        cam_err(" config is null! ");
        return -1;
    }

    if(!CHECK_ALIGN(config->buffer.width,2) || (config->buffer.width > 8192)){
        cam_err(" width[%d] is invalid! ",config->buffer.width);
        return -1;
    }

    if(0 == config->buffer.height || (config->buffer.height > 8192)){
        cam_err(" height[%d] is invalid! ",config->buffer.height);
        return -1;
    }

    if((0 == config->buffer.stride)
            || !CHECK_ALIGN(config->buffer.stride,16)
            || (config->buffer.stride/16 > ((JPGENC_FORMAT_YUV422 == (config->buffer.format & JPGENC_FORMAT_BIT)) ? 1024 : 512)))
    {
        cam_err(" stride[%d] is invalid! ",config->buffer.stride);
        return -1;
    }

    if(1 != config->buffer.stride % 256){
        cam_err(" stride[%d] need same as odd 128 ",config->buffer.stride);
    }

#if 0
    if((0 == config->buffer.input_buffer_y) || !CHECK_ALIGN(config->buffer.input_buffer_y,16)){
        cam_err(" input buffer y[0x%x] is invalid! ",config->buffer.input_buffer_y);
        return -1;
    }
    if((JPGENC_FORMAT_YUV420 == (config->buffer.format & JPGENC_FORMAT_BIT))
            && ((0== config->buffer.input_buffer_uv )|| !CHECK_ALIGN(config->buffer.input_buffer_uv, 16))){
        cam_err(" input buffer uv[0x%x] is invalid! ",config->buffer.input_buffer_uv);
        return -1;
    }

    if((JPGENC_FORMAT_YUV420 == (config->buffer.format & JPGENC_FORMAT_BIT))
            && (config->buffer.input_buffer_uv - config->buffer.input_buffer_y < config->buffer.stride*8*16)){
        cam_err(" buffer format is invalid! ");
        return -1;
    }
#endif

    if(config->buffer.quality > 100){
        cam_err(" quality[%d] is invalid, adjust to 100",config->buffer.quality);
        config->buffer.quality = 100;
    }

    return 0;
}

static int hjpeg150_cvdr_fmt_desc_vp_wr(uint32_t width, uint32_t height,
    uint32_t axi_address_start, cvdr_wr_fmt_desc_t *desc)
{
    /* uint32_t pix_size = 64; */
    /* uint32_t mbits_per_sec = pix_size * g_isp_clk; */
    //FIXME:this must be use ceil align up
    /* uint32_t num_DUs_per_line = width * pix_size / 8 / 128; */
    /* uint32_t total_num_bytes  = num_DUs_per_line * height * 128; */
    uint32_t total_num_bytes = width  * height * 8;
    if(0 == width){
       cam_err("width cannot be zero");
       return -1;
    }

    desc->pix_fmt        = DF_D64;
    desc->pix_expan      = EXP_PIX;
    //FIXME: this must bue user ceil to align up float to int
    /* desc->access_limiter = 32 * pix_size / 128  ; */
    desc->access_limiter = 16;
    desc->last_page      = (axi_address_start + total_num_bytes)>>15;
    desc->line_stride    = (width * 2) / 16 - 1;
    desc->line_wrap      = height;

    cam_info("%s (%d) acess_limiter = %d", __func__, __LINE__, desc->access_limiter);
    cam_info("%s (%d) last_page = %d", __func__, __LINE__, desc->last_page);
    cam_info("%s (%d) line_stride = %d", __func__, __LINE__, desc->line_stride);
    cam_info("%s (%d) width = %d,  height = %d", __func__, __LINE__, width, height);

    return 0;
}

#define PREFETCH_BY_PASS (1 << 31)
static int hjpeg150_set_vp_wr_ready(cvdr_wr_fmt_desc_t *desc, uint32_t buf_addr)
{
    void __iomem *cvdr_srt_base = s_hjpeg150.hw_ctl.cvdr_viraddr;//ioremap_nocache();
    int ret = 0;

    U_VP_WR_CFG tmp_cfg;
    tmp_cfg.reg32 = REG_GET(cvdr_srt_base + CVDR_SRT_VP_WR_CFG_25_OFFSET);
    tmp_cfg.bits.vpwr_pixel_format = desc->pix_fmt;
    tmp_cfg.bits.vpwr_pixel_expansion = desc->pix_expan;
    tmp_cfg.bits.vpwr_access_limiter = desc->access_limiter;
    tmp_cfg.bits.vpwr_last_page = desc->last_page;
    REG_SET(cvdr_srt_base + CVDR_SRT_VP_WR_CFG_25_OFFSET, tmp_cfg.reg32);

    U_VP_WR_AXI_LINE tmp_line;
    tmp_line.reg32 = REG_GET(cvdr_srt_base + CVDR_SRT_VP_WR_AXI_LINE_25_OFFSET);
    tmp_line.bits.vpwr_line_stride = desc->line_stride;
    tmp_line.bits.vpwr_line_wrap = desc->line_wrap;
    REG_SET(cvdr_srt_base + CVDR_SRT_VP_WR_AXI_LINE_25_OFFSET, tmp_line.reg32);

    U_VP_WR_AXI_FS tmp_fs;
    tmp_fs.reg32 = REG_GET(cvdr_srt_base + CVDR_SRT_VP_WR_AXI_FS_25_OFFSET);
    tmp_fs.bits.vpwr_address_frame_start = buf_addr >> 4;
    REG_SET(cvdr_srt_base + CVDR_SRT_VP_WR_AXI_FS_25_OFFSET, tmp_fs.reg32);
#ifndef SMMU
    REG_SET(cvdr_srt_base + CVDR_SRT_VP_WR_IF_CFG_25_OFFSET,(REG_GET(cvdr_srt_base + CVDR_SRT_VP_WR_IF_CFG_25_OFFSET)|(PREFETCH_BY_PASS )));
#endif
    return ret;
}

static int hjpeg150_set_nr_rd_config(unsigned char du, unsigned char limiter)
{
    void __iomem *cvdr_srt_base = s_hjpeg150.hw_ctl.cvdr_viraddr;//ioremap_nocache();
    U_NR_RD_CFG tmp;
    tmp.reg32 = REG_GET( cvdr_srt_base + CVDR_NR_RD_CFG1_OFFSET );
    tmp.bits.nrrd_allocated_du = du;
    tmp.bits.nrrd_access_limiter = limiter;
    tmp.bits.nrrd_enable = 1;
    REG_SET(cvdr_srt_base + CVDR_NR_RD_CFG1_OFFSET , tmp.reg32);
    return 0;
}

static void hjpeg150_do_cvdr_config(jpgenc_config_t* config)
{
    int ret = 0;
    uint32_t width = config->buffer.width;
    uint32_t height = config->buffer.height;
    uint32_t buf_addr = config->buffer.output_buffer + JPGENC_HEAD_SIZE;
    cvdr_wr_fmt_desc_t cvdr_wr_fmt;
    //FIXME:use ceil align up float to int
    unsigned char access_limiter = ACCESS_LIMITER;
    unsigned char allocated_du = ALLOCATED_DU;

    cam_info("enter %s(%d)", __func__, __LINE__);
    ret =  hjpeg150_cvdr_fmt_desc_vp_wr(width, height, buf_addr, &cvdr_wr_fmt);
    if(0 != ret){
        cam_err("%s (%d) config cvdr failed", __func__, __LINE__);
        return;
    }

    ret = hjpeg150_set_vp_wr_ready(&cvdr_wr_fmt, buf_addr);
    if(0 != ret){
        cam_err("%s (%d) set vp wr ready fail", __func__, __LINE__);
    }

    hjpeg150_set_nr_rd_config(allocated_du, access_limiter);
}

static void hjpeg150_do_config(jpgenc_config_t* config)
{
    int scaler;
    unsigned int tmp;
    int length;
    uint32_t temp;
    int i;

    uint32_t width_left  = config->buffer.width;
    uint32_t width_right = 0;
    void __iomem*  base_addr = s_hjpeg150.viraddr;


    cam_info("%s enter ",__func__);
    if(width_left >= 64)
    {
        width_left = ALIGN_DOWN((width_left/2), 16);
    }
    width_right = config->buffer.width - width_left;

    tmp = 0;
    if(JPGENC_FORMAT_YUV422 == (config->buffer.format & JPGENC_FORMAT_BIT))//YUV422
    {
        SET_FIELD_TO_REG(base_addr + JPGENC_JPE_PIC_FORMAT_REG,JPGENC_ENC_PIC_FORMAT,0);
        if (JPGENC_FORMAT_UYVY == config->buffer.format)//deafult format
        {
            REG_SET_FIELD(tmp,JPGENC_SWAPIN_Y_UV, 0);
            REG_SET_FIELD(tmp,JPGENC_SWAPIN_U_V, 0);
        }
        else if (JPGENC_FORMAT_VYUY == config->buffer.format)
        {
            REG_SET_FIELD(tmp,JPGENC_SWAPIN_Y_UV, 0);
            REG_SET_FIELD(tmp,JPGENC_SWAPIN_U_V, 1);
        }
        else if (JPGENC_FORMAT_YVYU == config->buffer.format)
        {
            REG_SET_FIELD(tmp,JPGENC_SWAPIN_Y_UV, 1);
            REG_SET_FIELD(tmp,JPGENC_SWAPIN_U_V, 1);
        }
        else//JPGENC_FORMAT_YUYV
        {
            REG_SET_FIELD(tmp,JPGENC_SWAPIN_Y_UV, 1);
            REG_SET_FIELD(tmp,JPGENC_SWAPIN_U_V, 0);
        }
        REG_SET(base_addr + JPGENC_INPUT_SWAP_REG,tmp);
    }
    else if(JPGENC_FORMAT_NV12 == config->buffer.format)
    {
            SET_FIELD_TO_REG(base_addr + JPGENC_JPE_PIC_FORMAT_REG,JPGENC_ENC_PIC_FORMAT,1);
            REG_SET_FIELD(tmp,JPGENC_SWAPIN_Y_UV, 0);
            REG_SET_FIELD(tmp,JPGENC_SWAPIN_U_V, 0);
            REG_SET(base_addr + JPGENC_INPUT_SWAP_REG,tmp);
    }
    else//JPGENC_FORMAT_NV21
    {
            SET_FIELD_TO_REG(base_addr + JPGENC_JPE_PIC_FORMAT_REG,JPGENC_ENC_PIC_FORMAT,1);
            REG_SET_FIELD(tmp,JPGENC_SWAPIN_Y_UV, 0);
            REG_SET_FIELD(tmp,JPGENC_SWAPIN_U_V, 1);//swap U and V
            REG_SET(base_addr + JPGENC_INPUT_SWAP_REG,tmp);
    }

    SET_FIELD_TO_REG(base_addr + JPGENC_JPE_ENC_HRIGHT1_REG,JPGENC_ENC_HRIGHT1,width_left -1);
    SET_FIELD_TO_REG(base_addr + JPGENC_JPE_ENC_VBOTTOM_REG,JPGENC_ENC_VBOTTOM,config->buffer.height - 1);
    SET_FIELD_TO_REG(base_addr + JPGENC_JPE_ENC_HRIGHT2_REG,JPGENC_ENC_HRIGHT2,width_right != 0 ? width_right -1 : 0);

    if(config->buffer.quality == 0)
    {
        SET_FIELD_TO_REG(base_addr + JPGENC_JPE_TQ_Y_SELECT_REG,JPGENC_TQ0_SELECT,0);
        SET_FIELD_TO_REG(base_addr + JPGENC_JPE_TQ_U_SELECT_REG,JPGENC_TQ1_SELECT,1);
        SET_FIELD_TO_REG(base_addr + JPGENC_JPE_TQ_V_SELECT_REG,JPGENC_TQ2_SELECT,1);
    }
    else
    {
        if(config->buffer.quality < 50)
            scaler = 5000/config->buffer.quality;
        else
            scaler = 200 - config->buffer.quality*2;

        /*q-table 2*/
        length = ARRAY_SIZE(luma_qtable2);
        SET_FIELD_TO_REG(base_addr + JPGENC_JPE_TABLE_ID_REG,JPGENC_TABLE_ID,2);
        for(i = 1;i < length;i = i + 2)
        {
            temp = (luma_qtable2[i - 1]*scaler + 50L)/100L;
            if(temp <= 0L)
                temp = 1L;
            if(temp >255L)
                temp = 255L;
            tmp = 0;
            REG_SET_FIELD(tmp,JPGENC_TABLE_WDATA_H,temp);
            temp = (luma_qtable2[i]*scaler + 50L)/100L;
            if(temp <= 0L)
                temp = 1L;
            if(temp >255L)
                temp = 255L;
            REG_SET_FIELD(tmp,JPGENC_TABLE_WDATA_L,temp);
            REG_SET(base_addr + JPGENC_JPE_TABLE_DATA_REG,tmp);
        }

        /*q-table 3*/
        length = ARRAY_SIZE(chroma_qtable2);
        SET_FIELD_TO_REG(base_addr + JPGENC_JPE_TABLE_ID_REG,JPGENC_TABLE_ID,3);
        for(i = 1;i < length;i = i + 2)
        {
            temp = (chroma_qtable2[i - 1]*scaler + 50L)/100L;
            if(temp <= 0L)
                temp = 1L;
            if(temp >255L)
                temp = 255L;
            tmp = 0;
            REG_SET_FIELD(tmp,JPGENC_TABLE_WDATA_H,temp);
            temp = (chroma_qtable2[i]*scaler + 50L)/100L;
            if(temp <= 0L)
                temp = 1L;
            if(temp >255L)
                temp = 255L;
            REG_SET_FIELD(tmp,JPGENC_TABLE_WDATA_L,temp);
            REG_SET(base_addr + JPGENC_JPE_TABLE_DATA_REG,tmp);
        }

        SET_FIELD_TO_REG(base_addr + JPGENC_JPE_TQ_Y_SELECT_REG,JPGENC_TQ0_SELECT,2);
        SET_FIELD_TO_REG(base_addr + JPGENC_JPE_TQ_U_SELECT_REG,JPGENC_TQ1_SELECT,3);
        SET_FIELD_TO_REG(base_addr + JPGENC_JPE_TQ_V_SELECT_REG,JPGENC_TQ2_SELECT,3);
    }

    SET_FIELD_TO_REG(base_addr + JPGENC_ADDRESS_Y_REG,JPGENC_ADDRESS_Y,config->buffer.input_buffer_y >> 4);

    if(JPGENC_FORMAT_YUV420 == (config->buffer.format & JPGENC_FORMAT_BIT)) {
        SET_FIELD_TO_REG(base_addr + JPGENC_ADDRESS_UV_REG,JPGENC_ADDRESS_UV,config->buffer.input_buffer_uv >> 4);
    }

    if(JPGENC_FORMAT_YUV420 == (config->buffer.format & JPGENC_FORMAT_BIT))
        SET_FIELD_TO_REG(base_addr + JPGENC_PREREAD_REG,JPGENC_PREREAD,4);
    else
        SET_FIELD_TO_REG(base_addr + JPGENC_PREREAD_REG,JPGENC_PREREAD,0);

    SET_FIELD_TO_REG(base_addr + JPGENC_STRIDE_REG,JPGENC_STRIDE,config->buffer.stride >> 4);
    SET_FIELD_TO_REG(base_addr + JPGENC_JPE_ENCODE_REG,JPGENC_ENCODE,1);
    SET_FIELD_TO_REG(base_addr + JPGENC_JPE_INIT_REG,JPGENC_JP_INIT,1);
}

static int hjpeg150_prepare_buf(jpgenc_config_t *cfg)
{
    cam_info("%s enter ",__func__);
    struct ion_handle* hdl = NULL;
    void *vaddr =  NULL;

    cam_info("%s: cfg->buffer.ion_fd is %d",__func__, cfg->buffer.ion_fd);

    /* check arg */
    if ((cfg->buffer.ion_fd <= 0) || IS_ERR_OR_NULL(s_hjpeg150.ion_client)) {
        cam_err("invalid ion: fd=%d, client=%p",
                cfg->buffer.ion_fd, s_hjpeg150.ion_client);
        return -EINVAL;
    }

    /* get hdl */
    hdl = ion_import_dma_buf(s_hjpeg150.ion_client, cfg->buffer.ion_fd);
    if (IS_ERR_OR_NULL(hdl)) {
        cam_err("failed to create ion handle!");
        return PTR_ERR(hdl);
    }
    cfg->buffer.ion_hdl  = hdl;

    /* map kernel addr */
    vaddr = ion_map_kernel(s_hjpeg150.ion_client, hdl);
    if (IS_ERR_OR_NULL(vaddr)) {
        cam_err("failed to map ion buffer(fd=%lx)!", (unsigned long)hdl);
        cfg->buffer.ion_hdl = NULL;
        cfg->buffer.ion_vaddr = NULL;
        ion_free(s_hjpeg150.ion_client, hdl);
        return PTR_ERR(vaddr);
    }

    cam_info("%s(%d) vaddr is %p hdl = %p",__func__, __LINE__, vaddr, hdl);
    cfg->buffer.ion_vaddr = vaddr;

    return 0;
}

static int hjpeg150_cfg_smmu(bool bypass)
{
    void __iomem* smmu_base_addr;
    cam_info("%s enter ",__func__);
    if(true ==  bypass){
        smmu_base_addr = ioremap_nocache(SMMU_BASE_ADDR, 8192);

        if(IS_ERR_OR_NULL(smmu_base_addr)){
            cam_err("%s(%d) remap failed",__func__, __LINE__);
            return -1;
        }
        REG_SET(smmu_base_addr + SMMU_GLOBAL_BYPASS, REG_GET(smmu_base_addr + SMMU_GLOBAL_BYPASS)|0x1) ;

        iounmap(smmu_base_addr);
    }

    return 0;
}

char *image_file_addr;
u32 image_file_size;
static int load_image_file(char *filename)
{
    struct kstat stat;
    mm_segment_t fs;
    struct file *fp = NULL;
    int file_flag = O_RDONLY;
    int ret = 0;


    if (NULL == filename) {
        cam_err("%s param error", __func__);
        return -EINVAL;
    }

    /* must have the following 2 statement */
    fs = get_fs();
    set_fs(KERNEL_DS);
    cam_info("%s (%d) opening file %s", __func__, __LINE__, filename);
    fp = filp_open(filename, file_flag, 0666);
    if (IS_ERR_OR_NULL(fp)) {
        cam_err("open file error!\n");
        set_fs(fs);
        return -ENOENT;
    }

    if (0 != vfs_stat(filename, &stat)) {
        cam_err("failed to get file stat!");
        ret = -EIO;
        goto ERROR;
    }

    image_file_size = stat.size;
    cam_notice("image file %s, file size : %d", filename, (u32) stat.size);
    ret = vfs_read(fp, (char *)image_file_addr, (u32) image_file_size, &fp->f_pos);
    if (ret != stat.size) {
        cam_err("read file error!, %s , ret=%d\n", filename, ret);
        ret = -EIO;
    } else {
        ret = 0;
    }

ERROR:
    /* must have the following 1 statement */
    set_fs(fs);
    filp_close(fp, 0);
    return ret;
}

static int dump_dbg_reg(void)
{
    int i;
    void __iomem *base = s_hjpeg150.viraddr;

    for(i = 0 ; i <= JPGENC_FORCE_CLK_ON_CFG_REG; ){
        cam_info("jpeg reg 0x%x = 0x%x", i, REG_GET(base + i));
        i += 4;
    }
    for(i = JPGENC_DBG_0_REG ; i <= JPGENC_DBG_13_REG; ){
        cam_info("dbg reg 0x%x = 0x%x", i, REG_GET(base + i));
        i += 4;
    }

    return 0;
}

static void hjpeg150_disabe_irq(void)
{
    void __iomem *base = s_hjpeg150.viraddr;
    cam_info("%s enter ",__func__);
    REG_SET(base + JPGENC_JPE_STATUS_IMR_REG, 0x00);
}

static void hjpeg150_enable_irq(void)
{
    void __iomem *base = s_hjpeg150.viraddr;
    cam_info("%s enter ",__func__);
    REG_SET(base + JPGENC_JPE_STATUS_IMR_REG, 0x30);
}

static int hjpeg150_encode_process(hjpeg_intf_t *i, void *cfg)
{
    cam_info("%s enter ",__func__);
    jpgenc_config_t* pcfg = NULL;
    long   jiffies = 0;
    int    ret = 0;
    u32 byte_cnt = 0;
    pcfg = (jpgenc_config_t *)cfg;


    cam_info("pcfg->buffer.width:%d\n\
            pcfg->buffer.height:%d\n\
            pcfg->buffer.stride:%d\n\
            pcfg->buffer.format:%#x\n\
            pcfg->buffer.quality:%d\n\
            pcfg->buffer.input_buffer_y:%#x\n\
            pcfg->buffer.input_buffer_uv:%#x\n\
            pcfg->buffer.output_buffer:%#x\n\
            pcfg->buffer.rst:%d\n\
            pcfg->buffer.ion_fd:%d\n",
            pcfg->buffer.width,
            pcfg->buffer.height,
            pcfg->buffer.stride,
            pcfg->buffer.format,
            pcfg->buffer.quality,
            pcfg->buffer.input_buffer_y,
            pcfg->buffer.input_buffer_uv,
            pcfg->buffer.output_buffer,
            pcfg->buffer.rst,
            pcfg->buffer.ion_fd);


    if(false == s_hjpeg150.power_on_state){
        cam_err("%s(%d)jpegenc is not powered on, encode processing terminated.",__func__, __LINE__);
        return -1;
    }

    if (0 != check_rst(pcfg)) {
        cam_err("%s(%d)checking rst failed, adjust to 0.",__func__, __LINE__);
    }

#ifndef CHIP_VERIFICATION
    ret = check_config(pcfg);
    if(0 != ret){
        cam_err("%s(%d)check_config failed, encode processing terminated.",__func__, __LINE__);
        return ret;
    }
#endif

#ifndef SMMU
    image_file_addr = ioremap_nocache(JPEG_INPUT_PHYADDR, (pcfg->buffer.width * pcfg->buffer.height) * 2);

    if(IS_ERR_OR_NULL(image_file_addr)){
        cam_err("%s(%d) remap failed",__func__, __LINE__);
        return -1;
    }

    load_image_file(&(pcfg->filename));
#endif

#ifdef SMMU
    hjpeg150_prepare_buf(pcfg);
#else
    hjpeg150_cfg_smmu(true);
#endif

    hjpeg150_disable_autogating();
    hjpeg150_do_cvdr_config(pcfg);
    hjpeg150_enable_irq();

    hjpeg150_do_config(pcfg);

    jiffies = msecs_to_jiffies(WAIT_ENCODE_TIMEOUT);
    if (down_timeout(&s_hjpeg150.buff_done, jiffies)) {
        cam_err("time out wait for jpeg encode");
        ret = -1;
    }

    hjpeg150_disabe_irq();
    hjpeg150_enable_autogating();

    hjpeg150_add_header(pcfg);

    cam_info("%s (%d) status irq is %d byt cnt is %x ", __func__, __LINE__,
            REG_GET(s_hjpeg150.viraddr + JPGENC_JPE_STATUS_RIS_REG),
            REG_GET(s_hjpeg150.viraddr + JPGENC_JPG_BYTE_CNT_REG ));

    byte_cnt = REG_GET(s_hjpeg150.viraddr + JPGENC_JPG_BYTE_CNT_REG );
    if(0 == byte_cnt){
        byte_cnt = pcfg->buffer.width * pcfg->buffer.height * 2 ;
    }else{
        byte_cnt += JPGENC_HEAD_SIZE;
    }
    pcfg->jpegSize = byte_cnt;

    cam_info("%s user byt cnt is %x ", __func__, pcfg->jpegSize);

#ifdef SMMU
    hjpeg150_dump_file("/data/img/out.jpg", pcfg->buffer.ion_vaddr, byte_cnt);

    ion_unmap_kernel(s_hjpeg150.ion_client, pcfg->buffer.ion_hdl);
    ion_free(s_hjpeg150.ion_client, pcfg->buffer.ion_hdl);

#else
    unsigned long addr = ioremap_nocache(JPEG_OUTPUT_PHYADDR, pcfg->buffer.width * pcfg->buffer.height * 2);
    hjpeg150_dump_file("/data/img/out.jpg", addr, byte_cnt);
    mb();

    iounmap((void *)addr);
    iounmap(image_file_addr);
#endif

    dump_dbg_reg();

    return ret;
}

void hjpeg150_init_hw_param(void)
{
    hjpeg150_hufftable_init();

    hjpeg150_qtable_init();

    hjpeg150_prefetch_init();

    hjpeg150_rstmarker_init();

    hjpeg150_synccfg_init();
}

static inline void hjpeg150_set_power_reg(unsigned long reg,unsigned int  value)
{
    void __iomem* regaddr;

    regaddr = ioremap_nocache(reg, 0x4);

    REG_SET(regaddr, value);

    iounmap(regaddr);
}

static int hjpeg150_clk_ctrl(bool flag)
{
    cam_info("%s enter\n",__func__);

    void __iomem* subctrl1;
    uint32_t set_clk;
    uint32_t cur_clk;
    int ret = 0;

    subctrl1 = ioremap_nocache(ISP_CORE_CTRL_1_REG, 0x4);

    if (true == flag)
        REG_SET(subctrl1, REG_GET(subctrl1)|0x1);
    else
        REG_SET(subctrl1, REG_GET(subctrl1)&0x0);

    set_clk = (true == flag ? 0x1 : 0x0);
    cur_clk = REG_GET(subctrl1);
    if (set_clk != cur_clk) {
        cam_err("%s(%d) isp jpeg clk status %d, clk write failed",__func__, __LINE__, cur_clk);
        ret = -EIO;
    }

    cam_info("%s(%d) isp jpeg clk status %d",__func__, __LINE__, cur_clk);
    iounmap(subctrl1);
    return ret;
}

static int hjpeg150_power_on(hjpeg_intf_t *i)
{
    cam_info("%s enter\n",__func__);
    int ret = 0;

    hjpeg150_t* phjpeg150 = NULL;

    phjpeg150 = I2HJPEG150(i);

    if(false == phjpeg150->power_on_state ){
        ret = hjpeg150_res_init(&phjpeg150->pdev->dev);
        if(ret != 0){
            cam_err("%s(%d) res init fail",__func__, __LINE__);
            return ret;
        }

        ret = hisp_jpeg_powerup();
        if(0 != ret){
            cam_err("%s(%d) jpeg power up fail",__func__, __LINE__);
            goto POWERUP_ERROR;
        }

        hjpeg150_clk_ctrl(true);
        /* ret = hjpeg150_clk_ctrl(true); */
        /* if (0 != ret) { */
        /*     cam_err("%s(%d) failed to open jpeg clk , prepare to power down jpeg!",__func__, __LINE__); */
        /*     goto FAILED_RET; */
        /* } */

        cam_info("%s (%d) reg 1 is %x ", __func__, __LINE__, REG_GET(s_hjpeg150.viraddr + 4));
        hjpeg150_init_hw_param();
        sema_init(&(phjpeg150->buff_done), 0);

        s_hjpeg150.ion_client = hisi_ion_client_create("hwcam-hjpeg");
        if (IS_ERR_OR_NULL(s_hjpeg150.ion_client )) {
            cam_err("failed to create ion client! \n");
            ret = -ENOMEM;
            goto FAILED_RET;
        }
        phjpeg150->power_on_state = true;
        cam_info("%s(%d) jpeg power on success",__func__, __LINE__);
    }
    return ret;

FAILED_RET:
    if(0 != hisp_jpeg_powerdn())
        cam_err("%s(%d) jpeg power down fail",__func__, __LINE__);

POWERUP_ERROR:
    if (0 != hjpeg150_res_deinit(&phjpeg150->pdev->dev))
        cam_err("%s(%d) res deinit fail",__func__, __LINE__);

    return ret;
}

static int hjpeg150_power_off(hjpeg_intf_t *i)
{
    int ret = 0;
    hjpeg150_t* phjpeg150 = NULL;
    phjpeg150 = I2HJPEG150(i);
    struct ion_client*  ion = NULL;


    if(true == phjpeg150->power_on_state){

        swap(s_hjpeg150.ion_client, ion);
        if (ion) {
            ion_client_destroy(ion);
        }

        if (0 != hjpeg150_clk_ctrl(false)) {
            cam_err("%s(%d) failed to close jpeg clk, continue to power down jpeg!",__func__, __LINE__);
        }

        ret = hisp_jpeg_powerdn();
        if(ret != 0){
            cam_err("%s(%d) jpeg power down fail",__func__, __LINE__);
        }

        if (0 != hjpeg150_res_deinit(&phjpeg150->pdev->dev)) {
            cam_err("%s(%d) res deinit fail",__func__, __LINE__);
        }

        phjpeg150->power_on_state = false;
        if (0 == ret)
            cam_info("%s(%d) jpeg power off success",__func__, __LINE__);
    }

    return ret;
}

static int hjpeg150_set_reg(hjpeg_intf_t* i, void* cfg)
{
    cam_info("%s enter\n",__func__);
    int ret = 0;
    hjpeg150_t* phjpeg150   = NULL;
    void __iomem* base_addr = 0;
    jpgenc_config_t* pcfg = NULL;
    uint32_t addr;
    uint32_t value;


    if(s_hjpeg150.power_on_state == false){
        cam_err("%s(%d)jpegenc is not powered on.",__func__, __LINE__);
        return -1;
    }

    pcfg = (jpgenc_config_t *)cfg;
    phjpeg150 = I2HJPEG150(i);
    base_addr = phjpeg150->viraddr;

    addr = pcfg->reg.addr;
    value = pcfg->reg.value;

    if((addr < JPGENC_JPE_ENCODE_REG)
            ||(addr > JPGENC_FORCE_CLK_ON_CFG_REG )){
        cam_info("%s input addr is invaild 0x%x\n",__func__, addr);
        ret = -1;
        return ret;
    }

    REG_SET(base_addr + addr, value);

    cam_info("%s input addr is  0x%x input value is %d\n",__func__, addr, value);

    return ret;
}

static int hjpeg150_get_reg(hjpeg_intf_t *i, void* cfg)
{
    cam_info("%s enter\n",__func__);
    int ret = 0;
    hjpeg150_t* phjpeg150   = NULL;
    void __iomem* base_addr = 0;
    jpgenc_config_t* pcfg = NULL;
    uint32_t addr;


    if(s_hjpeg150.power_on_state == false){
        return -1;
    }

    pcfg = (jpgenc_config_t *)cfg;
    phjpeg150 = I2HJPEG150(i);
    base_addr = phjpeg150->viraddr;

    addr = pcfg->reg.addr;

    if((addr < JPGENC_JPE_ENCODE_REG)
            ||(addr > JPGENC_FORCE_CLK_ON_CFG_REG )){
        cam_info("%s input addr is invaild 0x%x\n",__func__, addr);
        ret = -1;
        return ret;
    }

    pcfg->reg.value = REG_GET(base_addr + addr);

    cam_info("%s input addr is  0x%x input value is %d\n",__func__, addr, pcfg->reg.value);
    return ret;
}

static struct platform_driver
s_hjpeg150_driver =
{
    .driver =
    {
        .name = "huawei,hjpeg150",
        .owner = THIS_MODULE,
        .of_match_table = s_hjpeg150_dt_match,
    },
};

static int32_t hjpeg150_platform_probe(
        struct platform_device* pdev )
{
    cam_info("%s enter\n", __func__);
    int32_t ret;
    ret = hjpeg_register(pdev, &s_hjpeg150.intf);
    s_hjpeg150.pdev = pdev;
    s_hjpeg150.power_on_state = false;
    return ret;
}

static int __init
hjpeg150_init_module(void)
{
    cam_info("%s enter\n", __func__);
    return platform_driver_probe(&s_hjpeg150_driver,
            hjpeg150_platform_probe);
}

static void __exit
hjpeg150_exit_module(void)
{
    cam_info("%s enter\n", __func__);
    hjpeg_unregister(&s_hjpeg150.intf);
    platform_driver_unregister(&s_hjpeg150_driver);
}

module_init(hjpeg150_init_module);
module_exit(hjpeg150_exit_module);
MODULE_DESCRIPTION("hjpeg150 driver");
MODULE_LICENSE("GPL v2");



