
#include <opencv2/opencv.hpp>
#include <iostream>

int main()
{
    // 테스트 이미지 경로
    cv::Mat img = cv::imread("E:\\test\\coin.png");

    if (img.empty()) {
        std::cerr << "이미지를 불러오지 못했습니다. 경로를 확인하세요.\n";
        return -1;
    }

    std::cout << "이미지 크기: " << img.cols << " x " << img.rows << "\n";

    cv::imshow("Test", img);
    cv::waitKey(0);  // 아무 키나 누르면 종료
    return 0;
}
