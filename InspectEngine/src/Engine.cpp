#include "Engine.h"
#include <opencv2/opencv.hpp>

int Engine::Run(const unsigned char* gray, int w, int h, int stride)
{
	// 0. 이전 실행 결과 초기화 (핸들 재사용 시 잔여 값 방지)
	result_ = {};

	// 1. unsigned char* 를 cv::Mat 으로 감싸기
	cv::Mat src(h, w, CV_8UC1, const_cast<unsigned char*>(gray), stride);

	// 2. Gaussian Blur (노이즈 제거)
	cv::Mat blurred;
	cv::GaussianBlur(src, blurred, cv::Size(5, 5), 0);

	
	// 3. 테두리 네 변의 평균 밝기 찾기
	cv::Mat binary;

	double top = cv::mean(blurred.row(0))[0];
	double bottom = cv::mean(blurred.row(h - 1))[0];
	double left = cv::mean(blurred.col(0))[0];
	double right = cv::mean(blurred.col(w - 1))[0];
	double borderMean = (top + bottom + left + right) / 4.0;

	const double kBgBrightThreshold = 127.0;
	int threshType = (borderMean > kBgBrightThreshold) ? cv::THRESH_BINARY_INV : cv::THRESH_BINARY;
	threshType |= cv::THRESH_OTSU;

	// 4. Otsu 이진화
	cv::threshold(blurred, binary, 0, 255, threshType);

	// 4. (+) 모폴로지 - 이진화 다듬기
	cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));
	cv::morphologyEx(binary, binary, cv::MORPH_CLOSE, kernel);

	// 5. findContours (외곽선 덩어리 찾기)
	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(binary, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	// 6. 원형도 필터
	int count = 0;
	for (const auto& contour : contours)
	{
		// convexHull (이진화 다듬기)
		std::vector<cv::Point> hull;
		cv::convexHull(contour, hull);

		double area = cv::contourArea(hull);
		double perimeter = cv::arcLength(hull, true);
		
		if (perimeter <= 0.0) continue; // 둘레가 0보다 같거나 작으면 분모가 무한대가 되므로 가드
		
		const double kMinArea = 500.0; // 이보다 작은 윤곽은 노이즈로 간주
		if (area < kMinArea) continue; 
		
		double circularity = 4.0 * CV_PI * area / (perimeter * perimeter);

		if (circularity < 0.8) continue;

		// 7. 지름 측정
		cv::Point2f center;
		float radius;
		cv::minEnclosingCircle(hull, center, radius);


		double diameterPx = radius * 2.0;

		// 8. 결과 InspResult 에 담기
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
