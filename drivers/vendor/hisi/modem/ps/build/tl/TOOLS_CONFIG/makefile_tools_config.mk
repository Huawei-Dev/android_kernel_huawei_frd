# Created by Jeffery.zhai/199916 2012.03.21

#***********************************************************#
# include the define at the top
#***********************************************************#
include $(BALONG_TOPDIR)/build/scripts/make_base.mk

MOUDULES :=
ifneq ($(wildcard $(BALONG_TOPDIR)/modem/ps/as/tl/),)
MOUDULES += tools_config_tlas
endif
ifneq ($(wildcard $(BALONG_TOPDIR)/modem/ps/nas/tl/),)
MOUDULES += tools_config_tlnas
endif

# process
.PHONY: tools_config tools_config_lps tools_config_tlas tools_config_tlnas
tools_config : $(MOUDULES)
tools_config_lps : $(MOUDULES)
tools_config_tlas:
	$(MAKE)  -f TOOLS_CONFIG/makefile_tools_config_tlas.mk
tools_config_tlnas:
	$(MAKE)  -f TOOLS_CONFIG/makefile_tools_config_tlnas.mk

#clean
CLEAN_MOUDULES:= 
ifneq ($(wildcard $(BALONG_TOPDIR)/modem/ps/as/tl/),)
CLEAN_MOUDULES += clean-tools_config_tlas
endif
ifneq ($(wildcard $(BALONG_TOPDIR)/modem/ps/nas/tl/),)
CLEAN_MOUDULES += clean-tools_config_tlnas
endif

# process
.PHONY: clean-tools_config clean-tools_config_lps clean-tools_config_tlas clean-tools_config_tlnas
clean-tools_config: $(CLEAN_MOUDULES)
clean-tools_config_lps: $(CLEAN_MOUDULES)
clean-tools_config_tlas:
	$(MAKE)  -f TOOLS_CONFIG/makefile_tools_config_tlas.mk clean
clean-tools_config_tlnas:
	$(MAKE)  -f TOOLS_CONFIG/makefile_tools_config_tlnas.mk clean
