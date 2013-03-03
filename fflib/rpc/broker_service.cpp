#include "rpc/broker_service.h"
#include "log/log.h"
#include "base/timer_service.h"
#include "base/performance_daemon.h"

using namespace ff;

broker_service_t::broker_service_t():
    m_uuid(0),
    m_msg_uuid(100),
    m_master_broker(NULL),
    m_master_socket(NULL)
{
}

socket_ptr_t broker_service_t::get_socket(const rpc_service_t* rs_)
{
    return m_master_socket;
}

broker_service_t::~broker_service_t()
{
    
}
int broker_service_t::process_sync_data(push_init_data_t::in_t& out)
{
    for (size_t i = 0; i < out.group_name_vt.size(); ++i)
    {
        service_obj_mgr_t obj_mgr;
        obj_mgr.id     = out.group_id_vt[i];
        obj_mgr.name   = out.group_name_vt[i];

        m_service_obj_mgr.insert(make_pair(obj_mgr.id, obj_mgr));
    }
    
    for (size_t i = 0; i < out.id_info_vt.size(); ++i)
    {
        map<uint16_t, service_obj_mgr_t>::iterator it = m_service_obj_mgr.find(out.id_info_vt[i].sgid);
        
        if (it == m_service_obj_mgr.end())
        {
            logerror((BROKER, "broker_service_t::init_slave failed... sgid<%u> not exist", out.id_info_vt[i].sgid));
        }
        else
        {
            service_obj_t obj;
            obj.name       = it->second.name;
            obj.group_id   = out.id_info_vt[i].sgid;
            obj.id         = out.id_info_vt[i].sid;
            obj.socket_ptr = NULL;
            obj.node_id    = out.id_info_vt[i].node_id;

            it->second.service_objs[out.id_info_vt[i].sid] = obj;
        }
    }
    
    for (size_t i = 0; i < out.msg_name_vt.size(); ++i)
    {
        singleton_t<msg_name_store_t>::instance().add_msg(out.msg_name_vt[i], out.msg_id_vt[i]);
    }
    return 0;
}
int broker_service_t::init_slave(const string& host_, const string& listen_host_)
{
    m_master_socket = net_factory_t::connect(host_, this);

    if (NULL == m_master_socket)
    {
        logerror((BROKER, "broker_service_t::init_slave failed, can't connect to master broker<%s>", host_.c_str()));
        return -1;
    }

    m_master_broker = new rpc_service_t(this, 0, 0);
    
    rpc_future_t<sync_all_service_t::out_t> rpc_future;
    sync_all_service_t::in_t in;
    in.slave_host = listen_host_;
    
    const sync_all_service_t::out_t& out = rpc_future.call(m_master_broker, in);
    
    return out.value == true? 0: -1;
}

