#pragma once
#include <pylon/PylonIncludes.h>
#ifdef PYLON_WIN_BUILD
# include <pylon/PylonGUI.h>
#endif
#include <iostream>

//#include <pylon\gige\BaslerGigEInstantCamera.h>
#include <pylon/usb/BaslerUsbInstantCamera.h>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

// Namespace for using pylon objects.
using namespace Pylon;


class BaslerCam
{
public:
	BaslerCam();
	~BaslerCam();
public:
	bool OpenCam();
	bool CloseCam();
	void Grabing();
	void StopGrabing();
	bool ReceiveImage(cv::Mat &Image);
        bool ReceivedImageTrigger(cv::Mat &Image);
private:
	Pylon::CBaslerUsbInstantCamera camera; // library that grap image with a few line

	Pylon::CGrabResultPtr grab_result_ptr;
	Pylon::CImageFormatConverter fc;
	Pylon::CPylonImage image_py;
public: // control action
	bool grabing_status  = 0;
	bool connect_status = 0;
	bool disconnect_status = 0;
};


