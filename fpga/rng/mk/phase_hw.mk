############################## Setting up Kernel Variables ##############################
# Kernel compiler global settings
VPP_FLAGS += -t $(TARGET) --platform $(DEVICE) --save-temps
ifneq ($(TARGET), hw)
	VPP_FLAGS += -g
endif

VPP_FLAGS += -I ./src/

# Kernel linker flags
VPP_LDFLAGS += --kernel_frequency $(FREQ)
VPP_LDFLAGS += --vivado.impl.jobs 16 --config $(CFG_FILE)


############################## Setting Rules for Binary Containers (Building Kernels) ##############################
$(BUILD_DIR)/kernel.xclbin:  $(BINARY_CONTAINER_OBJS)
	mkdir -p $(BUILD_DIR)
	@echo $(BINARY_CONTAINER_OBJS)
ifeq ($(HOST_ARCH), x86)
	$(VPP) $(VPP_FLAGS) -l $(VPP_LDFLAGS) --temp_dir $(TEMP_DIR)  -o'$(BUILD_DIR)/kernel.link.xclbin' $(BINARY_CONTAINER_OBJS)
	$(VPP) -p $(BUILD_DIR)/kernel.link.xclbin -t $(TARGET) --platform $(DEVICE) --package.out_dir $(PACKAGE_OUT) -o $(BUILD_DIR)/kernel.xclbin
else
	$(VPP) $(VPP_FLAGS) -l $(VPP_LDFLAGS) --temp_dir $(TEMP_DIR) -o'$(BUILD_DIR)/kernel.xclbin' $(+)
endif


.PHONY: build
build: check-vitis  $(BINARY_CONTAINERS)

.PHONY: xclbin
xclbin: build
