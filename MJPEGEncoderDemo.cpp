
#include "mjpegencoder.h"
#include "mjpegdecoder.h"
#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>

#include <time.h>

int main()
{
    cv::Mat frame;
    cv::Mat dst;
    FILE* Encodefile;
    char out_name[40] = { 0, };
    char Encout_name[40] = { 0, };
    time_t timer;
    struct tm* t;
    timer = time(NULL);
    t = localtime(&timer);
    sprintf(out_name,    "./VIDEO/Out_%4d.%02d.%02d,%02d%.02d%.02d.avi", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
    sprintf(Encout_name, "./VIDEO/Enc_%4d.%02d.%02d,%02d%.02d%.02d.avi", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);


    cv::VideoCapture videoCapture("RawRecord.avi");

    mjpegEncoder mjpeg;
    AvMJPEGEncConfig conf;
    conf.width = 1280 ;
    conf.height = 720;
    conf.gop_size = 1;
    conf.max_b_frames = 0;
    conf.frame_rate = 24;
    mjpeg.Init(conf);


    MjpegDecoder m_mjpegDecoder;
    m_mjpegDecoder.initial();

    //int jpeg_quality = 75;
   // std::vector<int> params;
   // params.push_back(cv::IMWRITE_JPEG_QUALITY);
   // params.push_back(jpeg_quality);

    cv::Mat cvDst;
    int nWaitTime = 1;
        
    int fourcc1 = cv::VideoWriter::fourcc('D','I','V','X');
        cv::VideoWriter writer(out_name,fourcc1 , conf.frame_rate,cv::Size(conf.width,conf.height));

        if (!writer.isOpened())
        {
            std::cerr << "failed to open the file for writing" << std::endl;
            writer.release();
            return 1;
        }

    
        int encodesize=0;
        int sizesum1 = 0;
        int sizesum2 = 0;
        int sizesum3 = 0;
        int checkframe = 0;
        char *filename = Encout_name;  //인코딩상태 파일은 디코딩 다시 한뒤 정상 재생 확인.
        Encodefile = fopen(filename, "wb");

        if (!Encodefile) {
            fprintf(stderr, "Could not open %s\n",filename);
            exit(1);
        }

        while (1)
    {
            checkframe++;
        videoCapture >> frame;
        if (frame.empty())
            break;
      //  videoCapture.get(CV_CAP_PROP_BUFFERSIZE);
        cv::imshow("src", frame);     //원본 비디오 
       // cv::Mat _frame;
                    //src,dst
     //   cv::resize(frame, _frame, cv::Size(), 0.5, 0.5);

        double timePoint1 = cv::getTickCount();

       // std::vector<uchar> jpgSize;
                    //출력확장자,기록할이미지,버퍼,/파라미터=jpeg
        // cv::imencode(".jpg", _frame, jpgSize, params);      //imencode 함수는 이미지를 압축하고 결과에 맞게 크기가 조정 된 메모리 버퍼에 저장합니다

        double timePoint2 = cv::getTickCount();

        // cvDst = cv::imdecode(jpgSize, cv::IMREAD_COLOR);    //imdecode 함수는 메모리의 지정된 버퍼에서 이미지를 읽습니다  ,imread_color= 이미지를 3채널 BGR로 변환
      //   cv::imshow("cvdecode", cvDst);


        // do encode
        AVPacket* pkt = mjpeg.encode(frame);     //pkt ,data 에 인코딩 데이터
        std::cout << "encode" << std::endl;

        int size = pkt->size;
        uchar* data = nullptr;
        data = pkt->data;

        fwrite(pkt->data, 1, pkt->size, Encodefile);       //인코딩 데이터 파일로 저장

        encodesize += size;


        m_mjpegDecoder.decode(data, size, dst);   //dst에 디코딩 데이터 

        std::cout << "decode"<<std::endl;

        writer << dst;  // VideoWriter()                //디코딩 데이터 -> VideoWriter()

        cv::imshow("decode", dst);

        double timePoint3 = cv::getTickCount();


        //cv::Mat diff = dst - frame;
                 //cv::imshow("diff",diff);//Check whether there is a difference between the image before and after encoding and decoding
        sizesum1 += frame.elemSize()* frame.total();  //elemsize = 행렬요소 하나의 바이트 크기를 반환  total = 행렬 요소 전체 개수
        sizesum2 += size;
        sizesum3 += sizeof(dst);
        printf("    src      size: %d , encode size: %d ,  Frame: %d \n", sizesum1, sizesum2, checkframe);

        printf("whole encode size: %d  mjpeg encode size:%d,Fps:%.2f\n",
            encodesize, size, cv::getTickFrequency() / (timePoint3 - timePoint2));





        char chKey = cv::waitKey(nWaitTime);
        //ESC
        if (27 == chKey)break;
        else if (' ' == chKey) nWaitTime = !nWaitTime;
    }
    writer.release();
    fclose(Encodefile);

    return 0;
}