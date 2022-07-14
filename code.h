#ifndef CODE_H
#define CODE_H


#include <opencv2/opencv.hpp>            // opencv에서 지원하는 모든 기능을 포함
#include <iostream>
#include <string.h>
#include <math.h>


using namespace cv;
using namespace std;


extern bool human;     // 사람 유무  ( 있으면 true, 없으면 false)
extern bool clean;     // 필기 유무  ( 있으면 false, 없으면 true)
extern bool movingAlot; //사람움직임 많음
extern bool movingAlittle; // 사람움직임 적음
extern bool page_num; // 페이지 유무
extern bool title;    // 제목 유무

int main_cap(String pathStr);

#endif // CODE_H
