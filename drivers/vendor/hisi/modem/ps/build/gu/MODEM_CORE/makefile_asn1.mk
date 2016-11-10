#***********************************************************#
# include the define at the top
#***********************************************************#
include $(BALONG_TOPDIR)/build/scripts/make_base.mk

#***********************************************************#
# moudle name & core define
#***********************************************************#
OBC_LOCAL_CORE_NAME		?=mcore

OBC_LOCAL_MOUDLE_NAME	?=asn1_ccore



#***********************************************************#
# compiler flags
#***********************************************************#
#COMM FLAGS
include $(BALONG_TOPDIR)/modem/ps/build/gu/MODEM_CORE/ps_comm_flags.mk

#USER FLAGS
CC_USER_FLAGS   ?= 
AS_USER_FLAGS   ?=

ifeq ($(CFG_THUMB_COMPILE),YES)
CC_USER_FLAGS += -mthumb -mthumb-interwork -mlong-calls
CC_USER_FLAGS += -march=armv6
else

CC_USER_FLAGS += -march=armv7-a

endif

#***********************************************************#
# compiler defines
#***********************************************************#
#COMM DEFINES
include $(BALONG_TOPDIR)/modem/ps/build/gu/MODEM_CORE/ps_comm_defines.mk

#USER DEFINES
CC_USER_DEFINES ?=

ifneq ($(INSTANCE_ID) ,)
CC_USER_DEFINES	+=-D$(strip $(INSTANCE_ID) )
endif
				

#***********************************************************#
# include Directories
#***********************************************************#
#common include directories
include $(BALONG_TOPDIR)/modem/ps/build/gu/MODEM_CORE/ps_comm.inc

#gas include directories
OBC_LOCAL_INC_DIR ?=
OBC_LOCAL_INC_DIR += \
	$(BALONG_TOPDIR)/modem/config/log \
	$(BALONG_TOPDIR)/modem/config/nvim/include/gu \
	$(BALONG_TOPDIR)/modem/config/osa \
	$(BALONG_TOPDIR)/modem/include/drv \
	$(BALONG_TOPDIR)/modem/include/drv/common \
	$(BALONG_TOPDIR)/modem/include/drv/ccore \
	$(BALONG_TOPDIR)/modem/include/oam/comm/om \
	$(BALONG_TOPDIR)/modem/include/oam/gu/osa \
	$(BALONG_TOPDIR)/modem/include/oam/comm/scm \
	$(BALONG_TOPDIR)/modem/include/oam/gu/si \
	$(BALONG_TOPDIR)/modem/include/oam/comm/socp \
	$(BALONG_TOPDIR)/modem/include/oam/gu/usimm \
	$(BALONG_TOPDIR)/modem/include/oam/comm/errno \
	$(BALONG_TOPDIR)/modem/include/oam/comm/dms \
	$(BALONG_TOPDIR)/modem/include/oam/comm/nvim \
	$(BALONG_TOPDIR)/modem/include/oam/gu/log \
	$(BALONG_TOPDIR)/modem/include/oam/gu/om \
	$(BALONG_TOPDIR)/modem/include/oam/gu/hpa \
	$(BALONG_TOPDIR)/modem/include/oam/lt/ccore \
	$(BALONG_TOPDIR)/modem/include/oam/lt/common \
	$(BALONG_TOPDIR)/modem/include/phy/gphy \
	$(BALONG_TOPDIR)/modem/include/phy/wphy \
	$(BALONG_TOPDIR)/modem/include/ps/gups/acore \
	$(BALONG_TOPDIR)/modem/include/ps/gups/ccore \
	$(BALONG_TOPDIR)/modem/include/ps/gups/common \
	$(BALONG_TOPDIR)/modem/include/ps/tlps/acore \
	$(BALONG_TOPDIR)/modem/include/ps/tlps/ccore \
	$(BALONG_TOPDIR)/modem/include/ps/tlps/common \
	$(BALONG_TOPDIR)/modem/include/ps/nas \
	$(BALONG_TOPDIR)/modem/include/taf/acore \
    $(BALONG_TOPDIR)/modem/include/taf/ccore \
    $(BALONG_TOPDIR)/modem/include/taf/common \
	$(BALONG_TOPDIR)/modem/include/tools \
	$(BALONG_TOPDIR)/modem/include/med \
	$(BALONG_TOPDIR)/modem/include/nv/gu/gas \
	$(BALONG_TOPDIR)/modem/include/nv/gu/ttf \
	$(BALONG_TOPDIR)/modem/include/nv/gu/nas \
	$(BALONG_TOPDIR)/modem/include/nv/gu/oam \
	$(BALONG_TOPDIR)/modem/include/nv/gu/was \
	$(BALONG_TOPDIR)/modem/include/nv/gu/phy \
	$(BALONG_TOPDIR)/modem/include/nv/gu/codec \
	$(BALONG_TOPDIR)/modem/include/nv/tl/lps

