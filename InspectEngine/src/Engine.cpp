#include "Engine.h"
//#include <opencv2/opencv.hpp>

int Engine::Run(const unsigned char* gray, int w, int h, int stride)
{
	// cv::Mat 
	return 0;
}

void Engine::FillResult(InspResult* out)
{
	*out = result_;
}

const char* Engine::LastError() const
{
	return lastError_;
}
