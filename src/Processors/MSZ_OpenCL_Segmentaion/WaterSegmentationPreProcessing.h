#ifndef _WATER_SEG_PRE_PROCESSING
#define _WATER_SEG_PRE_PROCESSING

#include <highgui.h>
#include <cv.h>
#include <iostream>

#include "Types/Blobs/BlobContour.hpp"
#include "Types/Blobs/Blob.hpp"

namespace tb = Types::Blobs;

using namespace std;

class WaterSegmentationPreProcessing{
	private:
		int width;
		int height;
	public:
		WaterSegmentationPreProcessing();
		~WaterSegmentationPreProcessing();
		IplImage * Gradient(IplImage * img, IplImage * out);
		IplImage * Hmin(IplImage * img, int val, IplImage * out); //gray
		IplImage * Mcero(IplImage * img1, int n, IplImage *out);
		void FindListHead(IplImage * img, vector<int> &empty_vector);
		void ConvertToBlobs(IplImage * img, uchar * obraz, vector<int> findMinima, tb::Blob_vector &blobs );
};


#endif
