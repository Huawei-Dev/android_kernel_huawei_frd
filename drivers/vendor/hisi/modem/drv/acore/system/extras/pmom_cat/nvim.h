

enum
{
    NV_ICC_REQ_PRIORITY_HIGH_FLUSH      = 0x40,         /*ccore立即写入请求,需要等待回复*/
    NV_ICC_REQ_PRIORITY1_FLUSH          = 0x41,         /*ccore 延迟写入请求，不需要等待回复*/
    NV_ICC_REQ_PRIORITY_MID_LOW_FLUSH   = 0x42,
    NV_ICC_REQ_SHUT_DOWN_FLUSH          = 0x43,         /*ccore 关机写请求消息*/
    NV_ICC_REQ_RESUME                   = 0x44,         /*请求恢复DDR中的nv数据*/
    NV_ICC_REQ_RESUME_ITEM              = 0x45,         /*请求恢复DDR中的nv单个数据*/
    NV_ICC_CNF                          = 127,
    NV_ICC_BUTT                         = 0xFF,
};
#define NV_ICC_MSG_TYPE_SUM (7)
#define NV_ICC_MSG_ACORE    0
#define NV_ICC_MSG_CCORE    1


/*ICC请求NV_ICC_REQ_RESUME NV_ICC_REQ_RESUME消息结构*/
typedef struct _nv_icc_msg_stru_
{
    u32 send_slice;         /*发送时间*/
    u32 msg_type;           /*消息类型*/
    u32 sn;                 /*序列号*/
    u32 ret;                /*发送消息中存储要写入的NV数量,接收消息中存储处理结果*/
}nv_icc_msg_t;

/*描述单个Nv信息*/
typedef struct _nv_item_info_stru_
{
    u16 itemid;      /* NV ID */
    u16 modem_id;      /* NV Length */
}nv_item_info_t;

/*ICC请求NV_ICC_REQ_PRIORITY_HIGH_FLUSH NV_ICC_REQ_PRIORITY1_FLUSH消息结构*/
typedef struct _nv_icc_msg_single_stru_
{
    u32 send_slice;         /*发送时间*/
    u32 msg_type;           /*消息类型*/
    u32 sn;                 /*序列号*/
    u32 ret;                /*发送消息中存储要写入的NV数量,接收消息中存储处理结果*/
    nv_item_info_t data;    /*要写入的nv信息列表*/
}nv_icc_msg_single_t;

struct nv_icc_msg_name
{
    unsigned int msg_type;
    char         cpu[5];
    char         work[32];
};

int nvim_msg_parse(unsigned int typeid, char *in_buf, unsigned int len, char *out_buf, unsigned int *out_ptr);


