
KERNEL=$(kernel)
APP_DIR = $(subdir)
APP_SRC = $(wildcard src/$(APP_DIR)/*.cpp)
APP_SRC_RMDIR = $(patsubst src/$(APP_DIR)/%, %,$(APP_SRC))
APP_OBJS = $(patsubst %.cpp, %,$(APP_SRC_RMDIR))

NUMBERS := $(shell seq 1 ${number_of_cu})

EXTENDED_APP_OBJS := $(addprefix $(APP_OBJS)_,${NUMBERS})


TMP_APP_BINARY_CONTAINERS = $(patsubst %, $(TEMP_DIR)/$(APP_DIR)/%,$(APP_OBJS))
EXTENDED_APP_BINARY_CONTAINERS =  $(addprefix $(TMP_APP_BINARY_CONTAINERS)_,${NUMBERS})

APP_BINARY_CONTAINERS= $(patsubst %, %.xo,$(EXTENDED_APP_BINARY_CONTAINERS))

BINARY_CONTAINER_OBJS += $(APP_BINARY_CONTAINERS)
KERNEL_OBJS += $(EXTENDED_APP_OBJS)

VPP_FLAGS +=  -I src/$(APP_DIR)



SET_APP_DIR = $(eval COMPILE_APP_DIR=$(patsubst src/%/,%, $(dir $<)))
SET_KERNEL_NAME = $(eval KERNEL_NAME=$(patsubst src/$(COMPILE_APP_DIR)/%.cpp,%, $<))
SET_KERNEL_ID =  $(eval KERNEL_ID=$(patsubst $(TEMP_DIR)/$(COMPILE_APP_DIR)/$(KERNEL_NAME)_%.xo,%, ./$@))

$(TEMP_DIR)/$(APP_DIR)/$(KERNEL)%.xo: src/$(APP_DIR)/$(KERNEL).cpp
	$(SET_APP_DIR)
	$(SET_KERNEL_NAME)
	$(SET_KERNEL_ID)
	@echo  -e  ${BLUE}$(COMPILE_APP_DIR)/$(KERNEL_NAME)_$(KERNEL_ID)${NC}
	mkdir -p $(TEMP_DIR)/$(COMPILE_APP_DIR)
	$(VPP) $(VPP_FLAGS) -DKERNEL_ID=$(KERNEL_ID) -c -k $(KERNEL_NAME)_$(KERNEL_ID) --temp_dir $(TEMP_DIR)/$(COMPILE_APP_DIR)  -I'$(<D)' -o'$@' '$<'



unexport APP_SRC
unexport APP_DIR
unexport APP_SRC_RMDIR
unexport APP_BINARY_CONTAINERS

