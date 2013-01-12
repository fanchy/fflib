#ifndef _PLUGIN_FACTORY_H_
#define _PLUGIN_FACTORY_H_

#include <string>
using namespace std;

#include "plugin_i.h"

using namespace ff;

class plugin_factory_t
{
public:
    static plugin_ptr_t create_plugin(const string& path_);
};
#endif
