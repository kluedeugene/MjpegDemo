
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
    // ������ ���� ���� ����
    cdc_ctx_->width = mjpeg_config.width;
    // ������ ���� ���� ����
    cdc_ctx_->height = mjpeg_config.height;
    // fps�� �°� ����
    cdc_ctx_->time_base = { 1, mjpeg_config.frame_rate };  
    // �ʴ� ȭ�� ��� �� �� ���� 1�ʴ� ~�� ���
    cdc_ctx_->framerate = { mjpeg_config.frame_rate, 1 };
    // gop ������ ����.p ������(�� ������� diff�� �����Ǵ� ����)�� ���� ����
    cdc_ctx_->gop_size = mjpeg_config.gop_size;
    // b frame(�� ����� diff�� �����Ǵ� ����)�� ���� ����
    cdc_ctx_->max_b_frames = mjpeg_config.max_b_frames;
    // �ԷµǴ� �ȼ��� ���� ����. ���� YUV�� ������.
    cdc_ctx_->pix_fmt = AV_PIX_FMT_YUVJ420P;                            //�⺻ ������ YUV420P������ MJPEG  ����� ���� YUVJ420P�� ����Ͽ���. �����۵��ϰԵ�.
    
    //cdc_ctx_->codec_id = AV_CODEC_ID_H264;
    cdc_ctx_->codec_id = AV_CODEC_ID_MJPEG;

    cdc_ctx_->codec_type = AVMEDIA_TYPE_VIDEO;
   // cdc_ctx_->qmin = 2;                                        
    cdc_ctx_->qmax = 2;                                                  // ������ ����Ƽ ���� 2~ 31    31�� ���� ������  Raw ������ ���ڵ��ؾ� �뷮�� ���Ҽ�������. 
    //cdc_ctx_->qcompress = 0.6;
    AVDictionary* dict = nullptr;
    //av_dict_set(&dict, "preset", "slow", 0);

    //av_dict_set(&dict, "pix_fmt", "yuvj420p", 0);
    av_dict_set(&dict, "src_range", "1", 0);
    av_dict_set(&dict, "dst_range", "1",0);
    av_dict_set(&dict, "preset", "veryfast", 0);                    //���ڵ� �ð�. �뷮 �ݺ��
   
   // av_dict_set(&dict, "tune", "zerolatency", 0);
   

   
    //av_dict_set(&dict, "profile", "main", 0);                      //MJPEG ���� ���� �߻��ϴ� �κ�. �ּ�ó���ϸ� �Ǳ�ȴ�.
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

AVPacket* mjpegEncoder::encode(cv::Mat& mat) {  //videocapture()�� ���� mat ������ �������� �Է¹���. const cv: mat& mat �϶� ����  

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
    
        // ���ڵ� ����� ����.���� ���������� ���ڵ� ��ɿ��� ����� �޾�����
        // �ֽ� ���������� ��ɰ� ��� ������ ���� ó���� -> ���� �ٸ� �����忡��
        // ����� ó���� �� �ִ� ������ ����
    int r = avcodec_send_frame(cdc_ctx_, avf_); //frame 
    if (r < 0) {
        fprintf(stderr, "Error sending a frame for encoding\n");
        exit(1);
    }
    if (r >= 0) {

        // ���ڵ��� ����� �޾ƿ�
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