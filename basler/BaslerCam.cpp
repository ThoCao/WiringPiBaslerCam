#include "BaslerCam.h"


BaslerCam::BaslerCam()
{
	
}

bool BaslerCam::OpenCam(){
	Pylon::PylonInitialize();
	fc.OutputPixelFormat = Pylon::PixelType_Mono8;

	try {
		camera.Attach(Pylon::CTlFactory::GetInstance().CreateFirstDevice());
		std::cout << "Using Device: " << camera.GetDeviceInfo().GetModelName() << std::endl;
		std::cout << "Serial Number: " << camera.GetDeviceInfo().GetSerialNumber() << std::endl;
		// The parameter MaxNumBuffer can be used to control the count of buffers
		// allocated for grabbing. The default value of this parameter is 10.
		//  the number of queued buffers and the number of buffers that are ready for retrieval.

		//camera.RegisterConfiguration(new Pylon::CAcquireContinuousConfiguration, Pylon::RegistrationMode_ReplaceAll, Pylon::Cleanup_Delete);

		camera.Open();
		camera.MaxNumBuffer = 5;

		//// trigger in mode:  Acquisition OFF || Fram ON : software trigger
		//// Set the acquisition mode to continuous frame
		//camera.AcquisitionMode.SetValue(Basler_GigECameraParams::AcquisitionMode_Continuous);
		//// Select the acquisition start trigger
		//camera.TriggerSelector.SetValue(Basler_GigECameraParams::TriggerSelector_AcquisitionStart);
		camera.TriggerSelector.SetValue(Basler_UsbCameraParams::TriggerSelector_FrameStart);
		//// Set the mode for the selected trigger
		//camera.TriggerMode.SetValue(Basler_GigECameraParams::TriggerMode_Off);
		camera.TriggerMode.SetValue(Basler_UsbCameraParams::TriggerMode_On);
		// Set the trigger source for the Frame Start trigger to Software
		//camera.TriggerSource.SetValue(Basler_UsbCameraParams::TriggerSource_Software);
		// Set the trigger source for the Frame Start trigger to Software
		camera.TriggerSource.SetValue(Basler_UsbCameraParams::TriggerSource_Line1);
		// Set the trigger source to Line 1
		//camera.TriggerSource.SetValue(Basler_UsbCameraParams::TriggerSource_Line1);

		//// Disable the acquisition frame rate parameter (this will disable the cameras
		//// internal frame rate control and allow you to control the frame rate with
		//// software frame start trigger signals)
		//camera.AcquisitionFrameRateEnable.SetValue(false);
		//// Select the frame start trigger
		//camera.TriggerSelector.SetValue(Basler_GigECameraParams::TriggerSelector_FrameStart);
		//// Set the mode for the selected trigger
		//camera.TriggerMode.SetValue(Basler_GigECameraParams::TriggerMode_On);
		//// Set the source for the selected trigger
		//camera.TriggerSource.SetValue(Basler_GigECameraParams::TriggerSource_Software);
		//// Set for the timed exposure mode
		//camera.ExposureMode.SetValue(Basler_GigECameraParams::ExposureMode_Timed);
		//camera.ExposureTimeRaw.SetValue(100);
		//// Set the exposure time
		//camera.ExposureTimeAbs.SetValue(10000.0);
		// Execute an acquisition start command to prepare for frame acquisition
		//camera.AcquisitionStart.Execute();
		connect_status = 1;

		//camera.TestImageSelector = Basler_GigECameraParams::TestImageSelector_Off;
		//camera.PixelFormat.SetValue(Basler_GigECameraParams::PixelFormat_YUV422Packed);
		//camera.BinningHorizontalMode(Basler_GigECameraParams::BinningHorizontalMode_Average);

		//// set image size
		//if (IsWritable(camera.OffsetX))
		//{
		//	camera.OffsetX.SetValue(camera.OffsetX.GetMin());
		//}
		//if (IsWritable(camera.OffsetY))
		//{
		//	camera.OffsetY.SetValue(camera.OffsetY.GetMin());
		//}
		//camera.Width.SetValue(800);
		//camera.Height.SetValue(600);


		//camera.StartGrabbing();
	}
	catch (const  GenericException &e) {
		std::cout << "An exception occurred. " << e.GetDescription() << std::endl;
		connect_status = 0;
	}

	if (connect_status){
	   camera.Open();

	   camera.StartGrabbing();

	   connect_status = 1;
	   return 1;
	}
	else return 0;
}

