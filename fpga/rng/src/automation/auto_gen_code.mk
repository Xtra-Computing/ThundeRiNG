
CODE_GEN_PATH   =./src/automation
CODE_GEN_PARSER_PATH  = $(CODE_GEN_PATH)/parser
CODE_GEN_FILE =  $(CODE_GEN_PATH)/parser.cpp
CODE_GEN_FILE += $(CODE_GEN_PATH)/parser_debug.cpp
CODE_GEN_FILE += $(CODE_GEN_PARSER_PATH)/mem_interface.cpp
CODE_GEN_FILE += $(CODE_GEN_PARSER_PATH)/kernel_interface.cpp
CODE_GEN_FILE += $(CODE_GEN_PARSER_PATH)/customize.cpp
CODE_GEN_FILE += $(CODE_GEN_PARSER_PATH)/makefile.cpp


INCLUDE_FLAG += -I $(CODE_GEN_PATH)
INCLUDE_FLAG += -I $(CODE_GEN_PARSER_PATH)

AUTOGEN_CFLAG += -DSLR_NUM=$(CUs)

VAR_TRUE=true
APP = $(app)


.PHONY: code_gen.app
code_gen.app:
	g++  -static-libstdc++ $(INCLUDE_FLAG) $(AUTOGEN_CFLAG) $(CODE_GEN_FILE) -o code_gen.app