int broker_service_t::handle_broken(socket_ptr_t sock_)
{
    logwarn((BROKER, "broker_service_t::handle_broken bagin soket_ptr<%p>", sock_));
    lock_guard_t lock(m_mutex);

    vector<uint32_t> del_sgid;
    vector<uint32_t> del_sid;
    vector<uint32_t> del_callback_uuid;

    service_obj_map_t::iterator it = m_service_obj_mgr.begin();
    for (; it != m_service_obj_mgr.end(); ++it)
    {
        map<uint16_t, service_obj_t>::iterator it2 = it->second.service_objs.begin();
        for (; it2 != it->second.service_objs.end(); ++it2)
        {
            callback_map_t::iterator uuid_it = it2->second.m_callback_map.begin();
            for (; uuid_it != it2->second.m_callback_map.end(); ++uuid_it)
            {
                if (uuid_it->second.socket_ptr == sock_)
                {
                    del_callback_uuid.push_back(uuid_it->first);
                }
            }
            
            if (it2->second.socket_ptr == sock_)
            {
                del_sgid.push_back(it->first);
                del_sid.push_back(it2->first);
            }
            
            //! del all tmp callback uuid
            for (size_t i = 0; i < del_callback_uuid.size(); ++i)
            {
                it2->second.m_callback_map.erase(del_callback_uuid[i]);
            }
            del_callback_uuid.clear();
        }
    }
    
    for (size_t i = 0; i < del_sgid.size(); ++i)
    {
        logwarn((BROKER, "broker_service_t::handle_broken del sgid[%u], sid[%u]", del_sgid[i], del_sid[i]));
        m_service_obj_mgr[del_sgid[i]].service_objs.erase(del_sid[i]);
        if (m_service_obj_mgr[del_sgid[i]].service_objs.empty())
        {
            m_service_obj_mgr.erase(del_sgid[i]);
            logwarn((BROKER, "broker_service_t::handle_broken del sgid[%u]", del_sgid[i]));
        }
    }
    
    if (NULL != sock_->get_data<socket_session_info_t>())
    {
        for (vector<string>::iterator vit = m_all_slave_host.begin(); vit != m_all_slave_host.end(); ++vit)
        {
            if (*vit == sock_->get_data<socket_session_info_t>()->slave_host)
            {
                m_all_slave_host.erase(vit);
                break;
            }
        }

        delete sock_->get_data<socket_session_info_t>();
        sock_->set_data(NULL);
    }

    sock_->safe_delete();
    m_all_sockets.erase(sock_);

    logwarn((BROKER, "broker_service_t::handle_broken en ok"));
    return 0;
}

