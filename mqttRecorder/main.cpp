#include "App.h"
#include <string>

App *app;
#include <sys/resource.h>

int main(int argc, char **argv) {
	// core dumps may be disallowed by parent of this process; change that
	struct rlimit core_limits;
	core_limits.rlim_cur = core_limits.rlim_max = RLIM_INFINITY;
	setrlimit(RLIMIT_CORE, &core_limits);

	if (argc != 2)
	{
		fprintf (stderr,"missing conf file\n");
		return -1;
	}
	std::string confFile=string(argv[1]);
	app = new App(confFile);
	while(1) {
	    sleep(1);
    }

	return 0;
}
