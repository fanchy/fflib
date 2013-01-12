#include <iostream>
using namespace std;

#include "detail/task_queue_impl.h"
#include "thread.h"
#include "log.h"

using namespace ff;

int main(int argc, char* argv[])
{

	LOG.start("-log_path ./log -log_filename log -log_class FF,XX -log_print_screen true -log_print_file true -log_level 6");
	LOGDEBUG(("XX", "FFFFF"));
	LOGTRACE(("XX", "FFFFF"));
	LOGINFO(("XX", "FFFFF"));
	LOGWARN(("XX", "FFFFF"));
	LOGERROR(("XX", "FFFFF"));
	LOGFATAL(("XX", "FFFFF"));
	LOG.mod_class("TT", true);

	sleep(1);
	LOGFATAL(("TT", "FFFFF"));

	LOGFATAL(("FF", "DSDFFFFF%s", string("SFWEGGGGGGGGG")));

	LOG.stop();
    return 0;
}
