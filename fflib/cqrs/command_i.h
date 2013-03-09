
#ifndef _FF_COMMAD_I_
#define _FF_COMMAD_I_

#include "base/fftype.h"
#include "net/codec.h"

namespace ff
{

class command_i: public type_i
{
public:
    virtual ~command_i(){}
};

template<typename T>
class command_t: public auto_type_t<command_i, T> 
{
public:
    virtual ~command_t(){}
    bin_decoder_t& decoder() { return m_decoder; }
    bin_encoder_t& encoder() { return m_encoder; }
    virtual string encode()
    {
        enc();
        return encoder().get_buff();
    }
    virtual void decode(const string& src_buff_)
    {
        decoder().init(src_buff_);
        dec();
    }
    virtual void enc() = 0;
    virtual void dec() = 0;

private:
    bin_decoder_t m_decoder;
    bin_encoder_t m_encoder;
};


template<typename T>
class user_command_t: public auto_type_t<command_i, T> 
{
public:
    user_command_t():uid(0){}
    virtual ~user_command_t(){}
    bin_decoder_t& decoder() { return m_decoder; }
    bin_encoder_t& encoder() { return m_encoder; }
    virtual string encode()
    {
        encoder() << uid;
        enc();
        return encoder().get_buff();
    }
    virtual void decode(const string& src_buff_)
    {
        decoder().init(src_buff_) >> uid;
        dec();
    }
    virtual void enc() = 0;
    virtual void dec() = 0;

    int64_t get_id() const      { return uid; }
    void    set_id(int64_t id_) { uid = id_; }
private:
    bin_decoder_t m_decoder;
    bin_encoder_t m_encoder;
public:
    int64_t       uid;
};
}
#endif
