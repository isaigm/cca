#ifndef  CL_HANDLER_HPP
#define CL_HANDLER_HPP
#include <CL/cl.hpp>
#include "ocl.hpp"
struct CLHandler
{
    CLHandler() : device(ocl::getDevice()), context({device}), queue(context, device)
    {
    }
    cl::Device       device;
    cl::Context      context;
    cl::CommandQueue queue;

    template<class T>
    cl::Buffer requestMemory(std::vector<T> &hostMem, int flags)
    {
        cl::Buffer buffer(context, flags | CL_MEM_COPY_HOST_PTR, sizeof(T) * hostMem.size(), hostMem.data());
        return buffer;
    }
    template<class T>
    void requestRead(cl::Buffer& buffer, std::vector<T> &hostMem)
    {
        queue.enqueueReadBuffer(buffer, CL_TRUE, 0, sizeof(T) * hostMem.size(), hostMem.data());
    }
    template<class T>
    void requestWrite(cl::Buffer &buffer, std::vector<T> &hostMem)
    {
        queue.enqueueWriteBuffer(buffer, CL_TRUE, 0, sizeof(T) * hostMem.size(), hostMem.data());
    }
    void executeKernel(cl::Kernel &kernel, cl::NDRange globalWorkSize)
    {
        queue.enqueueNDRangeKernel(kernel, cl::NullRange, globalWorkSize);
    }
    void finishQueue()
    {
        queue.finish();
    }
};
#endif // ! CL_CONTEXT_HPP