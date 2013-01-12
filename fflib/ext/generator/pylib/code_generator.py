from pylib.inc import *


def convert_to_check_op(t):
    if t == 'int8' or t == 'int16' or t == 'int32':
        return 'IsInt'
    elif t == 'uint8' or t == 'uint16' or t == 'uint32':
        return 'IsUint'
    elif t == 'int64':
        return 'IsInt64'
    elif t == 'UInt64':
        return 'IsUint64'
    elif t == 'float':
        return 'IsDouble'
    elif t == 'string':
        return 'IsString'
    elif t == 'array':
        return 'IsArray'
    elif t == 'dictionary':
        return 'IsObject'
    else:
        return 'IsObject'

def convert_to_fetch_op(t, var_name = 'tmp_val', j_var = 'val'):
    if t == 'int8' or t == 'int16' or t == 'int32':
        return '%s = %s.GetInt()' % (var_name, j_var)
    elif t == 'uint8' or t == 'uint16' or t == 'uint32':
        return '%s = %s.GetUint()' % (var_name, j_var)
    elif t == 'int64':
        return '%s = %s.GetInt64()' % (var_name, j_var)
    elif t == 'uint64':
        return '%s = %s.GetUint64()' % (var_name, j_var)
    elif t == 'float':
        return '%s = %s.GetDouble();' % (var_name, j_var)
    elif t == 'string':
        return '%s = %s.GetString()' % (var_name, j_var)
    else:
        return '%s.parse(%s)' % (var_name, j_var)

inc_str = '''
#ifndef _IDL_DEF_I_
#define _IDL_DEF_I_

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
#include "json_instream.h"
#include "json_outstream.h"
#include "rapidjson/stringbuffer.h"
#include "smart_ptr/shared_ptr.h"
//! using namespace rapidjson;
using namespace ff;

typedef runtime_error        msg_exception_t;
typedef rapidjson::Document  json_dom_t;
typedef rapidjson::Value     json_value_t;

typedef int8_t int8;
typedef uint8_t uint8;
typedef int16_t int16;
typedef uint16_t uint16;
typedef int32_t int32;
typedef uint32_t uint32;
typedef int64_t int64;
typedef uint64_t uint64;

struct msg_t
{
    virtual ~msg_t(){}
    virtual string encode_json() const = 0;
};
typedef shared_ptr_t<msg_t> msg_ptr_t;

'''

