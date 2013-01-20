#ifndef _FF_OBJ_TOOL_H_
#define _FF_OBJ_TOOL_H_

#include "base/singleton.h"

#include <list>
#include <map>
#include <fstream>
using namespace std;

namespace ff
{

class obj_counter_i
{
public:
	obj_counter_i():m_ref_count(0){}
	virtual ~ obj_counter_i(){}
	void inc(int n) { (void)__sync_add_and_fetch(&m_ref_count, n); }
	void dec(int n) { __sync_sub_and_fetch(&m_ref_count, n); 	   }
	long val() const{ return m_ref_count; 						   }

	virtual string get_name() { return ""; }
protected:
	volatile long m_ref_count;
};

class obj_counter_summary_t
{
public:
	void reg(obj_counter_i* p)
	{
		m_all_counter.push_back(p);
	}

	map<string, long> get_all_obj_num()
	{
		map<string, long> ret;
		for (list<obj_counter_i*>::iterator it = m_all_counter.begin(); it != m_all_counter.end(); ++it)
		{
			ret.insert(make_pair((*it)->get_name(), (*it)->val()));
		}
		return ret;
	}

	void dump(const string& path_)
	{
		ofstream tmp_fstream;
		tmp_fstream.open(path_.c_str());
		map<string, long> ret = get_all_obj_num();
		map<string, long>::iterator it = ret.begin();

		time_t timep   = time(NULL);
		struct tm *tmp = localtime(&timep);

		char tmp_buff[256];
		sprintf(tmp_buff, "%04d%02d%02d-%02d:%02d:%02d",
				tmp->tm_year + 1900, tmp->tm_mon + 1, tmp->tm_mday,
				tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
		char buff[1024] = {0};

		snprintf(buff, sizeof(buff), "obj,num,%s\n", tmp_buff);
		tmp_fstream << buff;

		for (; it != ret.end(); ++it)
		{
			snprintf(buff, sizeof(buff), "%s,%ld\n", it->first.c_str(), it->second);
			tmp_fstream << buff;
		}

		tmp_fstream.flush();
	}
protected:
	list<obj_counter_i*>	m_all_counter;
};

template<typename T>
class obj_counter_t: public obj_counter_i
{
	obj_counter_t()
	{
		singleton_t<obj_counter_t<T> >::instance().reg(this);
	}
	virtual string get_name() { return TYPE_NAME(T); }
};

template<typename T>
T* new_obj()
{
	T* p = new T();
	singleton_t<obj_counter_t<T> >::instance().inc(1);
	return p;
}

template<typename T, typename ARG1>
T* new_obj(ARG1 arg1)
{
	T* p = new T(arg1);
	singleton_t<obj_counter_t<T> >::instance().inc(1);
	return p;
}

template<typename T, typename ARG1, typename ARG2>
T* new_obj(ARG1 arg1, ARG2 arg2)
{
	T* p = new T(arg1, arg2);
	singleton_t<obj_counter_t<T> >::instance().inc(1);
	return p;
}

template<typename T>
T* new_array(int n)
{
	T* p = new T[n];
	singleton_t<obj_counter_t<T> >::instance().inc(n);
	return p;
}

template<typename T>
void del_obj(T* p)
{
	if (p)
	{
		delete p;
		singleton_t<obj_counter_t<T> >::instance().dec(1);
	}
}

template<typename T>
void del_array(T* p, int n)
{
	if (p)
	{
		delete [] p;
		singleton_t<obj_counter_t<T> >::instance().dec(n);
	}
}

}

#endif