int broker_service_t::handle_msg(const message_t& msg_, socket_ptr_t sock_)
{
    lock_guard_t lock(m_mutex);

    msg_tool_t msg_tool;
    try{
        msg_tool.decode(msg_.get_body());
    }catch(exception& e_)
    {
        logerror((BROKER, "broker_service_t::handle_msg except<%s>", e_.what()));
        return -1;
    }

    logtrace((BROKER, "broker_service_t::handle_msg begin... cmd[%u], name[%s], sgid[%u], sid[%u], msgid[%u]",
                      msg_.get_cmd(), msg_tool.get_name().c_str(), msg_tool.get_group_id(), msg_tool.get_service_id(), msg_tool.get_msg_id()));
    
    if (msg_tool.get_group_id() == 0 && msg_tool.get_service_id() == 0)
    {
        if (msg_tool.get_msg_id() == rpc_msg_cmd_e::CREATE_SERVICE_GROUP)
        {
            create_service_group_t::in_t in;
            in.decode(msg_.get_body());
            rpc_callcack_t<create_service_group_t::out_t> rcb;
            rcb.init_data(rpc_msg_cmd_e::INTREFACE_CALLBACK, 0, 0, in.get_uuid());
            rcb.set_socket(sock_);
            create_service_group(in, rcb);
        }
        else if (msg_tool.get_msg_id() == rpc_msg_cmd_e::CREATE_SERVICE)
        {
            create_service_t::in_t in;
            in.decode(msg_.get_body());
            rpc_callcack_t<create_service_t::out_t> rcb;
            rcb.init_data(rpc_msg_cmd_e::INTREFACE_CALLBACK, 0, 0, in.get_uuid());
            rcb.set_socket(sock_);
            create_service(in, rcb);
        }
        else if (msg_tool.get_msg_id() == rpc_msg_cmd_e::REG_INTERFACE)
        {
            reg_interface_t::in_t in;
            in.decode(msg_.get_body());
            
            rpc_callcack_t<reg_interface_t::out_t> rcb;
            rcb.init_data(rpc_msg_cmd_e::INTREFACE_CALLBACK, 0, 0, in.get_uuid());
            rcb.set_socket(sock_);

            reg_interface(in, rcb);
        }
        else if (msg_tool.get_msg_id() == rpc_msg_cmd_e::SYNC_ALL_SERVICE)
        {
            loginfo((BROKER, "broker_service_t::handle_msg begin... cmd[%u], name[%s], sgid[%u], sid[%u], msgid[%u] sock[%p]",
                      msg_.get_cmd(), msg_tool.get_name().c_str(), msg_tool.get_group_id(), msg_tool.get_service_id(), msg_tool.get_msg_id(), sock_));

            sync_all_service_t::in_t in;
            in.decode(msg_.get_body());
            
            rpc_callcack_t<sync_all_service_t::out_t> rcb;
            rcb.init_data(rpc_msg_cmd_e::INTREFACE_CALLBACK, 0, 0, in.get_uuid());
            rcb.set_socket(sock_);
            
            sync_all_service(in, rcb);
            m_all_sockets.insert(sock_);
        }
        else if (msg_tool.get_msg_id() == rpc_msg_cmd_e::PUSH_INIT_DATA)
        {
            push_init_data_t::in_t in;
            in.decode(msg_.get_body());
            process_sync_data(in);
        }
        else if (msg_tool.get_msg_id() == rpc_msg_cmd_e::REG_SLAVE_BROKER)
        {
            reg_slave_broker_t::in_t in;
            in.decode(msg_.get_body());
            loginfo((BROKER, "broker_service_t::handle_msg REG_SLAVE_BROKER node_id[%u]", in.node_id));
            
            service_obj_map_t::iterator it = m_service_obj_mgr.begin();
            
            for (; it != m_service_obj_mgr.end(); ++it)
            {
                map<uint16_t, service_obj_t>::iterator it2 = it->second.service_objs.begin();
                for (; it2 != it->second.service_objs.end(); ++it2)
                {
                    if (it2->second.node_id == in.node_id)
                    {
                        it2->second.socket_ptr = sock_;
                    }
                }
            }
            sock_->set_data(new socket_session_info_t(in.node_id));
            m_all_sockets.insert(sock_);
        }
        else if (msg_tool.get_msg_id() == rpc_msg_cmd_e::PUSH_ADD_SERVICE_GROUP)
        {
            push_add_service_group_t::in_t in;
            in.decode(msg_.get_body());
            
            service_obj_mgr_t obj_mgr;
            obj_mgr.id     = in.sgid;
            obj_mgr.name   = in.name;
            
            m_service_obj_mgr.insert(make_pair(obj_mgr.id, obj_mgr));
        }
        else if (msg_tool.get_msg_id() == rpc_msg_cmd_e::PUSH_ADD_SERVICE)
        {
            push_add_service_t::in_t in;
            in.decode(msg_.get_body());
            
            loginfo((BROKER, "broker_service_t::handle_msg PUSH_ADD_SERVICE node_id[%u]", in.node_id));

            map<uint16_t, service_obj_mgr_t>::iterator it = m_service_obj_mgr.find(in.sgid);
            if (it == m_service_obj_mgr.end())
            {
                logerror((BROKER, "broker_service_t::handle_msg failed... sgid<%u> not exist", in.sgid));
            }
            else
            {
                service_obj_t obj;
                obj.name       = it->second.name;
                obj.group_id   = in.sgid;
                obj.id         = in.sid;
                obj.socket_ptr = find_socket_by_node(in.node_id);
                obj.node_id    = in.node_id;
                
                it->second.service_objs[in.sid] = obj;
            }
        }
        else if (msg_tool.get_msg_id() == rpc_msg_cmd_e::PUSH_ADD_MSG)
        {
            push_add_msg_t::in_t in;
            in.decode(msg_.get_body());
            singleton_t<msg_name_store_t>::instance().add_msg(in.name, in.msg_id);
        }
    }
    else
    {
        service_obj_map_t::iterator obj_mgr_it = m_service_obj_mgr.find(msg_tool.get_group_id());
        if (obj_mgr_it == m_service_obj_mgr.end())
        {
            logerror((BROKER, "broker_service_t::handle_msg sgid not found cmd[%u], name[%s], sgid[%u], sid[%u], msgid[%u]",
                              msg_.get_cmd(), msg_tool.get_name().c_str(), msg_tool.get_group_id(), msg_tool.get_service_id(), msg_tool.get_msg_id()));
            return -1;
        }

        map<uint16_t, service_obj_t>::iterator sobj_it = obj_mgr_it->second.service_objs.find(msg_tool.get_service_id());
        if (sobj_it == obj_mgr_it->second.service_objs.end())
        {
            logerror((BROKER, "broker_service_t::handle_msg sid not found cmd[%u], name[%s], sgid[%u], sid[%u], msgid[%u]",
                      msg_.get_cmd(), msg_tool.get_name().c_str(), msg_tool.get_group_id(), msg_tool.get_service_id(), msg_tool.get_msg_id()));
            return -1;
        }

        switch (msg_.get_cmd())
        {
            case rpc_msg_cmd_e::CALL_INTERFACE:
            {
                sobj_it->second.async_call(msg_tool, msg_.get_body(), sock_);
            }break;
            case rpc_msg_cmd_e::INTREFACE_CALLBACK:
            {
                sobj_it->second.interface_callback(msg_tool, msg_.get_body());
            }break;
        }
    }

    return 0;
}

