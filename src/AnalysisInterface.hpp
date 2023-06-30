#ifndef ANALYSIS_INTERFACE_HPP
#define ANALYSIS_INTERFACE_HPP

#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <chrono>
#include <algorithm>
#include <mpi.h>
#include <omp.h>
#include "DistributedTimer.hpp"
#include "MinMaxAnalysis.hpp"
#include "KalmanFilter.hpp"
#include <cstring>
#include <memory>
#include "kernelFlops.hpp"
#include <fstream>
#include <filesystem>
#include "FFT_Filter.hpp"
#include "ReadWriteMPI_IO.hpp"
#include "kernelFlops.hpp"
#include "SavitzkeyGolay.hpp"
#include "Profilling.hpp"

template<typename I, typename O>
class AnalysisInterface{

public :

AnalysisInterface
   ( int _rank
   , int _size );

~AnalysisInterface()
{}

void applyingFilters
       ( bool _Kalman_Filter
       , bool _FFT_Filter
       , bool _MinMax_Transform
       , bool _savitzkyGoly_Transform );

void apply();

void profillingApply();

void filtersApply();

void DistrbutionData();

void perforationStride(int _s);

void MinMaxUpdate_apply
       (std::vector<I> const & Input_Buffer);

void KalmanFilter_apply       
       (std::vector<I> const & Input_Buffer);

void FFTFilter_apply
       (std::vector<I> const & Input_Buffer);

void SavitzkyGoly_apply       
       (std::vector<I> const & Input_Buffer);

std::vector<I> const & ReadInputBuffer();

void 
writeOutputBuffer();
 
void
set_KalmanFilterConfiguration
       ( I const _R
       , I const _H
       , I const _Q
       , I       _P
       , I       _K);
void
Data_IO_Path
       ( std::string _inputDataPath
       , std::string _kalmanDataPath
       , std::string _fftDataPath
       , std::string _minMaxDataPath
       , std::string _SavitzkeyGoly); 

void
profileWorkSpace( std::string _profilePath); 


void
set_MinMaxConfiguration
       ( I const &  low_th
       , I const &  high_th);
             

void 
set_numThread(int const _nThread);

void 
benchmarkStatus(bool const _benchmarkState);

private:

int rank;
int size;
int nThread;
I  low_th;
I  high_th;
size_t TotalNumData;
double fileSizeGB;
I  R, H, Q, P, K, u_hat;
int s;

bool Kalman_FilterApplied;
bool FFT_FilterApplied;
bool MinMax_TransformApplied;
bool savitzkyGoly_Transform;
bool writingOutputData;

bool benchmarkState;
 
std::vector<size_t> numDataProc;
std::vector<size_t> offsetVec;
   
std::unique_ptr<MinMaxAnalysis<I,O>>  Maxmin;
std::unique_ptr< KalmanFilter<I,O> >  kalmanFilter;
std::unique_ptr< FFT_Filter<I,O> >    FFT_filter;
std::unique_ptr<ReadWriteMPI_IO<I,O>> readWriteParallel;
std::unique_ptr<SavitzkyGoly<I,O>>    savitzkyGoly;

std::unique_ptr<DistributedTimer<I,O>> Timer;
std::unique_ptr<ProfileHPC<I,O>>      profile;

std::string inputDataPath;
std::string kalmanDataPath;
std::string fftDataPath;
std::string minMaxDataPath;
std::string SavitzkeyGoly;
std::string profilePath;

flops ReadFlops = {0,0,0,sizeof(I),0}; 
flops WriteFlops = {0,0,0,0,sizeof(I)}; 
flops FFTInterData = {0,0,0,sizeof(I),0}; 
flops KalmanFlopsInfo = {7,1,5,sizeof(I),sizeof(O)};
flops MinMaxFlopsInfo = {1,1,4,2*sizeof(I),sizeof(O)};
flops SavitzkeyGolayyFlops  = {6,0,4,sizeof(I),sizeof(O)};
};

///////////////////////////////////////
////////// IMPLEMENTATION
///////////////////////////////////////

template<typename I, typename O>
AnalysisInterface<I,O>::AnalysisInterface
( int _rank, int _size)
:rank(_rank)
,size(_size)
,numDataProc(size)
,offsetVec(size)
,writingOutputData(true)
,s(1)
{
  Timer = std::unique_ptr< DistributedTimer<I,O> >
            ( new DistributedTimer<I,O>(rank,size) );
  profile = std::unique_ptr< ProfileHPC<I,O> >
            ( new ProfileHPC<I,O>(rank,size) );          
}


