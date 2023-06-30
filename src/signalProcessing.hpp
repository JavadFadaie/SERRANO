#ifndef IDEKOUSECASE_HPP
#define IDEKOUSECASE_HPP

#include<iostream>
#include<vector>
#include <cstdlib>
#include <cstring>
#include "signal_Distributor.hpp"
#include <FFT_Filter.hpp>
#include <memory>
#include "signalContainer.hpp"
#include "handelReadWrite.hpp"
#include "BlackScholes.hpp"
#include "WaveletFilter.hpp"

//https://towardsdatascience.com/time-series-classification-using-dynamic-time-warping-61dcd9e143f6
//https://stackoverflow.com/questions/44945098/how-can-i-use-knn-k-means-to-clustering-time-series-in-a-dataframe
  //Fast DTW
//https://www.audiolabs-erlangen.de/resources/MIR/FMP/C3/C3S2_DTWbasic.html
template<typename I, typename O>
class SignalProcessing{
   
 public:

 
/*
 struct Index2SensorName{
    Index2SensorName(int _signal_Id
                    ,std::string _sensor_name )
       :signal_Id(_signal_Id)
       ,sensor_name(_sensor_name)
       {}

   int signal_Id;
   std::string sensor_name;
  };
*/

 public : 

 SignalProcessing(int _rank, int _size);

 void apply();

 void applyKernel();

 void loading_Signal(); 
 
 void apply_Filters();

 void Data_IO_Path( std::string const & _inputDataPath);

 
 void perforationStride(int _s);

 void controlQualityLoop(double const _epsilon);

 void applyingFilters
     ( bool _Kalman_FilterApplied
     , bool _FFT_FilterApplied
     , bool _BlackScholes_Applied
     , bool _savitzkyGoly_Transform
     , bool _Wavelet_Filter );
                     
void profillingApply();

void set_KalmanFilterConfiguration( I const _R
                                  , I const _H
                                  , I const _Q
                                  , I       _P
                                  , I       _K);

void set_MinMaxConfiguration( I const &  _low_th
                            , I const & _high_th);

void Data_WorkSpace( std::string const & _data_Workspace);

void Profile_WorkSpace( std::string const & _profile_workspace);

void benchmarkStatus(bool const _benchmarkState);

void signalDistribution();

void setFilterSignalPath();

void apply_filter(std::vector<typename::signalContainer<I>> const & signalVec);

std::vector<O> filtersApply(std::vector<I> const & Input_Buffer, int signal_Index);


std::vector<O> const & 
KalmanFilter_apply(std::vector<I> const & Input_Buffer);

std::vector<O> const & 
SavitzkyGoly_apply(std::vector<I> const & Input_Buffer);

std::vector<O> const & 
FFTFilter_apply(std::vector<I> const & Input_Buffer);

std::vector<O> const &
WaveletFilter_apply(std::vector<I> const & Input_Buffer);

std::vector<O> const &
BlackScholes_apply(std::vector<I> const & Input_Buffer, int  signal_Index);

void computation_error_apply();

void profillingAndErrorComputation();

void writeVecSignal();

void Disply(std::vector<I> const & data);

void writeBinaryFile(std::vector<O> const & data, int ID);

 
 private:

 int rank;
 int size;
 
 std::unique_ptr<signalDistributor<I>> distribute;

 std::vector<typename::signalContainer<I>>  inputSignalContainerVec;
 std::vector<typename::signalContainer<O>>  outputSignalContainerVec;
   
 std::unique_ptr< ErrorComputation<I,O> > ErrorCompute;
  
 std::string inputDataPath;
 std::string outputpath;

 std::string data_workspace;
 std::string profile_workspace;
 
 std::string  kalman_filter_path;    
 std::string  blackScholes_path;
 std::string  fft_filter_path;  
 std::string  savitzkey_glay_path;  
 std::string  wavelet_filter_path;
 
std::unique_ptr<KalmanFilter<I,O>>    kalmanFilter;
std::unique_ptr<MinMaxAnalysis<I,O>>  minMaxTransform;
std::unique_ptr<FFT_Filter<I,O>>      FFT_filter;
std::unique_ptr<SavitzkyGoly<I,O>>    savitzkyGoly;
std::unique_ptr<BlackScholes<I,O>>    blackScholes;
std::unique_ptr<Wavelet_Filter<I,O>>  wavelet_filter;

bool Kalman_FilterApplied;
bool FFT_FilterApplied;
bool BlackScholes_Applied;
bool savitzkyGoly_Transform;
bool Wavelet_filter;
bool benchmarkState;


I  R, H, Q, P, K; 
I  low_th;
I  high_th;

int s;
int nThread;
double epsilon;

std::unique_ptr<DistributedTimer<I,O>> Timer;
std::unique_ptr<ProfileHPC<I,O>>      profile;
std::vector<O> outputNull;

double   relErrorInfo;
readWriteTool<I,O> tool;

};

