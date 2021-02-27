// chunyinglv@openailab.com
// 2020/02/18

#include "halide_gaussian_blur_ksize3.h"
#include "halide_gaussian_blur_ksize5.h"
#include "halide_gaussian_blur_ksize7.h"
#include "halide_gaussian_blur_ksize9.h"
// #include "halide_blur_ksize_all.h"
// #include "auto_halide_blur_ksize_all.h"
#include "HalideBuffer.h"

#include <opencv2/imgproc/imgproc.hpp>
#include <cmath>
#include <sys/time.h>
unsigned long get_cur_time(void)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);

    return (tv.tv_sec * 1000000 + tv.tv_usec);
}

#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

void maxerr(Halide::Runtime::Buffer<float>  pred, float* gt, int h,int w)
{
    float maxError = 0.f;
    for(int j = 0; j < h; j++){
        for(int i=0; i<w; i++){
            maxError = MAX(( float )fabs(gt[j*w+i] - pred(i,j)), maxError);
        }
    }
    printf("====================================\n");
    printf("maxError is %f\n", maxError);
    printf("====================================\n");
}
void get_ker(int ksize,float sigma,float*ker)
{
    int mid=(ksize-1)/2;
    float msum=0;
    for(int i=0;i<ksize;i++)
    {
        int x=i-mid;
        float term=exp(-x*x/(2*sigma*sigma));
        msum+=term;
        ker[i]=term;
    }
    for(int i=0;i<ksize;i++)
    {
        ker[i]/=msum;
    }
}
int main(int argc, char **argv) {
    if(argc<3)
    {
        printf("[usage] exe [h][w] <ksize=7> <rep=50> <debug=0>\n");
        return 1;
    } 
    int h=atoi(argv[1]);
    int w=atoi(argv[2]);
    int ksize = 7;
    float sigma=1.f;
    int debug=0;
    int repeat_count=50;

    if(argc>=4) ksize=atoi(argv[3]);
    if(argc>=5) repeat_count=atoi(argv[4]);
    if(argc>=6) debug=atoi(argv[5]);


    typedef int (*func_t)(struct halide_buffer_t *,
                          struct halide_buffer_t *,
                          struct halide_buffer_t *);

    func_t halide_func;
    // func_t halide_func_auto;
    switch(ksize)
    {
        case 3:
            halide_func = halide_gaussian_blur_ksize3;
            // halide_func_auto = auto_halide_gaussian_blur_ksize3;
            break;
        case 5:
            halide_func = halide_gaussian_blur_ksize5;
            // halide_func_auto = auto_halide_gaussian_blur_ksize5;
            break;
        case 7:
            halide_func = halide_gaussian_blur_ksize7;
            // halide_func_auto = auto_halide_gaussian_blur_ksize;

            break;
        case 9:
            halide_func = halide_gaussian_blur_ksize9;
            // halide_func_auto = auto_halide_gaussian_blur_ksize9;

            break;
    }

    int mid_ksize = (ksize-1)/2;
    float kerval[20]={0};
    get_ker( ksize, sigma,kerval);

    cv::Mat src = cv::Mat::zeros(h,w, CV_32FC1);
    cv::Mat dst = cv::Mat::zeros(h,w, CV_32FC1);
    float* cv_in = (float*)src.data;

    Halide::Runtime::Buffer<float> input(w, h, 1);
    for (int j = 0; j < input.height(); j++) {
        for (int i = 0; i < input.width(); i++) {
            input(i,j)=(j*input.width()+i)%23;
            cv_in[j*w+i]=(j*input.width()+i)%23;
        }
    }

    Halide::Runtime::Buffer<float> kernel(mid_ksize+1);
    for(int i=0;i<mid_ksize+1;i++)
    {
        kernel(i)=kerval[i+ mid_ksize];
    }
    // kernel(0)=0.270682;
    // kernel(1)=0.216745;
    // kernel(2)=0.111281;
    // kernel(3)=0.0366328;

    Halide::Runtime::Buffer<float> output(w, h, 1);


    halide_func(input,kernel,output);
    GaussianBlur(src, dst, cv::Size(ksize, ksize),sigma, sigma,cv::BORDER_REPLICATE);
    float* out_opencv = (float*)dst.data;
    maxerr(output, out_opencv, h,w);
    if(debug)
    {
        for (int j = 0; j < output.height(); j++) {
            for (int i = 0; i < output.width(); i++) {
                printf("%f  ",output(i,j));
            }
            printf("\n");
        }
    }
    

    unsigned long t0, t1;
    float totalTime = 0;
    for (int i = 0; i < repeat_count; i++)
    {
        t0 = get_cur_time();
        halide_func(input,kernel,output);
        t1 = get_cur_time();
        totalTime += ((float)(t1 - t0) / 1000.);
    }

    printf("halide\t%.4f ms [rep %d]\n",totalTime / repeat_count, repeat_count);


    // totalTime = 0;
    // for (int i = 0; i < repeat_count; i++)
    // {
    //     t0 = get_cur_time();
    //     halide_func_auto(input,kernel,output);
    //     t1 = get_cur_time();
    //     totalTime += ((float)(t1 - t0) / 1000.);
    // }
    // printf("halide_auto\t%.4f ms [rep %d]\n",totalTime / repeat_count, repeat_count);

    totalTime = 0;
    for (int i = 0; i < repeat_count; i++)
    {
        t0 = get_cur_time();
        GaussianBlur(src, dst, cv::Size(ksize, ksize),sigma, sigma,cv::BORDER_REPLICATE);
        t1 = get_cur_time();
        totalTime += ((float)(t1 - t0) / 1000.);
    }
    printf("opencv\t%.4f ms [rep %d]\n",totalTime / repeat_count, repeat_count);
    return 0;
}
