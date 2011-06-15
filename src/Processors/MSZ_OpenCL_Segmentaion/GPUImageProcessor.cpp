﻿
/*!
 * \file GPUImageProcessor.cpp
 * \brief Class responsible for managing the image processing.
 *
 * \author Mateusz Pruchniak
 * \date 2010-05-05
 */

#include "GPUImageProcessor.h"

GPUImageProcessor::GPUImageProcessor(int width,int height,int nChannels)
{

    GPUError = oclGetPlatformID(&cpPlatform);
    CheckError(GPUError);
    
	cl_uint uiNumAllDevs = 0;

	// Get the number of GPU devices available to the platform
    GPUError = clGetDeviceIDs(cpPlatform, CL_DEVICE_TYPE_GPU, 0, NULL, &uiNumAllDevs);
    CheckError(GPUError);
    uiDevCount = uiNumAllDevs;
 
    // Create the device list
    cdDevices = new cl_device_id [uiDevCount];
    GPUError = clGetDeviceIDs(cpPlatform, CL_DEVICE_TYPE_GPU, uiDevCount, cdDevices, NULL);
    CheckError(GPUError);
	
    // Create the OpenCL context on a GPU device
    GPUContext = clCreateContext(0, uiNumAllDevs, cdDevices, NULL, NULL, &GPUError);
    CheckError(GPUError);
   	 
    
    //The command-queue can be used to queue a set of operations (referred to as commands) in order.
    GPUCommandQueue = clCreateCommandQueue(GPUContext, cdDevices[0], 0, &GPUError);
    CheckError(GPUError);

    Transfer = new GPUTransferManager(GPUContext,GPUCommandQueue,width,height,nChannels);

    oclPrintDevName(LOGBOTH, cdDevices[0]);  
}

void GPUImageProcessor::CheckError(int code)
{
    switch(code)
    {
    case CL_SUCCESS:
        return;
        break;
   
    default:
         cout << "OTHERS ERROR" << endl;
    }
}

GPUImageProcessor::~GPUImageProcessor()
{
    delete Transfer;
    delete segmentation;

    if(GPUCommandQueue)clReleaseCommandQueue(GPUCommandQueue);
    if(GPUContext)clReleaseContext(GPUContext);
}

void GPUImageProcessor::AddProcessing(WatershedSegmentation* ws)
{
    segmentation = ws;
}

void GPUImageProcessor::Process()
{
   segmentation->Segmentation(GPUCommandQueue);
}
