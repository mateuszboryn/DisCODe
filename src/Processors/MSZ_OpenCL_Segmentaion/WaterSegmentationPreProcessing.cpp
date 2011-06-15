#include "WaterSegmentationPreProcessing.h"
#include <iostream>
#include <sstream>
#include <fstream> 

using namespace std;

WaterSegmentationPreProcessing::WaterSegmentationPreProcessing(){	 
}

WaterSegmentationPreProcessing::~WaterSegmentationPreProcessing(){
}


IplImage * WaterSegmentationPreProcessing::Gradient(IplImage * input, IplImage * output){
	IplImage * temp = cvCloneImage(input);
	cvSmooth(input,temp,CV_MEDIAN, 3, 0, 0, 0);
	cvSmooth(temp,input,CV_GAUSSIAN,3);
	cvReleaseImage(&temp);

	IplImage *img = cvCreateImage( cvSize( input->width, input->height ), IPL_DEPTH_8U, 1 );
	IplImage *tmp = cvCreateImage( cvSize( input->width, input->height ), IPL_DEPTH_8U, 1 );
	//IplImage *tmp2 = cvCreateImage( cvSize( input->width, input->height ), IPL_DEPTH_8U, 3 );
	cvCvtColor( input, img, CV_BGR2GRAY );
	/*cvCvtColor(input,tmp2,CV_BGR2HSV);
	uchar* data1    = (uchar *)tmp2->imageData;
	uchar* data2    = (uchar *)img->imageData;
	for(int i=0; i < input->height; i++)
		for(int j=0; j < input->width; j++)
			data2[i*(input->width)+j] = data1[(i*(input->width)+j)*3];
*/
	cvMorphologyEx(img, output, tmp, 0, CV_MOP_GRADIENT , 1);
	cvReleaseImage(&tmp);
	cvReleaseImage(&img);
	//cvReleaseImage(&tmp2);
	return output;
}



IplImage * WaterSegmentationPreProcessing::Mcero(IplImage * img1, int n, IplImage * unImg){

	IplImage * eroImg = cvCreateImage( cvSize( width, height ), IPL_DEPTH_8U, 1 );

	for(int i=0; i< n; i++){
		cvErode(unImg, eroImg, 0, 1);
		cvMax(eroImg,img1,unImg);
	}
	cvReleaseImage(&eroImg);

	return unImg;	
}

IplImage * WaterSegmentationPreProcessing::Hmin(IplImage * img, int val, IplImage * output){
	height = img->height;
	width = img->widthStep;
	
	IplImage * outImg = cvCreateImage( cvSize( width, height ), IPL_DEPTH_8U, 1 );
	IplImage * outTmp = cvCreateImage( cvSize( width, height ), IPL_DEPTH_8U, 1 );


	cvMinS( img, 255 - val, outImg);
	cvAddS( outImg, cvScalar(val), output, 0);
	
	IplImage * out1 = Mcero(img,val + 10,output);

	cvReleaseImage(&outImg);
	cvReleaseImage(&outTmp);

return out1;
}

void WaterSegmentationPreProcessing::FindListHead(IplImage * img, vector<int> &empty_vector){
	int * phase = (int*)img->imageData;
	empty_vector.clear();
	for(int i=0; i<img->height ; i++)
		for(int j=0; j<img->width; j++){
			if(phase[(i*(img->width) + j)] > -1){
					empty_vector.push_back(phase[i*(img->width) + j]);
			}
		}

		

}

void WaterSegmentationPreProcessing::ConvertToBlobs(IplImage * img, uchar * obraz, vector<int> findMinima, tb::Blob_vector &blobs )
{

	vector<int> chain_code(2*(img->width+1) + 1);

	chain_code[0] = 7;
	chain_code[1] = 6;
	chain_code[2] = 5;
	chain_code[img->width] = 0;
	chain_code[img->width+2] = 4;
	chain_code[2*(img->width+1) - 2] = 1;
	chain_code[2*(img->width+1) - 1] = 2;
	chain_code[2*(img->width+1)] = 3;


	int temp_chain_code;
	tb::Blob *currentBlob;
	CvSize imageSizes;
	CvPoint currentPoint;
	imageSizes = cvSize(img->width,img->height);
	int currentLabel;
	int * data = (int*)img->imageData;
	int counter = findMinima.size();	
	int old_position;

	while(counter >0){
		int position = findMinima[counter - 1];
		int my_const_position = position;
		currentPoint = cvPoint(position%(img->width),(position/img->width));
		currentLabel = position;
		currentBlob = new tb::Blob(currentLabel, currentPoint, imageSizes );
		tb::BlobContour *currBC = currentBlob->GetExternalContour();

		while(true){
			obraz[position] = 255;
			old_position = position;
			position = data[position];	
			data[old_position] = -1;
			if(position == -1)
				break;
			temp_chain_code = old_position + img->width + 1 - position;
			currBC->AddChainCode(chain_code[temp_chain_code]);

			if(position == my_const_position)
				break;
		}

		blobs.push_back(currentBlob);
		counter--;
	}

}

