
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/platform_device.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/sched.h>
#include <linux/list.h>
#include <linux/mutex.h>
#include <asm/cacheflush.h>
#include <linux/kthread.h>
#include <linux/freezer.h>
#include <linux/kernel.h>
#include <linux/version.h>

#include "tc_ns_client.h"
#include "teek_client_api.h"
#include "teek_client_id.h"
#include "teek_ns_client.h"
#define TEEC_GENERAL_SRV_OPEN_SESSION 1
#define TEEC_GENERAL_SRV_CLOSE_SESSION 0

#define TEE_SERVICE_IMAXCRYPTO \
{ \
    0x09090909, \
        0x0909, \
        0x0909, \
        { \
            0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09 \
        } \
}

#define PRE_ALLOC_CRYPTO_BUF_SIZE 2*1024*1024

typedef struct {
    uint32_t bufferL;
    uint32_t bufferH;
    uint32_t size;
} Cipher_TempMemoryReference;

typedef struct {
    uint32_t a;
    uint32_t b;
} Cipher_Value;

typedef union {
    Cipher_TempMemoryReference tmpref;
    Cipher_Value value;
} Cipher_Parameter;

/* when 32to64 TEEC_Operation can not the same, so use this */
typedef struct {
    uint32_t started;
    uint32_t paramTypes;
    Cipher_Parameter params[4];
} Cipher_Operation;


TEEC_Session session;
TEEC_Context context;
int session_cnt = 0;
char* g_crypto_in_buf;
char* g_crypto_out_buf;

static void operation_usrtokernel(Cipher_Operation* kusr_operation, TEEC_Operation* operation)
{
    uint32_t param_type[4];
    uint32_t param_cnt;
    unsigned long  temp;
    operation->started = kusr_operation->started;
    operation->paramTypes = kusr_operation->paramTypes;
    param_type[0] = TEEC_PARAM_TYPE_GET(operation->paramTypes, 0);
    param_type[1] = TEEC_PARAM_TYPE_GET(operation->paramTypes, 1);
    param_type[2] = TEEC_PARAM_TYPE_GET(operation->paramTypes, 2);
    param_type[3] = TEEC_PARAM_TYPE_GET(operation->paramTypes, 3);

    //user cipher operation not use sharemem
    for (param_cnt = 0;param_cnt < 4;param_cnt++) {
        if ((param_type[param_cnt] == TEEC_MEMREF_TEMP_INPUT)
                || (param_type[param_cnt] == TEEC_MEMREF_TEMP_OUTPUT)
                || (param_type[param_cnt] == TEEC_MEMREF_TEMP_INOUT)) {
            temp = (unsigned long)kusr_operation->params[param_cnt].tmpref.bufferH;
            operation->params[param_cnt].tmpref.buffer = (kusr_operation->params[param_cnt].tmpref.bufferL) |
                (temp << 32);
            operation->params[param_cnt].tmpref.size = kusr_operation->params[param_cnt].tmpref.size;
            TCDEBUG("temp buff %p\n", operation->params[param_cnt].tmpref.buffer);
        }  else if ((param_type[param_cnt] ==  TEEC_VALUE_INPUT)
                || (param_type[param_cnt] == TEEC_VALUE_OUTPUT)
                || (param_type[param_cnt] == TEEC_VALUE_INOUT)) {
            operation->params[param_cnt].value.a = kusr_operation->params[param_cnt].value.a;
            operation->params[param_cnt].value.b = kusr_operation->params[param_cnt].value.b;
        }
    }
}

static void operation_kerneltousr(TEEC_Operation* operation, Cipher_Operation* kusr_operation)
{
    uint32_t param_type[4];
    uint32_t param_cnt;
    param_type[0] = TEEC_PARAM_TYPE_GET(operation->paramTypes, 0);
    param_type[1] = TEEC_PARAM_TYPE_GET(operation->paramTypes, 1);
    param_type[2] = TEEC_PARAM_TYPE_GET(operation->paramTypes, 2);
    param_type[3] = TEEC_PARAM_TYPE_GET(operation->paramTypes, 3);

    //user cipher operation not use sharemem
    for (param_cnt = 0;param_cnt < 4;param_cnt++) {
        if ((param_type[param_cnt] == TEEC_MEMREF_TEMP_OUTPUT)
                || (param_type[param_cnt] == TEEC_MEMREF_TEMP_INOUT)) {
            kusr_operation->params[param_cnt].tmpref.size = operation->params[param_cnt].tmpref.size;
        }  else if ((param_type[param_cnt] == TEEC_VALUE_OUTPUT)
                || (param_type[param_cnt] == TEEC_VALUE_INOUT)) {
            kusr_operation->params[param_cnt].value.a = operation->params[param_cnt].value.a;
            kusr_operation->params[param_cnt].value.b = operation->params[param_cnt].value.b;
        }
    }
}

