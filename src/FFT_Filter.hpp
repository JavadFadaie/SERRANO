#ifndef FFTFILTER_HPP
#define FFTFILTER_HPP

#include<iostream>
#include<vector>
#include <fftw3-mpi.h>
#include <complex>
#include <cmath>
#include "signalContainer.hpp"

template<typename I,typename O>
class FFT_Filter {

 public:
 
 FFT_Filter
   ( std::vector<I> const & _InputData
   , int _rank
   , int _size);


 ~FFT_Filter(){
    // fftwf_destroy_plan(planf);
    // fftw_destroy_plan(plan);
  }


void FFTFilter_Apply();

void Caley_Tooky( std::vector<I> const & signal); 

void fft_rec( std::vector< std::complex<I> > & x
             , size_t const & N);

I * const getErrorInfo();            

void computationError();

void FFTFilter_apply();

bool powTwoCheck();

void FFTFilterParallel();

void computeMagnitute(); 
 
 
std::vector<O> const & getOutPut();

 
 private:

 int rank;
 int size;
 
 
 std::vector<I>  InputData;
 std::vector<std::complex<I>> x_out;
 
 std::vector<double[2]> InputComplexData;
 std::vector<O>  OutputData;
 std::vector<I> errorInfo;

 //fftwf_plan planf;
 //fftw_plan plan;
 std::unique_ptr< ErrorComputation<I,O> > ErrorCompute;

 int   signalIndex;
 std::string  outputpath;
 bool computingMagnitute;
 bool isFFTFilterCall;
 bool ErrorComputed; 
};

//////////////////////////////////////////
///////////// IMPLEMENTATION
//////////////////////////////////////////

template<typename I, typename O>
FFT_Filter<I,O>::FFT_Filter
          ( std::vector<I> const & _InputData
          , int _rank
          , int _size)
:InputData(_InputData)
,rank(_rank)
,size(_size)
,OutputData(_InputData.size())
,computingMagnitute(true)
,isFFTFilterCall(false)
,ErrorComputed(true)
{}



template<typename I, typename O>
void
FFT_Filter<I,O>::FFTFilter_Apply(){
   
     Caley_Tooky(InputData);     
}


template<typename I, typename O>
void
FFT_Filter<I,O>::Caley_Tooky
                ( std::vector<I> const & signal){

   x_out.resize(signal.size(),0); size_t N = signal.size(); OutputData.resize(N,0);
   for (int i = 0; i < signal.size(); i++) {
	      x_out[i] = std::complex<I>( signal[i], 0);
         x_out[i] *= 1; // Window
	}
  

   // Start recursion
   fft_rec(x_out, N);   
}

template<typename I,typename O>
void
FFT_Filter<I,O>::fft_rec
          ( std::vector< std::complex<I> > & x
          , size_t const & N)
{

   //check if it is splitted enough
    if(N <= 1){
      return;
    }
    
      
    //Split even and odd
    std::vector< std::complex<I> > odd(N/2,0);
    std::vector< std::complex<I> > even(N/2,0);
    
    // Split even and odd
    for(size_t i(0); i<N/2 ; i++){
       even[i] = x[i*2+0];
       odd[i]  = x[i*2+1];     
    }
    
    // Split on tasks
    fft_rec(even, N/2);
    fft_rec(odd, N/2);
    
    // Calculate DFT
	for (size_t k = 0; k < N/2; k++) {
		std::complex<I> t = exp(std::complex<I>(0, -2 * M_PI * k / N)) * odd[k];
		x[k] = even[k] + t;
		x[N / 2 + k] = even[k] - t;
	}
}

template<typename I, typename O>
void
FFT_Filter<I,O>::computeMagnitute()
{
  for(int i(0)
     ;    i<x_out.size()
     ;    i++)
   {
     I real = x_out[i].real(); I imag = x_out[i].imag(); 
     O magnitute ( sqrt(  pow( real,2 ) 
                        + pow( imag,2 ) 
                       )     
                 );
    //std::cout << x_out[i].real() <<"  " << x_out[i].imag() << std::endl;           
     OutputData[i] = magnitute;
   }

}



template<typename I, typename O>
std::vector<O> const &
FFT_Filter<I,O>::getOutPut()
{
   return OutputData;
}


 

template<typename I, typename O>
void
FFT_Filter<I,O>::computationError()
{
   //Specific implementation is required to compue the accuracy of the reault
   
}             


template<typename I,typename O>
void
FFT_Filter<I,O>::FFTFilter_apply()
{
   isFFTFilterCall = true;
    
   if(ErrorComputed)
   { 
     FFTFilterParallel();
     computationError();
   }
   {
     FFTFilterParallel();
   }   
  
}

template<typename I,typename O>
I * const 
FFT_Filter<I,O>::getErrorInfo(){
   return & errorInfo[0];
}


template<typename I,typename O>
void
FFT_Filter<I,O>::FFTFilterParallel()
{

   /*
   int iter(0);
      for(int i(0); i<InputData.size(); i++)
      {
         InputComplexData[iter][0]=InputData[i];
         InputComplexData[iter][1]=0;
         std::cout << InputComplexData[i][0] << "  " << InputComplexData[i][1] << std::endl;
         iter++;
      }

   if(powTwoCheck() == true)
   {
      if((std::is_same<I, float>::value))
      {   
      
      planf = 
      fftwf_mpi_plan_dft_1d
        ( N
        , & InputComplexData[0]
        , & InputComplexData[0]
        , MPI_COMM_WORLD
        , FFTW_FORWARD
        , FFTW_ESTIMATE);

     fftw_execute(planf);  
     
     }
     if((std::is_same<I, double>::value))
     {
     
      plan = 
      fftw_mpi_plan_dft_1d
        ( N
        , & InputComplexData[0]
        , & InputComplexData[0]
        , MPI_COMM_WORLD
        , FFTW_FORWARD
        , FFTW_ESTIMATE);

     fftw_execute(plan);
    } 
   }
   else
   {
     if(rank == 0)
      std::cout << "The Data set is not suitable for FFT Filter "<<
      "it should be shorten to power of two number of element"<< std::endl;
   }
  
   //Compute magnitute of spectrum
  if(computingMagnitute)
  {
    computeMagnitute();
  }
  */
}




template<typename I,typename O>
bool
FFT_Filter<I,O>::powTwoCheck(){
 size_t N = InputData.size();
 bool isPowTwo(true);
 size_t N_global(N);
 while(true){

   int res (N_global%2);
   if(res != 0){
      isPowTwo = false;
      break;
   }
   N_global=N_global/2;
   if(N_global==1){
      break;
   }
}

return isPowTwo;

}
#endif