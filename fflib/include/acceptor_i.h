#ifndef _ACCEPTOR_I_
#define _ACCEPTOR_I_

#include <string>
using namespace std;

#include "netbase.h"

namespace ff {

class acceptor_i: public fd_i
{
public:
    virtual ~acceptor_i(){}
    virtual int   open(const string& address_) = 0;

    int handle_epoll_write(){ return -1; }
};

}
#endif
