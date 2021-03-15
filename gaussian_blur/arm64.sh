export LD_LIBRARY_PATH=/home/chunying/research/Halide/build/bin/
./guass_blur_generator \
    -g halide_gaussian_blur \
    -f halide_gaussian_blur_ksize3 \
    -e object,c_header\
    -o libarm64/ \
    target=arm-64-linux-no_runtime \
    ksize=3

./guass_blur_generator \
    -g halide_gaussian_blur \
    -f halide_gaussian_blur_ksize5 \
    -e object,c_header\
    -o libarm64/ \
    target=arm-64-linux-no_runtime \
    ksize=5

./guass_blur_generator \
    -g halide_gaussian_blur \
    -f halide_gaussian_blur_ksize7 \
    -e object,c_header\
    -o libarm64/ \
    target=arm-64-linux-no_runtime \
    ksize=7

./guass_blur_generator \
    -g halide_gaussian_blur \
    -f halide_gaussian_blur_ksize9 \
    -e object,c_header\
    -o libarm64/ \
    target=arm-64-linux-no_runtime \
    ksize=9

# We can then use the generator to emit just the runtime:
./guass_blur_generator \
    -r halide_runtime_arm64 \
    -e object,c_header\
    -o libarm64/ \
    target=arm-64-linux


# Linking the standalone runtime with the three generated object files
# gives us three versions of the pipeline for varying levels of x86,
# combined with a single runtime that will work on nearly all x86
# processors.
ar q libarm64/halide_gaussian_blur_all.a \
    libarm64/halide_gaussian_blur_ksize3.o \
    libarm64/halide_gaussian_blur_ksize5.o \
    libarm64/halide_gaussian_blur_ksize7.o \
    libarm64/halide_gaussian_blur_ksize9.o \
    libarm64/halide_runtime_arm64.o
    
aarch64-linux-gnu-g++-8 gaussblur_run.cpp \
  halide_gaussian_blur_all.a \
 -I /home/chunying/research/Halide/build/include/ \
 -lpthread -ldl -std=c++11 \
  -o gaussblur_run \
  -lopencv_core -lopencv_imgproc - O3
  
  
# copy gaussblur_run and needed headers to firefly

