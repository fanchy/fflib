#交叉编译器路径
CROSS=
CP=/bin/cp
RM=-/bin/rm -rf
LN=/bin/ln -s 
CFLAGS=-g  -Wall -DLUA_USE_READLINE
LDFLAGS= -export-dynamic -lpthread -ldl  -lpython2.6 -llua
#链接库名
LIB_NAME=
#链接库版本
LIB_VER=1.0.0
#平台
ARCH=
# 二进制目标
BIN=msg_broker_server

#源文件目录
SrcDir= . ../../fflib/base ../../fflib/net ../../fflib/rpc ../../fflib/cqrs ../../fflib/gtest/src ./src src/plugin_impl  ../../fflib/ext/generator
#头文件目录
IncDir= ./include  ../../fflib/ ../../fflib/ext/ ../../fflib/ext/generator /usr/include/python2.6/ /usr/local/include/  ../../fflib/gtest/include ../../fflib/gtest/
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
