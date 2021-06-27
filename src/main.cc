#include "scene_manager.h"

int main(int argc, char ** argv){
	const char * title = "Die Lawaaierige Planeet";
	int w, h;

	if(argc > 2){
		w = atoi(argv[1]);
		h = atoi(argv[2]);
	}else{
		// w = 640;
		// h = 480;
		w = 1000;
		h = 800;
	}

	scene_manager::start(title, w, h);
	return 0;
}
