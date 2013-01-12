#=======================================================================================
#   Filename: Makefile
#   Description: Makefile Templete
#   Usage:  make            (默认使用i386下的g++编译                   ) 
#        
#            make clean      (remove objects, executable, prerequisits  ) 
#
#   Version: 1.0
#   Created: 
#   Revision: --- 
#   Author:  zhaoxinfeng
#   Company: 
#   Email:   gogozhaoxinfeng@gmail.com
#=======================================================================================
#交叉编译器路径
CROSS=
CP=/bin/cp
RM=-/bin/rm -rf
LN=/bin/ln -s 
CFLAGS=-g  -Wall
LDFLAGS= -lpthread
#链接库名
LIB_NAME=
#链接库版本
LIB_VER=1.0.0
#平台
ARCH=
# 二进制目标
BIN=chat_server

#源文件目录
SrcDir= . ../../fflib/src src  ../../fflib/ext/generator
#头文件目录
IncDir= ./include  ../../fflib/include/ ../../fflib/ext/ ../../fflib/ext/generator
#连接库目录
LibDir= 
SRCS=$(foreach dir,$(SrcDir),$(wildcard $(dir)/*.cpp))
#INCS=$(foreach dir,$(IncDir),$(wildcard $(dir)/*.h))
INCS=$(foreach dir,$(IncDir),$(addprefix -I,$(dir)))
LINKS=$(foreach dir,$(LibDir),$(addprefix -L,$(dir)))
CFLAGS := $(CFLAGS) $(INCS)
LDFLAGS:= $(LDFLAGS) $(LINKS)
CC=g++
ARCH=PC
OBJS = $(SRCS:%.cpp=%.o)
.PHONY:all clean

all:$(BIN)
$(BIN):$(OBJS)
	$(CC) $(LDFLAGS)  -o $(BIN) $(OBJS) 
	@echo -e " OK!\tComplie $@ "
# @$(LN) $(shell pwd)/$(LIB_NAME).$(LIB_VER) /lib/$(LIB_NAME)



#gcc 则要加 -lc选项(显示的链接C库)
#Wl 把参数传给链接器
#all:$(OBJS)
# @echo -e "[$(ARCH)] OK!\tComplie $@ "
# @$(LN) $(shell pwd)/$(LIB_NAME).$(LIB_VER) /lib/$(LIB_NAME)

#-fPIC 产生于位置无关的代码关能加载到任何地址
%.o:%.cpp
	@echo -e "[$(ARCH)] \t\tCompileing $@..."
	@$(CC) $(CFLAGS)  -c $< -o $@
#%.arm:%.cpp
# @echo -e "[$(ARCH)] \t\tCompileing $@..."
# @$(CC) $(CFLAGS) -fPIC -c $< -o $@
.PHONY: clean
clean:
	@echo -e "[$(ARCH)] \tCleaning files..."
	@$(RM) $(OBJS) $(BIN)
