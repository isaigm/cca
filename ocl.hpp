#ifndef  OCL_HPP
#define  OCL_HPP
#include <CL/cl.hpp>
#include <fstream>
#include <iostream>
#include <optional>
namespace ocl
{
    struct CompileResult
    {
        std::optional<cl::Program> program;
        std::string error;
    };
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
    CompileResult buildProgram(const cl::Context& context, const cl::Device& device, const std::string &sourceCode)
    {
        cl::Program::Sources sources;
        CompileResult result;
        sources.push_back({ sourceCode.c_str(), sourceCode.length() });
        cl::Program program(context, sources);
        if (program.build({ device }) != CL_SUCCESS)
        {
            result.program = std::nullopt;
            result.error   = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device).c_str();
            return result;
        }
        result.program = program;
        return result;
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
