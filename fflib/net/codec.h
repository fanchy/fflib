//! 二进制序列化 
#ifndef _CODEC_H_
#define _CODEC_H_

#include <string>
#include <stdexcept>
#include <vector>
#include <iostream>
#include <map>
using namespace std;

#include "net/message.h"
#include "base/singleton.h"
#include "base/atomic_op.h"
#include "base/lock.h"
#include "base/fftype.h"
#include "base/smart_ptr.h"

namespace ff {

class bin_encoder_t;
class bin_decoder_t;

struct codec_i
{
    virtual ~codec_i(){}
    virtual string encode()                      = 0;
    virtual void decode(const string& src_buff_) = 0;
};

struct codec_helper_i
{
    virtual ~codec_helper_i(){}
    virtual void encode(bin_encoder_t&) const = 0;
    virtual void decode(bin_decoder_t&)       = 0;
};

template<typename T>
struct option_t: public shared_ptr_t<T>
{
    option_t()
    {}
    option_t(const option_t&);
    void init()
    {
        shared_ptr_t<T>::reset();
        shared_ptr_t<T>::operator=(new T());
    }
};

class bin_decoder_t
{
public:
    bin_decoder_t(){}
    explicit bin_decoder_t(const string& src_buff_):
        m_index_ptr(src_buff_.data()),
        m_remaindered(src_buff_.size())
    {
    }
    bin_decoder_t& init(const string& str_buff_)
    {
        m_index_ptr = str_buff_.data();
        m_remaindered = str_buff_.size();
        return *this;
    }

    template<typename T>
    bin_decoder_t& operator >> (T& dest_)
    {
        return copy_value(&dest_, sizeof(T));
    }

    bin_decoder_t& operator >> (string& dest_)
    {
        return copy_value(dest_);
    }

    template<typename T>
    bin_decoder_t& operator >>(vector<T>& dest_vt_)
    {
        uint32_t vt_size = 0;
        copy_value(&vt_size, sizeof(vt_size));

        for (size_t i = 0; i < vt_size; ++i)
        {
            T tmp;
            (*this) >> tmp;
            dest_vt_.push_back(tmp);
        }
        return *this;
    }
    
    template<typename T, typename R>
    bin_decoder_t& operator >>(map<T, R>& dest_)
    {
        uint32_t size = 0;
        copy_value(&size, sizeof(size));
        
        for (size_t i = 0; i < size; ++i)
        {
            T key;
            R value;
            (*this) >> key >> value;
            dest_[key] = value;
        }
        return *this;
    }
    
    bin_decoder_t& operator >>(codec_helper_i& dest_)
    {
        dest_.decode(*this);
        return *this;
    }
    template<typename T>
    bin_decoder_t& operator >>(option_t<T>& dest_)
    {
        try{
            dest_.init();
            *this << *dest_;
        }
        catch(exception& e_)
        {
            dest_.reset();
        }
        return *this;
    }
private:
    bin_decoder_t& copy_value(void* dest_, uint32_t var_size_)
    {
        if (m_remaindered < var_size_)
        {
            throw runtime_error("bin_decoder_t:msg size not enough");
        }
        ::memcpy(dest_, m_index_ptr, var_size_);
        m_index_ptr     += var_size_;
        m_remaindered  -= var_size_;
        return *this;
    }
    
    bin_decoder_t& copy_value(string& dest_)
    {
        uint32_t str_len = 0;
        copy_value(&str_len, sizeof(str_len));

        if (m_remaindered < str_len)
        {
            throw runtime_error("bin_decoder_t:msg size not enough");
        }
        dest_.assign((const char*)m_index_ptr, str_len);
        m_index_ptr     += str_len;
        m_remaindered  -= str_len;
        return *this;
    }
    
private:
    const char*  m_index_ptr;
    size_t       m_remaindered;
};

class bin_encoder_t
{
public:
    bin_encoder_t(){}
    bin_encoder_t& init()
    {
        return *this;
    }

    const string& get_buff() const { return m_dest_buff; }

    template<typename T>
    bin_encoder_t& operator << (const T& var_)
    {
        return copy_value((const char*)(&var_), sizeof(var_));
    }
    
    bin_encoder_t& operator << (const string& str_)
    {
        return copy_value(str_);
    }
    
