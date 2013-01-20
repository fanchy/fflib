
#ifndef _FF_LOG_H_
#define _FF_LOG_H_

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <set>
#include <vector>
#include <fstream>
using namespace std;

#include "base/task_queue_impl.h"
#include "base/thread.h"
#include "base/singleton.h"

namespace ff
{

class str_format_t
{
	struct fmt_type_t
	{
		fmt_type_t():
			type('\0'),
			min_len(0),
			fill_char(' ')
		{}
		void clear()
		{
			type = '\0';
			min_len = 0;
			fill_char = ' ';
		}
		char 			type;//! % d,x,f,s,
		unsigned int 	min_len;
		char            fill_char;
	};
public:
	//! fmt_ like "xxx%d,xx%s"
	str_format_t(const char* fmt_ = "");
	virtual ~str_format_t();

	template<typename T>
	void append(T content_)
	{
		if (move_to_next_wildcard())
		{
			if (m_fmt_type.type == 'x')
			{
				char buff[64];
				snprintf(buff, sizeof(buff), "0x%x", (unsigned int)content_);
				m_num_buff = buff;
			}
			else
			{
				m_strstream << content_;
				m_strstream >> m_num_buff;
			}
			int width = m_fmt_type.min_len > m_num_buff.length()? m_fmt_type.min_len - m_num_buff.length(): 0;
			for (; width > 0; -- width)
			{
				m_result += m_fmt_type.fill_char;
			}
		}
		else
		{
			m_strstream << content_;
			m_strstream >> m_num_buff;
		}

		m_result += m_num_buff;
		m_strstream.clear();//! clear error bit,not content
		m_num_buff.clear();
	}
	void append(const char* str_);
	void append(const string& str_);
	const string& gen_result();
private:
	bool move_to_next_wildcard();

protected:
	const char* 	m_fmt;
	unsigned int    cur_format_index;
	unsigned int 	m_fmt_len;
	fmt_type_t      m_fmt_type;
	string      	m_result;
	stringstream    m_strstream;
	string          m_num_buff;
};

enum log_level_e
{
	LOG_FATAL = 0,
	LOG_ERROR,
	LOG_WARN,
	LOG_INFO,
	LOG_TRACE,
	LOG_DEBUG,
	LOG_LEVEL_NUM
};

class log_t
{
	enum log_e
	{
		MAX_LINE_NUM = 5000
	};

public:
	log_t(int level_, const string& all_class_, const string& path_, const string& file_,
		  bool print_file_, bool print_screen_);
	virtual ~log_t();

	void mod_level(int level_, bool flag_);
	void mod_class(const string& class_, bool flag_);
	void mod_print_file(bool flag_);
	void mod_print_screen(bool flag_);
	bool is_level_enabled(int level_);
	const char* find_class_name(const char* class_);

	void log_content(int level_, const char* str_class_, const string& content_);

protected:
	bool check_and_create_dir(struct tm* tm_val_);

protected:
	int 						m_enabled_level;
	typedef set<string>			str_set_t;
	typedef vector<str_set_t*>	ptr_vt_t;
	str_set_t*					m_enable_class_set;
	ptr_vt_t					m_class_set_history;

	struct tm					m_last_create_dir_tm;
	bool						m_enable_file;
	bool             			m_enable_screen;

