/*
 * voice_proxy_interface.h -- the interface define bwteen the hifi_voice and AP_voice_proxy
 * 
 * Copyright (c) 2015 Hisilicon Technologies CO., Ltd.
 */

#ifndef __VOICE_PROXY_INTERFACE_H__
#define __VOICE_PROXY_INTERFACE_H__


#define PROXY_VOICE_CODEC_MAX_DATA_LEN (32)
#define PROXY_VOICE_RTP_MAX_DATA_LEN (256)
#define PROXY_MSG_HEADER	unsigned long uwSenderCpuId;  \
							unsigned long uwSenderPid;    \
						 	unsigned long uwReceiverCpuId;\
						 	unsigned long uwReceiverPid;  \
						 	unsigned long uwLength;

enum proxy_voice_error_flag {
	PROXY_VOICE_NO_ERROR = 0,
	PROXY_VOICE_ERROR,
	PROXY_VOICE_ERRORFLAG_BUTT
};

enum proxy_voice_media_type {
	PROXY_VOICE_TYPE_AMR = 0,
	PROXY_VOICE_TYPE_EFR,
	PROXY_VOICE_TYPE_FR,
	PROXY_VOICE_TYPE_HR,
	PROXY_VOICE_TYPE_AMR2,
	PROXY_VOICE_TYPE_AMRWB,
	PROXY_VOICE_TYPE_G711,
	PROXY_VOICE_TYPE_EVRC,
	PROXY_VOICE_TYPE_EVRCB,
	PROXY_VOICE_TYPE_BUTT
};

enum proxy_voice_amr_rate_mode {
	PROXY_VOICE_AMR_RATE_MODE_475K = 0,
	PROXY_VOICE_AMR_RATE_MODE_515K,
	PROXY_VOICE_AMR_RATE_MODE_590K,
	PROXY_VOICE_AMR_RATE_MODE_670K,
	PROXY_VOICE_AMR_RATE_MODE_740K,
	PROXY_VOICE_AMR_RATE_MODE_795K,
	PROXY_VOICE_AMR_RATE_MODE_102K,
	PROXY_VOICE_AMR_RATE_MODE_122K,
	PROXY_VOICE_AMR_RATE_MODE_BUTT
};

/* DTX means Discontinuous Transmit technology */
enum proxy_voice_dtx_switch {
	PROXY_VOICE_DTXDISABLE = 0,
	PROXY_VOICE_DTXENABLE,
	PROXY_VOICE_DTXBUTT
};

enum voice_proxy_status_type {
	VOICE_PROXY_OK = 0,
	VOICE_PROXY_NOT_OK,
	VOICE_PROXY_BUTT
};

enum voice_proxy_socket_config {
	VOICE_PROXY_SOCKET_CONFIG = 0,
	VOICE_PROXY_SOCKET_NOT_CONFIG,
	VOICE_PROXY_SOCKET_BUTT
};

enum voice_proxy_amr_type_tx {
	VOICE_PROXY_AMR_TYPE_TX_SPEECH_GOOD = 0,
	VOICE_PROXY_AMR_TYPE_TX_SID_FIRST,
	VOICE_PROXY_AMR_TYPE_TX_SID_UPDATE,
	VOICE_PROXY_AMR_TYPE_TX_NO_DATA,
	VOICE_PROXY_AMR_TYPE_TX_SPEECH_DEGRADED,
	VOICE_PROXY_AMR_TYPE_TX_SPEECH_BAD,
	VOICE_PROXY_AMR_TYPE_TX_SID_BAD,
	VOICE_PROXY_AMR_TYPE_TX_ONSET,
	VOICE_PROXY_AMR_TYPE_TX_BUTT
};

/* the MsgID define between PROXY and Voice */
enum proxy_voice_msg_id {
	ID_PROXY_VOICE_LTE_RX_NTF = 0xDDF0,
	ID_VOICE_PROXY_LTE_RX_CNF = 0xDDF1,
	ID_VOICE_PROXY_LTE_RX_NTF = 0xDDF2,
	ID_PROXY_VOICE_LTE_RX_CNF = 0xDDF3,
	ID_VOICE_PROXY_LTE_TX_NTF = 0xDDF4,
	ID_PROXY_VOICE_LTE_TX_CNF = 0xDDF5,
	ID_PROXY_VOICE_LTE_TX_NTF = 0xDDF6,
	ID_VOICE_PROXY_LTE_TX_CNF = 0xDDF7,

