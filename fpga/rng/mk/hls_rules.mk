
APP_DIR = $(subdir)
APP_SRC = $(wildcard src/$(APP_DIR)/*.cpp)
APP_SRC_RMDIR = $(patsubst src/$(APP_DIR)/%, %,$(APP_SRC))
APP_OBJS = $(patsubst %.cpp, %,$(APP_SRC_RMDIR))
APP_BINARY_CONTAINERS = $(patsubst %, $(TEMP_DIR)/$(APP_DIR)/%.xo,$(APP_OBJS))
BINARY_CONTAINER_OBJS += $(APP_BINARY_CONTAINERS)
KERNEL_OBJS += $(APP_OBJS)

VPP_FLAGS +=  -I src/$(APP_DIR)



SET_APP_DIR = $(eval COMPILE_APP_DIR=$(patsubst src/%/,%, $(dir $<)))
SET_KERNEL_NAME = $(eval KERNEL_NAME=$(patsubst src/$(COMPILE_APP_DIR)/%.cpp,%, $<))

$(TEMP_DIR)/$(APP_DIR)/%.xo: src/$(APP_DIR)/%.cpp
	$(SET_APP_DIR)
	$(SET_KERNEL_NAME)
	@echo  -e  ${BLUE}$(COMPILE_APP_DIR)/$(KERNEL_NAME)${NC}
	mkdir -p $(TEMP_DIR)/$(COMPILE_APP_DIR)
	$(VPP) $(VPP_FLAGS) -c -k $(KERNEL_NAME) --temp_dir $(TEMP_DIR)/$(COMPILE_APP_DIR)  -I'$(<D)' -o'$@' '$<'



unexport APP_SRC
unexport APP_DIR
unexport APP_SRC_RMDIR
unexport APP_BINARY_CONTAINERS

