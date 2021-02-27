# the maximum level of parallelism available  4
# last-level cache size 1MB 512KB =512 * 1024 =524288  1024*1024=1048576
# the ratio between [the cost of a miss at the last level cache] and 
#                   [the cost of arithmetic on the target architecture] 60
export LD_LIBRARY_PATH=/home/chunying/research/Halide/build/bin/
./guass_blur_generator \
    -g halide_gaussian_blur \
    -f auto_halide_gaussian_blur_ksize3 \
    -e object,c_header\
    -o libarm64/ \
    target=arm-64-linux-no_runtime \
    ksize=3 \
    auto_schedule=true machine_params=4,1048576,60

./guass_blur_generator \
    -g halide_gaussian_blur \
    -f auto_halide_gaussian_blur_ksize5 \
    -e object,c_header\
    -o libarm64/ \
    target=arm-64-linux-no_runtime \
    ksize=5 \
    auto_schedule=true machine_params=4,1048576,60

./guass_blur_generator \
    -g halide_gaussian_blur \
    -f auto_halide_gaussian_blur_ksize7 \
    -e object,c_header\
    -o libarm64/ \
    target=arm-64-linux-no_runtime \
    ksize=7 \
    auto_schedule=true machine_params=4,1048576,60

./guass_blur_generator \
    -g halide_gaussian_blur \
    -f auto_halide_gaussian_blur_ksize9 \
    -e object,c_header\
    -o libarm64/ \
    target=arm-64-linux-no_runtime \
    ksize=9 \
    auto_schedule=true machine_params=4,1048576,60

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
ar q libarm64/auto_halide_gaussian_blur_all.a \
    libarm64/auto_halide_gaussian_blur_ksize3.o \
    libarm64/auto_halide_gaussian_blur_ksize5.o \
    libarm64/auto_halide_gaussian_blur_ksize7.o \
    libarm64/auto_halide_gaussian_blur_ksize9.o \
    libarm64/halide_runtime_arm64.o