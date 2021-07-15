/
// Created by liheng on 19-12-9.
//https://www.cnblogs.com/ziyu-trip/p/7075003.html

#ifndef ADAS_X264_ENCODER_H
#define ADAS_X264_ENCODER_H

#include <stdint.h>
#include "x264.h"
#include "opencv/cv.h"
#include "opencv/highgui.h"

struct x264_encoder {
    x264_param_t    param;
    char            preset[20];
    char            tune[20];
    char            profile[20];
    x264_t* h;
    x264_picture_t    pic_in;
    x264_picture_t    pic_out;
    long            colorspace;
    x264_nal_t* nal;
    int             iframe;
    int             iframe_size;
    int                inal;
};

class x264Encoder
{
public:

    x264Encoder();

    x264Encoder(int videoWidth, int videoHeight, int channel, int fps);

    ~x264Encoder();

    /** Create X264 encoder
           * @param[in] videoWidth video width
           * @param[in] videoHeight video height
           * @param[in] fps frame rate
           * @return returns true for success, false for failure.
     */
    bool Create(int videoWidth, int videoHeight, int channel = 3, int fps = 30);

    /** Encode a frame
      * @param[in] frame input a frame of image
      * @return returns the data size after encoding, 0 means encoding failed
*/
    int EncodeOneFrame(const cv::Mat& frame);

    /** Get the encoded frame data
      * Description: Called after EncodeOneFrame
      * @return returns bare x264 data
*/
    uchar* GetEncodedFrame() const;

    /** Destroy the X264 encoder
*/
    void Destory();

    // Is the encoder available
    bool IsValid() const;

private:

    void Init();

public:
    int m_width;
    int m_height;
    int m_channel;
    int m_fps;

protected:

    int m_widthstep;
    int m_lumaSize;
    int m_chromaSize;

    x264_encoder* m_encoder;
};

#endif //ADAS_X264_ENCODER_H