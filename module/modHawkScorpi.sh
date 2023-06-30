 module load cmake/3.16.4
 module load openmpi/4.0.4
 module load fftw/3.3.8
 module load scorep
 cmake .. -DCMAKE_CXX_COMPILER=scorep-mpic++ -DCMAKE_CC_COMPILER=scorep-mpicc -DCMAKE_CXX_FLAGS="-O3 -g -mtune=native" -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON


