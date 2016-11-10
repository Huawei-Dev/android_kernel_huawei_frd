
#include "omxvdec.h"
#include "platform.h"
#include "regulator.h"
#include "vfmw_ext.h"

#include <linux/hisi/hisi-iommu.h>   //for struct iommu_domain_data
#include <linux/iommu.h>             //for struct iommu_domain
#include <linux/platform_device.h>
#include <linux/regulator/consumer.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>
#include <linux/clk.h>


/********************************** MACRO *************************************/
#define REGULATOR_NAME          "ldo_vdec"
#define VCODEC_CLOCK_NAME       "clk_vdec"
#define VCODEC_CLK_RATE         "dec_clk_rate"
#define VDEC_QOS_MODE           0xE893000C

/*********************************** VARS *************************************/
static HI_U32  g_VdecClkRate_l  = 0;    //200MHZ
static HI_U32  g_VdecClkRate_h  = 0;    //400MHZ
static HI_U32  g_CurClkRate     = 0;
static HI_U32  g_vdecQosMode    = 0x2;
static HI_BOOL g_VdecPowerOn    = HI_FALSE;
static struct  regulator    *g_VdecRegulator  = HI_NULL;
static struct  iommu_domain *g_VdecSmmuDomain = HI_NULL;
static VFMW_DTS_CONFIG_S     g_DtsConfig;
static struct clk *g_PvdecClk = HI_NULL;


/******************************** LOCAL FUNC **********************************/
#ifdef HIVDEC_SMMU_SUPPORT

/*----------------------------------------
    func: iommu enable intf
 ----------------------------------------*/
static HI_U64 VDEC_Enable_Iommu(struct platform_device *pdev)
{
    HI_S32 ret = HI_FAILURE;
    struct device *dev = HI_NULL;

    dev = &pdev->dev;
    g_VdecSmmuDomain = iommu_domain_alloc(dev->bus);
    if (HI_NULL == g_VdecSmmuDomain)
    {
        OmxPrint(OMX_FATAL, "%s iommu_domain_alloc failed!\n", __func__);
        return HI_FAILURE;
    }

    ret = iommu_attach_device(g_VdecSmmuDomain, dev);
    if(ret)
    {
        OmxPrint(OMX_ERR, "iommu_attach_device failed!\n");
        goto out_free_domain;
    }
    return HI_SUCCESS;

out_free_domain:
    iommu_domain_free(g_VdecSmmuDomain);
    return HI_FAILURE;
}

/*----------------------------------------
    func: iommu disable intf
 ----------------------------------------*/
static HI_VOID VDEC_Disable_Iommu(struct platform_device *pdev)
{
    if((NULL != g_VdecSmmuDomain) && (NULL != pdev))
    {
        iommu_detach_device(g_VdecSmmuDomain, &pdev->dev);
        iommu_domain_free(g_VdecSmmuDomain);
        g_VdecSmmuDomain = NULL;
    }
}

/*----------------------------------------
    func: get smmu page table base addr
 ----------------------------------------*/
static HI_U64 VDEC_GetSmmuBasePhy(struct platform_device *pdev)
{
    struct iommu_domain_data *domain_data = HI_NULL;

    if (HI_FAILURE == VDEC_Enable_Iommu(pdev))
        return 0;

    domain_data = (struct iommu_domain_data *)(g_VdecSmmuDomain->priv);

    return (HI_U64)(domain_data->phy_pgd_base);
}

#endif


/*----------------------------------------
    func: initialize vcodec clock rate
 ----------------------------------------*/
