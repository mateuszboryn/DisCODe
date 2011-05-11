/*!
 * \file GPUTransferManager.cpp
 * \brief Class responsible for managing transfer to GPU.
 *
 * \author Mateusz Pruchniak
 * \date 2010-05-05
 */

#include "GPUTransferManager.h"

GPUTransferManager::~GPUTransferManager(void)
{
	 Cleanup();
}

GPUTransferManager::GPUTransferManager()
{
	inDevBuf = NULL;
	outDevBuf = NULL;
    	GPUInputOutput = NULL;
    	cmPinnedBuf = NULL;
}

GPUTransferManager::GPUTransferManager( cl_context GPUContextArg, cl_command_queue GPUCommandQueueArg, unsigned int width, unsigned int height, int channels )
{
    //cout << "data transfer konstr" << endl;
	
	nChannels = channels;
	GPUContext = GPUContextArg;
	ImageHeight = height;
	ImageWidth = width;
	GPUCommandQueue = GPUCommandQueueArg;

    // Allocate pinned input and output host image buffers:  mem copy operations to/from pinned memory is much faster than paged memory
    szBuffBytes_char = ImageWidth * ImageHeight * nChannels * sizeof (char);
	szBuffBytes_int = ImageWidth * ImageHeight * nChannels * sizeof (int);
    // This flag specifies that the application wants the OpenCL implementation to allocate memory from host accessible memory.


    cmPinnedBuf = clCreateBuffer(GPUContext, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR, szBuffBytes_int, NULL, &GPUError);
    CheckError(GPUError);

    // Enqueues a command to map a region of the buffer object given by buffer into the host address space and returns a pointer to this mapped region.
    GPUInputOutput = (cl_uint*)clEnqueueMapBuffer(GPUCommandQueue, cmPinnedBuf, CL_TRUE, CL_MAP_WRITE, 0, szBuffBytes_int, 0, NULL, NULL, &GPUError);
    CheckError(GPUError);


    inDevBuf = clCreateBuffer(GPUContext, CL_MEM_READ_WRITE, szBuffBytes_char, NULL, &GPUError);
	CheckError(GPUError);

	outDevBuf = clCreateBuffer(GPUContext, CL_MEM_READ_WRITE, szBuffBytes_int, NULL, &GPUError);
	CheckError(GPUError);

	outDevBuf_1 = clCreateBuffer(GPUContext, CL_MEM_READ_WRITE, szBuffBytes_int, NULL, &GPUError);
	CheckError(GPUError);
	
	outDevBuf_2 = clCreateBuffer(GPUContext, CL_MEM_READ_WRITE, szBuffBytes_int, NULL, &GPUError);
	CheckError(GPUError);
}





void GPUTransferManager::CheckError(int code)
{
    switch(code)
    {
    case CL_SUCCESS:
        return;
        break;
    case CL_INVALID_COMMAND_QUEUE:
        cout << "CL_INVALID_COMMAND_QUEUE" << endl;
        break;
    case CL_INVALID_CONTEXT:
        cout << "CL_INVALID_CONTEXT" << endl;
        break;
    case CL_INVALID_MEM_OBJECT:
        cout << "CL_INVALID_MEM_OBJECT" << endl;
        break;
    case CL_INVALID_VALUE:
        cout << "CL_INVALID_VALUE" << endl;
        break;
    case CL_INVALID_EVENT_WAIT_LIST:
        cout << "CL_INVALID_EVENT_WAIT_LIST" << endl;
        break;
    case CL_MEM_OBJECT_ALLOCATION_FAILURE:
        cout << "CL_MEM_OBJECT_ALLOCATION_FAILURE" << endl;
        break;
    case CL_OUT_OF_HOST_MEMORY:
        cout << "CL_OUT_OF_HOST_MEMORY" << endl;
        break;
    default:
         cout << "OTHERS ERROR" << endl;
    }
}

void GPUTransferManager::Cleanup()
{
    if(inDevBuf)clReleaseMemObject(inDevBuf);
	if(outDevBuf)clReleaseMemObject(outDevBuf);
	if(outDevBuf_1)clReleaseMemObject(outDevBuf_1);
	if(outDevBuf_2)clReleaseMemObject(outDevBuf_2);
}

void GPUTransferManager::ReceiveImage(IplImage * out, IplImage * out1,  IplImage * out2)
{
	szBuffBytes_int = ImageWidth * ImageHeight * nChannels * sizeof (int);
    GPUError = clEnqueueReadBuffer(GPUCommandQueue, outDevBuf, CL_TRUE, 0, szBuffBytes_int, (void*)GPUInputOutput, 0, NULL, NULL);
    CheckError(GPUError);
	GPUError = clEnqueueReadBuffer(GPUCommandQueue, outDevBuf_1, CL_TRUE, 0, szBuffBytes_int, (void*)(out1->imageData), 0, NULL, NULL);
    CheckError(GPUError);
	GPUError = clEnqueueReadBuffer(GPUCommandQueue, outDevBuf_2, CL_TRUE, 0, szBuffBytes_int, (void*)(out2->imageData), 0, NULL, NULL);
    CheckError(GPUError);
    out->imageData = (char*)GPUInputOutput;
}

void GPUTransferManager::SendImage( IplImage* imageToLoad )
{
	ImageHeight = imageToLoad->height;
    ImageWidth = imageToLoad->width;
	szBuffBytes_char = ImageWidth * ImageHeight * nChannels * sizeof (char);
	image = imageToLoad;
	int size = image->nSize;
    GPUError = clEnqueueWriteBuffer(GPUCommandQueue, inDevBuf, CL_TRUE, 0, szBuffBytes_char, (void*)imageToLoad->imageData, 0, NULL, NULL);
    CheckError(GPUError);
}

