module load cmake/3.15.2
module load openmpi/4.1.4
module load fftw/3.3.8
module load python/3.10
cmake .. -DCMAKE_CXX_COMPILER=mpic++ -DCMAKE_CC_COMPILER=mpicc -DCMAKE_CXX_FLAGS="-O3 -g -mtune=native" -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON

