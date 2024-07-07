#ifndef  OCL_HPP
#define  OCL_HPP
#include <CL/cl.hpp>
#include <fstream>
#include <iostream>
namespace ocl
{
    std::string loadKernelFromFile(std::string path)
    {
        std::fstream file(path);
        if (file.is_open())
        {
            std::string content(
                (std::istreambuf_iterator<char>(file)),
                std::istreambuf_iterator<char>());
            return content;
        }
        throw std::exception("error at opening file");
    }
    cl::Program buildProgram(cl::Context& context, cl::Device& device, std::string path)
    {
        cl::Program::Sources sources;
        std::string sourceCode = loadKernelFromFile(path);
        sources.push_back({ sourceCode.c_str(), sourceCode.length() });
        cl::Program program(context, sources);
        if (program.build({ device }) != CL_SUCCESS)
        {
            std::cerr << "error:" << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device) << "\n";
            throw std::exception("Compiling error");
        }
        return program;
    }
    cl::Device getDevice()
    {
        std::vector<cl::Platform> platforms;
        cl::Platform::get(&platforms);
        if (platforms.size() == 0)
        {
            throw std::exception("No OpenCL platform was found");
        }
        cl::Platform default_platform = platforms[0];
        std::cout << default_platform.getInfo<CL_PLATFORM_NAME>() << "\n";
        std::vector<cl::Device> devices;
        default_platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);
        if (devices.size() == 0)
        {
            throw std::exception("No devices were found");
           
        }
        return devices[0];
    }
};

#endif // ! OCL_HPP