socket_ptr_t broker_service_t::find_socket_by_node(uint16_t node_id_)
{
    for (set<socket_ptr_t>::iterator it = m_all_sockets.begin(); it != m_all_sockets.end(); ++it)
    {
        if (node_id_ == (*it)->get_data<socket_session_info_t>()->node_id)
        {
            return (*it);
        }
    }
    return NULL;
}

void broker_service_t::sync_all_service(sync_all_service_t::in_t& in_msg_, rpc_callcack_t<sync_all_service_t::out_t>& cb_)
{
    push_init_data_t::in_t ret;

    service_obj_map_t::iterator it = m_service_obj_mgr.begin();
    
    for (; it != m_service_obj_mgr.end(); ++it)
    {
        ret.group_name_vt.push_back(it->second.name);
        ret.group_id_vt.push_back(it->second.id);
        
        map<uint16_t, service_obj_t>::iterator it2 = it->second.service_objs.begin();
        for (; it2 != it->second.service_objs.end(); ++it2)
        {
            push_init_data_t::id_info_t id_info;
            id_info.sgid = it2->second.group_id;
            id_info.sid  = it2->second.id;
            id_info.node_id = it2->second.node_id;

            ret.id_info_vt.push_back(id_info);
        }
    }
    
    map<string, uint16_t>& all_msg = singleton_t<msg_name_store_t>::instance().all_msg();
    for (map<string, uint16_t>::iterator it3 = all_msg.begin(); it3 != all_msg.end(); ++it3)
    {
        ret.msg_name_vt.push_back(it3->first);
        ret.msg_id_vt.push_back(it3->second);
    }
    
    ret.set_uuid(in_msg_.get_uuid());
    ret.node_id = ++m_uuid;
    ret.bind_id = 0;//! 绑定到某个broker slave上

    logtrace((BROKER, "broker_service_t::sync_all_service slave_host<%s>", in_msg_.slave_host.c_str()));
    if (false == in_msg_.slave_host.empty())
    {
        m_all_slave_host.push_back(in_msg_.slave_host);
    }
    ret.broker_slave_host.insert(ret.broker_slave_host.end(), m_all_slave_host.begin(), m_all_slave_host.end());

    //! 为socket 设置session info
    cb_.get_socket()->set_data(new socket_session_info_t(ret.node_id, in_msg_.slave_host));

    ret.set_msg_id(singleton_t<msg_name_store_t>::instance().name_to_id(ret.get_name()));
    msg_sender_t::send(cb_.get_socket(), rpc_msg_cmd_e::PUSH_INIT_DATA, ret);
    sync_all_service_t::out_t out;
    out.value = true;
    cb_(out);
}

