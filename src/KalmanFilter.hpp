#ifndef KALMANFILTER_HPP
#define KALMANFILTER_HPP

#include <iostream>
#include <vector>
#include <cmath>
#include "UniformPartition.hpp"
#include "KalmanFilter.hpp"
#include <omp.h>
#include <mpi.h>
#include <algorithm>
#include <numeric>
#include "kernelFlops.hpp"
#include "DistributedErrorComputation.hpp"
#include <memory>
#include "signalContainer.hpp"

template<typename I,typename O>
class KalmanFilter{

  
  public:
  
  KalmanFilter
       ( std::vector<I> const & _InputData
       , int const & _rank
       , int const & _size  );

  
  KalmanFilter
       ( std::vector<I> const & _InputData);
  
  
  
  void Kalman_filtering_apply();

  void 
  Kalman_filtering(std::vector<I> const & inputSignal);

  
 ~ KalmanFilter(){}             
  
  void KalmanFilterApply();

  void computationError();

  void Kalmanfilter();

  void KalmanfilterPerforated();

  void KalmanFilterHybridParallel();
  
  void DistributedKalmanFilter();

  void DistributedKalmanFilterPerforation();
  
  void KalmanFilterSequential_inline();
  
  void KalmanFilterSequential_chunk();

  void
  KalmanFilterFunc
       ( I & U
       , I & U_hat
       , I & K
       , I & P);
  
              
  void
  set_KalmanFilterConfiguration
       ( I const _R
       , I const _H
       , I const _Q
       , I       _P
       , I       _K);   
  

  void set_numThread(int const _nThread);                

  void perforationStride(int _s);  
  
  std::vector<O> const & 
  getOutPut();

  I * const getErrorInfo();   

  double const  getError();

  private: 
  
  struct kalman {
    I P_temp;
    I K_temp;
    I U_temp;
    I array[5]; 
   };

  int  rank;
  int  size;
  int  nThread;
  I  F, R, H, Q, P, K;
  
  I const transition_matrices = 1; // F
  I const observation_matrices= 1; // H
  I const initial_state_covariance= 1; // P
  I const observation_covariance= 1; //   R
  I const  transition_covariance=1; //  Q
 
  
  std::vector<I> const  InputData;
  std::vector<O> OutputData;
  std::vector<O> outputSignal;
  std::vector<I> InputDataPerforated;
  std::vector<O> OutputDataPerforated;
  
  std::vector<kalman> threadInfo;
  std::vector<kalman> kalmanParInfo;
  std::vector<I> sendData;
  std::vector<I> receiveData;
  std::unique_ptr< ErrorComputation<I,O> > ErrorCompute;
 
  bool isKalmanFilterCall;
  bool ErrorComputed;
  int  s;
  double  relErrorInfo;
  I * errorInfo;
};

  
///////////////////////////////////
//////// IMPLEMENT
////////////////////////////////////


template<typename I,typename O>
KalmanFilter<I,O>::KalmanFilter
( std::vector<I> const & _InputData
, int const & _rank
, int const & _size)
:InputData(_InputData)
,OutputData(_InputData.size())
,rank(_rank)
,size(_size)
,sendData(3)
,receiveData(3)
,nThread(0)
,isKalmanFilterCall(false)
,ErrorComputed(true)
{}


template<typename I,typename O>
KalmanFilter<I,O>::KalmanFilter
( std::vector<I> const & _InputData)
:InputData(_InputData)
,OutputData(_InputData.size())
,sendData(3)
,receiveData(3)
{}


template<typename I,typename O>
void
KalmanFilter<I,O>
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
KalmanFilter<I,O>::perforationStride(int _s)
{   
   s = _s;         
}

template<typename I,typename O>
void
KalmanFilter<I,O>::set_numThread(int const _nThread){
  nThread = _nThread;
}



template<typename I,typename O>
void
KalmanFilter<I,O>::Kalman_filtering_apply()
{ 
    Kalman_filtering(InputData);
}


template<typename I,typename O>
void
KalmanFilter<I,O>::Kalman_filtering(std::vector<I> const & inputSignal)
{ 

 I u_hat = inputSignal[0];    
 for(int i(0); i< inputSignal.size(); i+=s){
  
   I u = inputSignal[i];
   
   KalmanFilterFunc
      ( u
      , u_hat
      , K
      , P);
  
   OutputData[i] = static_cast<O>(u_hat);
  // std::cout << OutputData[i] << "  " << u_hat << std::endl;
 }

 for(int i(0); i< inputSignal.size(); i++){
   I u = inputSignal[i];
   relErrorInfo += std::fabs(u - OutputData[i]);
 }

}


template<typename I, typename O>
double const 
KalmanFilter<I,O>::getError(){
  return sqrt(relErrorInfo);
}

template<typename I,typename O>
std::vector<O> const &
KalmanFilter<I,O>::getOutPut(){
 
   return OutputData;
}

