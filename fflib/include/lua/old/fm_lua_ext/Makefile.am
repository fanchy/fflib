SUBDIRS= .

INCLUDES= -I../include -I../lua-5.1.4/ -I$(top_srcdir)/lib/libjsoncpp/include -I$(top_srcdir)/lib/memcache  \
		  -I/opt/local/libmemcached/include -I$(top_srcdir)/lib/dbmid_client  -I$(top_srcdir)/lib/fm_asio   \
		  -I$(top_srcdir)/lib/friend_client -I/opt/local/boost/include/boost-1_44/

AM_CPPFLAGS = -Wall -O2  -fPIC

LDFLAGS = -L/opt/local/boost/lib/boost-1_44/ -L$(top_srcdir)/lib -L/opt/local/libmemcached/lib/ \
		  -lz -lcurl -lmemcached -lboost_thread-gcc41-mt -lboost_system-gcc41-1_44

lib_LTLIBRARIES = libfm_lua_ext.la

libfm_lua_ext_la_LIBADD = \
						$(top_srcdir)/lib/fm_lua/libfm_lua.la \
						$(top_srcdir)/lib/memcache/libfm_memcache_client.la  \
						$(top_srcdir)/lib/libjsoncpp/libjsoncpp.la \
						$(top_srcdir)/lib/utility/libfm_utility.la \
						$(top_srcdir)/lib/dbmid_client/libdbmid_client.la \
						$(top_srcdir)/lib/friend_client/libfriend_client.la 

libfm_lua_ext_la_SOURCES= fm_lua_ext.cpp


