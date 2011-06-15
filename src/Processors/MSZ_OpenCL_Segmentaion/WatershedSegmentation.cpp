/*!
 * \file WatershedSegmentation.cpp
 * \brief Abstract class for all WatershedSegmentations
 *
 * \author Mateusz Pruchniak
 * \date 2010-05-05
 */

#include "WatershedSegmentation.h"

WatershedSegmentation::WatershedSegmentation(void)
{

}

WatershedSegmentation::WatershedSegmentation(char* source, cl_context GPUContext ,GPUTransferManager* transfer)
{
    GPUTransfer = transfer;

    iBlockDimX = 16;
    iBlockDimY = 16;
    size_t szKernelLength;	
    size_t szKernelLengthWatershedSegmentation;
    size_t szKernelLengthSum;

    // Load OpenCL kernel

    SourceOpenCL = oclLoadProgSource("GPUCode.cl", "// My comment\n", &szKernelLength);
    SourceOpenCLWatershedSegmentation = oclLoadProgSource(source, "// My comment\n", &szKernelLengthWatershedSegmentation);
	//strncat (SourceOpenCL, SourceOpenCLWatershedSegmentation,szKernelLengthWatershedSegmentation );
	szKernelLengthSum = szKernelLength + szKernelLengthWatershedSegmentation;


	char* sourceCL = new char[szKernelLengthSum];
  	
	strcpy(sourceCL, SourceOpenCL);

	strcat(sourceCL, SourceOpenCLWatershedSegmentation);

	szBuffBytes_char = (GPUTransfer->ImageWidth) * (GPUTransfer->ImageHeight) * (GPUTransfer->nChannels) * sizeof(char);
	szBuffBytes_int = (GPUTransfer->ImageWidth) * (GPUTransfer->ImageHeight) * (GPUTransfer->nChannels) * sizeof(int);

	labels	= clCreateBuffer(GPUContext, CL_MEM_READ_WRITE, szBuffBytes_int, NULL, &GPUError);
	CheckError(GPUError);
	labels_lin	= clCreateBuffer(GPUContext, CL_MEM_READ_WRITE, szBuffBytes_int, NULL, &GPUError);
	CheckError(GPUError);
	read_label = clCreateBuffer(GPUContext, CL_MEM_READ_WRITE, szBuffBytes_int, NULL, &GPUError);
	CheckError(GPUError);
	labels_border = clCreateBuffer(GPUContext, CL_MEM_READ_WRITE, szBuffBytes_int, NULL, &GPUError);
	CheckError(GPUError);

	flag = clCreateBuffer(GPUContext, CL_MEM_READ_WRITE, 4*sizeof(int), NULL, &GPUError);
	CheckError(GPUError);


    // creates a program object for a context, and loads the source code specified by the text strings in
    //the strings array into the program object. The devices associated with the program object are the
    //devices associated with context.
    GPUProgram = clCreateProgramWithSource( GPUContext , 1, (const char **)&sourceCL, &szKernelLengthSum, &GPUError);
    CheckError(GPUError);

    // Build the program with 'mad' Optimization option
    char *flags = "-cl-mad-enable";

    GPUError = clBuildProgram(GPUProgram, 0, NULL, flags, NULL, NULL);
    CheckError(GPUError);

	GPUWatershedSegmentation.push_back(clCreateKernel(GPUProgram, "watershed_phaseI", &GPUError));
	GPUWatershedSegmentation.push_back(clCreateKernel(GPUProgram, "watershed_phaseII", &GPUError));
	GPUWatershedSegmentation.push_back(clCreateKernel(GPUProgram, "watershed_phaseIII", &GPUError));
	GPUWatershedSegmentation.push_back(clCreateKernel(GPUProgram, "watershed_phaseIV", &GPUError));
	GPUWatershedSegmentation.push_back(clCreateKernel(GPUProgram, "watershed_phaseV", &GPUError));
	GPUWatershedSegmentation.push_back(clCreateKernel(GPUProgram, "watershed_phaseVI", &GPUError));
	GPUWatershedSegmentation.push_back(clCreateKernel(GPUProgram, "watershed_phaseVII", &GPUError));
	GPUWatershedSegmentation.push_back(clCreateKernel(GPUProgram, "watershed_phaseVIII", &GPUError));
	GPUWatershedSegmentation.push_back(clCreateKernel(GPUProgram, "watershed_phaseIX", &GPUError));
	GPUWatershedSegmentation.push_back(clCreateKernel(GPUProgram, "watershed_phaseX", &GPUError));

}

