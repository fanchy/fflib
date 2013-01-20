
#include "base/log.h"
#include "base/atomic_op.h"
#include "base/strtool.h"
#include "base/arg_helper.h"

#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/time.h>

using namespace ff;

#define gettid() ::syscall(SYS_gettid)

str_format_t::str_format_t(const char* fmt_):
	m_fmt(fmt_),
	cur_format_index(0)
{
	m_fmt_len = ::strlen(fmt_);
}

str_format_t::~str_format_t()
{

}

bool str_format_t::move_to_next_wildcard()
{
	m_fmt_type.clear();
	char tmp = '\0';

	for (; cur_format_index < m_fmt_len; ++ cur_format_index)
	{
		tmp = m_fmt[cur_format_index];
		if (tmp != '%')
		{
			m_result += tmp;
			continue;
		}

		char next = m_fmt[cur_format_index + 1];
		if (next == '%')
		{
			cur_format_index += 1;
			m_result += next;
			continue;
		}

		//! 支持多种格式化 %c %s, %d, %ld, %u, %lu, %x, %X, 找到格式化的类型
		//for (++cur_format_index; cur_format_index < m_fmt_len; ++ cur_format_index)
		for (unsigned int i = 1 ; i <= 5; ++i)
		{
			char cur = m_fmt[cur_format_index + i];

			if (cur == '\0' || cur == '%')
			{
				break;
			}
			else if (cur == 'c' || cur == 'd' || cur == 'u' || cur == 'x' ||
					 cur == 'f' || cur == 's')
			{
				m_fmt_type.type    = cur;
				m_fmt_type.min_len = ::atoi(m_fmt + cur_format_index + 1);
				cur_format_index   = cur_format_index + i + 1;
				if (next == '0')
				{
					m_fmt_type.fill_char = next;
				}
				return true;
			}
			else if (cur == 'l')
			{
				char c_num = m_fmt[cur_format_index + i + 1];
				if ('d' == c_num || 'u' == c_num)
				{
					m_fmt_type.type    = c_num;
					m_fmt_type.min_len = ::atoi(m_fmt + cur_format_index + 1);
					cur_format_index   = cur_format_index + i + 2;
					if (next == '0')
					{
						m_fmt_type.fill_char = next;
					}
					return true;
				}
			}
		}
		m_result += tmp;

	}
	return false;
}

void str_format_t::append(const char* str_)
{
	if (move_to_next_wildcard())
	{
		unsigned int len = ::strlen(str_);
		int width = m_fmt_type.min_len > len? m_fmt_type.min_len -len: 0;
		for (; width > 0; -- width)
		{
			m_result += m_fmt_type.fill_char;
		}
	}

	m_result += str_;
}
void str_format_t::append(const string& str_)
{
	if (move_to_next_wildcard())
	{
		int width = m_fmt_type.min_len > str_.length()? m_fmt_type.min_len -str_.length(): 0;
		for (; width > 0; -- width)
		{
			m_result += m_fmt_type.fill_char;
		}
	}

	m_result += str_;
}

const string& str_format_t::gen_result()
{
	if (cur_format_index < m_fmt_len)
	{
		m_result.append(m_fmt + cur_format_index);
	}
	return m_result;
}


log_t::log_t(int level_, const string& all_class_, const string& path_, const string& file_,
		  	 bool print_file_, bool print_screen_):
	m_enabled_level(0),
	m_enable_class_set(new str_set_t()),
	m_file_name_index(0),
	m_line_num(0)
{
	::memset(&m_last_create_dir_tm, sizeof(m_last_create_dir_tm), 0);
	m_class_set_history.push_back(m_enable_class_set);

	for (int i = 0; i <= level_; ++i)
	{
		m_enabled_level |= (1 << i);
	}

	vector<string> vt_class;
	strtool::split(all_class_, vt_class);
	for (size_t i = 0; i < vt_class.size(); ++i)
	{
		m_enable_class_set->insert(vt_class[i]);
	}

	m_path 	      	= path_;
	m_filename      = file_;
	m_enable_file   = print_file_;
	m_enable_screen = print_screen_;
}

log_t::~log_t()
{
	for (ptr_vt_t::iterator it = m_class_set_history.begin(); it != m_class_set_history.end(); ++it)
	{
		delete (*it);
	}
	m_class_set_history.clear();
}

void log_t::mod_level(int level_, bool flag_)
{
	if (flag_)
	{
		m_enabled_level |= (1 << level_);
	}
	else
	{
		m_enabled_level &= ~(1 << level_);
	}
}

void log_t::mod_class(const string& class_, bool flag_)
{
	str_set_t* pset = new str_set_t(m_enable_class_set->begin(), m_enable_class_set->end());
	if (flag_)
	{
		pset->insert(class_);
	}
	else
	{
		pset->erase(class_);
	}

	m_class_set_history.push_back(pset);
	ATOMIC_SET(&m_enable_class_set, pset);
}

bool log_t::is_level_enabled(int level_)
{
	return m_enabled_level & (1 << level_);
}

void log_t::mod_print_file(bool flag_)
{
	m_enable_file = flag_;
}
void log_t::mod_print_screen(bool flag_)
{
	m_enable_screen = flag_;
}

const char* log_t::find_class_name(const char* class_)
{
	str_set_t* pset = ATOMIC_FETCH(&m_enable_class_set);
	str_set_t::iterator it = pset->find(class_);
	if (it != pset->end())
	{
		return (*it).c_str();
	}
	return NULL;
}