void broker_service_t::create_service_group(create_service_group_t::in_t& in_msg_, rpc_callcack_t<create_service_group_t::out_t>& cb_)
{
    logtrace((BROKER, "broker_service_t::create_service_group begin... service_name<%s>", in_msg_.service_name.c_str()));
 
    create_service_group_t::out_t ret;
    service_obj_map_t::iterator it = m_service_obj_mgr.begin();
    
    for (; it != m_service_obj_mgr.end(); ++it)
    {
        if (it->second.name == in_msg_.service_name)
        {
            break;
        }
    }

    if (it != m_service_obj_mgr.end())
    {
        ret.service_id = it->second.id;
        loginfo((BROKER, "broker_service_t::create_service_group begin... service_name<%s> has exist", in_msg_.service_name.c_str()));
    }
    else
    {
        service_obj_mgr_t obj_mgr;
        obj_mgr.id = ++m_uuid;
        obj_mgr.name = in_msg_.service_name;
        //obj_mgr.socket_ptr = cb_.get_socket();
        ret.service_id = obj_mgr.id;
        m_service_obj_mgr.insert(make_pair(obj_mgr.id, obj_mgr));
    }

    ret.set_uuid(in_msg_.get_uuid());
    
    push_add_service_group_t::in_t push_msg;
    push_msg.name = in_msg_.service_name;
    push_msg.sgid = ret.service_id;
    push_msg_except(NULL, rpc_msg_cmd_e::PUSH_ADD_SERVICE_GROUP, push_msg);
    cb_(ret);
}

void broker_service_t::create_service(create_service_t::in_t& in_msg_, rpc_callcack_t<create_service_t::out_t>& cb_)
{
    logtrace((BROKER, "broker_service_t::create_service begin... sgid<%u>, sid[%u]", in_msg_.new_service_group_id, in_msg_.new_service_id));

    create_service_t::out_t ret;
    service_obj_mgr_t& som = m_service_obj_mgr[in_msg_.new_service_group_id];
    map<uint16_t, service_obj_t>::iterator it = som.service_objs.find(in_msg_.new_service_id);

    if (it != som.service_objs.end())
    {
        ret.value = false;

        logerror((BROKER, "broker_service_t::create_service failed... sgid<%u>, sid[%u] exist", in_msg_.new_service_group_id, in_msg_.new_service_id));
    }
    else
    {
        service_obj_t obj;
        obj.name = som.name;
        obj.group_id = in_msg_.new_service_group_id;
        obj.id       = in_msg_.new_service_id;
        obj.node_id  = cb_.get_socket()->get_data<socket_session_info_t>()->node_id;
        obj.socket_ptr = cb_.get_socket();

        som.service_objs[in_msg_.new_service_id] = obj;
        
        ret.value = true;
    }
    ret.set_uuid(in_msg_.get_uuid());
    
    push_add_service_t::in_t push_msg;
    push_msg.sgid = in_msg_.new_service_group_id;
    push_msg.sid  = in_msg_.new_service_id;
    push_msg.node_id = cb_.get_socket()->get_data<socket_session_info_t>()->node_id;
    push_msg_except(NULL, rpc_msg_cmd_e::PUSH_ADD_SERVICE, push_msg);
    
    cb_(ret);
}