bool BaslerCam::CloseCam(){
	if (connect_status){
		if (grabing_status) camera.StopGrabbing();
		camera.Close();

		grabing_status = 0;
		return 1;
	}
	else return 0;
}
void BaslerCam::Grabing(){

		//camera.TriggerSoftware.Execute();
			
		if (camera.IsGrabbing()){
			grabing_status = 1;
			// wait for an image and then retrieve it
			bool found_status = camera.RetrieveResult(5000, grab_result_ptr, Pylon::TimeoutHandling_ThrowException);

			if (!found_status) return;
			// if Image grabbed successfully?
			if (grab_result_ptr->GrabSucceeded()){
				//fc.Convert(image_py, grab_result_ptr);
				cv::Mat cv_img = cv::Mat(grab_result_ptr->GetHeight(), grab_result_ptr->GetWidth(), CV_8UC3);//, (uint8_t*)image_py.GetBuffer());
				//cv::Mat cv_img2 = cv::Mat(grab_result_ptr->GetHeight(), grab_result_ptr->GetWidth(), CV_8UC3, (uint8_t*)image_py.GetBuffer());
				const uint8_t *pImageBuffer = (uint8_t*)grab_result_ptr->GetBuffer();
				int buffer_num = 0;

				for (int i = 0; i != cv_img.rows; i++){
					uchar *Mi = cv_img.ptr<uchar>(i);
					for (int j = 0; j != cv_img.cols * 3; j += 6){

						uint8_t  U = (uint8_t)pImageBuffer[buffer_num];
						uint8_t  Y1 = (uint8_t)pImageBuffer[buffer_num + 1];
						uint8_t  V = (uint8_t)pImageBuffer[buffer_num + 2];
						uint8_t Y2 = (uint8_t)pImageBuffer[buffer_num + 3];

						int B1 = 1.164*(Y1 - 16) + 2.018*(U - 128);
						if (B1 > 255) Mi[j] = 255;
						else if (B1 <= 0) Mi[j] = 255 + B1;
						else Mi[j] = B1;

						int G1 = 1.164*(Y1 - 16) - 0.813*(V - 128) - 0.391*(U - 128);
						if (G1 > 255) Mi[j + 1] = 255;
						else if (G1 <= 0) Mi[j] = 255 + G1;
						else Mi[j + 1] = G1;

						int R1 = 1.164*(Y1 - 16) + 1.596*(V - 128);
						if (R1 > 255) Mi[j + 2] = 255;
						else if (R1 <= 0) Mi[j] = 255 + R1;
						else Mi[j + 2] = R1;

						int B2 = 1.164*(Y2 - 16) + 2.018*(U - 128);
						if (B2 > 255) Mi[j+3] = 255;
						else if (B2 <= 0) Mi[j+3] = 255 + B2;
						else Mi[j + 3] = B2;

						int G2 = 1.164*(Y2 - 16) - 0.813*(V - 128) - 0.391*(U - 128);
						if (G2 > 255) Mi[j + 4] = 255;
						else if (G2 <= 0) Mi[j+4] = 255 + G2;
						else Mi[j + 4] = G2;

						int R2 = 1.164*(Y2 - 16) + 1.596*(V - 128);
						if (R2 > 255) Mi[j + 5] = 255;
						else if (R2 <= 0) Mi[j+5] = 255 + R2;
						else Mi[j + 5] = R2;

						buffer_num += 4;
					}
				}
				std::string window = "cv_img ";// +std::to_string(number);
				if (!cv_img.empty())cv::imshow(window, cv_img);
				cv::waitKey(10);
			}
			else{
				std::cout << "Error: " << grab_result_ptr->GetErrorCode() << " " << grab_result_ptr->GetErrorDescription() << std::endl;
			}

	}

}

void BaslerCam::StopGrabing(){
	if (grabing_status){
		camera.StopGrabbing();
		// reset connection
		grabing_status = 0;
	}
}

BaslerCam::~BaslerCam()
{
	// Releases all pylon resources
	//Pylon::PylonTerminate();
	if (connect_status == 1){
		if (grabing_status) camera.StopGrabbing();
		camera.Close();
	}

}


bool BaslerCam::ReceiveImage(cv::Mat &Image){

	if (this->connect_status == 0) return false;

	bool status = 0;
	camera.TriggerSoftware.Execute();

	
	if (camera.IsGrabbing()){
		grabing_status = 1;
		// wait for an image and then retrieve it
		bool found_status = camera.RetrieveResult(5000, grab_result_ptr, Pylon::TimeoutHandling_ThrowException);

		

		if (!found_status) return 0;
		// if Image grabbed successfully?
		if (grab_result_ptr->GrabSucceeded()){
			fc.Convert(image_py, grab_result_ptr);
			
			cv::Mat grab_img = cv::Mat(grab_result_ptr->GetHeight(), grab_result_ptr->GetWidth(), CV_8UC1, (uint8_t*)image_py.GetBuffer());

			grab_img.copyTo(Image);
			status = 1;
		}
		else{
			status = 0;
		}
	}
	
	return status;

}

bool BaslerCam::ReceivedImageTrigger(cv::Mat & Image)
{
	if (this->connect_status == 0) return false;

	bool status = 0;
	

	if (camera.IsGrabbing()) {
		grabing_status = 1;
		// wait for an image and then retrieve it
		bool found_status = camera.RetrieveResult(5000, grab_result_ptr, Pylon::TimeoutHandling_ThrowException);



		if (!found_status) return 0;
		// if Image grabbed successfully?
		if (grab_result_ptr->GrabSucceeded()) {
			fc.Convert(image_py, grab_result_ptr);

			cv::Mat grab_img = cv::Mat(grab_result_ptr->GetHeight(), grab_result_ptr->GetWidth(), CV_8UC1, (uint8_t*)image_py.GetBuffer());

			grab_img.copyTo(Image);
			status = 1;
		}
		else {
			status = 0;
		}
	}

	return status;
}