    template<typename T>
    bin_encoder_t& operator <<(const vector<T>& src_vt_)
    {
        uint32_t vt_size = (uint32_t)src_vt_.size();
        copy_value((const char*)(&vt_size), sizeof(vt_size));

        for (uint32_t i = 0; i < vt_size; ++i)
        {
            (*this) << src_vt_[i];
        }
        return *this;
    }
    template<typename T, typename R>
    bin_encoder_t& operator <<(const map<T, R>& src_)
    {
        uint32_t size = (uint32_t)src_.size();
        copy_value((const char*)(&size), sizeof(size));
        
        typename map<T, R>::const_iterator it = src_.begin();
        for (; it != src_.end(); ++it)
        {
            (*this) << it->first << it->second;
        }
        return *this;
    }    
    bin_encoder_t& operator <<(const codec_helper_i& dest_)
    {
        dest_.encode(*this);
        return *this;
    }
    template<typename T>
    bin_encoder_t& operator <<(option_t<T>& dest_)
    {
        if (dest_.get())
        {
            *this << *(dest_);
        }
        return *this;
    }
private:
    inline bin_encoder_t& copy_value(const string& str_)
    {
        uint32_t str_size = str_.size();
        copy_value((const char*)(&str_size), sizeof(str_size));
        copy_value(str_.data(), str_.size());
        return *this;
    }
    inline bin_encoder_t& copy_value(const void* src_, size_t size_)
    {
        m_dest_buff.append((const char*)(src_), size_);
        return *this;
    }

private:
    string         m_dest_buff;
};
    
struct rpc_msg_cmd_e
{
    enum
    {
        CREATE_SERVICE_GROUP     = 1,
        CREATE_SERVICE_GROUP_RET = 2,
        CREATE_SERVICE           = 3,
        CREATE_SERVICE_RET       = 4,
        REG_INTERFACE            = 5,
        REG_INTERFACE_RET        = 6,
        SYNC_ALL_SERVICE         = 7,
        SYNC_ALL_SERVICE_RET     = 8,
        CALL_INTERFACE           = 9,
        CALL_INTERFACE_RET       = 10,
        INTREFACE_CALLBACK       = 11,
        INTREFACE_CALLBACK_RE    = 12,
        PUSH_INIT_DATA           = 13,
        PUSH_ADD_SERVICE_GROUP   = 14,
        PUSH_ADD_SERVICE         = 15,
        PUSH_ADD_MSG             = 16,
        REG_SLAVE_BROKER         = 17
    };
};

struct msg_name_store_t
{
    struct data_t
    {
        string                m_null_str;
        map<string, uint16_t> m_name_to_id;
        map<uint16_t, string> m_id_to_name;
    };
    msg_name_store_t():
        m_data(new data_t())
    {
        m_data_history.push_back(m_data);
        this->set_msg_name2id(m_data, "create_service_group_t::in_t", rpc_msg_cmd_e::CREATE_SERVICE_GROUP);
        this->set_msg_name2id(m_data, "create_service_group_t::out_t", rpc_msg_cmd_e::CREATE_SERVICE_GROUP_RET);
        this->set_msg_name2id(m_data, "create_service_t::in_t", rpc_msg_cmd_e::CREATE_SERVICE);
        this->set_msg_name2id(m_data, "create_service_t::out_t", rpc_msg_cmd_e::CREATE_SERVICE_RET);
        this->set_msg_name2id(m_data, "reg_interface_t::in_t", rpc_msg_cmd_e::REG_INTERFACE);
        this->set_msg_name2id(m_data, "reg_interface_t::out_t", rpc_msg_cmd_e::REG_INTERFACE_RET);
        this->set_msg_name2id(m_data, "sync_all_service_t::in_t", rpc_msg_cmd_e::SYNC_ALL_SERVICE);
        this->set_msg_name2id(m_data, "sync_all_service_t::out_t", rpc_msg_cmd_e::SYNC_ALL_SERVICE_RET);
        this->set_msg_name2id(m_data, "push_init_data_t::in_t", rpc_msg_cmd_e::PUSH_INIT_DATA);
        this->set_msg_name2id(m_data, "push_add_service_group_t::in_t", rpc_msg_cmd_e::PUSH_ADD_SERVICE_GROUP);
        this->set_msg_name2id(m_data, "push_add_service_t::in_t", rpc_msg_cmd_e::PUSH_ADD_SERVICE);
        this->set_msg_name2id(m_data, "push_set_msg_name2id_t::in_t", rpc_msg_cmd_e::PUSH_ADD_MSG);
        this->set_msg_name2id(m_data, "reg_slave_broker_t::in_t", rpc_msg_cmd_e::REG_SLAVE_BROKER);
    }
    ~msg_name_store_t()
    {
        for (size_t i = 0; i < m_data_history.size(); ++i)
        {
            delete m_data_history[i];
        }
        m_data_history.clear();
    }
    template<typename MSG>
    void add_msg(uint16_t id_)
    {
        MSG tmp;
        this->add_msg(tmp.get_name(), id_);
    }
    void set_msg_name2id(data_t* data_, const string& name_, uint16_t id_)
    {
        data_->m_name_to_id[name_] = id_;
        data_->m_id_to_name[id_]   = name_;
    }
    void add_msg(const string& name_, uint16_t id_)
    {
        data_t* pdata = ATOMIC_FETCH(&m_data);
        if (pdata->m_name_to_id.find(name_) != pdata->m_name_to_id.end())
        {
            return;
        }
        data_t* new_data = new data_t();
        new_data->m_name_to_id = pdata->m_name_to_id;
        new_data->m_id_to_name = pdata->m_id_to_name;
        this->set_msg_name2id(new_data, name_, id_);
        ATOMIC_SET(&m_data, new_data);
        lock_guard_t lock(m_mutex);
        m_data_history.push_back(new_data);
    }
    uint16_t name_to_id(const string& name_)
    {
        data_t* pdata = ATOMIC_FETCH(&m_data);
        map<string, uint16_t>::iterator it = pdata->m_name_to_id.find(name_);
        if (it != pdata->m_name_to_id.end())
        {
            return it ->second;
        }
        return 0;
    }
    const string& id_to_name(uint16_t id_)
    {
        data_t* pdata = ATOMIC_FETCH(&m_data);
        map<uint16_t, string>::iterator it = pdata->m_id_to_name.find(id_);
        if (it != pdata->m_id_to_name.end())
        {
            return it ->second;
        }
        return pdata->m_null_str;
    }
    map<string, uint16_t>& all_msg()
    {
        data_t* pdata = ATOMIC_FETCH(&m_data);
        return pdata->m_name_to_id;
    }
    data_t*             m_data;
    vector<data_t*>     m_data_history;
    mutex_t             m_mutex;
};

template<typename T>
struct  msg_traits_t
{
    msg_traits_t():msg_id(0){}
    