template<typename I, typename O>
void
KalmanFilter<I,O>::computationError(){

   if(isKalmanFilterCall){
        ErrorCompute = std::unique_ptr<ErrorComputation<I,O>>
                 (new ErrorComputation<I,O>
                        ( R
                       , rank
                       , size)
                 );
        ErrorCompute->perforationStride(s);      
        ErrorCompute->computeInitialRelativeError
                 (  OutputData 
                 ,  InputData);
        errorInfo = ErrorCompute->getErrorInfo();
   }
}

template<typename I,typename O>
void
KalmanFilter<I,O>::KalmanFilterApply()
{
   isKalmanFilterCall = true;
   if(ErrorComputed)
   { 
      Kalmanfilter();
      computationError();
   }else
   {
      Kalmanfilter();
   }   
  
}


template<typename I,typename O>
I * const 
KalmanFilter<I,O>::getErrorInfo(){
   return errorInfo;
}

template<typename I,typename O>
void
KalmanFilter<I,O>::Kalmanfilter()
{ 
// std::cout << "This is stride perforation Kalman " << s << std::endl; 
 I u_hat(0); 
 int localdatasize (InputData.size());
 for(int i(0); i<localdatasize; i+=s){
  
   I u = InputData[i];
   
   KalmanFilterFunc
      ( u
      , u_hat
      , K
      , P);
      
   OutputData[i] = static_cast<O>(u_hat);
 }
 
 //Check for Distributed case
  if(size > 0){
     DistributedKalmanFilter();
  }

}



template<typename I,typename O>
void
KalmanFilter<I,O>::DistributedKalmanFilter(){

  if(size > 1){

    sendData[0] = static_cast<I>(OutputData[OutputData.size()-1]);
    sendData[1] = P;
    sendData[2] = K;
 
   
   if( rank < size-1){

     MPI_Send
       ( &sendData[0]      //data
       , sendData.size()   //count
       , mpi_get_type<I>() //datatype
       , (rank+1)          //destination
       , 0                 //tag
       , MPI_COMM_WORLD    //communicator  
       ); 
     }
   if( rank > 0){

     MPI_Recv
       ( &receiveData[0]    //data
       , receiveData.size() //count 
       , mpi_get_type<I>()  //datatype 
       , rank-1             //source
       , 0                  //tag
       , MPI_COMM_WORLD     //communicator 
       , MPI_STATUS_IGNORE  // status
       );
    }
 
    //recomputing the Kalman filter for the first iteration
    if(rank >0){

    I u_hat_recev = receiveData[0];
    I P_recev     = receiveData[1];
    I K_recev     = receiveData[2];
 
    for(int i(0); i<70; i+=s){
 
      I u = InputData[i];
  
      KalmanFilterFunc
        ( u
        , u_hat_recev
        , K_recev
        , P_recev);
      
      
      OutputData[i] = static_cast<O>(u_hat_recev);
      }
    }    

  }

}

template<typename I,typename O>
void
KalmanFilter<I,O>::KalmanfilterPerforated()
{
 I u_hat(0); 
 int localdatasize (InputData.size());
 for(int i(0); i<localdatasize; i+=s){
 
   I u = InputData[i];
   InputDataPerforated.push_back(InputData[i]);
   KalmanFilterFunc
      ( u
      , u_hat
      , K
      , P);
      
   OutputData[i] = static_cast<O>(u_hat);
   OutputDataPerforated.push_back(static_cast<O>(u_hat));
 }
 
 //Check for Distributed case
  if(size > 0){
     DistributedKalmanFilter();
  }

}


template<typename I,typename O>
void
KalmanFilter<I,O>::DistributedKalmanFilterPerforation(){

  if(size > 1){

    sendData[0] = static_cast<I>(OutputDataPerforated[OutputDataPerforated.size()-1]);
    sendData[1] = P;
    sendData[2] = K;
 
   
   if( rank < size-1){

     MPI_Send
       ( &sendData[0]      //data
       , sendData.size()   //count
       , mpi_get_type<I>() //datatype
       , (rank+1)          //destination
       , 0                 //tag
       , MPI_COMM_WORLD    //communicator  
       ); 
     }
   if( rank > 0){

     MPI_Recv
       ( &receiveData[0]    //data
       , receiveData.size() //count 
       , mpi_get_type<I>()  //datatype 
       , rank-1             //source
       , 0                  //tag
       , MPI_COMM_WORLD     //communicator 
       , MPI_STATUS_IGNORE  // status
       );
    }
 
    //recomputing the Kalman filter for the first iteration
    if(rank >0){

    I u_hat_recev = receiveData[0];
    I P_recev     = receiveData[1];
    I K_recev     = receiveData[2];
 
    for(int i(0); i<70; i+=s){
 
      I u = InputData[i];
  
      KalmanFilterFunc
        ( u
        , u_hat_recev
        , K_recev
        , P_recev);
      
      
      //OutputData[i] = static_cast<O>(u_hat_recev);
      OutputDataPerforated[i] = static_cast<O>(u_hat_recev);;
      }
    }    

  }

}