//////////////////////////////////
/////////////IMPLEMENTATION
///////////////////////////////////

template<typename I, typename O>
SignalProcessing<I,O>::SignalProcessing(int _rank, int _size)
:rank(_rank)
,size(_size)
,fft_filter_path("/Output_Data/FFTFilter")
,kalman_filter_path("/Output_Data/kalmanFilter")    
,blackScholes_path("/Output_Data/BlackScholes")
,savitzkey_glay_path("/Output_Data/SavitzkeyGolay")
,wavelet_filter_path("/Output_Data/WaveletFilter")
,nThread(1)
,epsilon(0)
,s(1)
,relErrorInfo(0)
{
  Timer = std::unique_ptr< DistributedTimer<I,O> >
            ( new DistributedTimer<I,O>(rank,size) );
  profile = std::unique_ptr< ProfileHPC<I,O> >
            ( new ProfileHPC<I,O>(rank,size) );

}


template<typename I,typename O>
void
SignalProcessing<I,O>::Data_IO_Path
               ( std::string const & _inputDataPath){
  inputDataPath  = _inputDataPath;
}


template<typename I,typename O>
void
SignalProcessing<I,O>::Data_WorkSpace( std::string const & _data_workspace){
  data_workspace = _data_workspace;
}

template<typename I,typename O>
void
SignalProcessing<I,O>::Profile_WorkSpace( std::string const & _profile_workspace){
  profile_workspace = _profile_workspace;
}

template<typename I, typename O>
void
SignalProcessing<I,O>::applyingFilters
    ( bool _Kalman_FilterApplied
    , bool _FFT_FilterApplied
    , bool _BlackScholes_Applied
    , bool _savitzkyGoly_Transform
    , bool _Wavelet_filter )
{
   Kalman_FilterApplied    = _Kalman_FilterApplied;
   FFT_FilterApplied       = _FFT_FilterApplied;
   BlackScholes_Applied    = _BlackScholes_Applied; 
   savitzkyGoly_Transform  = _savitzkyGoly_Transform;
   Wavelet_filter          = _Wavelet_filter; 
}  

