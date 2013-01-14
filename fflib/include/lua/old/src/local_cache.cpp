#include "local_cache.h"

#include <iostream>
using namespace std;

local_cache_t::local_cache_t()
{
}

local_cache_t::~local_cache_t()
{
}

bool local_cache_t::set(const string& key_, const string& value_)
{
    scoped_lock_t lock(m_mutex);

    m_cache[key_] = value_;
    return true;
}

string local_cache_t::get(const string& key_)
{
    scoped_lock_t lock(m_mutex);

    cache_map_t::iterator it = m_cache.find(key_);
    if (it == m_cache.end())
    {
        return "";
    }

    return it->second;
}

bool local_cache_t::del(const string& key_)
{
    scoped_lock_t lock(m_mutex);

    m_cache.erase(key_);
    return true;
}

bool local_cache_t::add(const string& key_, const string& value_)
{
    scoped_lock_t lock(m_mutex);

    return m_cache.insert(make_pair(key_, value_)).second;
}

void local_cache_t::dump()
{
    size_t n = 1;
    cache_map_t::iterator it = m_cache.begin();
    for (; it != m_cache.end(); ++ it)
    {
        cout << n << " " << it->first <<"    " << it->second <<"\n";
        ++ n;
    }
}

size_t local_cache_t::size()
{
    return m_cache.size();
}