static int set_general_srv_login_infor(TC_NS_DEV_File* dev_file, TEEC_Operation* operation)
{
    uint32_t param_type[4];
    param_type[2] = TEEC_PARAM_TYPE_GET(operation->paramTypes, 2);
    param_type[3] = TEEC_PARAM_TYPE_GET(operation->paramTypes, 3);

   /*if login info can get from user, donot use ca_deamon set in dev file */
    if (param_type[2] == TEEC_MEMREF_TEMP_INPUT && param_type[3] == TEEC_MEMREF_TEMP_INPUT) {
        TCDEBUG("user set the login info, no set again\n");
        return 0;
    }

    /* The daemon has failed to get login information or not supplied */
    if (dev_file->pkg_name_len == 0) {
        return -1;
    }
    /* get package name len and package name */
    operation->params[3].tmpref.size = dev_file->pkg_name_len;
    /* The 3rd parameter buffer points to the pkg name buffer in the
     * device file pointer */
    operation->params[3].tmpref.buffer = dev_file->pkg_name;

    TCDEBUG("pkg name len %d\n", dev_file->pkg_name_len);
    TCDEBUG("pkg name %s\n", dev_file->pkg_name);
    TCDEBUG("pub_key_len len %d\n", dev_file->pub_key_len);
    /* Set public key len and public key*/
    if (dev_file->pub_key_len != 0) {
        operation->params[2].tmpref.size = dev_file->pub_key_len;
        operation->params[2].tmpref.buffer = dev_file->pub_key;
    } else {
        uint32_t ca_uid;
        struct task_struct *task = current;
        const struct cred *cred;
        cred = get_task_cred(task);
        if (!cred) {
            TCDEBUG("failed to get task cred, so cannot get uid of the task\n");
            return -1;
        }

        /* If get public key failed, then get uid in kernel */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 13, 0))
		kuid_t kuid;

		kuid = cred->uid;
		ca_uid = kuid.val;
#else
        ca_uid = cred->uid;
#endif
        if (ca_uid == -1) {
            TCDEBUG("Failed to get uid of the task\n");
            return -1;
        }
        dev_file->pub_key_len = sizeof(ca_uid);
        operation->params[2].tmpref.size = dev_file->pub_key_len;

        memcpy(dev_file->pub_key, &ca_uid, dev_file->pub_key_len);
        operation->params[2].tmpref.buffer = dev_file->pub_key;
    }

    operation->paramTypes = TEEC_PARAM_TYPES(
            TEEC_NONE,
            TEEC_NONE,
            TEEC_MEMREF_TEMP_INPUT,
            TEEC_MEMREF_TEMP_INPUT);

    return 0;
}

static int check_permission(TC_NS_DEV_File* dev_file, TEEC_Operation *operation)
{
    TEEC_Result result;
    int ret = 0;
    TEEC_UUID svc_uuid = TEE_SERVICE_IMAXCRYPTO;
    TEEC_Session session;
    TEEC_Context context;

    result = TEEK_InitializeContext(
            NULL,
            &context);
    if(result != TEEC_SUCCESS) {
        TCERR("TEEK_InitializeContext failed!\n");
        ret = (int)result;
        goto error;
    }

    operation->started = 1;
    ret = set_general_srv_login_infor(dev_file, operation);
    if (ret < 0) {
        TCERR("set_general_srv_login_infor failed!\n");
        TEEK_FinalizeContext(&context);
        goto error;
    }

    result = TEEK_OpenSession(
            &context,
            &session,
            &svc_uuid,
            TEEC_LOGIN_IDENTIFY,
            NULL,
            operation,
            NULL);

    if (result != TEEC_SUCCESS)
    {
        TCERR("TEEK_OpenSession failed!\n");
        ret = (int)result;
        TEEK_FinalizeContext(&context);
        goto error;
    }

    TEEK_CloseSession(&session);
    TEEK_FinalizeContext(&context);

error:
    return ret;
}

