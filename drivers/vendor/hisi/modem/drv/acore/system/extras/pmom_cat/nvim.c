

#include <stdio.h>
#include "pmom_cat.h"
#include "nvim.h"

struct nv_icc_msg_name g_nv_msg_name_table[NV_ICC_MSG_TYPE_SUM] = {
    {NV_ICC_REQ_PRIORITY_HIGH_FLUSH,    "CCPU", "req write high priority nv"},
    {NV_ICC_REQ_PRIORITY1_FLUSH,        "CCPU", "req write high priority1 nv"},
    {NV_ICC_REQ_PRIORITY_MID_LOW_FLUSH, "CCPU", "req write low  priority nv"},
    {NV_ICC_REQ_SHUT_DOWN_FLUSH,        "CCPU", "req flush nv"},
    {NV_ICC_REQ_RESUME,                 "CCPU", "req resume all the nv data"},
    {NV_ICC_REQ_RESUME_ITEM, 		    "CCPU", "req resume single nv data"},
    {NV_ICC_CNF,        "ACPU", "cnf from ACPU"},
};

int nvim_msg_parse_acore( char *in_buf, unsigned int len, char *out_buf, unsigned int *out_ptr)
{
    int data_len = 0;
    nv_icc_msg_single_t *log_msg = (nv_icc_msg_single_t *)in_buf;
    int temp = 0;

    /* step1: 消息类型检查 */
    if(len != sizeof(*log_msg))
    {
        return PMOM_ERR;
    }

    for(temp = 0; temp < NV_ICC_MSG_TYPE_SUM; temp++)
    {
        if(g_nv_msg_name_table[temp].msg_type == log_msg->msg_type)
        {
            break;
        }
    }

    if(temp < NV_ICC_MSG_TYPE_SUM)
    {
        /* step2: 格式化消息到out_buf: 以回车结束 */
        data_len = snprintf(&out_buf[*out_ptr], PMOM_PARSE_STR_BUFSIZ,
                "[%s] [%s], nvid:0x%x, sn:0x%x, msg_type:0x%x, slice:0x%x\n",
                g_nv_msg_name_table[temp].cpu, g_nv_msg_name_table[temp].work, log_msg->data.itemid, log_msg->sn, log_msg->msg_type, log_msg->send_slice
                );
        *out_ptr = *out_ptr + data_len;
    }
    return PMOM_OK;
}

int nvim_msg_parse_ccore(char *in_buf, unsigned int len, char *out_buf, unsigned int *out_ptr)
{
    int data_len = 0;
    nv_icc_msg_t *log_msg = (nv_icc_msg_t *)in_buf;
    int temp = 0;

    /* step1: 消息类型检查 */
    if(len != sizeof(*log_msg))
    {
        return PMOM_ERR;
    }

    for(temp = 0; temp < NV_ICC_MSG_TYPE_SUM; temp++)
    {
        if(g_nv_msg_name_table[temp].msg_type == log_msg->msg_type)
        {
            break;
        }
    }

    if(temp < NV_ICC_MSG_TYPE_SUM)
    {
        /* step2: 格式化消息到out_buf: 以回车结束 */
        data_len = snprintf(&out_buf[*out_ptr], PMOM_PARSE_STR_BUFSIZ,
                "[%s] [%s], ret:0x%x, sn:0x%x, msg_type:0x%x, slice:0x%x\n",
                g_nv_msg_name_table[temp].cpu, g_nv_msg_name_table[temp].work, log_msg->ret, log_msg->sn, log_msg->msg_type, log_msg->send_slice
                );
        *out_ptr = *out_ptr + data_len;
    }
    return PMOM_OK;
}

int nvim_msg_parse(unsigned int typeid, char *in_buf, unsigned int len, char *out_buf, unsigned int *out_ptr)
{
    int data_len = 0;
    nv_icc_msg_t *log_msg = (nv_icc_msg_t *)in_buf;
    int temp = 0;

    printf("info: nv type_id=0x%x", typeid);
    if (typeid == NV_ICC_MSG_ACORE) {
        return nvim_msg_parse_acore(in_buf, len, out_buf, out_ptr);
    }

    return nvim_msg_parse_ccore(in_buf, len, out_buf, out_ptr);
}


