#include "ocl_utils.h"
#include <fstream>
#include <iostream>

namespace ocl
{
    std::string loadKernelFromFile(const std::string& path)
    {
        std::ifstream file(path);
        if (!file.is_open())
        {
            throw std::runtime_error("Error opening kernel file: " + path);
        }
        std::string content(
            (std::istreambuf_iterator<char>(file)),
            std::istreambuf_iterator<char>());
        return content;
    }

    cl::Device getDevice(cl_device_type deviceType)
    {
        std::vector<cl::Platform> platforms;
        cl::Platform::get(&platforms);
        if (platforms.empty())
        {
            throw std::runtime_error("No OpenCL platform was found");
        }

        for (const auto& platform : platforms)
        {
            std::vector<cl::Device> devices;
            platform.getDevices(deviceType, &devices);
            if (!devices.empty())
            {
                std::cout << "Using device: " << devices[0].getInfo<CL_DEVICE_NAME>() << "\n";
                std::cout << "On platform: " << platform.getInfo<CL_PLATFORM_NAME>() << "\n";
                return devices[0];
            }
        }
        throw std::runtime_error("No OpenCL devices of the specified type were found");
    }

    CompileResult buildProgram(const cl::Context& context, const cl::Device& device, const std::string& sourceCode)
    {
        cl::Program::Sources sources;
        CompileResult result;
        sources.push_back({ sourceCode.c_str(), sourceCode.length() });

        cl::Program program(context, sources);
        cl_int err = program.build({ device });

        if (err != CL_SUCCESS)
        {
            result.program = std::nullopt;
            result.error = "Build error (" + std::to_string(err) + "): " + program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device);
            return result;
        }

        result.program = program;
        return result;
    }
}