template<typename I,typename O>
void 
AnalysisInterface<I,O>::DistrbutionData(){

  //TO DO try to set the the path to be searched automated
FILE *p_file;
p_file = fopen(inputDataPath.c_str(),"rb");
fseek(p_file,0,SEEK_END);
size_t sizefile = ftell(p_file);
fileSizeGB = sizefile/pow(10,9);
TotalNumData= sizefile/sizeof(I);
fclose(p_file);

size_t rem(TotalNumData%size);
size_t displac(0);
for(int i(0); i<size; i++){
  numDataProc[i] = TotalNumData/size;
  offsetVec[i]   = displac; 
    if(rem > 0){
      numDataProc[i]++;
      rem--;      
    }
  displac += numDataProc[i];
 }

}



template<typename I,typename O>
void
AnalysisInterface<I,O>::perforationStride(int _s)
{   
   s = _s;         
}

template<typename I,typename O>
void
AnalysisInterface<I,O>::set_numThread(int const _nThread){
  nThread = _nThread;
}

template<typename I, typename O>
void 
AnalysisInterface<I,O>::benchmarkStatus(bool const _benchmarkState){
    benchmarkState = _benchmarkState;
}

template<typename I,typename O>
void
AnalysisInterface<I,O>::set_MinMaxConfiguration
             ( I const &  _low_th
             , I const & _high_th)
{
  low_th = _low_th;
  high_th = _high_th;
}

template<typename I, typename O>
void
AnalysisInterface<I,O>::applyingFilters
    ( bool _Kalman_FilterApplied
    , bool _FFT_FilterApplied
    , bool _MinMax_TransformApplied
    , bool _savitzkyGoly_Transform )
{
   Kalman_FilterApplied    = _Kalman_FilterApplied;
   FFT_FilterApplied       = _FFT_FilterApplied;
   MinMax_TransformApplied = _MinMax_TransformApplied; 
   savitzkyGoly_Transform  = _savitzkyGoly_Transform;
}       

template<typename I,typename O>
void
AnalysisInterface<I,O>::profileWorkSpace( std::string _profilePath){
    profilePath = _profilePath;
}

template<typename I,typename O>
void
AnalysisInterface<I,O>
   ::set_KalmanFilterConfiguration
    ( I const _R
    , I const _H
    , I const _Q
    , I       _P
    , I       _K)
{
  R = _R;
  H = _H;
  Q = _Q;
  P = _P;
  K = _K;
}             

template<typename I,typename O>
void
AnalysisInterface<I,O>
   ::Data_IO_Path
    ( std::string _inputDataPath
    , std::string _kalmanDataPath
    , std::string _fftDataPath
    , std::string _minMaxDataPath
    , std::string _SavitzkeyGoly)
{
  inputDataPath  = _inputDataPath;
  kalmanDataPath = _kalmanDataPath;
  fftDataPath    = _fftDataPath;
  minMaxDataPath = _minMaxDataPath;
  SavitzkeyGoly  = _SavitzkeyGoly;  
}

template<typename I,typename O>
void 
AnalysisInterface<I,O>::apply()
{
        //Distribute the Data across the ranks 
  DistrbutionData();

        //Applying the filters
  filtersApply();
}

template<typename I,typename O>
void 
AnalysisInterface<I,O>::profillingApply()
{
  profile->pathProfileSetting
             (profilePath
             ,kalmanDataPath
             ,minMaxDataPath
             ,fftDataPath
             ,SavitzkeyGoly);
             
  profile->applyingFilters
             ( Kalman_FilterApplied
             , FFT_FilterApplied
             , MinMax_TransformApplied
             , savitzkyGoly_Transform );  
  
  profile->perforationStride(s); 
  profile->setFileSize(fileSizeGB);
  profile->benchmarkStatus(benchmarkState);
  profile->execuationTimeInformation(Timer->timerInformation());   
  profile->computeEnergy();
  profile->generateProfillingFile();
}  

