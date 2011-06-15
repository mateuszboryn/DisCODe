/*!
 * \file GPUTransferManager.h
 * \brief File contains class responsible for managing transfer to GPU.
 *
 * \author Mateusz Pruchniak
 * \date 2010-05-05
 */

#pragma once

#include <oclUtils.h>
#include <iostream>
#include <vector>
#include <string>
#include "cv.h"
#include "cxmisc.h"
#include "highgui.h"
#include <algorithm>
#include <stdio.h>
#include <ctype.h>
#include <time.h>

using namespace std;

/*!
 * \class GPUTransferManager
 * \brief Class responsible for managing transfer between GPU and CPU.
 * \author Mateusz Pruchniak
 * \date 2010-05-05
 */
class GPUTransferManager
{
private:        

		/*!
		 * Mapped Pointer to pinned Host input and output buffer for host processing.
		 */
        cl_uint* GPUInputOutput;                 

		/*!
		 * OpenCL host memory input output buffer object:  pinned.
		 */
        cl_mem cmPinnedBuf;               

		/*!
		 * The size in bytes of the buffer memory object to be allocated.
		 */
        size_t szBuffBytes_char;

	size_t szBuffBytes_int;

		/*!
		 * Error code, only 0 is allowed.
		 */
        cl_int GPUError;

		/*!
		 * Image return from buffer.
		 */
        IplImage* image;                   
		

    public:

		/*!
		 * OpenCL command-queue, is an object where OpenCL commands are enqueued to be executed by the device.
		*/
        cl_command_queue GPUCommandQueue; 

		/*!
		 * Context defines the entire OpenCL environment, including OpenCL kernels, devices, memory management, command-queues, etc. Contexts in OpenCL are referenced by an cl_context object
		 */
        cl_context GPUContext;    

		/*!
		 * OpenCL device memory input/output buffer object.
		 */
        cl_mem inDevBuf;                 

		/*!
		 * OpenCL device memory input/output buffer object.
		 */
        cl_mem outDevBuf;
		cl_mem outDevBuf_1;
		cl_mem outDevBuf_2;

		/*!
		 * Image width.
		 */
        unsigned int ImageWidth;   

		/*!
		 * Image height.
		 */
        unsigned int ImageHeight;  

		/*!
		 * Number of color channels.
		 */
		int nChannels;

		/*!
		 * Destructor. Release buffers.
		 */
        ~GPUTransferManager();

        /*!
		 * Constructor. Allocate pinned and mapped memory for input and output host image buffers.
		 */
        GPUTransferManager( cl_context , cl_command_queue , unsigned int , unsigned int,  int nChannels );

		/*!
		 * Default Constructor.
		 */
        GPUTransferManager();

        /*!
		 * Send image to GPU memory.
		 */
        void SendImage( IplImage*  );

        /*!
		 * Get image from GPU memory.
		 */
        void ReceiveImage(IplImage *, IplImage *, IplImage *);
        
        
		/*!
		 * Release all buffors.
		 */
        void Cleanup();

        /*!
		 * Check error code.
		 */
        void CheckError(int );

};

