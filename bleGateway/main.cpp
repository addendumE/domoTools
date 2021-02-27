#include "AppConfig.h"
#include "App.h"

int main(int argc, char **argv) {
	if (argc != 2)
		{
			fprintf (stderr,"missing conf file\n");
			return -1;
		}
	std::string confFile=string(argv[1]);
	App *app = new App(confFile);
	while(1) {
	    sleep(1);
    }
	return 0;
}
