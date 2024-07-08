#ifndef  CFG_HPP
#define  CFG_HPP
#include <iostream>
namespace constants
{
    const static int WIDTH  = 1500;
    const static int HEIGHT = 720;
    const static std::string pathToFont   =  "C:\\Users\\isaig\\Downloads\\ProggyClean.ttf";
    const static std::string pathToKernel =  "C:\\Users\\isaig\\examples\\opencl\\perform_step.cl";
    const static std::string templateStr  =  "//ACTIVATION_FUNCTION";
    const static size_t SIZE		      = WIDTH * HEIGHT;
    const static std::string kernelName   = "perform_step";
    const static std::string defaultFunction = "float gaussian(float x, float b) {\n"
        "return 1.0f / pow(2.0f, (pow(x - b, 2.0f)));\n"
        "}\n"

        "float activation(float x) {\n"
        "return gaussian(x, 3.5f);\n"
        "}\n";
};
#endif // ! CFG_HPP
