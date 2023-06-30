module load compiler/gcc/11.2.0
module load mpi/openmpi/4.1.1-gcc-11.2.0
module load numlib/fftw/3.3.10-openmpi-4.1.1-gcc-11.2.0
cmake .. -DCMAKE_CXX_COMPILER=mpic++ -DCMAKE_C_COMPILER=mpicc -DCMAKE_CXX_FLAGS="-O3 -fopenmp -flto -W -march=znver2 -mtune=znver2 -mfma -mavx2 -fopt-info-vec -fopt-info-vec-optimized -finline-functions -flto -fomit-frame-pointer -fstrict-aliasing" -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON   