static const char* g_log_level_desp[] =
{
	"FATAL",
	"ERROR",
	"WARN ",
	"INFO ",
	"TRACE",
	"DEBUG"
};

static const char* g_log_color_head[] =
{
	"\033[0;35m",
	"\033[0;31m",
	"\033[1;34m",
	"\033[1;32m",
	"",
	"\033[1;33m"
};
static const char* g_log_color_tail[] =
{
	"\033[0m",
	"\033[0m",
	"\033[0m",
	"\033[0m",
	"",
	"\033[0m"
};
void log_t::log_content(int level_, const char* str_class_, const string& content_)
{
	struct timeval curtm;
	gettimeofday(&curtm, NULL);
	struct tm tm_val = *localtime(&(curtm.tv_sec));

	char log_buff[512];
	::snprintf(log_buff, sizeof(log_buff), "%02d:%02d:%02d.%03ld %s [%ld] [%s] ",
			tm_val.tm_hour, tm_val.tm_min, tm_val.tm_sec, curtm.tv_usec/1000,
			g_log_level_desp[level_], gettid(), str_class_);

	if (m_enable_file && check_and_create_dir(&tm_val))
	{
		m_file << log_buff << content_ << endl;
		m_file.flush();
	}

	if (m_enable_screen)
	{
		printf("%s%s%s%s\n", g_log_color_head[level_], log_buff, content_.c_str(), g_log_color_tail[level_]);
	}
}

bool log_t::check_and_create_dir(struct tm* tm_val_)
{
	bool time_equal = (m_last_create_dir_tm.tm_mday == tm_val_->tm_mday &&
					  m_last_create_dir_tm.tm_mon == tm_val_->tm_mon &&
					  m_last_create_dir_tm.tm_year == tm_val_->tm_year);

	char file[1024];

	if (false == time_equal)
	{
		int rc = ::access(m_path.c_str(), F_OK);
		if (0 != rc)
		{
			rc = ::mkdir(m_path.c_str(), 0777);
			if (rc != 0)
			{
				printf("log mkdir<%s>failed error<%s>\n", m_path.c_str(), ::strerror(errno));
				return false;
			}
		}

		::snprintf(file, sizeof(file), "%s/%d-%d-%d",
				   m_path.c_str(), tm_val_->tm_year + 1900, tm_val_->tm_mon + 1, tm_val_->tm_mday);
		rc = ::access(file, F_OK);
		if (0 != rc)
		{
			rc = ::mkdir(file, 0777);
			if (rc != 0)
			{
				printf("log mkdir file<%s>failed error<%s>\n", file, ::strerror(errno));
				return false;
			}
		}
	}

	if (false == time_equal || m_line_num >= MAX_LINE_NUM)
	{
		for (int i = 0; i < 10000; ++i)
		{
			::snprintf(file, sizeof(file), "%s/%d-%d-%d/%s.%d",
					   m_path.c_str(), tm_val_->tm_year + 1900, tm_val_->tm_mon + 1, tm_val_->tm_mday,
					   m_filename.c_str(), ++m_file_name_index);

			int rc = ::access(file, F_OK);
			if (0 == rc) {
				continue;
			}
			break;
		}

		m_file.open(file);
		m_line_num = 0;
		m_last_create_dir_tm = *tm_val_;
	}
	return true;
}

log_service_t::log_service_t():
	m_log(NULL)
{

}
log_service_t::~log_service_t()
{
	stop();
}

int log_service_t::start(const string& opt_)
{
	if (m_log) return 0;

	int level = 2;
	string path = "./log";
	string filename = "log";
	bool print_file = true;
	bool print_screen = false;

	arg_helper_t arg(opt_);
	if (!arg.get_option_value("-log_level").empty()) level = ::atoi(arg.get_option_value("-log_level").c_str());
	if (!arg.get_option_value("-log_path").empty()) path = arg.get_option_value("-log_level");
	if (!arg.get_option_value("-log_filename").empty()) path = arg.get_option_value("-log_filename");

	if (arg.get_option_value("-log_print_file") == "false" || arg.get_option_value("-log_print_file") == "0")
	{
		print_file = false;
	}
	if (arg.get_option_value("-log_print_screen") == "true" || arg.get_option_value("-log_print_screen") == "1")
	{
		print_screen = true;
	}

	m_log = new log_t(level, arg.get_option_value("-log_class"), path, filename, print_file, print_screen);
	m_thread.create_thread(task_binder_t::gen(&task_queue_t::run, &m_task_queue), 1);

	return 0;
}

int log_service_t::stop()
{
	if (NULL == m_log) return 0;

	m_task_queue.close();
	m_thread.join();
	delete m_log;
	m_log = NULL;
	return 0;
}

void log_service_t::mod_level(int level_, bool flag_)
{
	m_task_queue.produce(task_binder_t::gen(&log_t::mod_level, m_log, level_, flag_));
}

void log_service_t::mod_class(const string& class_, bool flag_)
{
	m_task_queue.produce(task_binder_t::gen(&log_t::mod_class, m_log, class_, flag_));
}
void log_service_t::mod_print_file(bool flag_)
{
	m_task_queue.produce(task_binder_t::gen(&log_t::mod_print_file, m_log, flag_));
}
void log_service_t::mod_print_screen(bool flag_)
{
	m_task_queue.produce(task_binder_t::gen(&log_t::mod_print_screen, m_log, flag_));
}