class code_generator_t:
    def __init__(self, struct_def_mgr, dest_filename):
        self.struct_def_mgr = struct_def_mgr
        self.dest_filename = dest_filename

    def gen_class_def_code(self, f):
        tmp_s = '''

template<typename T, typename R>
class msg_dispather_t
{
    typedef R                    socket_ptr_t;
    typedef int (msg_dispather_t<T, R>::*reg_func_t)(const json_value_t&, socket_ptr_t);
public:
    msg_dispather_t(T& msg_handler_):
        m_msg_handler(msg_handler_)
    {
        '''
        for struct in self.struct_def_mgr.get_all_struct():
            if -1 != struct.find('ret_t'):
                continue
            tmp_s += '''
        m_reg_func["%s"] = &msg_dispather_t<T, R>::%s_dispacher;
            ''' % (struct, struct)

        tmp_s += '\n    }\n'

        tmp_s += '''
    int dispath(const string& json_, socket_ptr_t sock_)
    {
        json_dom_t document;  // Default template parameter uses UTF8 and MemoryPoolAllocator.
        if (document.Parse<0>(json_.c_str()).HasParseError())
        {
            throw msg_exception_t("json format not right");
        }
        if (false == document.IsObject() && false == document.Empty())
        {
            throw msg_exception_t("json must has one field");
        }
    
        const json_value_t& val = document.MemberBegin()->name;
        const char* func_name   = val.GetString();
        typename map<string, reg_func_t>::const_iterator it = m_reg_func.find(func_name);
    
        if (it == m_reg_func.end())
        {
            char buff[512];
            snprintf(buff, sizeof(buff), "msg not supported<%s>", func_name);
            throw msg_exception_t(buff);
            return -1;
        }
        reg_func_t func = it->second;
    
        (this->*func)(document.MemberBegin()->value, sock_);
        return 0;
    }
        '''

        for struct in self.struct_def_mgr.get_all_struct():
            if -1 != struct.find('ret_t'):
                continue
            tmp_s += '''
    int %s_dispacher(const json_value_t& jval_, socket_ptr_t sock_)
    {
        shared_ptr_t<%s> ret_val(new %s());
        ret_val->parse(jval_);

        m_msg_handler.handle(ret_val, sock_);
        return 0;
    }
            ''' % (struct, struct, struct)

        tmp_s += '''
private:
    T&                      m_msg_handler;
    map<string, reg_func_t> m_reg_func;
};
        '''
        f.write(tmp_s)


    def format_field_declare_code(self, field_def, prefix = ''):
        type_str   = field_def.get_type()
        field_name = field_def.get_name()
        key_type = field_def.get_key_type()
        val_type = field_def.get_val_type()
        ret = prefix
        if val_type != '':
            ret = ret + 'map<%s, %s> %s;' %(key_type, val_type, field_name)
        elif key_type != '':
            ret = ret + 'vector<%s> %s;' %(key_type, field_name)
        else:
            ret = ret + '%s %s;' %(type_str, field_name)
        return ret

    def format_struct_declare_code(self, struct_def, prefix = ''):
        struct_name = struct_def.get_name()
        all_sub_struct = struct_def.get_all_struct()
        all_fields = struct_def.get_all_field()
        ret = '' + prefix
        ret += 'struct %s : public msg_t {\n' %(struct_name)
        for sub_struct in all_sub_struct:
            ret = ret + self.format_struct_declare_code(sub_struct, prefix + '    ') + '\n'

        for field_name in all_fields:
            ret = ret +  self.format_field_declare_code(all_fields[field_name], prefix + '    ') + '\n'
            

        ret = ret + prefix + '    int parse(const json_value_t& jval_) {\n'
        ret += '''
            json_instream_t in("%s");
            in''' % (struct_name)

        for field_name in all_fields:
            ret += '.decode("%s", jval_["%s"], %s)' % (field_name, field_name, field_name)

        ret += ';\n'
        ret += prefix + '        return 0;\n' + prefix + '    }\n'

        ret += '''
    string encode_json() const
    {
        rapidjson::Document::AllocatorType allocator;
        rapidjson::StringBuffer            str_buff;
        json_value_t                       ibj_json(rapidjson::kObjectType);
        json_value_t                       ret_json(rapidjson::kObjectType);

        this->encode_json_val(ibj_json, allocator);
        ret_json.AddMember("%s", ibj_json, allocator);

        rapidjson::Writer<rapidjson::StringBuffer> writer(str_buff, &allocator);
        ret_json.Accept(writer);
        string output(str_buff.GetString(), str_buff.GetSize());
        return output;
    }
        \n''' % (struct_name)

        ret += prefix + '    int encode_json_val(json_value_t& dest, rapidjson::Document::AllocatorType& allocator) const{\n'
        ret += '''
        json_outstream_t out(allocator);
		out'''

        for field_name in all_fields:    
            ret += '.encode("%s", dest, %s)' % (field_name, field_name)
        ret += ';\n'
        ret += prefix + '        return 0;\n' + prefix + '    }\n'
        

        ret = ret + '\n' + prefix + '};\n'
        return ret

    def gen_declare_code(self, f):
        f.write(inc_str)
        all_struct = self.struct_def_mgr.get_all_struct()
        for struct_name in all_struct:
            cur_struct = all_struct[struct_name]
            all_sub_struct = cur_struct.get_all_struct()
            ret_str = self.format_struct_declare_code(cur_struct)
            #print(ret_str)
            f.write(ret_str)
    def exe(self):
        f = open(self.dest_filename, "w")
        self.gen_declare_code(f)
        self.gen_class_def_code(f)
        f.write('\n#endif\n')

    
