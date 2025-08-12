#include "CLHandler.h"
#include "ocl_utils.h" 

CLHandler::CLHandler()
{
    
    device = ocl::getDevice(); 
    context = cl::Context({ device });
    queue = cl::CommandQueue(context, device);
}

void CLHandler::executeKernel(const cl::Kernel& kernel, const cl::NDRange& globalWorkSize)
{
    queue.enqueueNDRangeKernel(kernel, cl::NullRange, globalWorkSize);
}

void CLHandler::finishQueue()
{
    queue.finish();
}