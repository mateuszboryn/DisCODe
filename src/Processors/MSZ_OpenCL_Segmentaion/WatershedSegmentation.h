/*!
 * \file WatershedSegmentation.h
 * \brief Abstract class for all WatershedSegmentations.
 *
 * \author Mateusz Pruchniak
 * \date 2010-05-05
 */

#pragma once

#include <oclUtils.h>
#include <iostream>
#include "cv.h"
#include "highgui.h"
#include <vector>
#include <string>
#include <algorithm>
#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include "GPUTransferManager.h"

using namespace std;

/*!
 * \class WatershedSegmentation
 * \brief Abstract class for all WatershedSegmentations.
 * \author Mateusz Pruchniak
 * \date 2010-05-05
 */
class WatershedSegmentation
{
	protected:

		/*!
		 * Work-group size - dim X.
		 */
        int iBlockDimX;                    

		/*!
		 * Work-group size - dim Y.
		 */
        int iBlockDimY;                    

		/*!
		 * Error code, only 0 is allowed.
		 */
        cl_int GPUError;

		/*!
		 * Pointer to instance of class GPUTransferManager.
		 */
        GPUTransferManager* GPUTransfer;

		/*!
		 * Loaded .cl file, which contain code responsible for image processing.
		 */
        char* SourceOpenCLWatershedSegmentation;                 

		/*!
		 * Loaded .cl file, which contain support function.
		 */
	char* SourceOpenCL;              

		/*!
		 * Program is formed by a set of kernels, functions and declarations, and it's represented by an cl_program object.
		 */
        cl_program GPUProgram;              

		/*!
		 * Kernels are essentially functions that we can call from the host and that will run on the device
		 */
        vector<cl_kernel> GPUWatershedSegmentation; 

		/*!
		 * Global size of NDRange.
		 */
        size_t GPUGlobalWorkSize[2];     

		size_t szBuffBytes_char;
	    size_t szBuffBytes_int;

    public:

		/*
			buffers
		*/
		cl_mem flag;
		cl_mem read_label;
		cl_mem labels;
		cl_mem labels_lin;
		cl_mem labels_border;

		/*!
		* Default constructor. Nothing doing.
		*/
		WatershedSegmentation(void);

		/*!
		 * Constructor, creates a program object for a context, loads the source code (.cl files) and build the program.
		 */
        WatershedSegmentation(char* , cl_context GPUContext  ,GPUTransferManager* );

		/*!
		 * Destructor.
		 */
        ~WatershedSegmentation();

		void CheckError(int);

		bool Segmentation(cl_command_queue GPUCommandQueue);
        
};

