#SUBDIRS= .
SUBDIRS= . fm_lua_ext

INCLUDES= -I./include -I./lua-5.1.4/ -I$(top_srcdir)/lib/libjsoncpp/include \
		  -I/opt/local/boost/include/boost-1_44/

#CXXFLAGS = -Wall -g -O2 -fPIC
AM_CPPFLAGS = -Wall -O2  -fPIC -pedantic -DNDEBUG 

noinst_LTLIBRARIES = libfm_lua.la
libfm_lua_la_LIBFLAGS =
libfm_lua_la_LIBADD =

libfm_lua_la_SOURCES = \
		lua-5.1.4/ldo.c \
		lua-5.1.4/lcode.c \
		lua-5.1.4/lauxlib.c \
		lua-5.1.4/lstrlib.c \
		lua-5.1.4/ldblib.c \
		lua-5.1.4/lundump.c \
		lua-5.1.4/lopcodes.c \
		lua-5.1.4/lbaselib.c \
		lua-5.1.4/lapi.c \
		lua-5.1.4/lua.c \
		lua-5.1.4/lmathlib.c \
		lua-5.1.4/lmem.c \
		lua-5.1.4/lparser.c \
		lua-5.1.4/lvm.c \
		lua-5.1.4/ltable.c \
		lua-5.1.4/lfunc.c \
		lua-5.1.4/llex.c \
		lua-5.1.4/ldump.c \
		lua-5.1.4/lobject.c \
		lua-5.1.4/loadlib.c \
		lua-5.1.4/liolib.c \
		lua-5.1.4/lgc.c \
		lua-5.1.4/lzio.c \
		lua-5.1.4/ltablib.c \
		lua-5.1.4/ltm.c \
		lua-5.1.4/lstring.c \
		lua-5.1.4/linit.c \
		lua-5.1.4/loslib.c \
		lua-5.1.4/luac.c \
		lua-5.1.4/print.c \
		lua-5.1.4/ldebug.c \
		lua-5.1.4/lstate.c \
		lua-5.1.4/dtoa.c  \
		lua-5.1.4/fpconv.c  \
		lua-5.1.4/g_fmt.c  \
		lua-5.1.4/lua_cjson.c  \
		lua-5.1.4/strbuf.c    \
		src/fm_lua.cpp \
		src/lua_table.cpp \
		src/lua_type.cpp \
		src/lua_object.cpp \
		src/local_cache.cpp \
		src/fm_lua_json_converter.cpp