	ID_PROXY_VOICE_WIFI_RX_NTF = 0xDDF8,
	ID_VOICE_PROXY_WIFI_RX_CNF = 0xDDF9,
	ID_VOICE_PROXY_WIFI_TX_NTF = 0xDDFA,
	ID_PROXY_VOICE_WIFI_TX_CNF = 0xDDFB,
	ID_PROXY_VOICE_STATUS_IND = 0xDDFC,

	ID_PROXY_VOICE_ENCRYPT_KEY_BEGIN = 0xDDFD,
	ID_PROXY_VOICE_ENCRYPT_KEY_END = 0xDDFE,
	ID_PROXY_VOICE_DATA_MSGID_BUT
};

/*
 * describe: the struct of the Rx request between PROXY and hifi_voice by lte
 */
struct proxy_voice_lte_rx_notify {
	PROXY_MSG_HEADER
	unsigned short msg_id;
	unsigned short sn;
	unsigned long  ts;
	unsigned short codec_type;
	unsigned short dtx_enable;
	unsigned short rate_mode;
	unsigned short error_flag;
	unsigned short frame_type;
	unsigned short quality_idx;
	unsigned short data[PROXY_VOICE_CODEC_MAX_DATA_LEN];
	unsigned long  ssrc;
	unsigned long  reserved;
};

/*
 * describe: the struct of the Rx request between PROXY and hifi_voice by wifi
 * the struct is as the same to IMSA_VOICE_RTP_RX_DATA_IND_STRU
 */
struct proxy_voice_wifi_rx_notify {
	PROXY_MSG_HEADER
	unsigned short msg_id;
	unsigned short reserved;
	unsigned long  channel_id;
	unsigned long  port_type; /* Port Type: 0 RTP; 1 RTCP */
	unsigned short data_len;
	unsigned short data_seq;
	unsigned char  frag_seq;
	unsigned char  frag_max;
	unsigned short reserved2;
	unsigned char  data[PROXY_VOICE_RTP_MAX_DATA_LEN];
};

/*
 * describe: the struct of the confirm between PROXY and hifi_voice
 */
struct proxy_voice_confirm {
	PROXY_MSG_HEADER
	unsigned short msg_id;
	unsigned short reserved;
	unsigned long  result;
};

/*
 * describe: the struct of the Tx request between Voice-Proxy and hifi_voice by lte
 */
struct voice_proxy_lte_tx_notify {
	PROXY_MSG_HEADER
	unsigned short msg_id;
	unsigned short reserved;
	unsigned short amr_type;
	unsigned short frame_type;
	unsigned short data[PROXY_VOICE_CODEC_MAX_DATA_LEN];
};

/*
 * describe: the struct of the Tx request between Voice-Proxy and hifi_voice by wifi
 */
struct voice_proxy_wifi_tx_notify {
	PROXY_MSG_HEADER
	unsigned short msg_id;
	unsigned short reserved;
	unsigned long  channel_id;
	unsigned long  port_type;
	unsigned short date_len;
	unsigned short data_seq;
	unsigned char  frag_seq;
	unsigned char  frag_max;
	unsigned short reserved2;
	unsigned char  data[PROXY_VOICE_RTP_MAX_DATA_LEN];
};

struct voice_proxy_status {
	PROXY_MSG_HEADER
	unsigned short msg_id;
	unsigned short reserved;
	unsigned long  status;
	unsigned long  socket_cfg;
};

struct proxy_voice_encrypt_key_begin {
	PROXY_MSG_HEADER
	unsigned short msg_id;
	unsigned short reserved;
};

struct proxy_voice_encrypt_key_end {
	PROXY_MSG_HEADER
	unsigned short msg_id;
	unsigned short reserved;
	unsigned char encrypt_negotiation_result;
	unsigned char reserved2[3];
};

#endif /* __VOICE_PROXY_INTERFACE_H__ */