static HI_S32 VDEC_Init_ClockRate(struct device *dev)
{
    HI_S32 ret  = HI_FAILURE;
    HI_U32 rate = 0;
    struct device_node *np = dev->of_node;
    struct clk *pvdec_clk = HI_NULL;

    if(HI_NULL == dev)
    {
        OmxPrint(OMX_FATAL, "%s dev is NULL!\n", __func__);
        return HI_FAILURE;
    }
    pvdec_clk = devm_clk_get(dev, VCODEC_CLOCK_NAME);
    if (IS_ERR_OR_NULL(pvdec_clk))
    {
        OmxPrint(OMX_FATAL, "%s can not get clock!\n", __func__);
        return HI_FAILURE;
    }
    g_PvdecClk = pvdec_clk;

    ret = of_property_read_u32_index(np, VCODEC_CLK_RATE, 1, &g_VdecClkRate_l);
    if (ret)
    {
        OmxPrint(OMX_FATAL, "%s can not get g_VdecClkRate_l, return %d\n", __func__, ret);
        g_VdecClkRate_l = 0;
        return HI_FAILURE;
    }
    OmxPrint(OMX_ALWS, "%s get g_VdecClkRate_l = %u\n", __func__, g_VdecClkRate_l);

    ret = of_property_read_u32_index(np, VCODEC_CLK_RATE, 0, &g_VdecClkRate_h);
    if (ret)
    {
        OmxPrint(OMX_FATAL, "%s can not get g_VdecClkRate_h, return %d\n", __func__, ret);
        g_VdecClkRate_h = 0;
        return HI_FAILURE;
    }
    OmxPrint(OMX_ALWS, "%s get g_VdecClkRate_h = %u\n", __func__, g_VdecClkRate_h);

    rate = g_VdecClkRate_h;
    ret  = clk_set_rate(pvdec_clk, rate);
    if (ret)
    {
        OmxPrint(OMX_FATAL, "%s Failed to clk_set_rate, return %d\n", __func__, ret);
        return HI_FAILURE;
    }
    OmxPrint(OMX_ALWS, "%s set VdecClkRate = %u\n", __func__, rate);

    g_CurClkRate = rate;

    return HI_SUCCESS;
}


/*----------------------------------------
    func: get dts configure
          as reg base & irq num
 ----------------------------------------*/
static HI_S32 VDEC_GetDtsConfigInfo(struct platform_device *pdev, VFMW_DTS_CONFIG_S *pDtsConfig)
{
    HI_S32 ret = HI_FAILURE;
    struct device *dev = &pdev->dev;
    struct device_node *np = dev->of_node;
    struct resource res;

    if (HI_NULL == np)
    {
        OmxPrint(OMX_FATAL, "%s the device node is null\n", __func__);
        return HI_FAILURE;
    }

    if (HI_NULL == pDtsConfig)
    {
        OmxPrint(OMX_FATAL, "%s pDtsConfig is null\n", __func__);
        return HI_FAILURE;
    }

    //Get irq num, return 0 if failed
    pDtsConfig->MfdeIrqNum = irq_of_parse_and_map(np, 0);
    if (0 == pDtsConfig->MfdeIrqNum)
    {
        OmxPrint(OMX_FATAL, "%s irq_of_parse_and_map MfdeIrqNum failed!\n", __func__);
        return HI_FAILURE;
    }

    pDtsConfig->ScdIrqNum  = irq_of_parse_and_map(np, 1);
    if (0 == pDtsConfig->ScdIrqNum)
    {
        OmxPrint(OMX_FATAL, "%s irq_of_parse_and_map ScdIrqNum failed!\n", __func__);
        return HI_FAILURE;
    }

    pDtsConfig->BpdIrqNum  = irq_of_parse_and_map(np, 2);
    if (0 == pDtsConfig->BpdIrqNum)
    {
        OmxPrint(OMX_FATAL, "%s irq_of_parse_and_map BpdIrqNum failed!\n", __func__);
        return HI_FAILURE;
    }

    pDtsConfig->SmmuIrqNum = irq_of_parse_and_map(np, 3);
    if (0 == pDtsConfig->SmmuIrqNum)
    {
        OmxPrint(OMX_FATAL, "%s irq_of_parse_and_map SmmuIrqNum failed!\n", __func__);
        return HI_FAILURE;
    }

    //Get reg base addr & size, return 0 if success
    ret = of_address_to_resource(np, 0, &res);
    if (ret)
    {
        OmxPrint(OMX_FATAL, "%s of_address_to_resource failed! return %d\n", __func__, ret);
        return HI_FAILURE;
    }
    pDtsConfig->VdhRegBaseAddr = res.start;
    pDtsConfig->VdhRegRange    = resource_size(&res);

#ifdef HIVDEC_SMMU_SUPPORT
    //Get reg base addr & size, return 0 if failed
    pDtsConfig->SmmuPageBaseAddr = VDEC_GetSmmuBasePhy(pdev);
    if (0 == pDtsConfig->SmmuPageBaseAddr)
    {
        OmxPrint(OMX_FATAL, "%s Regulator_GetSmmuBasePhy failed!\n", __func__);
        return HI_FAILURE;
    }
#endif

    //Check if is FPGA platform
    ret = of_property_read_u32(np, "vdec_fpga", &pDtsConfig->IsFPGA);
    if (ret)
    {
        pDtsConfig->IsFPGA = 0;
        OmxPrint(OMX_ALWS, "Is not FPGA platform!\n");
    }

    /* get vdec qos mode */
    ret = of_property_read_u32(np, "vdec_qos_mode", &g_vdecQosMode);
    if(ret)
    {
        g_vdecQosMode = 0x2;
        OmxPrint(OMX_WARN, "get vdec qos mode failed set default!\n");
    }

    ret = VDEC_Init_ClockRate(dev);
    if (ret != HI_SUCCESS)
    {
        OmxPrint(OMX_ERR, "%s VDEC_Init_ClockRate failed!\n", __func__);
        return HI_FAILURE;
        //fixme: continue or return?
    }

    return HI_SUCCESS;
}