OBC_LOCAL_INC_DIR += \
    $(BALONG_TOPDIR)/modem/ps/inc/gu \
    $(BALONG_TOPDIR)/modem/ps/inc/lt \
    $(BALONG_TOPDIR)/modem/ps/comm/inc \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/common/LOGFILTER/Inc \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/common/DICC/Inc \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/common/FLOWCTRL/Inc \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/ccore/FLOWCTRL/Inc \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/common/LINK/Inc \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/ccore/LINK/Inc \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/ccore/MEMCTRL/Inc \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/ccore/MEM/Inc \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/common/STL/Inc \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/ccore/CDS/Inc \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/ccore/encodix/common \
    $(BALONG_TOPDIR)/modem/ps/comm/gu/zlib/Inc \
    $(BALONG_TOPDIR)/modem/ps/as/gu/inc/Pscfg \
    $(BALONG_TOPDIR)/modem/ps/as/gu/inc/Comm \
    $(BALONG_TOPDIR)/modem/ps/as/gu/inc/Gtf \
    $(BALONG_TOPDIR)/modem/ps/as/gu/inc/Wtf \
    $(BALONG_TOPDIR)/modem/ps/as/gu/inc/Gas \
    $(BALONG_TOPDIR)/modem/ps/as/gu/inc/Was \
	$(BALONG_TOPDIR)/modem/ps/as/gu/comm/ASN1/Codec \
	$(BALONG_TOPDIR)/modem/ps/as/gu/comm/Cipher/Inc \
	$(BALONG_TOPDIR)/modem/ps/as/gu/comm/Frmwk/Inc \
	$(BALONG_TOPDIR)/modem/ps/as/gu/comm/Pscfg/Inc \
	$(BALONG_TOPDIR)/modem/ps/as/gu/gas/COMMON/Inc \
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/MNTN/Inc \
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/DATA/Inc \
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GASM/Inc \
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/Inc \
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMC/Inc \
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMM/Inc \
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMSI/Inc \
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GRR/Inc \
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/RR/Inc \
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCBS/Inc \
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/ASN1/Inc \
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/LCS/Inc \
	$(BALONG_TOPDIR)/modem/ps/as/gu/was/PUBLIC \
    $(BALONG_TOPDIR)/modem/ps/as/gu/was/Inc \

OBC_LOCAL_INC_DIR += \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/ccore/encodix/gsm/r9/Inc

#***********************************************************#
# source files
#***********************************************************#
ifeq ($(INSTANCE_ID), INSTANCE_0)
OBC_LOCAL_SRC_FILE := \
    $(BALONG_TOPDIR)/modem/ps/as/gu/comm/ASN1/Codec/per_basic.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/comm/ASN1/Codec/per_com.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/comm/ASN1/Codec/per_down.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/comm/ASN1/Codec/per_frag.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/comm/ASN1/Codec/per_init.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/comm/ASN1/Codec/per_len.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/comm/ASN1/Codec/per_mark.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/comm/ASN1/Codec/per_mem.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/comm/ASN1/Codec/per_num.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/comm/ASN1/Codec/per_open.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/comm/ASN1/Codec/per_show.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/comm/ASN1/Codec/per_upper.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/comm/ASN1/Codec/per_main_comm.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/comm/ASN1/Codec/per_combine.c

OBC_LOCAL_SRC_FILE += \
    $(BALONG_TOPDIR)/modem/ps/as/gu/comm/ASN1/Codec/per_main.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/was/ASN1/Dec/message_list_rom_dec.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/was/ASN1/Dec/rrc_dec_per_rom.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/was/ASN1/Enc/message_list_rom_enc.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/was/ASN1/Enc/rrc_enc_per_rom.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/was/ASN1/Itf/WasAsn1Api.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/was/ASN1/DynEnc/message_list_rom_dyn_enc.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/was/ASN1/DynEnc/rrc_dyn_enc_per_rom.c

OBC_LOCAL_SRC_FILE += \
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/COMMON/Src/GasDynBuf.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/comm/ASN1/Codec/per_main_gas.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/ASN1/Src/GasAsn1Api.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/ASN1/Src/GasMsgLstRomDec.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/ASN1/Src/GasMsgLstRomDynEnc.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/ASN1/Src/GasRRLPV920DecPerRom.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/ASN1/Src/GasRRLPV920DynEncPerRom.c

endif



#***********************************************************
#include rules. must be droped at the bottom, OBB_BUILD_ACTION values: cc tqe lint fortify
#***********************************************************
include $(BALONG_TOPDIR)/build/scripts/rules/$(OBB_BUILD_ACTION)_rtosck_rules.mk

