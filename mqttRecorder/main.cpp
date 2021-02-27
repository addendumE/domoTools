#include "App.h"
#include <string>

App *app;

int main(int argc, char **argv) {

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
