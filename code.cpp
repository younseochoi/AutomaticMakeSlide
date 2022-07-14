#include "code.h"
#include <QFileDialog>
#include <QDebug>
#include <opencv2/opencv.hpp>
#include <experimental/filesystem>
#include <iostream>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

bool human;     // 사람 유무  ( 있으면 true, 없으면 false)
bool clean;     // 필기 유무  ( 있으면 false, 없으면 true)
bool movingAlot = false; //사람움직임 많음
bool movingAlittle = false; // 사람움직임 적음
bool movingNormal = false;
bool page_num; // 페이지 유무
bool title;    // 제목 유무


QString dir = "";
QString write_path = "";
String mkdirPath="";
using namespace std;

int threshold1 = 50;
Vec3b lower_blue1, upper_blue1, lower_blue2, upper_blue2, lower_blue3, upper_blue3;
Mat img_color;
Point pt;

int store()
{
    mkdirPath="/Users/choeyunseo/Desktop/make_PDF_";

    struct tm *newtime;
    time_t ltime;

    time(&ltime);  /* Get the time in seconds */
    newtime = localtime(&ltime); /* Convert it to the structure tm */
    char * t = asctime(newtime); /* Print the local time as a string */
    mkdirPath +=t;
    String pdf = mkdirPath+"/pdf";
    const char * c = mkdirPath.c_str();
    const char *c2 = pdf.c_str();



//    // Creating a directory
        mkdir(c, 0777);
        mkdir(c2,0777);
//       Qdedug() << "Error :  " << strerror(errno) << endl;

        return 0;
//    else
//        cout << c <<endl;
//        cout << "Directory created";
}

//--------------------------------------------------------------------------------------------
// 사람있음에서 트래킹에 사용하는 마우스 이벤트와 변수
struct CallbackParam {
   Mat frame; //영상
   Point pt1, pt2; //포인터
   Rect roi; //roi
   bool drag;// drag 여부
   bool updated; //update 여부
};
void onMouse(int event, int x, int y, int flags, void* param) {
   CallbackParam *p = (CallbackParam *)param;
   if (event == EVENT_LBUTTONDOWN) {
      p->pt1.x = x;
      p->pt1.y = y;
      p->pt2 = p->pt1;
      p->drag = true;
   }
   if (event == EVENT_LBUTTONUP) {
      int w = x - p->pt1.x;
      int h = y - p->pt1.y;
      p->roi.x = p->pt1.x;
      p->roi.y = p->pt1.y;
      p->roi.width = w;
      p->roi.height = h;
      p->drag = false;
      if (w >= 10 && h >= 10) {
         p->updated = true;
      }
   }
   if (p->drag && event == EVENT_MOUSEMOVE) {
      if (p->pt2.x != x || p->pt2.y != y) {
         Mat img = p->frame.clone();
         p->pt2.x = x;
         p->pt2.y = y;
         rectangle(img, p->pt1, p->pt2, Scalar(0, 255, 0), 1);
         imshow("Tracker", img);
      }
   }
}

