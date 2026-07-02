#include "Engine.h"
#include <opencv2/opencv.hpp>
#include <cstdio>   // [DEBUG] 임시: printf 로 중간 결과 찍기

int Engine::Run(const unsigned char* gray, int w, int h, int stride)
{
	// 1. unsigned char* 를 cv::Mat 으로 감싸기
	cv::Mat src(h, w, CV_8UC1, const_cast<unsigned char*>(gray), stride);

	// 2. Gaussian Blur (노이즈 제거)
	cv::Mat blurred;
	cv::GaussianBlur(src, blurred, cv::Size(5, 5), 0);

	// 3. Otsu 이진화
	cv::Mat binary;
	cv::threshold(blurred, binary, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);

	// 4. findContours (외곽선 덩어리 찾기)
	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(binary, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	// [DEBUG] 임시: 윤곽 몇 개 잡혔나?  (0이면 이진화/로드 문제, 여러 개면 필터 문제)
	fprintf(stderr, "[DEBUG] contours = %zu\n", contours.size());

	// 5. 원형도 필터
	int count = 0;
	for (const auto& contour : contours)
	{
		double area = cv::contourArea(contour);
		double perimeter = cv::arcLength(contour, true);
		
		if (perimeter <= 0.0) continue; // 둘레가 0보다 같거나 작으면 분모가 무한대가 되므로 가드
		
		const double kMinArea = 500.0; // 이보다 작은 윤곽은 노이즈로 간주
		if (area < kMinArea) continue; 
		
		double circularity = 4.0 * CV_PI * area / (perimeter * perimeter);

		// [DEBUG] 임시: 필터에 걸리기 전, 각 덩어리의 넓이/원형도/바운딩박스 찍기
		cv::Rect bb = cv::boundingRect(contour);
		fprintf(stderr, "[DEBUG]   area=%.1f  circ=%.3f  bbox=(%d,%d %dx%d)\n",
			area, circularity, bb.x, bb.y, bb.width, bb.height);

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
