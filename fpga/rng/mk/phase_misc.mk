



HOST_ARCH := x86
SYSROOT :=
include ./utils.mk
XSA :=
ifneq ($(DEVICE), )
XSA := $(call device2xsa, $(DEVICE))
endif


EMCONFIG_DIR = $(TEMP_DIR)
BINARY_CONTAINERS += $(BUILD_DIR)/kernel.xclbin

PACKAGE_OUT = ./package_$(APP).$(TARGET)


include $(XF_PROJ_ROOT)/common/includes/opencl/opencl.mk

CXXFLAGS += $(opencl_CXXFLAGS) -Wall -O0 -g -std=c++11
LDFLAGS += $(opencl_LDFLAGS)

ifeq ($(findstring 2018, $(DEVICE)), 2018)
$(error [ERROR]: This example is not supported for $(DEVICE).)
endif
ifeq ($(findstring qep, $(DEVICE)), qep)
$(error [ERROR]: This example is not supported for $(DEVICE).)
endif
ifeq ($(findstring aws-vu9p-f1, $(DEVICE)), aws-vu9p-f1)
$(error [ERROR]: This example is not supported for $(DEVICE).)
endif
ifeq ($(findstring samsung, $(DEVICE)), samsung)
$(error [ERROR]: This example is not supported for $(DEVICE).)
endif
ifeq ($(findstring zc702, $(DEVICE)), zc702)
$(error [ERROR]: This example is not supported for $(DEVICE).)
endif
ifeq ($(findstring nodma, $(DEVICE)), nodma)
$(error [ERROR]: This example is not supported for $(DEVICE).)
endif


.PHONY: all clean cleanall emconfig
all:
	$(MAKE) info
	$(MAKE) check-devices $(EXECUTABLE) $(BINARY_CONTAINERS)
	$(MAKE) emconfig



emconfig:$(EMCONFIG_DIR)/emconfig.json
$(EMCONFIG_DIR)/emconfig.json:
	emconfigutil --platform $(DEVICE) --od $(EMCONFIG_DIR)


############################## Cleaning Rules ##############################
# Cleaning stuff
clean:
	-$(RMDIR) $(EXECUTABLE) $(XCLBIN)/{*sw_emu*,*hw_emu*}
	-$(RMDIR) profile_* TempConfig system_estimate.xtxt *.rpt *.csv
	-$(RMDIR) src/*.ll *v++* .Xil emconfig.json dltmp* xmltmp* *.log *.jou *.wcfg *.wdb
	-$(RMDIR) *.app
	-$(RMDIR) *.protoinst

cleanall: clean
	-$(RMDIR) build_dir* sd_card*
	-$(RMDIR) package.*
	-$(RMDIR) _x* *xclbin.run_summary qemu-memory-_* emulation _vimage pl* start_simulation.sh *.xclbin
	-$(RMDIR) fast_build

