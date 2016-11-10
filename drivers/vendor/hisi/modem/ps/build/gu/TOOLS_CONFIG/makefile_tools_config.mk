# Created by Jeffery.zhai/199916 2012.03.21

#***********************************************************#
# include the define at the top
#***********************************************************#
include $(BALONG_TOPDIR)/build/scripts/make_base.mk

MOUDULES :=
ifneq ($(wildcard $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/),)
MOUDULES += tools_config_gttf tools_config_wttf
endif
ifneq ($(wildcard $(BALONG_TOPDIR)/modem/ps/as/gu/gas/),)
MOUDULES += tools_config_gas
endif
ifneq ($(wildcard $(BALONG_TOPDIR)/modem/ps/as/gu/was/),)
MOUDULES += tools_config_was
endif
ifneq ($(wildcard $(BALONG_TOPDIR)/modem/ps/nas/comm/),)
MOUDULES += tools_config_gucnas
endif

# process
.PHONY: tools_config tools_config_gups tools_config_gttf tools_config_wttf tools_config_gas tools_config_was tools_config_gucnas
tools_config: $(MOUDULES)
tools_config_gups: $(MOUDULES)
tools_config_gttf:
	$(MAKE)  -f TOOLS_CONFIG/makefile_tools_config_gttf.mk
tools_config_wttf:
	$(MAKE)  -f TOOLS_CONFIG/makefile_tools_config_wttf.mk
tools_config_gas:
	$(MAKE)  -f TOOLS_CONFIG/makefile_tools_config_gas.mk
tools_config_was:
	$(MAKE)  -f TOOLS_CONFIG/makefile_tools_config_was.mk
tools_config_gucnas:
	$(MAKE)  -f TOOLS_CONFIG/makefile_tools_config_gucnas.mk


CLEAN_MOUDULES:= 
ifneq ($(wildcard $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/),)
CLEAN_MOUDULES += clean-tools_config_gttf clean-tools_config_wttf
endif
ifneq ($(wildcard $(BALONG_TOPDIR)/modem/ps/as/gu/gas/),)
CLEAN_MOUDULES += clean-tools_config_gas
endif
ifneq ($(wildcard $(BALONG_TOPDIR)/modem/ps/as/gu/was/),)
CLEAN_MOUDULES += clean-tools_config_was
endif
ifneq ($(wildcard $(BALONG_TOPDIR)/modem/ps/nas/comm/),)
CLEAN_MOUDULES += clean-tools_config_gucnas
endif

# process
.PHONY: clean-tools_config clean-tools_config_gups clean-tools_config_gttf clean-tools_config_wttf clean-tools_config_gas clean-tools_config_was clean-tools_config_gucnas
clean-tools_config: $(CLEAN_MOUDULES)
clean-tools_config_gups: $(CLEAN_MOUDULES)
clean-tools_config_gttf:
	$(MAKE)  -f TOOLS_CONFIG/makefile_tools_config_gttf.mk clean
clean-tools_config_wttf:
	$(MAKE)  -f TOOLS_CONFIG/makefile_tools_config_wttf.mk clean
clean-tools_config_gas:
	$(MAKE)  -f TOOLS_CONFIG/makefile_tools_config_gas.mk clean
clean-tools_config_was:
	$(MAKE)  -f TOOLS_CONFIG/makefile_tools_config_was.mk clean
clean-tools_config_gucnas:
	$(MAKE)  -f TOOLS_CONFIG/makefile_tools_config_gucnas.mk clean