template<typename I,typename O>
void
KalmanFilter<I,O>::KalmanFilterHybridParallel()
{

 threadInfo.resize(nThread);
 kalmanParInfo.resize(nThread);
 omp_set_num_threads(nThread);
 #pragma omp parallel
 {
 
  
   I P1 = P;        //initial error covariance (must be zero)
   I K1 = K;        //initial Kalman gain
   I u_hat(0);      //initial estimation

   int tid = omp_get_thread_num();
  // affinity_set_aff(tid);

   UniformPartition<int> P(0, InputData.size(), nThread);
   int Offset = P.begin(tid+0);
   int Endset = P.begin(tid+1);
       
   for(int i(Offset); i<Endset; i++){
 
      double u = InputData[i];
   
      KalmanFilterFunc
        ( u
        , u_hat
        , K1
        , P1);
  
       OutputData[i] = static_cast<O>(u_hat);
   
      threadInfo[tid].P_temp = P1;
      threadInfo[tid].K_temp = K1;
      threadInfo[tid].U_temp = u_hat; 
   }  
  
   #pragma omp barrier 
   {
  
   int tid = omp_get_thread_num();
  
   if(tid !=0 ){ 
 
    I  P1   ( threadInfo[tid-1].P_temp);        //initial error covariance (must be zero)
  	I  K1   ( threadInfo[tid-1].K_temp);          //initial Kalman gain
    I  u_hat(threadInfo[tid-1].U_temp);
 
  	int tid = omp_get_thread_num();
  	UniformPartition<int> P(0, InputData.size(), nThread);
  	int Offset = P.begin(tid+0);
  	int Endset = P.begin(tid+1);
     
       
    for(int i(Offset); i<Offset+70; i++){
 
      I u = InputData[i];
      KalmanFilterFunc
        ( u
        , u_hat
        , K1
        , P1);
       OutputData[i] = static_cast<O>(u_hat);
       
       kalmanParInfo[tid].P_temp=P1;
       kalmanParInfo[tid].K_temp=K1;
      
      }

          
    }
     
   }
   
 }
 
 
 //Check for Distributed case
  if(size >0 ){
    // Reinitilize the P and K that should be comminucate to next procs
    P = threadInfo[nThread-1].P_temp;
    K = threadInfo[nThread-1].K_temp; 

    DistributedKalmanFilter();
  }
}



template<typename I,typename O>
void
KalmanFilter<I,O>::KalmanFilterFunc
       ( I & u
       , I & u_hat
       , I & k
       , I & p)
 {
   k = p*H/(H*p*H+R);              //update Kalman gain
   u_hat = u_hat + k*( u - H*u_hat);//update estimated 
  // std::cout << u_hat << "  " << u << "  " << k <<"  " << p*H <<"  " << H*p*H+R << std::endl;
   p = (1-k*H)*p +Q;                //update error covariance
 }




template<typename I,typename O>
void
KalmanFilter<I,O>::KalmanFilterSequential_chunk()
{
 
 int chunkDataSize(22400);

 I u_hat(0); 
 int datasize (InputData.size());
 int offset(0); int endset(0); int rep(0);
 int iter(0); int kk(0);
 int localChunkSize(0);
 
 while(true)
 {
 
   if(iter >= datasize){
     break;
   }
  
   if((rep+1)*chunkDataSize < datasize ){
       endset = (rep+1)*chunkDataSize;
       localChunkSize = chunkDataSize;
    }else
     {
       endset = datasize;
       localChunkSize = datasize - (rep)*chunkDataSize;
     }

  
  std::vector<I> ChunkData(localChunkSize);
  int jj(0);
  for( iter=offset; iter<endset; iter++ ){
      ChunkData[jj] = InputData[iter];
      jj++;
   }
 

   for(int ii(0); ii<ChunkData.size(); ii++ )
   {
      I u = ChunkData[ii];
       
       K = P*H/(H*P*H+R);               //update Kalman gain
       u_hat = u_hat + K*( u - H*u_hat);//update estimated 
       P = (1-K*H)*P +Q;                //update error covariance
       
       OutputData[kk] = static_cast<O>(u_hat);
       kk++;
    }
    offset = iter;
    rep++;
 }
 
 //Check for Distributed case
  if(size > 0){
     DistributedKalmanFilter();
  }
}

template<typename I,typename O>
void
KalmanFilter<I,O>::KalmanFilterSequential_inline()
{
 I u_hat(0); 
 int localdatasize (InputData.size());
 for(int i(0); i<localdatasize; i++){
 
   I u = InputData[i];
   
   K = P*H/(H*P*H+R);               //update Kalman gain
   u_hat = u_hat + K*( u - H*u_hat);//update estimated 
   P = (1-K*H)*P +Q;                //update error covariance
  
   OutputData[i] = static_cast<O>(u_hat);
 }
 
 //Check for Distributed case
  if(size > 0){
     DistributedKalmanFilter();
  }
 
 }



#endif