    uint16_t get_id()
    {
        if (0 == msg_id)
        {
            T tmp;
            msg_id = singleton_t<msg_name_store_t>::instance().name_to_id(tmp.get_name());   
        }
        return msg_id;
    }
    uint16_t msg_id;
};

struct msg_i : public codec_i
{
    msg_i(const char* msg_name_):
        cmd(0),
        uuid(0),
        service_group_id(0),
        service_id(0),
        msg_id(0),
        msg_name(msg_name_),
        encode_name(false)
    {}
    
    void set(uint16_t group_id, uint16_t id_, uint32_t uuid_, uint16_t msg_id_)
    {
        service_group_id = group_id;
        service_id       = id_;
        uuid             = uuid_;
        msg_id           = msg_id_;
    }

    uint16_t cmd;
    uint16_t get_group_id()   const{ return service_group_id; }
    uint16_t get_service_id() const{ return service_id;       }
    uint32_t get_uuid()       const{ return uuid;             }
    
    uint16_t get_msg_id()     const{ return msg_id;           }
    const string& get_name()  const
    {
        if (msg_name.empty() == false)
        {
            return msg_name;
        }
        return singleton_t<msg_name_store_t>::instance().id_to_name(this->get_msg_id());
    }
    
    void     set_uuid(uint32_t id_)   { uuid = id_;  }
    void     set_msg_id(uint16_t id_) { msg_id = id_;}
    void     set_sgid(uint16_t sgid_) { service_group_id = sgid_;}
    void     set_sid(uint16_t sid_)   { service_id = sid_; }
    uint32_t uuid;
    uint16_t service_group_id;
    uint16_t service_id;
    uint16_t msg_id;
    string   msg_name;


