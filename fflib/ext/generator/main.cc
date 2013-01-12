#include <stdint.h>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <iostream>
using namespace std;

#include "rapidjson/document.h"     // rapidjson's DOM-style API
#include "rapidjson/prettywriter.h" // for stringify JSON
#include "rapidjson/filestream.h"   // wrapper of C stream for prettywriter as output
//! using namespace rapidjson;

#include "msg_def.h"

class socket_t
{
public:
    void async_write(msg_ptr_t msg_)
    {
        //! TODO do io write
        cout <<"wile send:" << msg_->encode_json() <<"\n";
    }
};

typedef socket_t* socket_ptr_t;

class logic_service_t
{
public:
    void handle(shared_ptr_t<get_friends_req_t> req_,  socket_ptr_t sock_)
    {
        cout << "req uid:" << req_->uid <<"\n";
        //! DO some logic code
        shared_ptr_t<all_friends_ret_t> msg(new all_friends_ret_t());

        for (int i = 0; i < 10; ++i)
            msg->friends.push_back(i);

        sock_->async_write(msg);
    }
};


int main(int argc, char* argv[])
{
    try
    {
        string tmp = "{\"get_friends_req_t\":{\"uid\":12345}}";
        logic_service_t logic_service;
        msg_dispather_t<logic_service_t, socket_ptr_t> msg_dispather(logic_service);
        //! ÕâÀïÊµ¼ÊÉÏÓ¦¸Ã±»ÍøÂç²ãµ÷ÓÃ
        socket_ptr_t sock = new socket_t();
        msg_dispather.dispath(tmp, sock);
    }
    catch(exception& e)
    {
        cout <<"e:"<< e.what() <<"\n";
    }
    cout <<"main end ok\n";
}
