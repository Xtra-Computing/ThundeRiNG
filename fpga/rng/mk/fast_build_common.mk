CC = gcc
CXX = g++

COMPILE_CONFIG_PATH :=../../mk


#debug
CFLAGS	:=	-g -O2 -D__FAST_BUILD_TEST__
#device
#CFLAGS	:=	-mcpu=cortex-a9 -mfpu=neon

#specific
#CFLAGS	+= 	-ftree-vectorize -mvectorize-with-neon-quad -mfloat-abi=softfp

#fast math
#CFLAGS	+= 	-ffast-math

#libs
#LIBS	:=	-Wl,-Bstatic -lm -lrt -static-libstdc++ -Wl,-Bdynamic -lpthread 
LIBS	:=	-lm -lrt -lpthread 


