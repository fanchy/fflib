#ifndef _ARG_HELPER_H_
#define _ARG_HELPER_H_

#include <string>
#include <vector>
using namespace std;

#include "base/strtool.h"

class arg_helper_t
{
public:
    arg_helper_t(int argc, char* argv[])
    {
        for (int i = 0; i < argc; ++i)
        {
            m_args.push_back(argv[i]);
        }
    }
    arg_helper_t(string arg_str_)
    {
        vector<string> v;
        strtool::split(arg_str_, v, " ");
        m_args.insert(m_args.end(), v.begin(), v.end());
    }
    string get_option(int idx_) const
    {   
        if ((size_t)idx_ >= m_args.size())
        {   
                return ""; 
        }   
        return m_args[idx_];
    }   
    bool is_enable_option(string opt_) const
    {
        for (size_t i = 0; i < m_args.size(); ++i)
        {
            if (opt_ == m_args[i])
            {
                    return true;
            }
        }
        return false;
    }

    string get_option_value(string opt_) const
    {
        string ret;
        for (size_t i = 0; i < m_args.size(); ++i)
        {   
            if (opt_ == m_args[i])
            {   
                size_t value_idx = ++ i;
                if (value_idx >= m_args.size())
                {
                        return ret;
                }
                ret = m_args[value_idx];
                return ret;
            }   
        }	
        return ret;
}
private:
    vector<string>  m_args;
};

#endif

