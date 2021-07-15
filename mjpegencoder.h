#ifndef _AV_MJPEG_H_
#define _AV_MJPEG_H_


#include <opencv2/opencv.hpp>

#define __STDC_CONSTANT_MACROS
#ifdef __cplusplus
extern "C" {
#endif
#include <libavutil/time.h>
#include <libavutil/mathematics.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavdevice/avdevice.h>
#include <libswresample/swresample.h>
#ifdef __cplusplus
};
#endif


typedef struct AvMJPEGEncConfig_T {

    int width = 1280;
    int height = 720;
    int frame_rate = 24;
    int64_t bit_rate = 320000;
    int gop_size = 10;
    int max_b_frames = 0;
}AvMJPEGEncConfig;

class mjpegEncoder {

public:
    mjpegEncoder();
    ~mjpegEncoder();
    int Init(AvMJPEGEncConfig mjpeg_config);
    AVPacket* encode(cv::Mat& mat);
    void Destory();
private:
    const AVCodec* cdc_;
    AVCodecContext* cdc_ctx_;
    AVFrame* avf_;
    AVPacket* avp_;
    int frame_size_;
    int pts_;
};

#endif