WatershedSegmentation::~WatershedSegmentation()
{
	if(read_label)clReleaseMemObject(read_label);
	if(labels)clReleaseMemObject(labels);
	if(labels_lin)clReleaseMemObject(labels_lin);
	if(labels_border)clReleaseMemObject(labels_border);
    if(GPUProgram)clReleaseProgram(GPUProgram);
	for(int i=0; i < GPUWatershedSegmentation.size();i++)
		if(GPUWatershedSegmentation[i])clReleaseKernel(GPUWatershedSegmentation[i]);
	
}


void WatershedSegmentation::CheckError(int code)
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

bool  WatershedSegmentation::Segmentation(cl_command_queue GPUCommandQueue){

//I etap
    GPUError = clSetKernelArg(GPUWatershedSegmentation[0], 0, sizeof(cl_mem), (void*)&GPUTransfer->inDevBuf);
	GPUError |= clSetKernelArg(GPUWatershedSegmentation[0], 1, sizeof(cl_mem), (void*)&read_label);
    GPUError |= clSetKernelArg(GPUWatershedSegmentation[0], 2, sizeof(cl_uint), (void*)&GPUTransfer->ImageWidth);
    GPUError |= clSetKernelArg(GPUWatershedSegmentation[0], 3, sizeof(cl_uint), (void*)&GPUTransfer->ImageHeight);
    if(GPUError) return false;

	int flaga_stop;
	size_t GPULocalWorkSize[2]; 
    GPULocalWorkSize[0] = iBlockDimX;
    GPULocalWorkSize[1] = iBlockDimY;
    GPUGlobalWorkSize[0] = shrRoundUp((int)GPULocalWorkSize[0], GPUTransfer->ImageWidth); 
    GPUGlobalWorkSize[1] = shrRoundUp((int)GPULocalWorkSize[1], (int)GPUTransfer->ImageHeight);

	
    clEnqueueNDRangeKernel( GPUCommandQueue, GPUWatershedSegmentation[0], 2, NULL, GPUGlobalWorkSize, GPULocalWorkSize, 0, NULL, NULL);

	clEnqueueCopyBuffer( GPUCommandQueue, read_label, labels, 0, 0, szBuffBytes_int, 0, NULL, NULL);

//II etap
	GPUError = clSetKernelArg(GPUWatershedSegmentation[1], 0, sizeof(cl_mem), (void*)&labels);
	GPUError |= clSetKernelArg(GPUWatershedSegmentation[1], 1, sizeof(cl_mem), (void*)&read_label );
    GPUError |= clSetKernelArg(GPUWatershedSegmentation[1], 2, sizeof(cl_uint), (void*)&GPUTransfer->ImageWidth);
    GPUError |= clSetKernelArg(GPUWatershedSegmentation[1], 3, sizeof(cl_uint), (void*)&GPUTransfer->ImageHeight);
    if(GPUError) return false;

	clEnqueueNDRangeKernel( GPUCommandQueue, GPUWatershedSegmentation[1], 2, NULL, GPUGlobalWorkSize, GPULocalWorkSize, 0, NULL, NULL);

	clEnqueueCopyBuffer( GPUCommandQueue, read_label, labels, 0, 0, szBuffBytes_int, 0, NULL, NULL);
//III etap

	GPUError = clSetKernelArg(GPUWatershedSegmentation[2], 0, sizeof(cl_mem), (void*)&labels);
	GPUError |= clSetKernelArg(GPUWatershedSegmentation[2], 1, sizeof(cl_mem), (void*)&read_label);
    GPUError |= clSetKernelArg(GPUWatershedSegmentation[2], 2, sizeof(cl_uint), (void*)&GPUTransfer->ImageWidth);
    GPUError |= clSetKernelArg(GPUWatershedSegmentation[2], 3, sizeof(cl_uint), (void*)&GPUTransfer->ImageHeight);
    if(GPUError) return false;

	clEnqueueNDRangeKernel( GPUCommandQueue, GPUWatershedSegmentation[2], 2, NULL, GPUGlobalWorkSize, GPULocalWorkSize, 0, NULL, NULL);

//IV etap

do{	
	flaga_stop = 0;
	clEnqueueCopyBuffer( GPUCommandQueue, read_label, labels_lin, 0, 0, szBuffBytes_int, 0, NULL, NULL);

	GPUError = clSetKernelArg(GPUWatershedSegmentation[3], 0, sizeof(cl_mem), (void*)&labels);
	GPUError |= clSetKernelArg(GPUWatershedSegmentation[3], 1, sizeof(cl_mem), (void*)&labels_lin);
	GPUError |= clSetKernelArg(GPUWatershedSegmentation[3], 2, sizeof(cl_mem), (void*)&flag);
	GPUError |= clSetKernelArg(GPUWatershedSegmentation[3], 3, sizeof(cl_mem), (void*)&read_label);
    GPUError |= clSetKernelArg(GPUWatershedSegmentation[3], 4, sizeof(cl_uint), (void*)&GPUTransfer->ImageWidth);
    GPUError |= clSetKernelArg(GPUWatershedSegmentation[3], 5, sizeof(cl_uint), (void*)&GPUTransfer->ImageHeight);
    if(GPUError) return false;

	clEnqueueNDRangeKernel( GPUCommandQueue, GPUWatershedSegmentation[3], 2, NULL, GPUGlobalWorkSize, GPULocalWorkSize, 0, NULL, NULL);


	clEnqueueCopyBuffer( GPUCommandQueue, read_label, labels_lin, 0, 0, szBuffBytes_int, 0, NULL, NULL);	
//V etap


	GPUError = clSetKernelArg(GPUWatershedSegmentation[4], 0, sizeof(cl_mem), (void*)&labels);
	GPUError |= clSetKernelArg(GPUWatershedSegmentation[4], 1, sizeof(cl_mem), (void*)&labels_lin);
	GPUError |= clSetKernelArg(GPUWatershedSegmentation[4], 2, sizeof(cl_mem), (void*)&flag);
	GPUError |= clSetKernelArg(GPUWatershedSegmentation[4], 3, sizeof(cl_mem), (void*)&read_label);
    GPUError |= clSetKernelArg(GPUWatershedSegmentation[4], 4, sizeof(cl_uint), (void*)&GPUTransfer->ImageWidth);
    GPUError |= clSetKernelArg(GPUWatershedSegmentation[4], 5, sizeof(cl_uint), (void*)&GPUTransfer->ImageHeight);
    if(GPUError) return false;

	clEnqueueNDRangeKernel( GPUCommandQueue, GPUWatershedSegmentation[4], 2, NULL, GPUGlobalWorkSize, GPULocalWorkSize, 0, NULL, NULL);
	
	GPUError = clEnqueueReadBuffer(GPUCommandQueue, flag, CL_TRUE, 0,sizeof(int), (void*)&flaga_stop, 0, NULL, NULL);
	if(GPUError) return false;
	
}while(flaga_stop);

//VI etap

	clEnqueueCopyBuffer( GPUCommandQueue, read_label, labels_lin, 0, 0, szBuffBytes_int, 0, NULL, NULL);

	GPUError = clSetKernelArg(GPUWatershedSegmentation[5], 0, sizeof(cl_mem), (void*)&labels);
	GPUError |= clSetKernelArg(GPUWatershedSegmentation[5], 1, sizeof(cl_mem), (void*)&labels_lin);
	GPUError |= clSetKernelArg(GPUWatershedSegmentation[5], 2, sizeof(cl_mem), (void*)&GPUTransfer->inDevBuf);
	GPUError |= clSetKernelArg(GPUWatershedSegmentation[5], 3, sizeof(cl_mem), (void*)&read_label);
    GPUError |= clSetKernelArg(GPUWatershedSegmentation[5], 4, sizeof(cl_uint), (void*)&GPUTransfer->ImageWidth);
    GPUError |= clSetKernelArg(GPUWatershedSegmentation[5], 5, sizeof(cl_uint), (void*)&GPUTransfer->ImageHeight);
    if(GPUError) return false;

	clEnqueueNDRangeKernel( GPUCommandQueue, GPUWatershedSegmentation[5], 2, NULL, GPUGlobalWorkSize, GPULocalWorkSize, 0, NULL, NULL);

//VII etap

	clEnqueueCopyBuffer( GPUCommandQueue, read_label, labels_lin, 0, 0, szBuffBytes_int, 0, NULL, NULL);

	GPUError = clSetKernelArg(GPUWatershedSegmentation[6], 0, sizeof(cl_mem), (void*)&labels_lin);
	GPUError |= clSetKernelArg(GPUWatershedSegmentation[6], 1, sizeof(cl_mem), (void*)&GPUTransfer->inDevBuf);
	GPUError |= clSetKernelArg(GPUWatershedSegmentation[6], 2, sizeof(cl_mem), (void*)&read_label);
    GPUError |= clSetKernelArg(GPUWatershedSegmentation[6], 3, sizeof(cl_uint), (void*)&GPUTransfer->ImageWidth);
    GPUError |= clSetKernelArg(GPUWatershedSegmentation[6], 4, sizeof(cl_uint), (void*)&GPUTransfer->ImageHeight);
    if(GPUError) return false;

	clEnqueueNDRangeKernel( GPUCommandQueue, GPUWatershedSegmentation[6], 2, NULL, GPUGlobalWorkSize, GPULocalWorkSize, 0, NULL, NULL);

//VIII etap

	clEnqueueCopyBuffer( GPUCommandQueue, read_label, labels_lin, 0, 0, szBuffBytes_int, 0, NULL, NULL);

	GPUError = clSetKernelArg(GPUWatershedSegmentation[7], 0, sizeof(cl_mem), (void*)&labels_lin);
	GPUError |= clSetKernelArg(GPUWatershedSegmentation[7], 1, sizeof(cl_mem), (void*)&read_label);
    GPUError |= clSetKernelArg(GPUWatershedSegmentation[7], 2, sizeof(cl_uint), (void*)&GPUTransfer->ImageWidth);
    GPUError |= clSetKernelArg(GPUWatershedSegmentation[7], 3, sizeof(cl_uint), (void*)&GPUTransfer->ImageHeight);
    if(GPUError) return false;

	clEnqueueNDRangeKernel( GPUCommandQueue, GPUWatershedSegmentation[7], 2, NULL, GPUGlobalWorkSize, GPULocalWorkSize, 0, NULL, NULL);


	clEnqueueCopyBuffer( GPUCommandQueue, read_label, labels_lin, 0, 0, szBuffBytes_int, 0, NULL, NULL);

	GPUError = clSetKernelArg(GPUWatershedSegmentation[8], 0, sizeof(cl_mem), (void*)&labels_lin);
	GPUError |= clSetKernelArg(GPUWatershedSegmentation[8], 1, sizeof(cl_mem), (void*)&read_label);
	GPUError |= clSetKernelArg(GPUWatershedSegmentation[8], 2, sizeof(cl_mem), (void*)&labels_border);
    GPUError |= clSetKernelArg(GPUWatershedSegmentation[8], 3, sizeof(cl_uint), (void*)&GPUTransfer->ImageWidth);
    GPUError |= clSetKernelArg(GPUWatershedSegmentation[8], 4, sizeof(cl_uint), (void*)&GPUTransfer->ImageHeight);
	GPUError |= clSetKernelArg(GPUWatershedSegmentation[8], 5, sizeof(cl_mem), (void*)&flag);
    if(GPUError) return false;

	clEnqueueNDRangeKernel( GPUCommandQueue, GPUWatershedSegmentation[8], 2, NULL, GPUGlobalWorkSize, GPULocalWorkSize, 0, NULL, NULL);


	clEnqueueCopyBuffer( GPUCommandQueue, read_label, labels, 0, 0, szBuffBytes_int, 0, NULL, NULL);

	GPUError = clSetKernelArg(GPUWatershedSegmentation[9], 0, sizeof(cl_mem), (void*)&labels);
	GPUError |= clSetKernelArg(GPUWatershedSegmentation[9], 1, sizeof(cl_mem), (void*)&labels_lin);
	GPUError |= clSetKernelArg(GPUWatershedSegmentation[9], 2, sizeof(cl_mem), (void*)&read_label);
	GPUError |= clSetKernelArg(GPUWatershedSegmentation[9], 3, sizeof(cl_mem), (void*)&labels_border);
    GPUError |= clSetKernelArg(GPUWatershedSegmentation[9], 4, sizeof(cl_uint), (void*)&GPUTransfer->ImageWidth);
    GPUError |= clSetKernelArg(GPUWatershedSegmentation[9], 5, sizeof(cl_uint), (void*)&GPUTransfer->ImageHeight);
	GPUError |= clSetKernelArg(GPUWatershedSegmentation[9], 6, sizeof(cl_mem), (void*)&flag);
    if(GPUError) return false;

	clEnqueueNDRangeKernel( GPUCommandQueue, GPUWatershedSegmentation[9], 2, NULL, GPUGlobalWorkSize, GPULocalWorkSize, 0, NULL, NULL);

	//GPUError = clEnqueueReadBuffer(GPUCommandQueue, flag, CL_TRUE, 0,sizeof(int), (void*)&flaga_stop, 0, NULL, NULL);

	//printf("uwaga: %d",flaga_stop);

	clEnqueueCopyBuffer( GPUCommandQueue, read_label, GPUTransfer->outDevBuf, 0, 0, szBuffBytes_int, 0, NULL, NULL);
	clEnqueueCopyBuffer( GPUCommandQueue, labels_border, GPUTransfer->outDevBuf_1, 0, 0, szBuffBytes_int, 0, NULL, NULL);
	clEnqueueCopyBuffer( GPUCommandQueue, labels, GPUTransfer->outDevBuf_2, 0, 0, szBuffBytes_int, 0, NULL, NULL);

return true;
}
