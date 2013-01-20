
#ifndef _FF_USER_MGR_H_
#define _FF_USER_MGR_H_

#include "user/user.h"
#include "base/singleton.h"

#include <map>
#include <stdexcept>
using namespace std;

namespace ff
{
class user_mgr_t
{
public:
    user_t& get_user(int64_t id_)
    {
        map<int64_t, user_t*>::iterator it = m_users.find(id_);
        if (it == m_users.end())
        {
            throw runtime_error("none user");
        }
        return *(it->second);
    }

    void add_user(user_t* user_)
    {
        m_users[user_->get_id()] = user_;
    }

    void del_user(int64_t uid_)
    {
        m_users.erase(uid_);
    }
private:
    map<int64_t, user_t*>   m_users;
};

    
#define USER_MGR             singleton_t<user_mgr_t >::instance()
};

#endif



