
APP_DIR = $(subdir)

CPP_SRCS = $(wildcard $(UPPER_DIR)/$(APP_DIR)/*.cpp)
CPP_APP_SRC_RMDIR = $(patsubst $(UPPER_DIR)/$(APP_DIR)/%, %,$(CPP_SRCS))

SRC_OBJS = $(patsubst %.cpp, %,$(CPP_APP_SRC_RMDIR))


C_SRCS = $(wildcard $(UPPER_DIR)/$(APP_DIR)/*.c)
C_APP_SRC_RMDIR = $(patsubst $(UPPER_DIR)/$(APP_DIR)/%, %,$(C_SRCS))

SRC_OBJS += $(patsubst %.c, %,$(C_APP_SRC_RMDIR))



APP_BINARY_CONTAINERS = $(patsubst %, $(TEMP_DIR)/$(UPPER_DIR)/$(APP_DIR)/%.o,$(SRC_OBJS))

ALL_OBJECTS += $(APP_BINARY_CONTAINERS)

CXXFLAGS +=  -I $(UPPER_DIR)/$(APP_DIR)


SET_APP_DIR = $(eval COMPILE_APP_DIR=$(dir $<))
#SET_SRC = $(eval KERNEL_NAME=$(patsubst src/$(COMPILE_APP_DIR)/%.cpp,%, $<))

$(TEMP_DIR)/$(UPPER_DIR)/$(APP_DIR)/%.o: $(UPPER_DIR)/$(APP_DIR)/%.cpp
	$(SET_APP_DIR)
	@echo  -e  ${PURPLE}$<${NC}
	@echo "build for cpp "$<
	mkdir -p $(TEMP_DIR)/$(COMPILE_APP_DIR)
	@$(CXX) $(CXXFLAGS)  -o $@  -c $<
	@$(CXX) $(CXXFLAGS)  -MM -MF  $(patsubst %.o,%.d,$@) $<


$(TEMP_DIR)/$(UPPER_DIR)/$(APP_DIR)/%.o: $(UPPER_DIR)/$(APP_DIR)/%.c
	$(SET_APP_DIR)
	@echo  -e  ${PURPLE}$<${NC}
	@echo "build for c "$<
	mkdir -p $(TEMP_DIR)/$(COMPILE_APP_DIR)
	@$(CXX) $(CXXFLAGS)  -o $@  -c $<
	@$(CXX) $(CXXFLAGS)  -MM -MF  $(patsubst %.o,%.d,$@) $<


unexport SRC_OBJS
unexport APP_SRC
unexport APP_DIR
unexport APP_SRC_RMDIR
unexport APP_BINARY_CONTAINERS

