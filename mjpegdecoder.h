#ifndef MJPEGDECODER_H
#define MJPEGDECODER_H

#include <string.h>
#include <opencv2/opencv.hpp>
//C++ references C language header files
extern "C"
{
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <libavutil/opt.h>
#include <libavutil/channel_layout.h>
#include <libavutil/parseutils.h>
#include <libavutil/samplefmt.h>
#include <libavutil/fifo.h>
#include <libavutil/intreadwrite.h>
#include <libavutil/dict.h>
#include <libavutil/mathematics.h>
#include <libavutil/pixdesc.h>
#include <libavutil/avstring.h>
#include <libavutil/imgutils.h>
#include <libavcodec/avcodec.h>
#include <libavfilter/avfilter.h>
#include <libavfilter/buffersrc.h>
#include <libavfilter/buffersink.h>
}

class MjpegDecoder
{
public:
    MjpegDecoder();
    ~MjpegDecoder();
    /*************************************************
      Function:initial
             Description: Initialization
             Input: None
             Output: None
             Return: error code
             Others: None
    *************************************************/
    int initial();
    /*************************************************
      Function:decode
             Description: Decoding
             Input: pDataIn-data to be decoded, nInSize-length of data to be decoded
             Output: pDataOut-decoded data, nWidth-decoded image width, nHeight-decoded image height
             Return: error code
             Others: The decoded data is in RGB16 format
    *************************************************/
    int decode(uint8_t* pDataIn, int nInSize, cv::Mat& res);
    /*************************************************
      Function:unInitial
             Description: destroy
             Input: None
             Output: None
             Return: None
             Others: None
    *************************************************/
    void unInitial();

private:
    int avframe_to_cvmat(AVFrame* frame, cv::Mat& res);
    AVFrame* cvmat2avframe(cv::Mat mat);
private:
   const AVCodec* codec;
    AVCodecContext* context;
    AVFrame* frame;
    AVPacket packet;


};

#endif // MJPEGDECODER_H