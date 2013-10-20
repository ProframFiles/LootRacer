#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "akj_ogl.h"
#include "cs314_timing.hpp"
#include "sdl_application.hpp"
#include "cs314_p4_log.hpp"

//why on earth does SDL def main?
#if defined(main)
	#undef main
#endif // main

int main(int argc, char *argv[])
{
	p4::Log::Info("Creating GL context");
	// make sure the context outlives the application
	cSDLGLContext gl_context(1920, 1080);
	{
		p4::Log::Info("Creating Application");
		cSDLApplication my_game(gl_context);
		p4::Log::Info("Running Application");
		my_game.Run();
	}
	
	return 0;
}

