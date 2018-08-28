#include <iostream>

#include "Gui/CvGui.hpp"

using namespace CvGui;

volatile bool isPlaying = false;
std::shared_ptr<PushButton> button1;
std::shared_ptr<PushButton> button2;

// ---- Callbacks -----
void switchFlow(void*, void*) {
	isPlaying = !isPlaying;
	
	if(button1)
		button1->setText(isPlaying ? "Stop":"Play");
}
void replay(void* ptrRawCap, void*) {
	isPlaying = true;
	
	if(button1)
		button1->setText("Stop");
	
	cv::VideoCapture* pCap = reinterpret_cast<cv::VideoCapture*>(ptrRawCap);
	if(pCap != nullptr)
		pCap->set(cv::CAP_PROP_POS_AVI_RATIO, 0);
}

// ------------------  M a i n ---------------------
/*	use [program.exe] videoPath */

int main(int argc, char* argv[]) {
	// Constantes
	const double FPS 		= 30.0; // Hz
	const int SAMPLE_TIME 	= (int)(1000/FPS); // Milliseconds
	
	// Video path
	std::string videoPath = "";
	if(argc != 2) {
		std::cout << "Bad usage" << std::endl;
		return -1;
	}
	else 
		videoPath = std::string(argv[1]);
	
	cv::VideoCapture cap(videoPath);
	if(!cap.isOpened())	{
		std::cout << "Video doesn't exist !" << std::endl;
		return -1;
	}
	
	cv::Mat frame = cv::Mat::zeros(
		static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT)), 
		static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH)), 
		CV_8UC3
	);
	
	// Gui
	Gui<AddPolicy::Row> gui;
	auto screen	= std::make_shared<Displayable>("Frame", frame);
	button1 = std::make_shared<PushButton>("Stop", cv::Size(50, 30));
	button2 = std::make_shared<PushButton>("Replay", cv::Size(50, 30));
	
	gui.add(screen);
	gui.add(button1, button2);
	
	// Event
	button1->listen(PushButton::onClick, switchFlow, nullptr);
	button2->listen(PushButton::onClick, replay, &cap);
	
	// Display
	isPlaying = true;
	gui.show();
	do {
		if(isPlaying) {
			cap >> frame;
			if(!frame.empty()) {
				screen->setFrame(frame);
			}
		}
	} while(gui.wait(SAMPLE_TIME) != gui.KEY_ESCAPE && !gui.wasClosed());
	
	cap.release();
	return 0;
}