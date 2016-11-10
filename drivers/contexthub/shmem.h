#ifndef __LINUX_SHMEM_H__
#define __LINUX_SHMEM_H__
#include "protocol.h"

extern int shmem_notifier_register(obj_tag_t module_id,
	void (*notifier_call)(void __iomem *buf_addr, unsigned int buf_size));
extern int shmem_notifier_unregister(obj_tag_t module_id);
extern int shmem_send(obj_tag_t module_id, const void *usr_buf, unsigned int usr_buf_size);

#endif