    bin_encoder_t& init_encoder()
    {
        if (false == encode_name)
        {
            return encoder.init() << uuid << service_group_id << service_id << msg_id;
        }
        else
        {
            return encoder.init() << msg_name;
        }
    }
    bin_decoder_t& init_decoder(const string& buff_)
    {
        return decoder.init(buff_) >> uuid >> service_group_id >> service_id >> msg_id;
    }
    
    void set_gate() { encode_name = true; }
    bin_decoder_t decoder;
    bin_encoder_t encoder;
    bool          encode_name;
};

struct base_msg_t: public msg_i
{
    base_msg_t(const char* name_):
        msg_i(name_)
    {}
    virtual string encode()
    {
        return init_encoder().get_buff();
    }
    virtual void decode(const string& src_buff_)
    {
        init_decoder(src_buff_);
    }
};

struct msg_tool_t: public base_msg_t
{
    msg_tool_t():
        base_msg_t("")
    {}
};
    
struct gate_msg_tool_t: public msg_i
{
    gate_msg_tool_t():
        msg_i("")
    {}
    virtual string encode()
    {
        string dest = init_encoder().get_buff();
        return dest += packet_body.substr(4 + msg_name.size());
    }
    virtual void gate_decode(const string& src_buff_)
    {
        packet_body = src_buff_;
        decoder.init(src_buff_) >> msg_i::msg_name;
    }
    virtual void decode(const string& src_buff_)
    {
        init_decoder(src_buff_);
    }
    string packet_body;
};

struct client_msg_i : public codec_i
{
    client_msg_i(const char* msg_name_):
        msg_name(msg_name_)
    {}
    
    const string& get_name()  const {return msg_name;}

    bin_encoder_t& init_encoder()
    {
        return encoder.init() << msg_name;
    }
    bin_decoder_t& init_decoder(const string& buff_)
    {
        return decoder.init(buff_) >> msg_name;
    }

    string        msg_name;
    bin_decoder_t decoder;
    bin_encoder_t encoder;
};


struct bool_ret_msg_t: public msg_i
{
    bool_ret_msg_t(const char* name_ = "bool_ret_msg_t"):
        msg_i(name_),
        value(false)
    {}
    virtual string encode()
    {
        return (init_encoder()<< value).get_buff();
    }
    virtual void decode(const string& src_buff_)
    {
        init_decoder(src_buff_) >> value;
    }
    bool value;
};

struct create_service_group_t
{
    struct in_t: public msg_i
    {
        in_t():
            msg_i("create_service_group_t::in_t")
        {}
        virtual string encode()
        {
            return (init_encoder()<< service_name).get_buff();
        }
        virtual void decode(const string& src_buff_)
        {
            init_decoder(src_buff_) >> service_name;
        }
        string service_name;
    };
    struct out_t: public msg_i
    {
        out_t():
            msg_i("create_service_group_t::out_t")
        {}
        virtual string encode()
        {
            return (init_encoder()<< service_id).get_buff();
        }
        virtual void decode(const string& src_buff_)
        {
            init_decoder(src_buff_) >> service_id;
        }
        uint16_t service_id;
    };
};

struct create_service_t
{
    struct in_t: public msg_i
    {
        in_t():
            msg_i("create_service_t::in_t")
        {}
        virtual string encode()
        {
            return (init_encoder()<< new_service_group_id << new_service_id).get_buff();
        }
        virtual void decode(const string& src_buff_)
        {
            init_decoder(src_buff_) >> new_service_group_id >> new_service_id;
        }
        uint16_t new_service_group_id;
        uint16_t new_service_id;
    };
    struct out_t: public bool_ret_msg_t
    {
        out_t(): bool_ret_msg_t("create_service_group_t::out_t"){}
    };
};

struct reg_interface_t
{
    struct in_t: public msg_i
    {
        in_t():
            msg_i("reg_interface_t::in_t")
        {}
        virtual string encode()
        {
            return (init_encoder()<< sgid << sid << in_msg_name << out_msg_name).get_buff();
        }
        virtual void decode(const string& src_buff_)
        {
            init_decoder(src_buff_) >> sgid >> sid >> in_msg_name >> out_msg_name;
        }
        uint16_t sgid;
        uint16_t sid;
        string in_msg_name;
        string out_msg_name;
    };
    struct out_t: public msg_i
    {
        out_t():
            msg_i("reg_interface_t::out_t"),
            alloc_id(0),
            out_alloc_id(0)
        {}
        virtual string encode()
        {
            return (init_encoder()<< alloc_id << out_alloc_id).get_buff();
        }
        virtual void decode(const string& src_buff_)
        {
            init_decoder(src_buff_) >> alloc_id >> out_alloc_id;
        }
        uint16_t alloc_id;
        uint16_t out_alloc_id;
    };
};

struct sync_all_service_t
{
    struct in_t: public msg_i
    {
        in_t():
            msg_i("sync_all_service_t::in_t")
        {}
        virtual string encode()
        {
            return (init_encoder() << slave_host).get_buff() ;
        }
        virtual void decode(const string& src_buff_)
        {
            init_decoder(src_buff_) >> slave_host;
        }

