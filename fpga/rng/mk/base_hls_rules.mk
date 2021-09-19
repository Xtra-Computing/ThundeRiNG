
APP_DIR = $(subdir)

APP_SRC = $(wildcard $(UPPER_DIR)/$(APP_DIR)/*.cpp)
APP_SRC_RMDIR = $(patsubst $(UPPER_DIR)/$(APP_DIR)/%, %,$(APP_SRC))
APP_OBJS = $(patsubst %.cpp, %,$(APP_SRC_RMDIR))
APP_BINARY_CONTAINERS = $(patsubst %, $(TEMP_DIR)/$(UPPER_DIR)/$(APP_DIR)/%.xo,$(APP_OBJS))

BINARY_CONTAINER_OBJS += $(APP_BINARY_CONTAINERS)
KERNEL_OBJS += $(APP_OBJS)

VPP_FLAGS +=  -I $(UPPER_DIR)/$(APP_DIR)

SET_APP_DIR = $(eval COMPILE_APP_DIR=$(dir $<))
SET_KERNEL_NAME = $(eval KERNEL_NAME=$(patsubst $(dir $<)%.cpp,%, $<))
SET_LOCAL_CONFIG_FILE = $(eval LOCAL_CONFIG_FILE=$(patsubst %.cpp,%.ini, $<))



$(TEMP_DIR)/$(UPPER_DIR)/$(APP_DIR)/%.xo: $(UPPER_DIR)/$(APP_DIR)/%.cpp
	$(SET_APP_DIR)
	$(SET_KERNEL_NAME)
	$(SET_LOCAL_CONFIG_FILE)
	$(SET_TMP_LOCAL_CONFIG_FILE)

	@[ -e ${LOCAL_CONFIG_FILE} ] && { \
		echo found local config file ; \
	} || { \
	 	echo no local config file ; \
	}

	@echo  -e  ${BLUE}$(KERNEL_NAME) in $(COMPILE_APP_DIR)${NC}
	mkdir -p $(TEMP_DIR)/$(COMPILE_APP_DIR)

	@[ -e ${LOCAL_CONFIG_FILE} ] && { \
		$(VPP) --config ${LOCAL_CONFIG_FILE} $(VPP_FLAGS) -c -k $(KERNEL_NAME) --temp_dir $(TEMP_DIR)/$(COMPILE_APP_DIR)  -I'$(<D)' -o'$@' '$<' ;\
	} || { \
		$(VPP) $(VPP_FLAGS) -c -k $(KERNEL_NAME) --temp_dir $(TEMP_DIR)/$(COMPILE_APP_DIR)  -I'$(<D)' -o'$@' '$<' ;\
	}




unexport LOCAL_CONFIG_FILE


unexport APP_SRC
unexport APP_DIR
unexport APP_SRC_RMDIR
unexport APP_BINARY_CONTAINERS