	ofstream 					m_file;
	string                      m_path;
	string                      m_filename;
	unsigned int                m_file_name_index;
	unsigned int                m_line_num;
};

#define LOG_IMPL_NONE_ARG(func, LOG_LEVEL) 															\
	inline void func(const char* class_, const char* fmt_)													\
	{																								\
		if (m_log->is_level_enabled(LOG_LEVEL))														\
		{																							\
			const char* class_name_str = m_log->find_class_name(class_);							\
			if (class_name_str)																		\
			{																						\
				m_task_queue.produce(task_binder_t::gen(&log_t::log_content, m_log, LOG_LEVEL,		\
									 class_name_str, string(fmt_)));								\
			}																						\
		}																							\
	}

#define LOG_IMPL_ARG1(func, LOG_LEVEL) 																\
	template <typename ARG1>																		\
	inline void func(const char* class_, const char* fmt_, const ARG1& arg1_)								\
	{																								\
		if (m_log->is_level_enabled(LOG_LEVEL))														\
		{																							\
			const char* class_name_str = m_log->find_class_name(class_);							\
			if (class_name_str)																		\
			{																						\
				str_format_t dest(fmt_);															\
				dest.append(arg1_);																	\
				m_task_queue.produce(task_binder_t::gen(&log_t::log_content, m_log, LOG_LEVEL,		\
									 class_name_str, dest.gen_result()));						 	\
			}																						\
		}																							\
	}


#define LOG_IMPL_ARG2(func, LOG_LEVEL) 																\
	template <typename ARG1, typename ARG2>															\
	inline void func(const char* class_, const char* fmt_, const ARG1& arg1_, const ARG2& arg2_)			\
	{																								\
		if (m_log->is_level_enabled(LOG_LEVEL))														\
		{																							\
			const char* class_name_str = m_log->find_class_name(class_);							\
			if (class_name_str)																		\
			{																						\
				str_format_t dest(fmt_);															\
				dest.append(arg1_);																	\
				dest.append(arg2_);																	\
				m_task_queue.produce(task_binder_t::gen(&log_t::log_content, m_log, LOG_LEVEL,		\
									 class_name_str, dest.gen_result()));						 	\
			}																						\
		}																							\
	}

#define LOG_IMPL_ARG3(func, LOG_LEVEL) 																\
	template <typename ARG1, typename ARG2, typename ARG3>											\
	inline void func(const char* class_, const char* fmt_, const ARG1& arg1_, const ARG2& arg2_,			\
			  const ARG3& arg3_)																	\
	{																								\
		if (m_log->is_level_enabled(LOG_LEVEL))														\
		{																							\
			const char* class_name_str = m_log->find_class_name(class_);							\
			if (class_name_str)																		\
			{																						\
				str_format_t dest(fmt_);															\
				dest.append(arg1_);																	\
				dest.append(arg2_);																	\
				dest.append(arg3_);																	\
				m_task_queue.produce(task_binder_t::gen(&log_t::log_content, m_log, LOG_LEVEL,		\
									 class_name_str, dest.gen_result()));						 	\
			}																						\
		}																							\
	}

#define LOG_IMPL_ARG4(func, LOG_LEVEL) 																\
	template <typename ARG1, typename ARG2, typename ARG3, typename ARG4>							\
	inline void func(const char* class_, const char* fmt_, const ARG1& arg1_, const ARG2& arg2_,			\
			  const ARG3& arg3_, const ARG4& arg4_)													\
	{																								\
		if (m_log->is_level_enabled(LOG_LEVEL))														\
		{																							\
			const char* class_name_str = m_log->find_class_name(class_);							\
			if (class_name_str)																		\
			{																						\
				str_format_t dest(fmt_);															\
				dest.append(arg1_);																	\
				dest.append(arg2_);																	\
				dest.append(arg3_);																	\
				dest.append(arg4_);																	\
				m_task_queue.produce(task_binder_t::gen(&log_t::log_content, m_log, LOG_LEVEL,		\
									 class_name_str, dest.gen_result()));						 	\
			}																						\
		}																							\
	}

#define LOG_IMPL_ARG5(func, LOG_LEVEL) 																\
	template <typename ARG1, typename ARG2, typename ARG3, typename ARG4, typename ARG5>			\
	inline void func(const char* class_, const char* fmt_, const ARG1& arg1_, const ARG2& arg2_,			\
			  const ARG3& arg3_, const ARG4& arg4_, const ARG5& arg5_)								\
	{																								\
		if (m_log->is_level_enabled(LOG_LEVEL))														\
		{																							\
			const char* class_name_str = m_log->find_class_name(class_);							\
			if (class_name_str)																		\
			{																						\
				str_format_t dest(fmt_);															\
				dest.append(arg1_);																	\
				dest.append(arg2_);																	\
				dest.append(arg3_);																	\
				dest.append(arg4_);																	\
				dest.append(arg5_);																	\
				m_task_queue.produce(task_binder_t::gen(&log_t::log_content, m_log, LOG_LEVEL,		\
									 class_name_str, dest.gen_result()));						 	\
			}																						\
		}																							\
	}

#define LOG_IMPL_ARG6(func, LOG_LEVEL) 																\
	template <typename ARG1, typename ARG2, typename ARG3, typename ARG4, typename ARG5,			\
			  typename ARG6>																		\
	inline void func(const char* class_, const char* fmt_, const ARG1& arg1_, const ARG2& arg2_,			\
			  const ARG3& arg3_, const ARG4& arg4_, const ARG5& arg5_, const ARG6& arg6_)			\
	{																								\
		if (m_log->is_level_enabled(LOG_LEVEL))														\
		{																							\
			const char* class_name_str = m_log->find_class_name(class_);							\
			if (class_name_str)																		\
			{																						\
				str_format_t dest(fmt_);															\
				dest.append(arg1_);																	\
				dest.append(arg2_);																	\
				dest.append(arg3_);																	\
				dest.append(arg4_);																	\
				dest.append(arg5_);																	\
				dest.append(arg6_);																	\
				m_task_queue.produce(task_binder_t::gen(&log_t::log_content, m_log, LOG_LEVEL,		\
									 class_name_str, dest.gen_result()));						 	\
			}																						\
		}																							\
	}

#define LOG_IMPL_MACRO(async_logdebug, LOG_DEBUG) 	\
	LOG_IMPL_NONE_ARG(async_logdebug, LOG_DEBUG)  	\
	LOG_IMPL_ARG1(async_logdebug, LOG_DEBUG)	  	\
	LOG_IMPL_ARG2(async_logdebug, LOG_DEBUG)	  	\
	LOG_IMPL_ARG3(async_logdebug, LOG_DEBUG)		\
	LOG_IMPL_ARG4(async_logdebug, LOG_DEBUG)		\
	LOG_IMPL_ARG5(async_logdebug, LOG_DEBUG)		\
	LOG_IMPL_ARG6(async_logdebug, LOG_DEBUG)

class log_service_t
{
public:
	log_service_t();
	~log_service_t();
	int start(const string& opt_);
	int stop();

