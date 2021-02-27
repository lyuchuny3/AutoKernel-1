HALIDE_BUILD_DIR=/home/chunying/research/Halide/build
export LD_LIBRARY_PATH=${HALIDE_BUILD_DIR}/bin

echo "step1 build [xxx_gen]"
g++ gaussblur_gen.cpp \
  ${HALIDE_BUILD_DIR}/distrib/tools/GenGen.cpp \
 -I ${HALIDE_BUILD_DIR}/include/ \
 -L ${HALIDE_BUILD_DIR}/bin/ -lHalide\
  -lpthread -ldl -std=c++11 \
  -o gaussblur_gen

echo "step2 build [xxx_lib,header]"
EXE=gaussblur_gen
GEN_NAME=halide_gaussian_blur
FUNC_NAME1=halide_gaussian_blur_ksize
LIB_DIR=.
TARGET=x86-64-linux

func(){
    ./$1 \
    -g $2 \
    -f $3$4 \
    -e object,c_header\
    -o $5 \
    target=$6 \
    ksize=$4
}
func $EXE $GEN_NAME $FUNC_NAME1 3 $LIB_DIR $TARGET-no_runtime
func $EXE $GEN_NAME $FUNC_NAME1 5 $LIB_DIR $TARGET-no_runtime
func $EXE $GEN_NAME $FUNC_NAME1 7 $LIB_DIR $TARGET-no_runtime
func $EXE $GEN_NAME $FUNC_NAME1 9 $LIB_DIR $TARGET-no_runtime

./gaussblur_gen \
    -r halide_runtime_$TARGET \
    -e object,c_header\
    -o $LIB_DIR \
    target=$TARGET

ar q halide_gaussian_blur_all.a \
    halide_gaussian_blur_ksize3.o \
    halide_gaussian_blur_ksize5.o \
    halide_gaussian_blur_ksize7.o \
    halide_gaussian_blur_ksize9.o \
    halide_runtime_$TARGET.o

echo "step3 build [xxx_run]"
g++ gaussblur_run.cpp \
  halide_gaussian_blur_all.a \
 -I ${HALIDE_BUILD_DIR}/include/ \
 -lpthread -ldl -std=c++11 \
  -o gaussblur_run \
  -lopencv_core -lopencv_imgproc -O3

echo "step4 run [xxx_run]"
export HL_NUM_THREADS=1
./gaussblur_run 480 640 7 100