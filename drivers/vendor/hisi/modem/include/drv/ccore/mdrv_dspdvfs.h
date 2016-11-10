#ifndef __MDRV_CCORE_DSPDVFS_H__
#define __MDRV_CCORE_DSPDVFS_H__
#ifdef __cplusplus
extern "C"
{
#endif

enum Dspdvs_pmu_type
{
    DSPDVS_BBP = 1,
    DSPDVS_DSP,
    DSPDVS_BUTT
};

enum Dspdfs_DSP_Core_ID
{
    TLDSP = 1,     /*tlbbe16*/
    CDSP,         /*cbbe16*/
    DSP_BUTT
};

/*****************************************************************************
* 函 数 名  : mdrv_dspdfs_qos_request
*
* 功能描述  : 注册dspdfs_qos
*
* 输入参数  : u32 core_id - DSP_core ID
* 输出参数  : u32 *req_id - 请求ID
*
* 返 回 值  : 0，成功；-1，失败
*
* 其它说明  :
*
*****************************************************************************/
int mdrv_dspdfs_qos_request(unsigned int core_id, unsigned int *req_id);

/*****************************************************************************
* 函 数 名  : mdrv_dspdfs_qos_update
*
* 功能描述  : dsp调频投票接口，更新DFS QoS
*
* 输入参数  : u32 req_id - 请求ID
              u32 req_value - 请求频率值 
* 输出参数  : 无
*
* 返 回 值  : 0，成功；-1，失败
*
* 其它说明  :
*
*****************************************************************************/
int mdrv_dspdfs_qos_update(unsigned int req_id, unsigned int req_value);

/*****************************************************************************
* 函 数 名  : mdrv_dspdfs_qos_get
*
* 功能描述  : 获取对应req_id的投票频率
*
* 输入参数  : req_id - 请求ID
* 输出参数  :
*
* 返 回 值  : 成功返回对应req_id的投票频率
*             失败返回 -1
*
* 其它说明  : 
*
*****************************************************************************/
int mdrv_dspdfs_qos_get(unsigned int req_id);

/*****************************************************************************
* 函 数 名  : mdrv_dspdvs_get_bbp_voltage
*
* 功能描述  : 获取TLDSP对BBP电压的投票值
*
* 输入参数  : 无
* 输出参数  : 无 
*
* 返 回 值  : 成功返回TLDSP最新一次的BBP调压请求
*             失败返回 -1
*
* 其它说明  : 
*
*****************************************************************************/
int mdrv_dspdvs_get_bbp_voltage(void);

/*****************************************************************************
* 函 数 名  : mdrv_dspdvs_get_low_voltage_profile
*
* 功能描述  : 获取DSP/BBP最低电压值
*
* 输入参数  : req_type: 1-BBP 2-DSP
* 输出参数  : 无 
*
* 返 回 值  : 成功返回DSP/BBP最低电压值
*             失败返回 -1
*
* 其它说明  : 
*
*****************************************************************************/
int mdrv_dspdvs_get_low_voltage_profile(unsigned int req_type);

/*****************************************************************************
* 函 数 名  : mdrv_dspdvs_voltage_qos
*
* 功能描述  : 根据调压请求值，对dsp或者BBP进行调压
*
* 输入参数  : u32 req_type: 1-BBP 2-DSP
              u32 voltage_req:需要的电压
* 输出参数  : 无 
*
* 返 回 值  : 无
*
* 其它说明  : 
*
*****************************************************************************/
void mdrv_dspdvs_voltage_qos(unsigned int req_type, unsigned int voltage_req);


#ifdef __cplusplus
}
#endif
#endif

