#ifndef _FF_XML_H_
#define _FF_XML_H_

#include <stdint.h>
#include <stdlib.h>

#include <string>
using namespace std;

#include <xml/tinyxml.h>

namespace ff
{

class ffxml_t
{
public:
    ffxml_t();
    ~ffxml_t();
    int    load(const string& file_);
    //! root.a
    //! root.{b}
    //! root.@1.{b} root.@2.c
    const char* get(const string& node_name_);
    size_t      size(const string& node_name_);

protected:
    pair<TiXmlElement*, const char*> get_node(const string& node_name_);
private:
    TiXmlDocument       m_xml_doc;
};
}

#endif
