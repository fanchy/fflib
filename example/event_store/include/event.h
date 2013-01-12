#ifndef _EVENT_I_H_
#define _EVENT_I_H_

#include <string>
using namespace std;

#include "serializer.h"

class event_i: public serializer_i
{
public:
    virtual ~event_i(){}
};

class event_dispather_i
{
public:
    ~event_dispather_i(){}
    virtual int dispath(const string& json_) = 0;
};
#endif

