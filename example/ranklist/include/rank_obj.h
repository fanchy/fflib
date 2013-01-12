#ifndef _RANK_OBJ_H_
#define _RANK_OBJ_H_

class rank_obj_t
{
public:
    rank_obj_t():m_rank(0){}
    virtual ~rank_obj_t() {}
    virtual long get_attr(int AttrId) { return -1; }

    void set_rank(int rank_) { m_rank = rank_; }
    int  get_rank() const   { return m_rank; }
private:
    int m_rank;
};

#endif
