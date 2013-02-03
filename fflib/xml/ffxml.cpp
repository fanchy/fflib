
#include "xml/ffxml.h"
#include "base/strtool.h"
#include "tinyxml.h"
using namespace ff;

ffxml_t::ffxml_t()
{
    
}
ffxml_t::~ffxml_t()
{
    
}

int ffxml_t::load(const string& file_)
{
    if (false == m_xml_doc.LoadFile(file_.c_str()))
    {
        return -1;
    }
    return 0;
}
const char* ffxml_t::get(const string& node_name_)
{
    pair<TiXmlElement*, const char*> ret = get_node(node_name_);
    if (ret.second != NULL)
    {
        return ret.second;
    }
    return "";
}
pair<TiXmlElement*, const char*> ffxml_t::get_node(const string& node_name_)
{
    const char* ret = "";
    TiXmlElement* cur_elem = NULL;
    vector<string> node_vt;
    strtool_t::split(node_name_, node_vt, ".");
    if (node_vt.empty()) //! 必须有root名称
    {
        return make_pair(cur_elem, ret);
    }
    cur_elem = m_xml_doc.FirstChildElement(node_vt[0].c_str());
    if (NULL == cur_elem)
    {
        return make_pair(cur_elem, ret);
    }
    ret = cur_elem->GetText();
    
    for (size_t i = 1; i < node_vt.size(); ++i)
    {
        string& node = node_vt[i];
        if (node[0] == '{' && node[node.size() - 1] == '}')//! 获取属性 root.{a}
        {
            if (node.size() == 2) break;//! 空属性
            node.erase(node.begin() + (node.size() - 1));
            node.erase(node.begin());
            if (node.size() > 1)
            {
                int index = ::atoi(node.c_str() + 1);
                TiXmlAttribute* attr = cur_elem->FirstAttribute();
                while (attr && index > 0)
                {
                    -- index;
                    attr = attr->Next();
                }
                if (attr == NULL || index > 0)
                {
                    //! 索引越界
                    ret = "";
                    break;
                }
                if (node[0] == '@')//! 获取属性中第N个值
                {
                    ret = attr->Value();
                    break;
                }
                else if (node[0] == '&')//! 获取属性中第N个name
                {
                    ret = attr->Name();
                    break;
                }
            }
            //! 通过key获取属性值
            ret = cur_elem->Attribute(node.c_str());
            break;
        }
        else if (node[0] == '@')//! 获取子标签 值
        {
            int index = ::atoi(node.c_str() + 1);
            TiXmlElement* tmp = cur_elem->FirstChildElement();
            while (tmp && index > 0)
            {
                -- index;
                tmp = tmp->NextSiblingElement();
            }
            if (tmp == NULL || index != 0)
            {
                //! 没有找到
                ret = "";
                break;
            }
            cur_elem = tmp;
            ret = cur_elem->GetText();
        }
        else if (node[0] == '&')//! 获取子标签 name
        {
            int index = ::atoi(node.c_str() + 1);
            TiXmlElement* tmp = cur_elem->FirstChildElement();
            while (tmp && index > 0)
            {
                -- index;
                tmp = tmp->NextSiblingElement();
            }
            if (tmp == NULL || index != 0)
            {
                //! 没有找到
                ret = "";
                break;
            }
            cur_elem = tmp;
            ret = cur_elem->Value();
            break;
        }
        else//! 获取特定子标签
        {
//            TiXmlElement* tmp = cur_elem->FirstChildElement();
//            while (tmp)
//            {
//                if (tmp->Value() == node)
//                {
//
//                    break;
//                }
//                tmp = tmp->NextSiblingElement();
//            }
                cur_elem = cur_elem->FirstChildElement(node.c_str());
                if (NULL == cur_elem)
                {
                    break;
                }
                ret = cur_elem->GetText();
        }
    }

    return make_pair(cur_elem, ret);
}
size_t ffxml_t::size(const string& node_name_)
{
    size_t ret = 0;
    pair<TiXmlElement*, const char*> pnode = get_node(node_name_);
    if (pnode.first == NULL)
    {
        return ret;
    }
    if (false == node_name_.empty() && node_name_[node_name_.length() - 1] == '}')//! 获取属性的数量
    {
        TiXmlAttribute* attr = pnode.first->FirstAttribute();
        while (attr)
        {
            ++ ret;
            attr = attr->Next();
        }
    }
    else
    {
        TiXmlElement* tmp = pnode.first->FirstChildElement();
        while (tmp)
        {
            ++ ret;
            tmp = tmp->NextSiblingElement();
        }
    }
    return ret;
}