        string slave_host;
    };
    struct out_t: public bool_ret_msg_t
    {
        out_t(): bool_ret_msg_t("sync_all_service_t::out_t"){}
    };
};

struct push_init_data_t
{
    struct id_info_t//: public codec_helper_i
    {
        /*virtual void encode(bin_encoder_t& be_) const
        {
            be_ << sgid << sid << node_id;
        }
        virtual void decode(bin_decoder_t& bd_)
        {
            bd_ >> sgid >> sid >> node_id;
        }*/
        uint16_t sgid;
        uint16_t sid;
        uint16_t node_id;
    };
    struct in_t: public msg_i
    {
        in_t():
            msg_i("push_init_data_t::in_t")
        {}
        virtual string encode()
        {
            return (init_encoder()<< group_name_vt << group_id_vt << id_info_vt << msg_name_vt << msg_id_vt << node_id << bind_id << broker_slave_host).get_buff() ;
        }
        virtual void decode(const string& src_buff_)
        {
            init_decoder(src_buff_) >> group_name_vt >> group_id_vt >> id_info_vt >> msg_name_vt >> msg_id_vt >> node_id >> bind_id >> broker_slave_host;
        }
        vector<string>      group_name_vt;
        vector<uint16_t>    group_id_vt;
        
        vector<id_info_t>   id_info_vt;
        vector<string>      msg_name_vt;
        vector<uint16_t>    msg_id_vt;
        
        uint32_t            node_id;
        uint32_t            bind_id;
        
        vector<string>      broker_slave_host;
    };
};
        
struct push_add_service_group_t
{
    struct in_t: public msg_i
    {
        in_t():
            msg_i("push_add_service_group_t::in_t")
        {}
        virtual string encode()
        {
            return (init_encoder() << name << sgid).get_buff() ;
        }
        virtual void decode(const string& src_buff_)
        {
            init_decoder(src_buff_) >> name >> sgid;
        }
        string      name;
        uint16_t    sgid;
    };
};

struct push_add_service_t
{
    struct in_t: public msg_i
    {
        in_t():
            msg_i("push_add_service_t::in_t")
        {}
        virtual string encode()
        {
            return (init_encoder() << sgid << sid << node_id).get_buff() ;
        }
        virtual void decode(const string& src_buff_)
        {
            init_decoder(src_buff_) >> sgid >> sid >> node_id;
        }
        uint16_t    sgid;
        uint16_t    sid;
        uint16_t    node_id;
    };
};

struct push_add_msg_t
{
    struct in_t: public msg_i
    {
        in_t():
            msg_i("push_add_msg_t::in_t")
        {}
        virtual string encode()
        {
            return (init_encoder() << name << msg_id).get_buff() ;
        }
        virtual void decode(const string& src_buff_)
        {
            init_decoder(src_buff_) >> name >> msg_id;
        }
        string      name;
        uint16_t    msg_id;
    };
};

struct reg_slave_broker_t
{
    struct in_t: public msg_i
    {
        in_t():
            msg_i("reg_slave_broker_t::in_t")
        {}
        virtual string encode()
        {
            return (init_encoder() << node_id).get_buff() ;
        }
        virtual void decode(const string& src_buff_)
        {
            init_decoder(src_buff_) >> node_id;
        }
        uint16_t    node_id;
    };
};

template<typename T>
class ffmsg_t: public msg_i
{
public:
    ffmsg_t():
        msg_i(TYPE_NAME(T).c_str())
    {}
    virtual ~ffmsg_t(){}
};

typedef bool_ret_msg_t ffmsg_bool_t;
}

#endif
