
#include "mjpegencoder.h"


mjpegEncoder::mjpegEncoder() {

    cdc_ = NULL;
    cdc_ctx_ = NULL;
    avf_ = NULL;
    avp_ = NULL;
}

mjpegEncoder::~mjpegEncoder()
{
    Destory();
}

int mjpegEncoder::Init(AvMJPEGEncConfig mjpeg_config) {

    pts_ = 0;

    //cdc_ = avcodec_find_encoder(AV_CODEC_ID_H264);
    cdc_ = avcodec_find_encoder(AV_CODEC_ID_MJPEG);
    if (!cdc_) {

        return -1;
    }
    cdc_ctx_ = avcodec_alloc_context3(cdc_);
    if (!cdc_ctx_) {

        return -1;
    }
    //cdc_ctx_->bit_rate = mjpeg_config.bit_rate;//The picture is blurred
    // 영상의 가로 길이 설정
    cdc_ctx_->width = mjpeg_config.width;
    // 영상의 세로 길이 설정
    cdc_ctx_->height = mjpeg_config.height;
    // fps에 맞게 설정
    cdc_ctx_->time_base = { 1, mjpeg_config.frame_rate };  
    // 초당 화면 출력 장 수 설정 1초당 ~장 출력
    cdc_ctx_->framerate = { mjpeg_config.frame_rate, 1 };
    // gop 사이즈 설정.p 프레임(앞 영상과의 diff로 생성되는 영상)의 길이 설정
    cdc_ctx_->gop_size = mjpeg_config.gop_size;
    // b frame(뒤 영상과 diff로 생성되는 영상)의 길이 설정
    cdc_ctx_->max_b_frames = mjpeg_config.max_b_frames;
    // 입력되는 픽셀의 포맷 설정. 현재 YUV로 설정함.
    cdc_ctx_->pix_fmt = AV_PIX_FMT_YUVJ420P;                            //기본 설정은 YUV420P였으나 MJPEG  사용을 위해 YUVJ420P를 사용하였다. 정상작동하게됨.
    
    //cdc_ctx_->codec_id = AV_CODEC_ID_H264;
    cdc_ctx_->codec_id = AV_CODEC_ID_MJPEG;

    cdc_ctx_->codec_type = AVMEDIA_TYPE_VIDEO;
   // cdc_ctx_->qmin = 2;                                        
    cdc_ctx_->qmax = 2;                                                  // 동영상 퀄리티 지정 2~ 31    31이 가장 안좋음  Raw 비디오를 인코딩해야 용량을 비교할수있을듯. 
    //cdc_ctx_->qcompress = 0.6;
    AVDictionary* dict = nullptr;
    //av_dict_set(&dict, "preset", "slow", 0);

    //av_dict_set(&dict, "pix_fmt", "yuvj420p", 0);
    av_dict_set(&dict, "src_range", "1", 0);
    av_dict_set(&dict, "dst_range", "1",0);
    av_dict_set(&dict, "preset", "veryfast", 0);                    //인코딩 시간. 용량 반비례
   
   // av_dict_set(&dict, "tune", "zerolatency", 0);
   

   
    //av_dict_set(&dict, "profile", "main", 0);                      //MJPEG 사용시 오류 발생하는 부분. 주석처리하면 되긴된다.
   // av_dict_set(&dict, "vcodec", "mjpeg", 0);


    avf_ = av_frame_alloc();
    avp_ = av_packet_alloc();
    if (!avf_ || !avp_) {

        return -1;
    }
    av_init_packet(avp_);

    frame_size_ = cdc_ctx_->width * cdc_ctx_->height;
    avf_->format = cdc_ctx_->pix_fmt;
    avf_->width = cdc_ctx_->width;
    avf_->height = cdc_ctx_->height;
    // alloc memory
    int r = av_frame_get_buffer(avf_, 0);
    if (r < 0) {

        return -1;
    }
    r = av_frame_make_writable(avf_);
    if (r < 0) {

        return -1;
    }
    return avcodec_open2(cdc_ctx_, cdc_, &dict);
}

void mjpegEncoder::Destory() {

    if (cdc_ctx_) avcodec_free_context(&cdc_ctx_);
    if (avf_) av_frame_free(&avf_);
    if (avp_) av_packet_free(&avp_);
}

AVPacket* mjpegEncoder::encode(cv::Mat& mat) {  //videocapture()를 통해 mat 형식의 프레임을 입력받음. const cv: mat& mat 일때 오류  

    if (mat.empty()) return NULL;
    
     cv::resize(mat, mat, cv::Size(cdc_ctx_->width, cdc_ctx_->height));
    
    cv::Mat yuv;
    cv::cvtColor(mat, yuv, cv::COLOR_BGR2YUV_I420);
    unsigned char* pdata = yuv.data;

    // fill yuv420
    // yyy yyy yyy yyy
    // uuu
    // vvv

    avf_->data[0] = pdata;                              
    avf_->data[1] = pdata + frame_size_;
    avf_->data[2] = pdata + frame_size_ * 5 / 4;
    avf_->pts = pts_++;
    
        // 인코딩 명령을 내림.과거 버전에서는 인코딩 명령에서 결과를 받았지만
        // 최신 버전에서는 명령과 결과 리턴을 따로 처리함 -> 서로 다른 쓰레드에서
        // 명령을 처리할 수 있는 장점이 있음
    int r = avcodec_send_frame(cdc_ctx_, avf_); //frame 
    if (r < 0) {
        fprintf(stderr, "Error sending a frame for encoding\n");
        exit(1);
    }
    if (r >= 0) {

        // 인코딩된 결과를 받아옴
        r = avcodec_receive_packet(cdc_ctx_, avp_);  //packet
        if (r == 0) {

            avp_->stream_index = avf_->pts;
            return avp_;
        }
        if (r == AVERROR(EAGAIN) || r == AVERROR_EOF) {

            return NULL;
        }
    }
    return NULL;
}