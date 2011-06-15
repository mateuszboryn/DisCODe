
void GetData(__global uchar* dataIn, __local uchar* dataOut, int iDevGMEMOffset, int iLocalPixOffset, int nChannels)
{
	dataOut[iLocalPixOffset*nChannels] = dataIn[iDevGMEMOffset*nChannels];
	dataOut[iLocalPixOffset*nChannels+1] = dataIn[iDevGMEMOffset*nChannels+1];
	dataOut[iLocalPixOffset*nChannels+2] = dataIn[iDevGMEMOffset*nChannels+2];
	if( nChannels == 4 )
	{
		dataOut[iLocalPixOffset*nChannels+3] = dataIn[iDevGMEMOffset*nChannels+3];
	}
}

void SetZERO(__local uchar* dataOut, int iLocalPixOffset, int nChannels)
{
	dataOut[iLocalPixOffset*nChannels] = (char)0;
	dataOut[iLocalPixOffset*nChannels+1] = (char)0;
	dataOut[iLocalPixOffset*nChannels+2] = (char)0;
	if( nChannels == 4 )
	{
		dataOut[iLocalPixOffset*nChannels+3] = (char)0;
	}
}


uchar4 GetDataFromLocalMemory( __local uchar* data,  int iLocalPixOffset , int nChannels)
{
	uchar4 pix;
	pix.x = data[iLocalPixOffset*nChannels];
	pix.y = data[iLocalPixOffset*nChannels+1];
	pix.z = data[iLocalPixOffset*nChannels+2];
	return pix;
}

void setData(__global char* data, char x , char y, char z, int iDevGMEMOffset , int nChannels)
{
	data[iDevGMEMOffset*nChannels] = x;
	data[iDevGMEMOffset*nChannels+1] = y;
	data[iDevGMEMOffset*nChannels+2] = z;
}

uchar4 GetDataFromGlobalMemory( __global uchar* data,  int iDevGMEMOffset , int nChannels)
{
	uchar4 pix;
	pix.x = data[iDevGMEMOffset*nChannels];
	pix.y = data[iDevGMEMOffset*nChannels+1];
	pix.z = data[iDevGMEMOffset*nChannels+2];
	return pix;
}



