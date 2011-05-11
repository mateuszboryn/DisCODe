/*!
 * \file KW_Palm_LUT.cpp
 * \brief
 * \author kwasak
 * \date 2010-11-05
 */

#include <memory>
#include <string>

#include <iostream>

#include <cv.h>
#include <highgui.h>

#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "MSZ_OpenCL_Segmentation.hpp"
#include "Logger.hpp"

#include "../BlobExtractor/ComponentLabeling.hpp"
#include "Types/Blobs/BlobOperators.hpp"

namespace Processors {
namespace OpenCL_Segmentation{


OpenCL_Segmentation::OpenCL_Segmentation(const std::string & name) : Base::Component(name)
{
	LOG(LTRACE) << "Hello OpenCL_Segmentation\n";
}

OpenCL_Segmentation::~OpenCL_Segmentation()
{
	LOG(LTRACE) << "Good bye OpenCL_Segmentation\n";
}

bool OpenCL_Segmentation::onInit()
{

	myWG = new WaterSegmentationPreProcessing();
	GPU = new GPUImageProcessor(props.maxWidth,props.maxHeight,props.channels);
	WS = new WatershedSegmentation("OpenClSegmentation.cl",GPU->GPUContext,GPU->Transfer);
	GPU->AddProcessing(WS);

	

	h_onNewImage.setup(this, &OpenCL_Segmentation::onNewImage);
	registerHandler("onNewImage", &h_onNewImage);

	registerStream("in_img", &in_img);
	registerStream("out_img", &out_img);
	registerStream("out_img1", &out_img1);
	registerStream("out_blobs", &out_blobs);

	newImage = registerEvent("newImage");
	newBlobs = registerEvent("newBlobs");

	return true;
}

bool OpenCL_Segmentation::onFinish()
{
	LOG(LTRACE) << "OpenCL_Segmentation::finish\n";

 	//delete WS;
	delete GPU;
	delete myWG;
	return true;
}

bool OpenCL_Segmentation::onStep()
{
	LOG(LTRACE) << "OpenCL_Segmentation::step\n";
	return true;
}

bool OpenCL_Segmentation::onStop()
{
	return true;
}

bool OpenCL_Segmentation::onStart()
{
	return true;
}

void OpenCL_Segmentation::onNewImage()
{
	LOG(LTRACE) << "OpenCL_Segmentation::onNewImage\n";

try {
	img = in_img.read().clone();

Timer t;

	IplImage* out_gradient = cvCreateImage( img.size(), IPL_DEPTH_8U, 1 );
	IplImage* out_hmin = cvCreateImage( img.size(), IPL_DEPTH_8U, 1 );
	IplImage* temp = cvCreateImage(img.size(), IPL_DEPTH_8U, 1 );
	
	IplImage* test = cvCreateImage( img.size(), IPL_DEPTH_32S, 1 ); // to wazne ze obraz ma 2 kanaly
	IplImage* bords = cvCreateImage( img.size(), IPL_DEPTH_32S, 1 );
	IplImage* org_bords = cvCreateImage( img.size(), IPL_DEPTH_32S, 1 );

	IplImage * gradient = myWG->Gradient(&(IplImage(img)), out_gradient);
	IplImage * newImg =  myWG->Hmin(gradient, 10, out_hmin);
	
	
		
	GPU->Transfer->SendImage(newImg);
t.Start();
	GPU->Process();
t.Stop();
	
	GPU->Transfer->ReceiveImage(test,bords,org_bords);
				
	myWG->FindListHead(bords,findMinima);
	
	uchar* temp_data = (uchar*)temp->imageData;
//OBLICZANIE CZASU
cout<<"czas:"<<t.GetTime()<<endl;
	

	tb::Blob_vector res;
	myWG->ConvertToBlobs(test, temp_data, findMinima, res );

	tb::BlobResult result(res);
	tb::Blob * currentBlob;

	result.Filter( result, B_EXCLUDE, tb::BlobGetArea(), B_LESS, 10);


	out_img1.write(img);
	out_blobs.write(result);

	
newBlobs->raise();
newImage->raise();

	cvReleaseImage(&temp);
	cvReleaseImage(&out_gradient);
	cvReleaseImage(&out_hmin);
	cvReleaseImage(&test);
	cvReleaseImage(&bords);
	cvReleaseImage(&org_bords);



	}
	catch (...) {
		LOG(LERROR) << "Cos sie walło.";
	}
}


}//: namespace OpenCL_Segmentation_
}//: namespace Processors
