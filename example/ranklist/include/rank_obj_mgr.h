#ifndef _RANK_OBJ_MGR_H_
#define _RANK_OBJ_MGR_H_

#include <map>
#include <algorithm>
using namespace std;

class rank_obj_t;

class rank_obj_mgr_t
{
public:
    virtual int add(long id, rank_obj_t* obj_)
    {
        return m_objs.insert(make_pair(id, obj_)).second == true? 0: -1;
    }
    virtual void del(long id_)
    {
        m_objs.erase(id_);
    }
    virtual rank_obj_t* find(long id_)
    {
        map<long, rank_obj_t*>::iterator it = m_objs.find(id_);
        return it != m_objs.end()? it->second: NULL;
    }

    template<typename T>
    void foreach(T func_)
    {
        for (map<long, rank_obj_t*>::iterator it = m_objs.begin(); it != m_objs.end(); ++it)
        {
            func_(it->second);
        }
    }

private:
    map<long, rank_obj_t*>    m_objs;
};
#endif