void mouse_callback(int event, int x, int y, int flags, void *param)
{
    if (event == EVENT_LBUTTONDOWN)
    {
        cout << "클릭클릭 :: " << x << y << endl;
        pt = Point(x, y);

        Vec3b color_pixel = img_color.at<Vec3b>(y, x);
        // circle(img_color, Point(50, 50), 20, Scalar(0, 0, 255), 2);
        Mat bgr_color = Mat(1, 1, CV_8UC3, color_pixel);

        Mat hsv_color;
        cvtColor(bgr_color, hsv_color, COLOR_BGR2HSV);

        int hue = hsv_color.at<Vec3b>(0, 0)[0];
        int saturation = hsv_color.at<Vec3b>(0, 0)[1];
        int value = hsv_color.at<Vec3b>(0, 0)[2];

        cout << "hue = " << hue << endl;
        cout << "saturation = " << saturation << endl;
        cout << "value = " << value << endl;

        if (hue < 10)
        {
            cout << "case 1" << endl;
            lower_blue1 = Vec3b(hue - 10 + 180, threshold1, threshold1);
            upper_blue1 = Vec3b(180, 255, 255);
            lower_blue2 = Vec3b(0, threshold1, threshold1);
            upper_blue2 = Vec3b(hue, 255, 255);
            lower_blue3 = Vec3b(hue, threshold1, threshold1);
            upper_blue3 = Vec3b(hue + 10, 255, 255);
        }
        else if (hue > 170)
        {
            cout << "case 2" << endl;
            lower_blue1 = Vec3b(hue, threshold1, threshold1);
            upper_blue1 = Vec3b(180, 255, 255);
            lower_blue2 = Vec3b(0, threshold1, threshold1);
            upper_blue2 = Vec3b(hue + 10 - 180, 255, 255);
            lower_blue3 = Vec3b(hue - 10, threshold1, threshold1);
            upper_blue3 = Vec3b(hue, 255, 255);
        }
        else
        {
            cout << "case 3" << endl;

            lower_blue1 = Vec3b(hue, threshold1, threshold1);
            upper_blue1 = Vec3b(hue + 10, 255, 255);
            lower_blue2 = Vec3b(hue - 10, threshold1, threshold1);
            upper_blue2 = Vec3b(hue, 255, 255);
            lower_blue3 = Vec3b(hue - 10, threshold1, threshold1);
            upper_blue3 = Vec3b(hue, 255, 255);
        }

        cout << "hue = " << hue << endl;
        cout << "#1 = " << lower_blue1 << "~" << upper_blue1 << endl;
        cout << "#2 = " << lower_blue2 << "~" << upper_blue2 << endl;
        cout << "#3 = " << lower_blue3 << "~" << upper_blue3 << endl;
    }
}


//--------------------------------------------------------------------------------------------
 double getDiff(const Mat& I1, const Mat& I2, CallbackParam image_ptr) {
    Mat st, gray, sumimage;
    Point ptr1, ptr2;

    ptr1 = image_ptr.pt1;
    ptr2 = image_ptr.pt2;
    //cout << "getDiff() " <<ptr1 << ptr2<< endl;
    int h1, h2, w1, w2;

    w1 = ptr1.x;
    w2 = ptr2.x;
    h1 = ptr1.y;
    h2 = ptr2.y;

    // if(!(w1&&w2&&h1&&h2)){
    //     w2 = I1.cols;
    //     h2 =I1.rows;
    // }
    //    cout <<w1 <<w2 <<h1 << h2<<endl;
    Mat img1 = I1(Range(h1, h2), Range(w1, w2));
    Mat img2 = I2(Range(h1, h2), Range(w1, w2));

    st = Mat::zeros(h2 - h1, w2 - w1, COLOR_BGR2GRAY);
    gray = Mat::zeros(h2 - h1, w2 - w1, COLOR_BGR2GRAY);
    sumimage = Mat::zeros(h2 - h1, w2 - w1, COLOR_BGR2GRAY);

    absdiff(img1, img2, st);

    cvtColor(st, gray, COLOR_BGR2GRAY);

    threshold(gray, sumimage, 30, 255, THRESH_BINARY);

    Scalar total_s = sum(sumimage);

    double size = sumimage.rows * sumimage.cols;

    double result = total_s[0] * (1 / size) *(1 / 255.0);
    //cout << result <<endl;
    return result;

 }


 //--------------------------------------------------------------------------------------------

