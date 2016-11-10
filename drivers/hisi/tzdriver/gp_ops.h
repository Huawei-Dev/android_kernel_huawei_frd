#ifndef _GP_OPS_H_
#define _GP_OPS_H_

extern struct ion_client *drm_ion_client;
int tc_user_param_valid(TC_NS_ClientContext *client_context, int n);
int tc_client_call(TC_NS_ClientContext *context, TC_NS_DEV_File *dev_file,
		   uint8_t flags);
int tee_init_crypto(char *hash_type);
int tee_calc_task_hash(char *digest);

#endif
