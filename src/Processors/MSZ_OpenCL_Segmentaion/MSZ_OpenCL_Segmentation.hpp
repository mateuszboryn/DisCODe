/*!
 * \file
 * \brief
 */

#ifndef OPENCL_SEG_HPP_
#define OPENCL_SEG_HPP_

#include <oclUtils.h>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#include "Component_Aux.hpp"
#include "Component.hpp"
#include "Panel_Empty.hpp"
#include "DataStream.hpp"
#include "Props.hpp"
#include "timer.h"

#include "GPUTransferManager.h"
#include "GPUImageProcessor.h"
#include "WatershedSegmentation.h"
#include "WaterSegmentationPreProcessing.h"

#include "Types/Blobs/BlobContour.hpp"
#include "Types/Blobs/Blob.hpp"
#include "Types/Blobs/BlobResult.hpp"

#include <vector>

namespace tb = Types::Blobs;

#include <fstream> 

#include <cv.h>
#include <highgui.h>

namespace Processors {
namespace OpenCL_Segmentation{

using namespace cv;


struct Props: public Base::Props
{

	/*!
	* Image max width.
	*/
	int maxWidth;

	/*!
	* Image max height.
	*/
	int maxHeight;

	/*!
	* Image channels.
	*/
	int channels;

void load(const ptree & pt)
{
	maxWidth = pt.get("iniWidth", 1);
	maxHeight = pt.get("iniHeight", 1);
	channels = pt.get("channels", 1);
}


void save(ptree & pt)
{
}

Props() {
}

virtual ~Props() {
}

};

class OpenCL_Segmentation: public Base::Component
{
public:
	/*!
	 * Constructor.
	 */
	OpenCL_Segmentation(const std::string & name = "");

	/*!
	 * Destructor
	 */

	Base::Props * getProperties()
	{
		return &props;
	}

	virtual ~OpenCL_Segmentation();


protected:

	/*!
	 * Connects source to given device.
	 */
	bool onInit();

	/*!
	 * Disconnect source from device, closes streams, etc.
	 */
	bool onFinish();

	/*!
	 * Retrieves data from device.
	 */
	bool onStep();

	/*!
	 * Start component
	 */
	bool onStart();

	/*!
	 * Stop component
	 */
	bool onStop();


	/*!
	 * Event handler function.
	 */
	void onNewImage();

	/// Event handler.
	/// New image is waiting
	Base::EventHandler <OpenCL_Segmentation> h_onNewImage;

	/// Input data stream
	Base::DataStreamIn<cv::Mat> in_img;

	Base::Event * newImage;

	/// Output data stream - image with drawn blobs
	Base::DataStreamOut<cv::Mat> out_img;
	Base::DataStreamOut<cv::Mat> out_img1;
	Mat img;

	WaterSegmentationPreProcessing * myWG;
	GPUImageProcessor* GPU;
	WatershedSegmentation * WS;	
	
	vector<int> findMinima;

	/// Event raised, when set of blobs is extracted
	Base::Event * newBlobs;

	/// Output data stream - list of detected blobs
	Base::DataStreamOut<Types::Blobs::BlobResult> out_blobs;

	Props props;

};

}//: namespace OpenCL_Segmentation_
}//: namespace Processors


/*
 * Register processor component.
 */
REGISTER_PROCESSOR_COMPONENT("OpenCL_Segmentation", Processors::OpenCL_Segmentation::OpenCL_Segmentation, Common::Panel_Empty)

#endif /* OpenCL_Segmentation_HPP_ */

