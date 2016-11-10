
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/regulator/consumer.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>
#include <linux/clk.h>
#include <linux/clkdev.h>

#include "venc_regulator.h"
#include "hi_drv_mem.h"
#include <linux/hisi/hisi-iommu.h>
#include "drv_venc.h"
#include "drv_venc_efl.h"

#define REGULATOR_NAME          "ldo_venc"
#define MATCH_TABLE_TYPE        "hisi,kirin960-venc"
#define REGULATOR_DRIVER_NAME   "venc_regulator"
#define VENC_CLOCK_NAME         "clk_venc"
#define VENC_CLOCK_VOLT         "venc_volt_hold"
#define VENC_CLK_RATE           "enc_clk_rate"
#define VENC_QOS_MODE           0xE894000C

static struct regulator *g_VencRegulator = NULL;
static VeduEfl_DTS_CONFIG_S g_VencDtsConfig;
static struct clk *g_PvencClk = HI_NULL;
static struct clk *g_PvencVolt= HI_NULL;
struct iommu_domain* g_hisi_mmu_domain = HI_NULL;
HI_BOOL g_voltHold = HI_FALSE;

static HI_U32 g_vencQosMode = 0x2;

static int Venc_Enable_Iommu(struct platform_device *pdev)
{
    struct iommu_domain *hisi_domain      = NULL;
    struct iommu_domain_data* domain_data = NULL;
    struct device *dev    = NULL;
    uint64_t phy_pgd_base = 0;
    HI_S32 ret = 0;

    if (pdev == NULL || (&pdev->dev == NULL))
    {
        HI_ERR_VENC("%s,  invalid Parameters!\n", __func__);
        return -1;
    }
    dev = &pdev->dev;

	/* create iommu domain */
    hisi_domain = iommu_domain_alloc(dev->bus);
    if (!hisi_domain) {
        HI_ERR_VENC("%s, iommu_domain_alloc failed!\n", __func__);
        return -1;
    }

    /* attach device */
    ret = iommu_attach_device(hisi_domain, dev);
    if(ret)
    {
        HI_ERR_VENC("iommu_attach_device failed!\n");
        goto out_free_domain;
    }
    g_hisi_mmu_domain = hisi_domain;

    domain_data = (struct iommu_domain_data *)(g_hisi_mmu_domain->priv);
    if(NULL != domain_data)
    {
        phy_pgd_base = (uint64_t)(domain_data->phy_pgd_base);
        HI_INFO_VENC("%s, phy_pgd_base:0x%x\n", __func__, phy_pgd_base);
    }
    else
    {
        goto out_detach_device;
    }

	return 0;

out_detach_device:
    iommu_detach_device(g_hisi_mmu_domain, dev);
out_free_domain:
    iommu_domain_free(hisi_domain);
    return -1;
}

static int Venc_Disable_Iommu(struct platform_device *pdev)
{
    if((NULL != g_hisi_mmu_domain) && (NULL != pdev))
    {
        iommu_detach_device(g_hisi_mmu_domain, &pdev->dev);
        iommu_domain_free(g_hisi_mmu_domain);
        g_hisi_mmu_domain = NULL;
        return 0;
    }

    return -1;
}