int main_cap(String pathStr) {
//   String path = dir.toStdString();
    Mat img_hsv, img_prev_mask, border, img_result_mask, img_mask;
      Mat prevFrame, addFrame, tmpFrame;
     int flag = 0;


     if (human && movingAlot)
        {
           namedWindow("img_color");
           setMouseCallback("img_color", mouse_callback);

           // namedWindow("set content");
           // setMouseCallback("set content", onMouse);
        }
    String path = pathStr;
   VideoCapture capture(path);
   char buf[256];
   char c;
   int index = 0;
   int frameNum = -1;
   double prevResult = 0.0;
   String complite = write_path.toStdString();
   Mat background, image, gray, result, foregroundMask, changeFrame, origin, prevorigin, currentFrame;
   CallbackParam param, page_param, title_param, content_param;
   Mat frame, m_backproj, hsv;
   Mat m_model3d;
   Rect m_rc;
   float hrange[] = { 0,180 };
   float vrange[] = { 0,255 };
   float srange[] = { 0,255 };
   const float* ranges[] = { hrange,srange,vrange };
   int channels[] = { 0,1,2 };
   int hist_sizes[] = { 16,16,16 };
    int count =0;

   bool tracking = false;
   bool start = false;

   bool init_title = true;
   bool init_page_num = true;
   store();
    mkdirPath = mkdirPath + "/Img_%06d.jpg";
    const char * mkdirPathc = mkdirPath.c_str();
//   cout << clean << "moving" << movingAlittle<<movingNormal<<movingAlot <<endl;
   //맨 처음 frame을 background 라고 가정
   capture >> background;
   capture >> img_hsv;

   cvtColor(background, background, COLOR_BGR2GRAY);
   while (true) {
       if(human && movingAlot){

           capture.read(img_color);
                   // ++frameNum;
                   if (img_color.cols == 0 && img_color.rows == 0)
                      break;

                   imshow("img_color", img_color);
                   imshow("Tracker", img_color);

                   if (frameNum < 0)
                   {
                      cout << "색깔 입력 ::: " << pt << endl;
                      if (clean)
                      {
                         cout << "클린 버전 :: 첫 번째 프레임 저장" << endl;
                         sprintf(buf, mkdirPathc, index);
                         imwrite(buf, img_color);
//                         index++;
                      }

                      origin = img_color.clone();
                      prevorigin = img_color.clone();
                      img_mask = Mat::zeros(img_color.rows, img_color.cols, CV_8UC1);
                      img_prev_mask = img_mask.clone();

                      currentFrame = img_mask.clone();
                      prevFrame = currentFrame.clone();
                      changeFrame = currentFrame.clone();
                      addFrame = currentFrame.clone();

                      param.frame = img_color;
                      param.drag = false;
                      param.updated = false;
                      setMouseCallback("Tracker", onMouse, &param); //관심영역 지정

                      if (param.drag)
                      {
                         if (waitKey(33) == 27)
                            break;
                         continue;
                      }
                      if (param.updated)
                      {
                         start = true;
                      }

                      if (!start)
                      {
                         waitKey(0);
                      }
                      cout << "내용 입력 받기  ::: " << param.pt1 << param.pt2 << endl;
                      // boder = Mat::zeros(param.roi.height, param.roi.width, CV_8U);
                      border = Mat::zeros(img_color.rows, img_color.cols, CV_8U);
                      rectangle(border, Rect(param.pt1, param.pt2), Scalar(255), -1);

                      waitKey(0);
                   }

                   frameNum++;
                   if (frameNum % 120 != 0)
                      continue;

                   cvtColor(img_color, img_hsv, COLOR_BGR2HSV);

                   Mat img_mask1, img_mask2, img_mask3;
                   inRange(img_hsv, lower_blue1, upper_blue1, img_mask1);
                   inRange(img_hsv, lower_blue2, upper_blue2, img_mask2);
                   inRange(img_hsv, lower_blue3, upper_blue3, img_mask3);

                   img_mask = img_mask1 | img_mask2 | img_mask3;
                   img_mask = img_mask & border;

                   dilate(img_mask, img_mask, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

                   // morphological opening 작은 점들을 제거
                   erode(img_mask, img_mask, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
                   dilate(img_mask, img_mask, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

                   dilate(img_mask, img_mask, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
                   erode(img_mask, img_mask, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));


                   imshow("img_mask", img_mask);
//                   imshow("prevFrame", prevFrame);
//                   imshow("currentFrame", currentFrame);
                   // waitKey(0);
                   //마스크 만들기 끝.

                   // addFrame구함
                   //   cout <<"여기야!???????????!!!!" <<endl;
                   add(currentFrame, img_mask, addFrame); // 1 + 3

                   addFrame = addFrame & border;

//                   imshow("addFrame", addFrame);

                   //방법 1
                   Scalar total_prev, total_add;
                   total_prev = sum(currentFrame);
                   total_add = sum(addFrame);
                   int r_w, r_h;
                   r_w = param.roi.width;
                   r_h = param.roi.height;

                   double change_pixel = ((total_add[0] - total_prev[0]) / (255.0 * r_w * r_h));

                   if (change_pixel > 0.08)
                   {
                      cout << "영상 변화 o ::: " << change_pixel << " ::: " << frameNum << " ::: " << endl;

                       if (clean)
                       {
                           sprintf(buf, mkdirPathc, index);
                           imwrite(buf, img_color);
                           index++;
                       } else {
                           sprintf(buf, mkdirPathc, index);
                           imwrite(buf, prevorigin);
                           index++;
                           // waitKey(0);
                       }

                      // waitKey(0);
                      //검출된 직후에


                      // imshow("after", changeFrame);

                   }

                   //

                   if (frameNum % 120 == 0)
                   {
                      prevFrame = currentFrame.clone();
                      currentFrame = img_mask.clone();

                      prevorigin = origin.clone();
                      origin = img_color.clone();
                      // img_prev_mask = img_mask.clone();
                   }

                   c = (char)waitKey(1);
                   if (c == 27) // ESC 누르면 꺼짐
                      break;
                   count =1;


       }
      else if (human) { //사람 있음
         if (frameNum < 0) { // TODO: 혜경이한테 물어보자
            //click and drag on ima ge to set ROI
            capture >> frame;  //첫번재 프레임 display
            imshow("Tracker", frame);
            param.frame = frame;
            param.drag = false;
            param.updated = false;
            setMouseCallback("Tracker", onMouse, &param); //관심영역 지정
         }
         frameNum++;
         if (capture.grab() == 0)break;
         if (frame.rows == 0 && frame.cols == 0)
                  break;
         if (frameNum % 12 != 0) continue; //요거는 배속영상이면 나머지 연산하는 숫자를 줄여주는게 좋음.

         if (param.drag) { //트래킹을 위한 영역지정
            if (waitKey(33) == 27) break;
            continue;

         }
         cvtColor(frame, hsv, COLOR_BGR2HSV); //hsv 로 변경
         if (param.updated) { //드래그한 영역 histogram 계산

            Rect rc = param.roi;
            Mat mask = Mat::zeros(rc.height, rc.width, CV_8U);
            ellipse(mask, Point(rc.width / 2, rc.height / 2), Size(rc.width / 2, rc.height / 2), 0, 0, 360, 255, FILLED);
            Mat roi(hsv, rc);
            calcHist(&roi, 1, channels, mask, m_model3d, 3, hist_sizes, ranges);
            m_rc = rc;
            param.updated = false;
            tracking = true;
            //imshow("mask",mask);
         }
         capture >> frame;
         if (tracking) {
            calcBackProject(&hsv, 1, channels, m_model3d, m_backproj, ranges);
            // tracking
            CamShift(m_backproj, m_rc, TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 20, 1));
            rectangle(frame, m_rc, Scalar(0, 0, 255), 3);
            //rectangle(image, m_rc, Scalar(0,0,0),-1);
            start = true;
         }
         imshow("Tracker", frame); //image display
        //imshow("image",image);
         if (!start) { //트래킹 시작
            waitKey(0);
            start = true;
         }

         capture >> image;


         if (frameNum <= 0) {
            currentFrame = image.clone();
            changeFrame = image.clone();
            origin = image.clone();
            prevorigin = image.clone();

            //클린버전이면 맨 처음 화면 저장
            if (clean) {
               //sprintf_s(buf, "./result/preOriginImg_%06d.jpg", index++);
//               sprintf(buf, "/Users/choeyunseo/build-untitled10-Unnamed3-Release/result/preOriginImg_%06d.jpg", index++);
                sprintf(buf,mkdirPathc, index++);
               imwrite(buf, image);
            }

         }
         //   if(frameNum%10!=0) continue;
         cvtColor(image, gray, COLOR_BGR2GRAY);
         absdiff(background, gray, foregroundMask); // foregroundMask=배경-현재frame
         threshold(foregroundMask, foregroundMask, 50, 255, THRESH_BINARY); //임계값확인
         //foregroundMask.copyTo(foregroundImg); // foregroundMask를 img에 복사
        //gray.copyTo(foregroundImg, foregroundMask); //foregroundMask가 흰색인 부분만 copy

         int w, h, m_x;
         w = m_rc.width;
         h = foregroundMask.cols;

         if (movingAlot) { //사람움직임 많음 -> 사각형 범위 크게
            m_x = m_rc.x - 100;
         }
         else if (movingAlittle) { // 사람움직임 적음 -> 사가가형 범위 작게
            m_x = m_rc.x - 50;
         }
         else { // 사람움직임 보통
            m_x = m_rc.x - 75;
         }
         Rect maskRect(m_x, 0, 2 * w, h);
         // Rect maskRect (m_rc.x-75,0,2*w,h);
         rectangle(foregroundMask, maskRect, Scalar(1), -1);

         Scalar total_s = sum(foregroundMask);
         double size = foregroundMask.rows * foregroundMask.cols;


         double result = total_s[0] * (1 / size) *(1 / 255.0);

         if (result >= 0.7) {
            cout << "조명변화나 시점변화일 가능성 큼" << endl;
            continue;
         }
         if (result - prevResult > 0.002) {
            cout << "예비 ::: ";
            cout << result << endl;
            cout << result - prevResult << endl;
         }


         if (!clean) { //필기버전 검출 코드
            if (result - prevResult > 0.02) { // 필기버전.
            //if( prevResult - result > 0.02){ // 필기버전.
               cout << "----------검출!-------------" << endl;
               prevorigin = origin.clone();
               origin = currentFrame.clone();
               //sprintf_s(buf, "/Users/choeyunseo/build-untitled10-Unnamed3-Release/result/preOriginImg_%06d.jpg"", index);

//               sprintf(buf, "/Users/choeyunseo/build-untitled10-Unnamed3-Release/result/preOriginImg_%06d.jpg", index);
                sprintf(buf,mkdirPathc, index++);
               imwrite(buf, prevorigin);
//               index++;
            }
         }
         else { //클린버전 검출코드
            if (prevResult - result > 0.02) { //클린 버전.
             //if(result - prevResult > 0.02){
               cout << "—————검출!——————" << endl;
               prevorigin = origin.clone();
               origin = currentFrame.clone();
               //sprintf_s(buf, "./result/preOriginImg_%06d.jpg", index);
//               sprintf(buf, "/Users/choeyunseo/build-untitled10-Unnamed3-Release/result/preOriginImg_%06d.jpg", index);
                sprintf(buf,mkdirPathc, index++);
               imwrite(buf, currentFrame);
               //imwrite(buf , origin);
               //imwrite(buf , image);
//               index++;
            }

         }
         prevResult = result;

         if (frameNum % 2 == 0) {

            prevorigin = origin.clone();
            origin = currentFrame.clone();
            background = gray.clone();
            currentFrame = image.clone();
         }

         imshow("foregroundMask", foregroundMask);

         c = (char)waitKey(1);
         if (c == 27)       // ESC 누르면 꺼짐
            break;


      }
      else { //사람없는 버전

       // content_param.pt1=Point(0,0);
       // content_param.pt2=Point(image.rows,image.cols);
       capture >> image; // TODO:
//       imshow("Tracker",image);
       // param.frame = frame;
       // param.drag = false;
       // param.updated = false;

       // setMouseCallback("Tracker",onMouse,&param); //관심영역 지정

         if (frameNum < 0) {
             imshow("Tracker",image);
             cout << "frameNum <0, 초기설정" <<endl;
            capture >> image;
            currentFrame = image.clone();
            changeFrame = image.clone();
            origin = image.clone();
            prevorigin = image.clone();

             if (clean)//클린 버전시 첫 프레임 저장
            {
               //sprintf_s(buf, "./result/testImg_%06d.jpg", index);
//               sprintf(buf, "/Users/choeyunseo/build-untitled10-Unnamed3-Release/result/preOriginImg_%06d.jpg", index);
                 sprintf(buf,mkdirPathc,index);
               imwrite(buf, currentFrame);
               cout << "사람없음 :: 클린버전 첫저장" << endl;
               index++;
            }

            cout << "내용받기 : ";
//            imshow("Tracker", image);
            param.frame = image;
            param.drag = false;
            param.updated = false;


            setMouseCallback("Tracker", onMouse, &param); //관심영역 지정

            if (param.drag) {

                     if (waitKey(33) == 27) break;
                     continue;
                  }
                  if(param.updated){
                      start = true;
                  }
                  if(!start){
                    waitKey(0);
                  }
            content_param = param;
            cout << content_param.pt1<<content_param.pt2<<endl;

         } //////----
         if (image.rows == 0 && image.cols == 0)
                  break;
         if(index == 2 && init_page_num){
             imshow("Tracker",image);
            if (page_num) {//페이지 있으니까 입력, 페이지를 입력받았으면 다른사항들은 고려하지 않아도 되기 때문에 if, elseif 문으로 고려
               cout << "page_num있음"<< frameNum<<endl;
               // if (frameNum < 0) {
                  //frameNum++;
                   cout << "쪽수입력받기 : ";
                  //click and drag on image to set ROI
//                  imshow("Tracker", image);
                  param.frame = image;
                  param.drag = false;
                  param.updated = false;
                  setMouseCallback("Tracker", onMouse, &param); //관심영역 지정



                  if (param.drag) {

                     if (waitKey(33) == 27) break;
                     continue;
                  }
                  if(param.updated){
                      start = true;
                  }
                  if(!start){
                    waitKey(0);
                  }
                   page_param = param;
                   cout << page_param.pt1<<page_param.pt2<<endl;
                   //page_num = false;
                   //waitKey(0);
               // }
            }
            // TODO: 혜경이한테 물어보기
            else if (title) { //페이지가 없고, 타이틀이 있는경우 타이틀이 안바뀌어도 내용은 바뀌는 경우가 있어서 타이틀과 내용 모두 입력받음
//                frameNum++;
               //제목받기
               cout << "제목받기 : ";
//               imshow("Tracker", image);
               param.frame = image;
               param.drag = false;
               param.updated = false;
               setMouseCallback("Tracker", onMouse, &param); //관심영역 지정

               if (frameNum % 12 != 0) continue; //요거는 배속영상이면 나머지 연산하는 숫자를 줄여주는게 좋음.

               if (param.drag) {

                     if (waitKey(33) == 27) break;
                     continue;
                  }
                  if(param.updated){
                      start = true;
                  }
                  if(!start){
                    waitKey(0);
                  }
                  title_param = param;
                  cout << title_param.pt1<<title_param.pt2<<endl;
                  //waitKey(0);
               //내용받기
                cout << "제목받기 - 내용도 추가로 : ";
//               imshow("Tracker", image);
               param.frame = image;
               param.drag = false;
               param.updated = false;
               setMouseCallback("Tracker", onMouse, &param); //관심영역 지정

               if (param.drag) {

                     if (waitKey(33) == 27) break;
                     continue;
                  }
                  if(param.updated){
                      start = true;
                  }
                  if(!start){
                    waitKey(0);
                  }
               content_param = param;
               cout << content_param.pt1<<content_param.pt2<<endl;
                  //waitKey(0);
                  //title = false;
            }
           init_page_num =false;
           destroyAllWindows();


//            frameNum++;

            //내용받기 -> 요건 if문 왜 안넣었나용?
           //  cout << "내용받기 : ";
           //  imshow("Tracker", image);
           //  param.frame = image;
           //  param.drag = false;
           //  param.updated = false;


           //  setMouseCallback("Tracker", onMouse, &param); //관심영역 지정

           //  if (param.drag) {

           //           if (waitKey(33) == 27) break;
           //           continue;
           //        }
           //        if(param.updated){
           //            start = true;
           //        }
           //        if(!start){
           //          waitKey(0);
           //        }
           //  content_param = param;
           //  cout << content_param.pt1<<content_param.pt2<<endl;

          }
         ++frameNum;

         if (capture.grab() == 0) break;
         if (frameNum % 24 != 0) continue; //요거는 배속영상이면 나머지 연산하는 숫자를 줄여주는게 좋음.

         double res = 0, res_1 = 0, res_2 = 0;

         if (page_num && index >= 2) {// 쪽수 있을 시
           cout<<"여기는?" <<endl;
            //res = getDiff(origin, currentFrame, page_param);
            res = getDiff(origin, image, page_param);}
         else if (title && index >= 2) { // 제목 있을 시
            //res_1 = getDiff(origin, currentFrame, title_param);
            res_1 = getDiff(origin, image, title_param);
            if (res < 0.03) { // TODO: 0.03 기준 어떻게 잡았는지 질문
               //res_2 = getDiff(origin, currentFrame, content_param);
               res_2 = getDiff(origin, image, content_param);
               res = (res_1 + res_2) / 2.0;
            }
            else res = res_1;
         } // 내용 있을 시
         else{
             cout <<"처음엔"<<endl;
            //res = getDiff(origin, currentFrame, content_param);
            res = getDiff(origin, image, content_param);

            }

         cout << "main ::::: " <<res << endl;
         if (res > 0.03) {
            //changeFrame = currFrame.clone(); //클린버전
            //changeFrame = prevFrame.clone(); //필기버전 -> // TODO: 마지막 프레임일때는 currFrame도 저장해주어야함!
            if (!clean) { //필기버전시 이전의 이전 프레임 검출
               changeFrame = prevorigin.clone(); //prev필기 진짜진짜 필기버전
               cout <<"필기버전 검출 :::"<<endl;
            }
            else { //클린 버전시 현재 프레임 검출
               //changeFrame = currentFrame.clone();
               changeFrame = image.clone();
               cout <<"클린버전 검출 :::"<<endl;
            }
            cout << "차이: " << res << endl;
            //sprintf_s(buf, "./result/testImg_%06d.jpg", index);
            //sprintf(buf, "./result/preOriginImg_%06d.jpg", index);
//            sprintf(buf,"/Users/choeyunseo/build-untitled10-Unnamed3-Release/result/preOriginImg_%06d.jpg",index);
            sprintf(buf,mkdirPathc,index);
            imwrite(buf, changeFrame);
            // cout << index << endl;
            index++;
         }

         if (frameNum % 2 == 0)
         {
            prevorigin = origin.clone();
            //origin = currentFrame.clone();
            origin = image.clone();
         }
         c = (char)waitKey(1);
         if (c == 27)       // ESC 누르면 꺼짐
            break;
      }


   //}

 }
// if (!clean) {//while문 빠져나오고 사람있는 필기버전 프레임 저장
       //   sprintf_s(buf, "./result/preOriginImg_%06d.jpg", index++);
       //sprintf(buf, "/Users/choeyunseo/build-untitled10-Unnamed3-Release/result/preOriginImg_%06d.jpg", index++);
        sprintf(buf, mkdirPathc, index++);
         imwrite(buf, origin);
 //     }
      //sprintf_s(buf, "/Users/choeyunseo/build-untitled10-Unnamed3-Release/result/preOriginImg_%06d.jpg"", index++);
//   sprintf(buf, mkdirPathc, index++);
//   imwrite(buf, origin);  // TODO: 마지막 프레임 저장 논의
   destroyAllWindows();
   return 0;
}
