#交叉编译器路径
CROSS=
CP=/bin/cp
RM=-/bin/rm -rf
LN=/bin/ln -s 
CFLAGS=-g  -Wall
LDFLAGS= -lpthread -llua -ldl
#链接库名
LIB_NAME=
#链接库版本
LIB_VER=1.0.0
#平台
ARCH=
# 二进制目标
BIN=app_lua

#源文件目录
SrcDir= . src  ../../../fflib/src
#头文件目录
IncDir= ./include ../../../fflib/include/ ../../../fflib/include/lua/  ../../../fflib/ext
#连接库目录
LibDir= /usr/local/lib
SRCS=$(foreach dir,$(SrcDir),$(wildcard $(dir)/*.cpp))
#INCS=$(foreach dir,$(IncDir),$(wildcard $(dir)/*.h))
INCS=$(foreach dir,$(IncDir),$(addprefix -I,$(dir)))
LINKS=$(foreach dir,$(LibDir),$(addprefix -L,$(dir)))
CFLAGS := $(CFLAGS) $(INCS)
LDFLAGS:= $(LINKS) $(LDFLAGS) 
CC=g++
ARCH=PC
OBJS = $(SRCS:%.cpp=%.o)
.PHONY:all clean

all:$(BIN)
$(BIN):$(OBJS)
	$(CC) -o $(BIN) $(OBJS) $(LDFLAGS)  
	@echo -e " OK!\tComplie $@ "
# @$(LN) $(shell pwd)/$(LIB_NAME).$(LIB_VER) /lib/$(LIB_NAME)

%.o:%.cpp
	@echo -e "[$(ARCH)] \t\tCompileing $@..."
	@$(CC) $(CFLAGS)  -c $< -o $@
.PHONY: clean
clean:
	@echo -e "[$(ARCH)] \tCleaning files..."
	@$(RM) $(OBJS) $(BIN)
