#ifndef CL_HANDLER_HPP
#define CL_HANDLER_HPP

#include <CL/cl2.hpp>
#include <vector>

class CLHandler
{
public:
    CLHandler();

    template<typename T>
    cl::Buffer createBuffer(size_t elementCount, cl_mem_flags flags);

    template<typename T>
    cl::Buffer createBuffer(const std::vector<T>& hostMem, cl_mem_flags flags);

    template<typename T>
    void readBuffer(const cl::Buffer& buffer, std::vector<T>& hostMem);

    template<typename T>
    void writeBuffer(const cl::Buffer& buffer, const std::vector<T>& hostMem);

    void executeKernel(const cl::Kernel& kernel, const cl::NDRange& globalWorkSize);
    void finishQueue();

    cl::Device device;
    cl::Context context;

private:
    cl::CommandQueue queue;

    template <typename T>
    static size_t sizeOfVector(const std::vector<T>& vec)
    {
        return sizeof(T) * vec.size();
    }
};

template<typename T>
cl::Buffer CLHandler::createBuffer(size_t elementCount, cl_mem_flags flags)
{
    return cl::Buffer(context, flags, sizeof(T) * elementCount);
}

template<typename T>
cl::Buffer CLHandler::createBuffer(const std::vector<T>& hostMem, cl_mem_flags flags)
{
    return cl::Buffer(context, flags | CL_MEM_COPY_HOST_PTR, sizeOfVector(hostMem), (void*)hostMem.data());
}

template<typename T>
void CLHandler::readBuffer(const cl::Buffer& buffer, std::vector<T>& hostMem)
{
    queue.enqueueReadBuffer(buffer, CL_TRUE, 0, sizeOfVector(hostMem), hostMem.data());
}

template<typename T>
void CLHandler::writeBuffer(const cl::Buffer& buffer, const std::vector<T>& hostMem)
{
    queue.enqueueWriteBuffer(buffer, CL_TRUE, 0, sizeOfVector(hostMem), hostMem.data());
}

#endif // CL_HANDLER_HPP