/*----------------------------------------
    func: get dts configure
          as reg base & irq num
 ----------------------------------------*/
static HI_S32 VDEC_Config_QOS(void)
{
    HI_S32 ret = HI_FAILURE;
    HI_U64* qos_addr = HI_NULL;

    qos_addr = (HI_U64 *)ioremap(VDEC_QOS_MODE, 4);
    if(qos_addr)
    {
        writel(g_vdecQosMode, qos_addr);
        iounmap(qos_addr);
        qos_addr = HI_NULL;
        ret = HI_SUCCESS;
    }
    else
    {
        OmxPrint(OMX_ERR, "ioremap VDEC_QOS_MODE reg failed!\n");
        ret = HI_FAILURE;
    }

    return ret;
}

/******************************** SHARE FUNC **********************************/

/*----------------------------------------
    func: regulator & dts config
          initialization
 ----------------------------------------*/
HI_S32 VDEC_Regulator_Initialize(HI_VOID *pParam)
{
    HI_S32 ret = HI_FAILURE;
    struct platform_device *pdev = (struct platform_device *)pParam;

    g_VdecRegulator = HI_NULL;

    if (NULL == pdev)
    {
       OmxPrint(OMX_FATAL, "%s, invalid params!", __func__ );
       return HI_FAILURE;
    }

    g_VdecRegulator = devm_regulator_get(&pdev->dev, REGULATOR_NAME);
    if (HI_NULL == g_VdecRegulator)
    {
        OmxPrint(OMX_FATAL, "%s get regulator failed!\n", __func__);
        return HI_FAILURE;
    }
    else if (IS_ERR(g_VdecRegulator))
    {
        OmxPrint(OMX_FATAL, "%s get regulator failed, error no = %ld!\n", __func__, PTR_ERR(g_VdecRegulator));
        g_VdecRegulator = HI_NULL;
        return HI_FAILURE;
    }

    memset(&g_DtsConfig, 0, sizeof(VFMW_DTS_CONFIG_S));
    ret = VDEC_GetDtsConfigInfo(pdev, &g_DtsConfig);
    if (ret != HI_SUCCESS)
    {
        OmxPrint(OMX_FATAL, "%s Regulator_GetDtsConfigInfo failed.\n", __func__);
        return HI_FAILURE;
    }

    ret = VFMW_SetDtsConfig(&g_DtsConfig);
    if (ret != HI_SUCCESS)
    {
        OmxPrint(OMX_FATAL, "%s VFMW_SetDtsConfig failed.\n", __func__);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/*----------------------------------------
    func: regulator deinitialize
 ----------------------------------------*/
HI_VOID VDEC_Regulator_DeInitialize(HI_VOID *pParam)
{
    struct platform_device *pdev = (struct platform_device *)pParam;

    if (pdev != NULL)
    {
        VDEC_Disable_Iommu(pdev);
    }
}


/*----------------------------------------
    func: enable regulator
 ----------------------------------------*/
HI_S32 VDEC_Regulator_Enable(HI_VOID)
{
    HI_S32 ret = HI_FAILURE;

    if (HI_TRUE == g_VdecPowerOn)
    {
        return HI_SUCCESS;
    }
    if (HI_NULL == g_PvdecClk)
    {
        OmxPrint(OMX_FATAL, "%s g_PvdecClk is NULL!\n", __func__);
        return HI_FAILURE;
    }
    ret = clk_prepare_enable(g_PvdecClk);
    if (ret != 0)
    {
        OmxPrint(OMX_FATAL, "%s clk_prepare_enable failed!\n", __func__);
        return HI_FAILURE;
    }

    if (IS_ERR_OR_NULL(g_VdecRegulator))
    {
       OmxPrint(OMX_WARN, "%s g_VdecRegulator = NULL!", __func__ );
       return HI_FAILURE;
    }
    ret = regulator_enable(g_VdecRegulator);
    if (ret != 0)
    {
        OmxPrint(OMX_FATAL, "%s enable regulator failed!\n", __func__);
        goto error_clk_unprepare;
    }

    ret = VDEC_Config_QOS();
    if(ret != 0)
    {
        OmxPrint(OMX_ERR, "VDEC_Config_QOS failed!\n");
        goto error_regulator_disable;
    }

    g_VdecPowerOn = HI_TRUE;
    OmxPrint(OMX_INFO, "%s enable regulator success!\n", __func__);

    return HI_SUCCESS;

error_regulator_disable:
    regulator_disable(g_VdecRegulator);

error_clk_unprepare:
    clk_disable_unprepare(g_PvdecClk);
    return HI_FAILURE;
}



/*----------------------------------------
    func: disable regulator
 ----------------------------------------*/
HI_S32 VDEC_Regulator_Disable(HI_VOID)
{
    HI_S32 ret = HI_FAILURE;

    if (HI_FALSE == g_VdecPowerOn)
    {
        return HI_SUCCESS;
    }

    if (IS_ERR_OR_NULL(g_VdecRegulator))
    {
       OmxPrint(OMX_WARN, "%s g_VdecRegulator = NULL!", __func__ );
       return HI_FAILURE;
    }
    ret = regulator_disable(g_VdecRegulator);
    if (ret != 0)
    {
        OmxPrint(OMX_FATAL, "%s disable regulator failed!\n", __func__);
        return HI_FAILURE;
    }

    if (HI_NULL == g_PvdecClk)
    {
        OmxPrint(OMX_FATAL, "%s g_PvdecClk is NULL!\n", __func__);
        return HI_FAILURE;
    }
    clk_disable_unprepare(g_PvdecClk);

    g_VdecPowerOn = HI_FALSE;

    OmxPrint(OMX_INFO, "%s disable regulator success!\n", __func__);

    return HI_SUCCESS;
}


/*----------------------------------------
    func: decoder clock rate select
 ----------------------------------------*/
HI_S32 VDEC_Regulator_SetClkRate(struct device *dev, eCLK_RATE eClkRate)
{
    struct device_node *np = HI_NULL;
    struct clk *pvdec_clk  = HI_NULL;
    HI_U32 rate     = 0;
    HI_S32 ret      = 0;

    if (g_DtsConfig.IsFPGA)
    {
        OmxPrint(OMX_INFO, "Is FPGA platform, no need to change VCodec rate!\n");
        return HI_SUCCESS;
    }

    if (HI_NULL == dev)
    {
        OmxPrint(OMX_FATAL, "Invalid param pdev = NULL [%s]\n", __func__);
        return HI_FAILURE;
    }

    np = dev->of_node;
    if (HI_NULL == np)
    {
        OmxPrint(OMX_ERR, "Invalid node = NULL [%s]\n", __func__);
        return HI_FAILURE;
    }

    pvdec_clk = devm_clk_get(dev, VCODEC_CLOCK_NAME);
    if (IS_ERR_OR_NULL(pvdec_clk))
    {
        OmxPrint(OMX_ERR, "Couldn't get clk! [%s]\n", __func__);
        return HI_FAILURE;
    }

    rate = clk_get_rate(pvdec_clk);
    if (g_CurClkRate == rate)
    {
        OmxPrint(OMX_INFO, "VCodec already in %u HZ clock rate\n", rate);
        return HI_SUCCESS;
    }

    if (CLK_RATE_LOW == eClkRate)
    {
        if (0 == g_VdecClkRate_l)
        {
            OmxPrint(OMX_ERR, "%s g_VdecClkRate_l = 0, get it again.\n", __func__);
            ret = of_property_read_u32_index(np, VCODEC_CLK_RATE, 1, &g_VdecClkRate_l);
            if (ret)
            {
                OmxPrint(OMX_ERR, "%s, can not get g_VdecClkRate_l, return %d\n", __func__, ret);
                g_VdecClkRate_l = 0;
                return HI_FAILURE;
            }
        }
        rate = g_VdecClkRate_l;
    }
    else if (CLK_RATE_HIGH == eClkRate)
    {
        if (0 == g_VdecClkRate_h)
        {
            OmxPrint(OMX_ERR, "%s g_VdecClkRate_h = 0, get it again.\n", __func__);
            ret = of_property_read_u32_index(np, VCODEC_CLK_RATE, 0, &g_VdecClkRate_h);
            if (ret)
            {
                OmxPrint(OMX_ERR, "%s, can not get g_VdecClkRate_h,return %d\n", __func__, ret);
                g_VdecClkRate_h = 0;
                return HI_FAILURE;
            }
        }
        rate = g_VdecClkRate_h;
    }
    else
    {
        OmxPrint(OMX_ERR, "[%s] unsupport clk rate enum %d\n", __func__, eClkRate);
        return HI_FAILURE;
    }

    OmxPrint(OMX_INFO, "Prepare to change VCodec clock rate to %u HZ\n", rate);

    ret = clk_set_rate(pvdec_clk, rate);
    if (ret)
    {
        OmxPrint(OMX_ERR, "Failed to clk_set_rate %u HZ[%s] ret : %d\n", rate, __func__, ret);
        return HI_FAILURE;
    }

    OmxPrint(OMX_INFO, "Success changed VCodec clock rate to %u HZ\n", rate);

    g_CurClkRate = rate;

    return HI_SUCCESS;
}


/*----------------------------------------
    func: regulator read proc entry
 ----------------------------------------*/
HI_VOID VDEC_Regulator_Read_Proc(HI_VOID *pParam, HI_VOID *v)
{
    struct seq_file *p = (struct seq_file *)pParam;

    PROC_PRINT(p, "----------------------- Regulator Info -----------------------\n\n");

    PROC_PRINT(p, "IsFpga           :%-10d\n", g_DtsConfig.IsFPGA);
    PROC_PRINT(p, "PowerState       :%-10s\n", (HI_TRUE==g_VdecPowerOn)?"ON":"OFF");
    if (0 == g_DtsConfig.IsFPGA)
    {
    PROC_PRINT(p, "ClockRate        :%-10d (%d L/%d H)\n", g_CurClkRate, g_VdecClkRate_l, g_VdecClkRate_h);
    }

    PROC_PRINT(p, "MfdeIrqNum       :%-10d  | ScdIrqNum          :%d\n",  g_DtsConfig.MfdeIrqNum,     g_DtsConfig.ScdIrqNum);
    PROC_PRINT(p, "BpdIrqNum        :%-10d  | SmmuIrqNum         :%d\n",  g_DtsConfig.BpdIrqNum,      g_DtsConfig.SmmuIrqNum);
    PROC_PRINT(p, "VdhRegBaseAddr   :%-10x  | VdhRegRange        :%d\n",  g_DtsConfig.VdhRegBaseAddr, g_DtsConfig.VdhRegRange);
    PROC_PRINT(p, "SmmuPageBaseAddr :%-12llx| \n",                        g_DtsConfig.SmmuPageBaseAddr);

    PROC_PRINT(p, "--------------------------------------------------------------\n\n");

    return;
}