template<typename I,typename O>
void
SignalProcessing<I,O>::set_KalmanFilterConfiguration( I const _R
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
SignalProcessing<I,O>::set_MinMaxConfiguration( I const &  _low_th
                                              , I const & _high_th)
{
  low_th = _low_th;
  high_th = _high_th;
}

template<typename I,typename O>
void
SignalProcessing<I,O>::perforationStride(int _s)
{   
   s = _s;         
}

template<typename I, typename O>
void
SignalProcessing<I,O>::controlQualityLoop(double const _epsilon){
   epsilon = _epsilon;
}

template<typename I, typename O>
void 
SignalProcessing<I,O>::benchmarkStatus(bool const _benchmarkState){
   benchmarkState = _benchmarkState;
}

template<typename I,typename O>
void
SignalProcessing<I,O>::setFilterSignalPath()
{
   kalman_filter_path = data_workspace + kalman_filter_path;    
   fft_filter_path = data_workspace + fft_filter_path;  
   blackScholes_path = data_workspace + blackScholes_path;
   savitzkey_glay_path = data_workspace + savitzkey_glay_path;  
}

template<typename I,typename O>
void
SignalProcessing<I,O>::signalDistribution()
{
   distribute = std::unique_ptr<signalDistributor<I> >(
                         new signalDistributor<I>(rank, size)
                                  );
   distribute->Data_IO_Path( inputDataPath);
   distribute->apply();
}



template<typename I,typename O>
void
SignalProcessing<I,O>::apply()
{
     applyKernel();
   
   profillingAndErrorComputation();
}

template<typename I,typename O>
void
SignalProcessing<I,O>::applyKernel()
{
 
    Timer->start("R");
      loading_Signal();
    Timer->stop("R");

    Timer->start("K");
      apply_Filters();
    Timer->stop("K"); 

    Timer->start("W");
       writeVecSignal();
    Timer->stop("W"); 

}

template<typename I,typename O>
void
SignalProcessing<I,O>::profillingAndErrorComputation()
{
    //computation_error_apply();
 
    profillingApply();
}

template<typename I,typename O>
void
SignalProcessing<I,O>::loading_Signal()
{
   setFilterSignalPath();
   signalDistribution();
   for(int i(0); i< distribute->procSignalFiles().size(); i++){
         int classID = -1;
         std::string fullPath = distribute->procSignalFiles()[i];
         int signalIndex = distribute->procSignalIndex()[i];
         std::vector<I> const & inputSignalVal( tool.readBinary(fullPath) );
         typename::signalContainer<I> inputSignal(signalIndex,inputSignalVal,classID);
       	 inputSignalContainerVec.push_back(inputSignal);   
   }
  
}


template<typename I,typename O>
void
SignalProcessing<I,O>::apply_Filters()
{ 
   for(int i(0); i< inputSignalContainerVec.size(); i++){
         int classID = -1;
         std::vector<I> const & inputSignalVal (inputSignalContainerVec[i].signalVal); 
         int signalIndex(inputSignalContainerVec[i].signalId); 
         std::vector<O> const & outputSignalVal (filtersApply(inputSignalVal,signalIndex) );
         typename::signalContainer<O> outputSignal(signalIndex,outputSignalVal,classID);
       	 outputSignalContainerVec.push_back(outputSignal);   
    }  
   
}


template<typename I,typename O>
std::vector<O> 
SignalProcessing<I,O>::filtersApply(std::vector<I> const & Input_Buffer, int signal_Index){

  
if(Kalman_FilterApplied)
{     //Calling the Kalman Filter 
  return KalmanFilter_apply(Input_Buffer);  
}            
else if(FFT_FilterApplied)
{    //Caklling Fast Fourier Transformation
  return FFTFilter_apply(Input_Buffer);
}  
else if(savitzkyGoly_Transform)
{    //Calling the Savitzkey Golay Filter  
  return SavitzkyGoly_apply(Input_Buffer);
}
else if(BlackScholes_Applied)
{    //Calling the Savitzkey Golay Filter  
  return BlackScholes_apply(Input_Buffer, signal_Index);
}else{

  return outputNull;
}

}

template<typename I, typename O>
void
SignalProcessing<I,O>::computation_error_apply(){

   ErrorCompute = std::unique_ptr<ErrorComputation<I,O>>
                 (new ErrorComputation<I,O>
                        (rank
                       , size)
                 );
   ErrorCompute->perforationStride(s); 

  for(int i(0); i<outputSignalContainerVec.size(); i++)
  {   
   std::vector<I> const & inputSignal(inputSignalContainerVec[i].signalVal);
   std::vector<O> const & outputSignal(outputSignalContainerVec[i].signalVal);
   ErrorCompute->computeError( outputSignal, inputSignal);
  } 
  

 // std::cout << errorInfo[0] << "  " << errorInfo[1] << std::endl; 

}



template<typename I,typename O>
void 
SignalProcessing<I,O>::profillingApply()
{ 
  profile->signalProcessingDataPath(kalman_filter_path
                                   ,blackScholes_path
                                   ,fft_filter_path
                                   ,savitzkey_glay_path);

  profile->Data_WorkSpace(data_workspace); 
  profile->Profile_WorkSpace(profile_workspace);  

  profile->applyingFilters
             ( Kalman_FilterApplied
             , FFT_FilterApplied
             , BlackScholes_Applied
             , savitzkyGoly_Transform );  

  profile->perforationStride(s); 
  profile->controlQualityLoop(epsilon);
  profile->setFileSize(tool.numFileInsideDirectory(inputDataPath));
  profile->benchmarkStatus(benchmarkState);
  profile->execuationTimeInformation(Timer->timerInformation());   
  profile->errorInfoFilters(relErrorInfo);
  profile->computeEnergy();
  profile->generateProfillingFile();
 
}

template<typename I,typename O>
std::vector<O> const &
SignalProcessing<I,O>::FFTFilter_apply(std::vector<I> const & Input_Buffer)
{
   FFT_filter = std::unique_ptr< FFT_Filter<I,O> >
               (new FFT_Filter<I,O>(Input_Buffer, rank, size) );
    
   FFT_filter->FFTFilter_Apply();
   FFT_filter->computeMagnitute();
   outputpath = fft_filter_path;
  return FFT_filter->getOutPut();
 
}   


template<typename I,typename O>
std::vector<O> const &
SignalProcessing<I,O>::KalmanFilter_apply(std::vector<I> const & Input_Buffer){
   
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

   outputpath =  kalman_filter_path;
   kalmanFilter->Kalman_filtering_apply();
   relErrorInfo = kalmanFilter->getError();
  
 return  kalmanFilter->getOutPut();
}

template<typename I,typename O>
std::vector<O> const &
SignalProcessing<I,O>::WaveletFilter_apply(std::vector<I> const & Input_Buffer){

  wavelet_filter = std::unique_ptr<Wavelet_Filter<I,O>>
             (new Wavelet_Filter<I,O>(Input_Buffer, rank, size));
                
  wavelet_filter->Wavelet_Filter_Apply();

  return wavelet_filter->getOutPut();
}

template<typename I,typename O>
std::vector<O> const &
SignalProcessing<I,O>::BlackScholes_apply(std::vector<I> const & Input_Buffer, int  signal_Index)
{
  blackScholes = std::unique_ptr< BlackScholes<I,O> >
          (new BlackScholes<I,O>( rank,size) );

  blackScholes->Data_WorkSpace( data_workspace); 

  blackScholes->blackscholes_apply(Input_Buffer, signal_Index); 
  outputpath =  blackScholes_path;
  
  return blackScholes->getOutPut();      
}


template<typename I,typename O>
std::vector<O> const & 
SignalProcessing<I,O>::SavitzkyGoly_apply(std::vector<I> const & Input_Buffer){

 savitzkyGoly = std::unique_ptr< SavitzkyGoly<I,O> >
                     ( new SavitzkyGoly<I,O> (Input_Buffer,rank, size));
 
 savitzkyGoly->perforationStride(s);          
 
 outputpath =  savitzkey_glay_path;
 savitzkyGoly->Savitzky_Goly_apply();
 relErrorInfo = savitzkyGoly->getError();

 return savitzkyGoly->getOutPut(); 
}


template<typename I,typename O>
void
SignalProcessing<I,O>::Disply(std::vector<I> const & data){
    std::cout << "this is data size  "<< data.size() << std::endl;
    for(int i(0); i<data.size(); i++){
        std::cout<< rank << "  " <<i<<"  " << data[i] << std::endl;
    }
}

template<typename I, typename O>
void
SignalProcessing<I,O>::writeVecSignal(){
   bool write2TextFile(false); bool write2BinFile(true);
   for(int i(0); i<outputSignalContainerVec.size(); i++){
     int signalIndex = outputSignalContainerVec[i].signalId;
     std::vector<O> const & outputSignal (outputSignalContainerVec[i].signalVal);
     std::string freqsignal = outputpath + "/signal_filter_"+ std::to_string(signalIndex); 
     if(write2BinFile){
       tool.writeBuffer2Binary( outputSignal , freqsignal);
     }
     if(write2TextFile){
       tool.writeBuffer2Text( outputSignal , freqsignal);
     }
   }  
}



template<typename I,typename O>
void 
SignalProcessing<I,O>::apply_filter(std::vector<typename::signalContainer<I>> const & signalVec){

/*
  if(Kalman_FilterApplied)
  {
    Timer->start("K");
    KalmanFilter_apply(signalVec);  
    Timer->stop("K");
  }
 
  if(MinMax_TransformApplied)
  {
    Timer->start("M");
    MinMaxUpdate_apply(signalVec);
    Timer->stop("M");
  }
  
  if(FFT_FilterApplied)
  { 
    Timer->start("F");
    FFTFilter_apply(signalVec);
    Timer->stop("F");
  }

  if(savitzkyGoly_Transform)
  { std::cout << "this is sav gol filter "<< std::endl;
    Timer->start("S");
    SavitzkyGoly_apply(signalVec);
    Timer->stop("S");
  }
*/
}

#endif
