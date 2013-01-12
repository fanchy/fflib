#ifndef _SERIALIZER_I_H_
#define _SERIALIZER_I_H_

#include <string>
using namespace std;

#include "rapidjson/document.h"     // rapidjson's DOM-style API
#include "rapidjson/prettywriter.h" // for stringify JSON
#include "rapidjson/filestream.h"   // wrapper of C stream for prettywriter as output
#include "rapidjson/stringbuffer.h"
#include "generator/json_instream.h"
#include "generator/json_outstream.h"

typedef runtime_error        msg_exception_t;
typedef rapidjson::Document  json_dom_t;
typedef rapidjson::Value     json_value_t;
class serializer_i
{
public:
    virtual ~serializer_i(){}
    virtual int decode(const json_value_t& jval_) = 0;
    virtual string encode() const          = 0;
};
#endif