template<typename I,typename O>
void 
AnalysisInterface<I,O>::filtersApply(){

         //Allocate the number of elment to the rank
 size_t allocLocal = numDataProc[rank] ;

        //Read and populate the local Buffer
  Timer->start("R");
  std::vector<I> const & InputData(ReadInputBuffer());     
  Timer->stop("R");
  
 
        //Execute Parallel Kalman Filter
if(Kalman_FilterApplied)
{ 
  Timer->start("K");
  KalmanFilter_apply(InputData);  
  Timer->stop("K");
}

       //Execute the Min Max and Update Analysis
if(MinMax_TransformApplied)
{
  Timer->start("M");
  MinMaxUpdate_apply(InputData);        
  Timer->stop("M");
}
 
        //Execute Parallel Fast Fourier Transformation
if(FFT_FilterApplied)
{
  Timer->start("F");
  FFTFilter_apply(InputData);
  Timer->stop("F");
}
     
        //Execute of Parallel Savitzkey Goly Filter
if(savitzkyGoly_Transform)
{ 
  Timer->start("S");
  SavitzkyGoly_apply(InputData);
  Timer->stop("S");
}
        //Write the output local Buffer
if(writingOutputData)
{        
  Timer->start("W");       
  writeOutputBuffer();
  Timer->stop("W");
}

}

     


template<typename I,typename O>
void
AnalysisInterface<I,O>::KalmanFilter_apply       
       (std::vector<I> const & Input_Buffer){
   
  kalmanFilter = std::unique_ptr< KalmanFilter<I,O> >
           (new KalmanFilter<I,O>(Input_Buffer,rank,size)  );
 
  kalmanFilter->set_KalmanFilterConfiguration
    ( R
    , H
    , Q
    , P
    , K);
   kalmanFilter->set_numThread(nThread); 
   kalmanFilter->perforationStride(s);
   kalmanFilter->KalmanFilterApply();
   profile->errorInfoFilters(kalmanFilter->getErrorInfo());
  
}


template<typename I,typename O>
void 
AnalysisInterface<I,O>:: MinMaxUpdate_apply
      (std::vector<I> const & Input_Buffer)
{

  Maxmin = std::unique_ptr< MinMaxAnalysis<I,O> >
          (new MinMaxAnalysis<I,O>(Input_Buffer, rank,size)  );
 
  Maxmin->MinMaxParameter(low_th, high_th);
  Maxmin->perforationStride(s);
  Maxmin->MinMaxUpdate_apply();
  profile->errorInfoFilters(Maxmin->getErrorInfo());
}    

template<typename I,typename O>
void
AnalysisInterface<I,O>::FFTFilter_apply
      (std::vector<I> const & Input_Buffer)
{ 
  /*
  FFT_filter = std::unique_ptr< FFT_Filter<I,O> >
             (new FFT_Filter<I,O>(Input_Buffer, rank, size, TotalNumData, s) );
  FFT_filter->FFTFilter_apply();
  //profile->errorInfoKalmanFilter(FFT_filter->getErrorInfo());
  */
}      

template<typename I,typename O>
void
AnalysisInterface<I,O>::SavitzkyGoly_apply       
       (std::vector<I> const & Input_Buffer){

 savitzkyGoly = std::unique_ptr< SavitzkyGoly<I,O> >
                     ( new SavitzkyGoly<I,O> (Input_Buffer,rank, size));
 savitzkyGoly->perforationStride(s);          
 savitzkyGoly->savitzkyGolay_apply(); 
 profile->errorInfoFilters( savitzkyGoly->getErrorInfo());

}


template<typename I,typename O>
std::vector<I> const & 
AnalysisInterface<I,O>::ReadInputBuffer()
{

  readWriteParallel = std::unique_ptr< ReadWriteMPI_IO<I,O> >
           ( new ReadWriteMPI_IO<I,O>(rank, size, numDataProc, offsetVec) );
 
  readWriteParallel->inputFileSetPath(inputDataPath);
  readWriteParallel->readInputBuffer();

  return readWriteParallel->InputBuffer();
}


template<typename I,typename O>
void
AnalysisInterface<I,O>::writeOutputBuffer()
{
     // write file for min max
  if(MinMax_TransformApplied) 
  { 
    readWriteParallel->outputFileSetPath(minMaxDataPath);
    readWriteParallel->writeOutputBuffer( Maxmin->getOutPut() );
  }
     // write file for Kalman filter
  if(Kalman_FilterApplied)
  {
    readWriteParallel->outputFileSetPath(kalmanDataPath);
    readWriteParallel->writeOutputBuffer( kalmanFilter->getOutPut() );
  }
    // write file for FFT filter
  if(FFT_FilterApplied)
  { 
    readWriteParallel->outputFileSetPath(fftDataPath);
    readWriteParallel->writeOutputBuffer(  FFT_filter->getOutPut() );
  }
    //write the Savitzkey Goly filter
  if(savitzkyGoly_Transform)
  { 
    readWriteParallel->outputFileSetPath(SavitzkeyGoly);
    readWriteParallel->writeOutputBuffer( savitzkyGoly->getOutPut() );
  } 

}
              

#endif
