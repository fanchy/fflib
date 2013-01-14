#ifndef _FM_LUA_JSON_H_
#define _FM_LUA_JSON_H_

#include "fm_lua.h"
#include "fm_json.h"
#include <math.h>

typedef Json::Value fm_json_value_t;
typedef double lua_number_t;
typedef Json::FastWriter json_writer_t;

namespace fm_lua
{    
    class json_iterator_t
    {
    public:
        json_iterator_t(fm_json_value_t* json_value_):
            m_json_value(json_value_),
            m_it(json_value_->begin())
        {
        }
    
        bool is_end()
        {
            return m_it == m_json_value->end();
        }
        void   next()
        {
            ++m_it;
        }
        string key()
        {
            return string(m_it.memberName());
        }
    private:
        fm_json_value_t*      m_json_value;
        Json::Value::iterator m_it;
    };


    static fm_json_value_t* json_decode(string src_)
    {
        Json::Reader reader;
        fm_json_value_t* ret = new fm_json_value_t();

        if (!reader.parse(src_, *ret, false))
        {
            //cout <<"can't parse json:<" << src_ << ">\n";
            return ret;
        }

        return ret;
    }

    static fm_json_value_t* get_by_number(fm_json_value_t* value_, lua_number_t key_)
    {
        try
        {
			long   i_num = (long)::floor(key_);
			if (key_ > i_num)
			{
				char buff[128];
				snprintf(buff, sizeof(buff), "%g", key_);
				return &(*value_)[buff];
			}
			else
			{
				return &(*value_)[i_num];
			}

		}
        catch(exception& e)
        {
            return NULL;
        }
    }
    static fm_json_value_t* get_by_string(fm_json_value_t* value_, string key_)
    {
        try
        {
            return &(*value_)[key_];
        }
        catch(exception& e)
        {
            return NULL;
        }
    }
    static void append_number(fm_json_value_t* fm_json_value_, lua_number_t value_)
    {
		long   i_num = (long)::floor(value_);
		if (value_ > i_num)
		{
			fm_json_value_->append(value_);
		}
		else
		{
			fm_json_value_->append(i_num);
		}
    }
    static void append_string(fm_json_value_t* fm_json_value_, string value_)
    {
        fm_json_value_->append(value_);
    }
    static void append_bool(fm_json_value_t* fm_json_value_, bool value_)
    {
        fm_json_value_->append(value_);
    }
/* no use
    static void append_null(fm_json_value_t* fm_json_value_)
    {
        fm_json_value_->append(fm_json_value_t());
    }
*/
    static void append_userdata(fm_json_value_t* fm_json_value_, fm_json_value_t* value_)
    {
        fm_json_value_->append(*value_);
    }
    static void init_empty_array(fm_json_value_t* fm_json_value_)
    {
        *fm_json_value_ = fm_json_value_t(Json::arrayValue);
    }
    
    static void set_number(fm_json_value_t* fm_json_value_, string key_, lua_number_t value_)
    {
		long   i_num = (long)::floor(value_);
		if (value_ > i_num)
		{
			(*fm_json_value_)[key_] = value_;
		}
		else
		{
			(*fm_json_value_)[key_] = i_num;
		}
    }
    static void set_string(fm_json_value_t* fm_json_value_, string key_, string value_)
    {
        (*fm_json_value_)[key_] = value_;
    }
    static void set_bool(fm_json_value_t* fm_json_value_, string key_, bool value_)
    {
        (*fm_json_value_)[key_] = value_;
    }
/* no use
    static void set_null(fm_json_value_t* fm_json_value_, string key_)
    {
        (*fm_json_value_)[key_] = fm_json_value_t();
    }
*/
    static void set_userdata(fm_json_value_t* fm_json_value_,  string key_, fm_json_value_t* value_)
    {
        (*fm_json_value_)[key_] = (*value_);
    }
    static string lua_to_json(fm_json_value_t* fm_json_value_)
    {
        json_writer_t writer;
        string ret = writer.write(*fm_json_value_);
        return ret;
    }

};

LUA_REGISTER_BEGIN(fm_lua_json)

REGISTER_CLASS_BASE("fm_json_value_t", fm_json_value_t, void())
REGISTER_CLASS_METHOD("fm_json_value_t", "is_array", fm_json_value_t, &fm_json_value_t::isArray)
REGISTER_CLASS_METHOD("fm_json_value_t", "is_object", fm_json_value_t, &fm_json_value_t::isObject)
REGISTER_CLASS_METHOD("fm_json_value_t", "is_number", fm_json_value_t, &fm_json_value_t::isNumeric)
REGISTER_CLASS_METHOD("fm_json_value_t", "is_string", fm_json_value_t, &fm_json_value_t::isString)
REGISTER_CLASS_METHOD("fm_json_value_t", "is_bool", fm_json_value_t, &fm_json_value_t::isBool)
REGISTER_CLASS_METHOD("fm_json_value_t", "is_null", fm_json_value_t, &fm_json_value_t::isNull)
REGISTER_CLASS_METHOD("fm_json_value_t", "size", fm_json_value_t, &fm_json_value_t::size)
REGISTER_CLASS_METHOD("fm_json_value_t", "clear", fm_json_value_t, &fm_json_value_t::clear)
REGISTER_CLASS_METHOD("fm_json_value_t", "as_number", fm_json_value_t, &fm_json_value_t::asDouble)
REGISTER_CLASS_METHOD("fm_json_value_t", "as_string", fm_json_value_t, &fm_json_value_t::asString)
REGISTER_CLASS_METHOD("fm_json_value_t", "as_bool", fm_json_value_t, &fm_json_value_t::asBool)

REGISTER_STATIC_FUNCTION("fm", "json_decode", fm_lua::json_decode)
REGISTER_STATIC_FUNCTION("fm", "get_by_number", fm_lua::get_by_number)
REGISTER_STATIC_FUNCTION("fm", "get_by_string", fm_lua::get_by_string)

REGISTER_STATIC_FUNCTION("fm", "append_number", fm_lua::append_number)
REGISTER_STATIC_FUNCTION("fm", "append_string", fm_lua::append_string)
REGISTER_STATIC_FUNCTION("fm", "append_bool", fm_lua::append_bool)
REGISTER_STATIC_FUNCTION("fm", "append_userdata", fm_lua::append_userdata)
REGISTER_STATIC_FUNCTION("fm", "init_empty_array", fm_lua::init_empty_array)

REGISTER_STATIC_FUNCTION("fm", "set_number", fm_lua::set_number)
REGISTER_STATIC_FUNCTION("fm", "set_string", fm_lua::set_string)
REGISTER_STATIC_FUNCTION("fm", "set_bool", fm_lua::set_bool)
REGISTER_STATIC_FUNCTION("fm", "set_userdata", fm_lua::set_userdata)


REGISTER_STATIC_FUNCTION("fm", "lua_to_json", fm_lua::lua_to_json)

REGISTER_CLASS_BASE("json_iterator_t", fm_lua::json_iterator_t, void(fm_json_value_t*))
REGISTER_CLASS_METHOD("json_iterator_t", "is_end", fm_lua::json_iterator_t, &fm_lua::json_iterator_t::is_end)
REGISTER_CLASS_METHOD("json_iterator_t", "next", fm_lua::json_iterator_t, &fm_lua::json_iterator_t::next)
REGISTER_CLASS_METHOD("json_iterator_t", "key", fm_lua::json_iterator_t, &fm_lua::json_iterator_t::key)

LUA_REGISTER_END
#endif
