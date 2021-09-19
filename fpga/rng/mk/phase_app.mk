FREQ := 300

EXECUTABLE := $(APP).app
BUILD_DIR := ./build_dir_$(APP)
TEMP_DIR := ./_x_$(APP)



include app/$(APP)/kernel.mk


CFG_FILE ?= app/$(APP)/kernel.cfg