int general_srv_session_handle(TC_NS_DEV_File* dev_file, void* argp)
{
	if(dev_file)
	{
	    TCDEBUG("s00311356:general_srv_session_handle:package_name_len is %u\n", dev_file->pkg_name_len);
	}
	else
	{
		TCDEBUG("s00311356:general_srv_session_handle:dev_file is null");
	}
    TEEC_Result result;
    int ret = 0;
    int handle = 0;
    TEEC_UUID svc_uuid = TEE_SERVICE_IMAXCRYPTO;

    TC_SRV_ClientContext srv_context;
    TEEC_Operation operation;
    void* usr_operation;
    Cipher_Operation kusr_operation;

    memset(&operation, 0x0, sizeof(TEEC_Operation));
    if(copy_from_user(&srv_context, argp, sizeof(TC_SRV_ClientContext))){
        TCERR("copy argp from user failed\n");
        ret =  -ENOMEM;
        return ret;
    }

    //consider when usr is 32bit
    usr_operation = (void*)srv_context.operationh;
    usr_operation = (void*)(((long)usr_operation << 32) | (srv_context.operationl));
    TCDEBUG("usr_operation: %p, oph: %x, opl: %x\n", usr_operation, srv_context.operationh, srv_context.operationl);
    /* operation from user, used to check operation pointer is valid */
    if(copy_from_user(&kusr_operation, usr_operation, sizeof(Cipher_Operation))){
        TCERR("copy usr operation from user failed\n");
        ret =  -ENOMEM;
        return ret;
    }
    operation_usrtokernel(&kusr_operation, &operation);

    handle = srv_context.cmd_id;
    if (handle == TEEC_GENERAL_SRV_OPEN_SESSION) {
        if (session_cnt > 0) {
            session_cnt++;
            ret = check_permission(dev_file, &operation);
            TCDEBUG("---open session in kernel, session open count %d, check permission ret %d\n", session_cnt, ret);
            return ret;
        }

        /* do first session open, session only open once */
        result = TEEK_InitializeContext(
                NULL,
                &context);

        if(result != TEEC_SUCCESS) {
            TCERR("TEEK_InitializeContext failed!\n");
            ret = (int)result;
            goto error;
        }

        operation.started = 1;
        ret = set_general_srv_login_infor(dev_file, &operation);
        if (ret < 0) {
            TCERR("set_general_srv_login_infor failed!\n");
            TEEK_FinalizeContext(&context);
            goto error;
        }

        result = TEEK_OpenSession(
                &context,
                &session,
                &svc_uuid,
                TEEC_LOGIN_IDENTIFY,
                NULL,
                &operation,
                NULL);

        if (result != TEEC_SUCCESS)
        {
            TCERR("TEEK_OpenSession failed!\n");
            ret = (int)result;
            TEEK_FinalizeContext(&context);
            goto error;
        }
        session_cnt = 1;
        TCDEBUG("---open session in kernel, first session open, session count %d\n", session_cnt);
    } else if (TEEC_GENERAL_SRV_CLOSE_SESSION == handle){
        session_cnt--;
        TCDEBUG("---close session in kernel, session close count %d\n", session_cnt);
        if (session_cnt == 0) {
            TEEK_CloseSession(&session);
            TEEK_FinalizeContext(&context);
        } else if(session_cnt < 0) {
            TCERR("session cnt error: %d\n", session_cnt);
            session_cnt = 0;
        }
    } else {
        TCERR("general srv err cmd\n");
    }

error:
    return ret;
}

int general_srv_cmd_handle(void* argp)
{
    TC_SRV_ClientContext srv_context;
    TEEC_Operation operation;
    TEEC_Result result;
    int ret = 0;
    void* usr_operation;
    Cipher_Operation kusr_operation;

    memset(&operation, 0x0, sizeof(TEEC_Operation));
    if(copy_from_user(&srv_context, argp, sizeof(TC_SRV_ClientContext))){
        TCERR("copy argp from user failed\n");
        ret =  -ENOMEM;
        return ret;
    }

    //consider when usr is 32bit
    usr_operation = (void*)srv_context.operationh;
    usr_operation = (void*)(((long)usr_operation << 32) | (srv_context.operationl));
    TCDEBUG("usr_operation: %p, oph: %x, opl: %x\n", usr_operation, srv_context.operationh, srv_context.operationl);
    /* operation from user, used to check operation pointer is valid */
    if(copy_from_user(&kusr_operation, usr_operation, sizeof(Cipher_Operation))){
        TCERR("copy usr operation from user failed\n");
        ret =  -ENOMEM;
        return ret;
    }
    operation_usrtokernel(&kusr_operation, &operation);

    /* kalloc 2M may fail in kernel, alloc when init, copy from user to kernel now */
    TCDEBUG("---general srv cmd handle\n");
    /* operation will change to client_context in invokecommand, do pointer operation */
    result = TEEK_InvokeCommand(&session, srv_context.cmd_id, &operation, NULL);
    if (result != TEEC_SUCCESS) {
        ret = (int)result;
        return ret;
    }

    //copy for size ret
    operation_kerneltousr(&operation, &kusr_operation);
    if(copy_to_user(usr_operation, &kusr_operation, sizeof(Cipher_Operation))){
        TCERR("copy from user failed\n");
        ret =  -ENOMEM;
        return ret;
    }

    return ret;
}

void general_srv_init(void)
{/*
    g_crypto_in_buf  = (void*) __get_free_pages(GFP_KERNEL, get_order(ROUND_UP(PRE_ALLOC_CRYPTO_BUF_SIZE, SZ_4K)));
    g_crypto_out_buf  = (void*) __get_free_pages(GFP_KERNEL, get_order(ROUND_UP(PRE_ALLOC_CRYPTO_BUF_SIZE, SZ_4K)));
    if(!g_crypto_in_buf || !g_crypto_out_buf){
        TCERR("g_mem_pre_allocated failed\n");
    }
*/
}
