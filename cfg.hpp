#ifndef  CFG_HPP
#define  CFG_HPP
#include <iostream>
namespace constants
{
	const static int WIDTH  = 1920;
	const static int HEIGHT = 1080;
	const static std::string pathToFont   =  "C:\\Users\\isaig\\Downloads\\ProggyClean.ttf";
	const static std::string pathToKernel =  "C:\\Users\\isaig\\examples\\opencl\\perform_step.cl";
	const static std::string templateStr  =  "//ACTIVATION_FUNCTION";
	const static size_t SIZE		      = WIDTH * HEIGHT;
	const static std::string kernelName   = "perform_step";
};
#endif // ! CFG_HPP
