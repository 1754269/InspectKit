
#include <opencv2/opencv.hpp>
#include <iostream>
#include "InspectEngine.h"

int main(int argc, char** argv)
{
    // 1. 이미지 경로 받기
    if (argc < 2) {
        std::cerr << "Usage: InspectConsole.exe <image path>\n";
        return 1;
    }

    cv::Mat img = cv::imread(argv[1]);

    if (img.empty())
    {
        std::cerr << "이미지 없음";
        return -1;
    }


    // 2. gray 변환, 픽셀 포인터, stride 추출
    cv::Mat grayImg;
    cv::cvtColor(img, grayImg, cv::COLOR_BGR2GRAY); // 3채널 -> 1채널

    const unsigned char* gray = grayImg.data;
    int w = grayImg.cols;
    int h = grayImg.rows;
    int stride = (int)grayImg.step;

    // [DEBUG] 임시: 엔진과 동일한 이진화를 여기서도 돌려 눈으로 확인용 저장
    {
        cv::Mat dbgBlur, dbgBin;
        cv::GaussianBlur(grayImg, dbgBlur, cv::Size(5, 5), 0);
        cv::threshold(dbgBlur, dbgBin, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
        cv::imwrite("E:/test/debug_gray.png", grayImg);   // 회색 원본
        cv::imwrite("E:/test/debug_binary.png", dbgBin);  // 이진화 결과
        std::cerr << "[DEBUG] saved debug_gray.png / debug_binary.png ("
                  << w << "x" << h << ")\n";
    }

    // 3. 핸들 생성
    InspHandle handle = Insp_Create();
    
    if (handle == nullptr)
    {
        std::cerr << "엔진 핸들 생성 실패";
        return -1;
    }

    // 4. Run
    int rc = Insp_Run(handle, gray, w, h, stride);
    

    if (rc != 0)
    {
        std::cerr << Insp_GetLastError(handle) << "\n";
        Insp_Destroy(handle);
        return -1;
    }

    // 5. 결과 받기
    InspResult result;
    Insp_GetResult(handle, &result);
    


    // 6. 결과 출력
    std::cout << "검출 개수: " << result.objectCount << "\n";
    for (int i = 0; i < result.objectCount; i++)
    {
        std::cout << " [" << i << "] 지름 = " << result.measuredValue[i] << " px (judgement=" << result.judgement[i] << ")\n";
    }

    std::cout << "처리시간: " << result.processMs << " ms\n";


    // 7. 해제

    Insp_Destroy(handle);

    

    return 0;
}
