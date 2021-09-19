############################## Help Section ##############################
.PHONY: help

help::
	$(ECHO) "Makefile Usage:"
	$(ECHO) "  make all TARGET=<sw_emu/hw_emu/hw> DEVICE=<FPGA platform> HOST_ARCH=<aarch32/aarch64/x86> EDGE_COMMON_SW=<rootfs and kernel image path>"
	$(ECHO) "      Command to generate the design for specified Target and Shell."
	$(ECHO) "      By default, HOST_ARCH=x86. HOST_ARCH and EDGE_COMMON_SW is required for SoC shells"
	$(ECHO) ""
	$(ECHO) "  make clean "
	$(ECHO) "      Command to remove the generated non-hardware files."
	$(ECHO) ""
	$(ECHO) "  make cleanall"
	$(ECHO) "      Command to remove all the generated files."
	$(ECHO) ""
	$(ECHO)  "  make test DEVICE=<FPGA platform>"
	$(ECHO)  "     Command to run the application. This is same as 'run' target but does not have any makefile dependency."
	$(ECHO)  ""
	$(ECHO) "  make sd_card TARGET=<sw_emu/hw_emu/hw> DEVICE=<FPGA platform> HOST_ARCH=<aarch32/aarch64/x86> EDGE_COMMON_SW=<rootfs and kernel image path>"
	$(ECHO) "      Command to prepare sd_card files."
	$(ECHO) "      By default, HOST_ARCH=x86. HOST_ARCH and EDGE_COMMON_SW is required for SoC shells"
	$(ECHO) ""
	$(ECHO) "  make run TARGET=<sw_emu/hw_emu/hw> DEVICE=<FPGA platform> HOST_ARCH=<aarch32/aarch64/x86> EDGE_COMMON_SW=<rootfs and kernel image path>"
	$(ECHO) "      Command to run application in emulation."
	$(ECHO) "      By default, HOST_ARCH=x86. HOST_ARCH and EDGE_COMMON_SW is required for SoC shells"
	$(ECHO) ""
	$(ECHO) "  make build TARGET=<sw_emu/hw_emu/hw> DEVICE=<FPGA platform> HOST_ARCH=<aarch32/aarch64/x86> EDGE_COMMON_SW=<rootfs and kernel image path>"
	$(ECHO) "      Command to build xclbin application."
	$(ECHO) "      By default, HOST_ARCH=x86. HOST_ARCH and EDGE_COMMON_SW is required for SoC shells"
	$(ECHO) ""
	$(ECHO) "  make host HOST_ARCH=<aarch32/aarch64/x86> EDGE_COMMON_SW=<rootfs and kernel image path>"
	$(ECHO) "      Command to build host application."
	$(ECHO) "      By default, HOST_ARCH=x86. HOST_ARCH and EDGE_COMMON_SW is required for SoC shells"
	$(ECHO) ""

include mk/color.mk

.PHONY: setenv
ifeq ($(MAKELEVEL), 0)
setenv:
	bash -c "source  ~/env/vitis20.2.sh; \
	for var in \$$(compgen -v); do export \$$var; done; \
	$(MAKE) $@"
else

setenv:
	@echo "env has been setup"
endif


.PHONY: info
info:
	$(shell ./src/automation.sh > generator.log)
	@echo  $(BINARY_CONTAINERS)
	@echo  $(MK_PATH)
	@echo  "#####################################################################"
	@echo  -e  ${BLUE}$(APP)${NC}
	@echo  "cfg file:"
	@echo  -e  ${RED}$(CFG_FILE)${NC}
	@echo  "kernels:"
	@echo  -e  ${RED}$(KERNEL_OBJS)${NC}
	@echo  "global vpp flags:"
	@echo  -e  ${RED}$(VPP_FLAGS)${NC}
	@echo  "#####################################################################"

############################## Setting up Project Variables ##############################
# Points to top directory of Git repository
MK_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
COMMON_REPO ?= $(shell bash -c 'export MK_PATH=$(MK_PATH); echo  $${MK_PATH%fpga/*}')
PWD = $(shell readlink -f .)
XF_PROJ_ROOT = $(shell readlink -f $(COMMON_REPO))

CP = cp -rf
VPP := v++


BINARY_CONTAINER_OBJS :=
ALL_OBJECTS :=
KERNEL_OBJS :=
CXXFLAGS :=
LDFLAGS  :=

VPP_FLAGS :=
VPP_LDFLAGS :=

TARGET :=



