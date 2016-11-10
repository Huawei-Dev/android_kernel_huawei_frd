obj-y	+= hisi_atfisp.o
ifeq ($(CONFIG_ATFDRIVER), y)
EXTRA_CFLAGS += -Idrivers/atfdriver \
				-Idrivers/hisi/tzdriver \
				-Idrivers/hisi/load_image \
				-I$(src)/../../../../../../../external/efipartition
endif