static HI_S32 Venc_GetDtsConfigInfo(struct platform_device *pdev, VeduEfl_DTS_CONFIG_S *pDtsConfig)
{
    HI_U32 rate_h = 0;
    HI_U32 rate_n = 0;
    HI_S32 ret    = HI_FAILURE;
    struct resource res;
    struct clk *pvenc_clk = HI_NULL;
    struct clk *pvenc_volt = HI_NULL;
    struct device *dev = &pdev->dev;
    struct device_node *np = dev->of_node;
    struct iommu_domain_data *domain_data = NULL;

    memset(&res, 0, sizeof(res));
    if ((HI_NULL == dev) || (HI_NULL == np) || (HI_NULL == pDtsConfig))
    {
        HI_ERR_VENC("invalid argument dev:0x%x, np:0x%x, pDtsConfig:0x%x \n", dev, np, pDtsConfig);
        return HI_FAILURE;
    }

    /* 1 read IRQ num from dts */
    pDtsConfig->VeduIrqNum = irq_of_parse_and_map(np, 0);
    if (0 == pDtsConfig->VeduIrqNum)
    {
        HI_ERR_VENC("%s, irq_of_parse_and_map VeduIrqNum failed!\n", __func__);
        return HI_FAILURE;
    }

    pDtsConfig->MmuIrqNum = irq_of_parse_and_map(np, 1);
    if (0 == pDtsConfig->MmuIrqNum)
    {
        HI_ERR_VENC("%s, irq_of_parse_and_map MmuIrqNum failed!\n", __func__);
        return HI_FAILURE;
    }

    pDtsConfig->VedusecIrqNum  = irq_of_parse_and_map(np, 2);
    if (0 == pDtsConfig->VedusecIrqNum)
    {
        HI_ERR_VENC("%s, irq_of_parse_and_map VedusecIrqNum failed!\n", __func__);
        //return HI_FAILURE;
    }

    pDtsConfig->MmusecIrqNum = irq_of_parse_and_map(np, 3);
    if (0 == pDtsConfig->MmusecIrqNum)
    {
        HI_ERR_VENC("%s, irq_of_parse_and_map MmusecIrqNum failed!\n", __func__);
        //return HI_FAILURE;
    }

    /* 2 read venc register start address, range */
    ret = of_address_to_resource(np, 0, &res);
    if (ret)
    {
        HI_ERR_VENC("%s of_address_to_resource failed! ret = %d\n", __func__, ret);
        return HI_FAILURE;
    }
    pDtsConfig->VencRegBaseAddr = res.start;
    pDtsConfig->VencRegRange    = resource_size(&res);

    /* 3 read venc clk rate [low, high], venc volt clock */
    pvenc_clk  = devm_clk_get(dev, VENC_CLOCK_NAME);
    pvenc_volt = devm_clk_get(dev, VENC_CLOCK_VOLT);
    if (IS_ERR_OR_NULL(pvenc_volt)|| IS_ERR_OR_NULL(pvenc_clk))
    {
        HI_ERR_VENC("can not get venc clock, pvenc_volt = 0x%x, pvenc_clk = 0x%x!\n", pvenc_volt, pvenc_clk);
        return HI_FAILURE;
    }
    g_PvencClk  = pvenc_clk;
    g_PvencVolt = pvenc_volt;

    ret = of_property_read_u32_index(np, VENC_CLK_RATE, 0, &rate_h);
    ret += of_property_read_u32_index(np, VENC_CLK_RATE, 1, &rate_n);
    if (ret)
    {
        HI_ERR_VENC("%s can not get venc rate, return %d\n", __func__, ret);
        return HI_FAILURE;
    }
    pDtsConfig->normalRate = rate_n;
    pDtsConfig->highRate   = rate_h;

    /* 4 fpga platform */
    ret = of_property_read_u32(np, "venc_fpga", &pDtsConfig->IsFPGA);
    if (ret)
    {
        HI_INFO_VENC("%s, read failed!\n", __func__);
    }

    /* 5 get venc qos mode */
    ret = of_property_read_u32(np, "venc_qos_mode", &g_vencQosMode);
    if(ret)
    {
        g_vencQosMode = 0x2;
        HI_ERR_VENC("get venc qos mode failed set default!\n");
    }

    domain_data = (struct iommu_domain_data *)(g_hisi_mmu_domain->priv);
    if(NULL != domain_data)
    {
        pDtsConfig->SmmuPageBaseAddr = (uint64_t)(domain_data->phy_pgd_base);
        HI_INFO_VENC("%s, SmmuPageBaseAddr:0x%x\n", __func__, pDtsConfig->SmmuPageBaseAddr);
    }

    if (0 == pDtsConfig->SmmuPageBaseAddr)
    {
        HI_ERR_VENC("%s, Regulator_GetSmmuBasePhy failed!\n", __func__);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static HI_S32 Venc_Regulator_Get(struct platform_device *pdev)
{
    HI_INFO_VENC("enter %s!\n", __func__);

    g_VencRegulator = HI_NULL;
    g_VencRegulator = devm_regulator_get(&pdev->dev, REGULATOR_NAME);
    if (IS_ERR_OR_NULL(g_VencRegulator))
    {
        HI_ERR_VENC("%s, get regulator failed, error no = %ld!\n", __func__, PTR_ERR(g_VencRegulator));
        g_VencRegulator = HI_NULL;
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static HI_S32 Venc_config_QOS(void)
{
    HI_S32 ret = HI_FAILURE;
    HI_U64* qos_addr = HI_NULL;

    qos_addr = (HI_U64 *)ioremap(VENC_QOS_MODE, 4);
    if(qos_addr)
    {
       writel(g_vencQosMode, qos_addr);
       iounmap(qos_addr);
       qos_addr = HI_NULL;
       ret = HI_SUCCESS;
    }
    else
    {
        HI_ERR_VENC("ioremap VENC_QOS_MODE reg failed!\n");
        ret = HI_FAILURE;
    }

    return ret;
}

HI_S32 Venc_Regulator_Init(struct platform_device *pdev)
{
    HI_S32 ret = 0;

    if(NULL == pdev)
    {
        HI_ERR_VENC("%s, invalid argument!!\n", __func__);
        return HI_FAILURE;
    }

    /* 1 get regulator interface */
    ret = Venc_Regulator_Get(pdev);
	if (ret < 0)
    {
        HI_ERR_VENC("%s, Venc_Regulator_Get failed!\n", __func__);
        return HI_FAILURE;
	}

    /* 2 create smmu domain */
    ret = Venc_Enable_Iommu(pdev);
    if(ret < 0)
    {
        HI_ERR_VENC("%s, VENC_Enable_Iommu failed!\n", __func__);
        return HI_FAILURE;
    }

    /* 3 read venc dts info from dts */
    memset(&g_VencDtsConfig, 0, sizeof(VeduEfl_DTS_CONFIG_S));
    ret = Venc_GetDtsConfigInfo(pdev, &g_VencDtsConfig);
    if (HI_SUCCESS != ret)
    {
        HI_ERR_VENC("%s VENC_GetDtsConfigInfo failed.\n", __func__);
        return HI_FAILURE;
    }

    /* 4 set dts into to efi */
    ret = VENC_SetDtsConfig(&g_VencDtsConfig);
    if (HI_SUCCESS != ret)
    {
        HI_ERR_VENC("%s VENC_SetDtsConfig failed.\n", __func__);
        return HI_FAILURE;
    }

    HI_INFO_VENC("exit %s()\n", __func__);
    return HI_SUCCESS;
}

HI_VOID Venc_Regulator_Deinit(struct platform_device *pdev)
{
    if(NULL != pdev)
    {
        Venc_Disable_Iommu(pdev);
    }

	return;
}

HI_S32 Venc_Regulator_Enable(HI_VOID)
{
    HI_S32 ret = HI_FAILURE;

    if(IS_ERR_OR_NULL(g_PvencClk) || IS_ERR_OR_NULL(g_PvencVolt) || IS_ERR_OR_NULL(g_VencRegulator))
    {
        HI_ERR_VENC("invalid_argument g_PvencClk:0x%x, g_PvencVolt:0x%x, g_VencRegulator:0x%x\n",
                    g_PvencClk, g_PvencVolt, g_VencRegulator);
        return HI_FAILURE;
    }

    ret = clk_set_rate(g_PvencClk, g_VencDtsConfig.normalRate);
    if(ret != 0)
    {
        HI_ERR_VENC("clk_set_rate high/2 failed!\n");
        return HI_FAILURE;
    }

    ret = clk_prepare_enable(g_PvencClk);
    if (ret != 0)
    {
        HI_ERR_VENC("clk_prepare_enable failed!\n");
        return HI_FAILURE;
    }

#ifdef VENC_VOLT_HOLD
    if(!g_voltHold)
    {
        ret = clk_prepare_enable(g_PvencVolt);
        if (ret != 0)
        {
            HI_ERR_VENC("clk_prepare_enable venc_volt_hold failed!\n");
            goto on_error_clk;
        }
        g_voltHold = HI_TRUE;
        HI_INFO_VENC("venc volt hold success!");
    }
#endif

    ret = regulator_enable(g_VencRegulator);
    if (ret != 0)
    {
        HI_ERR_VENC("enable regulator failed!\n");
        goto on_error_volt;
    }

    ret = Venc_config_QOS();
    if(ret != 0)
    {
        HI_ERR_VENC("Venc_config_QOS failed!\n");
        goto on_error_regulator;
    }

    HI_INFO_VENC("power on!! \n");
    return HI_SUCCESS;

on_error_regulator:
    regulator_disable(g_VencRegulator);

on_error_volt:
    clk_disable_unprepare(g_PvencVolt);

on_error_clk:
    clk_disable_unprepare(g_PvencClk);
    return HI_FAILURE;
}

HI_S32 Venc_Regulator_Disable(HI_BOOL disVoltHold)
{
    HI_S32 ret = HI_FAILURE;
    HI_DBG_VENC("%s, Venc_Regulator_Disable g_VencRegulator:0x%x\n", __func__, g_VencRegulator);

    if(IS_ERR_OR_NULL(g_PvencClk) || IS_ERR_OR_NULL(g_PvencVolt) || IS_ERR_OR_NULL(g_VencRegulator))
    {
        HI_ERR_VENC("invalid_argument g_PvencClk:0x%x, g_PvencVolt:0x%x, g_VencRegulator:0x%x\n",
                    g_PvencClk, g_PvencVolt, g_VencRegulator);
        return HI_FAILURE;
    }

	ret = regulator_disable(g_VencRegulator);
    if (ret != 0)
    {
        HI_ERR_VENC("disable regulator failed!\n");
        return HI_FAILURE;
    }

    clk_disable_unprepare(g_PvencClk);

#ifdef VENC_VOLT_HOLD
    if(disVoltHold && g_voltHold)
    {
        clk_disable_unprepare(g_PvencVolt); // always kept 0.8v, if venc exist
        g_voltHold = HI_FALSE;
        HI_INFO_VENC("venc volt hold release success!");
    }
#endif

    HI_INFO_VENC("power off!!\n");

    return HI_SUCCESS;
}
