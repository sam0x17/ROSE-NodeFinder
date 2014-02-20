/*
 * srcloader.C
 *
 * A basic command line for creating *.ast files from single
 * source files or large projects spanning multiple files
 *
 *  Created on: Feb 6, 2014
 *      Author: Sam Kelly <kellys@dickinson.edu>
 */
#include <SourceLoader.h>

int main(int argc, char **argv)
{
	// process command line arguments
	if(argc == 1)
	{
		if(strcmp("start", argv[0]))
		{
			std::cout << "start command executed" << std::endl;
			return 0;
		} else if(strcmp("stop", argv[0])) {
			std::cout << "stop command executd" << std::endl;
			return 0;
		}
	}
	
	// attempt to load source file into ROSE
   SgProject *project = frontend(argc, argv);
   SgNode *root_node = (SgNode*)project;
}