void broker_service_t::reg_interface(reg_interface_t::in_t& in_msg_, rpc_callcack_t<reg_interface_t::out_t>& cb_)
{
    reg_interface_t::out_t ret;
    ret.alloc_id = -1;
    ret.set_uuid(in_msg_.get_uuid());

    logtrace((BROKER, "broker_service_t::reg_interface sgid[%u], sid[%u]", in_msg_.sgid, in_msg_.sid));

    service_obj_mgr_t& som = m_service_obj_mgr[in_msg_.sgid];
    map<uint16_t, service_obj_t>::iterator it = som.service_objs.find(in_msg_.sid);
    
    if (it != som.service_objs.end())
    {
        uint16_t tmp_id = singleton_t<msg_name_store_t>::instance().name_to_id(in_msg_.in_msg_name);
        if (tmp_id != 0)
        {
            ret.alloc_id = tmp_id;
        }
        else
        {
            ret.alloc_id = ++ m_msg_uuid;
        }
        tmp_id = singleton_t<msg_name_store_t>::instance().name_to_id(in_msg_.out_msg_name);
        if (tmp_id != 0)
        {
            ret.out_alloc_id = tmp_id;
        }
        else
        {
            ret.out_alloc_id = ++ m_msg_uuid;
        }
        
        singleton_t<msg_name_store_t>::instance().add_msg(in_msg_.in_msg_name, ret.alloc_id);
        singleton_t<msg_name_store_t>::instance().add_msg(in_msg_.out_msg_name, ret.out_alloc_id);

        logtrace((BROKER, "broker_service_t::reg_interface sgid[%u], sid[%u] alloc_id[%u]", in_msg_.sgid, in_msg_.sid, ret.alloc_id));
    }

    cb_(ret);
    {
        push_add_msg_t::in_t push_msg;
        push_msg.name    = in_msg_.in_msg_name;
        push_msg.msg_id  = ret.alloc_id;
        push_msg_except(cb_.get_socket(), rpc_msg_cmd_e::PUSH_ADD_MSG, push_msg);
    }
    {
        push_add_msg_t::in_t push_msg;
        push_msg.name    = in_msg_.out_msg_name;
        push_msg.msg_id  = ret.out_alloc_id;
        push_msg_except(cb_.get_socket(), rpc_msg_cmd_e::PUSH_ADD_MSG, push_msg);
    }
}

void broker_service_t::service_obj_t::async_call(msg_i& msg_, const string& body_, socket_ptr_t sp_)
{
    proc_stack_t stack;
    struct timeval now;
    gettimeofday(&now, NULL);
    
    stack.start_time = now.tv_sec*1000000 + now.tv_usec;

    stack.uuid    = msg_.get_uuid();
    stack.req_msg = msg_.get_name(); 
    stack.socket_ptr= sp_;

    uint32_t uuid = ++m_uuid;

    //! 直接修改消息包内的uuid
    string dest = body_;
    *((uint32_t*)dest.data()) = uuid;

    m_callback_map[uuid] = stack;
    msg_sender_t::send(socket_ptr, rpc_msg_cmd_e::CALL_INTERFACE , dest);
    logtrace((BROKER, "broker_service_t::service_obj_t::async_call socket<%p>", socket_ptr));
    
}

int broker_service_t::service_obj_t::interface_callback(msg_i& msg_, const string& body_)
{
    callback_map_t::iterator it = m_callback_map.find(msg_.get_uuid());
    if (it != m_callback_map.end())
    {
        string dest = body_;
        *((uint32_t*)dest.data()) = it->second.uuid;

        msg_sender_t::send(it->second.socket_ptr, rpc_msg_cmd_e::INTREFACE_CALLBACK, dest);
        
        struct timeval now;
        gettimeofday(&now, NULL);

        long cost = now.tv_sec*1000000 + now.tv_usec - it->second.start_time;

        singleton_t<performance_daemon_t>::instance().post(it->second.req_msg, cost);
        m_callback_map.erase(it);
        return 0;
    }
    else
    {
        logerror((BROKER, "broker_service_t::service_obj_t::interface_callback none uuid[%u]", msg_.get_uuid()));
    }

    return -1;
}

void broker_service_t::push_msg_except(socket_ptr_t socket_, uint16_t cmd_, msg_i& msg_)
{
    msg_.set_msg_id(singleton_t<msg_name_store_t>::instance().name_to_id(msg_.get_name()));
    set<socket_ptr_t>::iterator it = m_all_sockets.begin();
    for (; it != m_all_sockets.end(); ++it)
    {
        if (*it != socket_)
        {
            //msg_sender_t::send(*it, cmd_, msg_);
            msg_sender_t::send(*it, rpc_msg_cmd_e::CALL_INTERFACE , msg_);
            logtrace((BROKER, "broker_service_t::push_msg_except socket_[%p], msg<%s>", *it, msg_.get_name().c_str()));
        }
    }
}