	LOG_IMPL_MACRO(async_logdebug, LOG_DEBUG);
	LOG_IMPL_MACRO(async_logtrace, LOG_TRACE);
	LOG_IMPL_MACRO(async_loginfo, LOG_INFO);
	LOG_IMPL_MACRO(async_logwarn, LOG_WARN);
	LOG_IMPL_MACRO(async_logerror, LOG_ERROR);
	LOG_IMPL_MACRO(async_logfatal, LOG_FATAL);

	void mod_level(int level_, bool flag_);
	void mod_class(const string& class_, bool flag_);
	void mod_print_file(bool flag_);
	void mod_print_screen(bool flag_);
private:
	log_t*			m_log;
	thread_t        m_thread;
	task_queue_t    m_task_queue;
};

#define BROKER  "BROKER"
#define RPC     "RPC"
#define FF      "FF"
#define MSG_BUS "MSG_BUS"

#define LOG singleton_t<log_service_t>::instance()
#define LOGDEBUG(content)  singleton_t<log_service_t>::instance().async_logdebug content
#define LOGTRACE(content)  singleton_t<log_service_t>::instance().async_logtrace content
#define LOGINFO(content)   singleton_t<log_service_t>::instance().async_loginfo  content
#define LOGWARN(content)   singleton_t<log_service_t>::instance().async_logwarn  content
#define LOGERROR(content)  singleton_t<log_service_t>::instance().async_logerror content
#define LOGFATAL(content)  singleton_t<log_service_t>::instance().async_logfatal content
}

#endif


