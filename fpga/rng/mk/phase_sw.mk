############################## Setting up Host Variables ##############################
#Include Required Host Source Files
CXXFLAGS += -I$(XF_PROJ_ROOT)/common/includes/xcl2

# Host compiler global settings
CXXFLAGS += -fmessage-length=0
CXXFLAGS += -I ./src/
LDFLAGS += -lrt -lstdc++ -lxilinxopencl

ifneq ($(HOST_ARCH), x86)
	LDFLAGS += --sysroot=$(SYSROOT)
endif



#ALL_OBJECTS_PATH:=$(addprefix $(TEMP_DIR)/host/,$(SRC_OBJS))
#ALL_OBJECTS:= $(addsuffix .o, $(ALL_OBJECTS_PATH))

############################## Setting Rules for Host (Building Host Executable) ##############################
$(EXECUTABLE):  check-xrt | $(ALL_OBJECTS)
	@echo ${LIGHT_BLUE}"Start linking files"${NC}
	@echo $(LDFLAGS)
	@$(CXX)   $(CXXFLAGS) $(LDFLAGS) $(ALL_OBJECTS) -Xlinker -o$@ $(LDFLAGS) $(LIBS)
	@echo ${LIGHT_BLUE}"Build done"${NC}



.PHONY: cleanhost
cleanhost:
	$(MAKE) cleanobj
	$(MAKE) $(EXECUTABLE)
	@echo "build a clean host"

cleanobj:
	-$(RMDIR) $(EXECUTABLE)
	-$(RMDIR) $(ALL_OBJECTS)


.PHONY: host
host: $(EXECUTABLE)
	@echo $(SRC_OBJS)
	@echo $(CXXFLAGS)
	@echo ${BINARY_CONTAINER_OBJS}


.PHONY: hostemu
hostemu: cleanhost
	@echo $(SRC_OBJS)
	@echo $(CXXFLAGS)
	@echo ${BINARY_CONTAINER_OBJS}
	kill $(shell pidof xsim) | true
	XCL_EMULATION_MODE=hw_emu ./$(EXECUTABLE) ${BUILD_DIR}/kernel.xclbin