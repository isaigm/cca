#ifndef OCL_UTILS_HPP
#define OCL_UTILS_HPP

#include <CL/cl2.hpp>
#include <string>
#include <vector>
#include <optional>

namespace ocl
{
    
    struct CompileResult
    {
        std::optional<cl::Program> program;
        std::string error;
    };

    std::string loadKernelFromFile(const std::string& path);
    cl::Device getDevice(cl_device_type deviceType = CL_DEVICE_TYPE_GPU);
    CompileResult buildProgram(const cl::Context& context, const cl::Device& device, const std::string& sourceCode);
}

#endif // OCL_UTILS_HPP