#include "Engine.h"
#include <opencv2/opencv.hpp>

int Engine::Run(const unsigned char* gray, int w, int h, int stride)
{
	// 1. unsigned char* 를 cv::Mat 으로 감싸기
	cv::Mat src(h, w, CV_8UC1, const_cast<unsigned char*>(gray), stride);

	// 2. Gaussian Blur (노이즈 제거)
	cv::Mat blurred;
	cv::GaussianBlur(src, blurred, cv::Size(5, 5), 0);

	// 3. Otsu 이진화
	cv::Mat binary;
	cv::threshold(blurred, binary, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

	// 4. findContours (외곽선 덩어리 찾기)
	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(binary, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	// 5. 원형도 필터
	int count = 0;
	for (const auto& contour : contours)
	{
		double area = cv::contourArea(contour);
		double perimeter = cv::arcLength(contour, true);
		if (perimeter <= 0.0) continue;
		double circularity = 4.0 * CV_PI * area / (perimeter * perimeter);

		if (circularity < 0.8) continue;

		// 6. 지름 측정
		cv::Point2f center;
		float radius;
		cv::minEnclosingCircle(contour, center, radius);


		double diameterPx = radius * 2.0;

		// 7. 결과 InspResult 에 담기
		if (count >= 32) break;
		result_.measuredValue[count] = diameterPx;
		result_.judgement[count] = 0;
		count++;
	}
	
	result_.objectCount = count;
	result_.resultCode = 0;

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
