// chunyinglv@openailab.com
// 2020/02/18

#include "Halide.h"
using namespace Halide;

class HalideGaussianBlur:public Halide::Generator<HalideGaussianBlur>{
    public:
    // for runtime parameter
    Input<Buffer<float>> input{"input", 3}; 
    Input<Buffer<float>> kernel{"kernel", 1};
    Output<Buffer<float>> blur_x{"blur_x", 3};

    //compiler-time parameter
    GeneratorParam<uint8_t> ksize{"ksize",
                                3 /* default value */,
                                3 /* minimum value */,
                                11 /* maximum value */};
    Func blur_y;
    Var x, y, c;
    //todo pad mode select by user(runtime)
    void generate() {
        Func in_bounded = Halide::BoundaryConditions::repeat_edge(input);
        
        //         blur_y(x, y, c) = ( kernel(0) * in_bounded(x, y, c) +
        //                             kernel(1) * (in_bounded(x, y-1, c) +
        //                                         in_bounded(x, y+1, c)));
        //         blur_x(x, y, c) = (kernel(0) * blur_y(x, y, c) +
        //                             kernel(1) * (blur_y(x-1, y, c) +
        //                                             blur_y(x+1, y, c)));
        Expr tmp1,tmp2;
        int mid_ksize = (ksize-1)/2;
        tmp1 = kernel(0) * in_bounded(x, y, c);
        for(int i = 1;i <mid_ksize + 1; i++)
        {
            tmp1 = tmp1 + kernel(i) * (in_bounded(x, y-i, c) +
                                       in_bounded(x, y+i, c));
        }
        blur_y(x, y, c) = tmp1; 
        //Func blur_x;
        tmp2 = kernel(0) * blur_y(x, y, c);         
        for(int i = 1;i <mid_ksize + 1; i++)
        {
            tmp2 = tmp2 + kernel(i) * (blur_y(x-i, y, c) +
                                       blur_y(x+i, y, c));
        }
        blur_x(x, y, c) = tmp2; 
    }
    void schedule() {
        if (auto_schedule) {
            input.set_estimates({{10, 1024}, {10, 1024}, {0, 3}});
            blur_x.set_estimates({{10, 1024}, {10, 1024}, {0, 3}});
            kernel.set_estimates({{3, 11}});
        }
        else
        {
            blur_x.compute_root().vectorize(x, 8).parallel(y);
            blur_y.compute_at(blur_x, y).vectorize(x, 8);
        }
    }

};
HALIDE_REGISTER_GENERATOR(HalideGaussianBlur, halide_gaussian_blur)
