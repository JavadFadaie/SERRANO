#ifndef MinMax_HPP
#define MinMax_HPP

#include<iostream>
#include<vector>
#include <omp.h>
#include "UniformPartition.hpp"
#include "DistributedErrorComputation.hpp"
#include "signalContainer.hpp"

template<typename I,typename O>
class MinMaxAnalysis{
 
 public:
 
 MinMaxAnalysis(std::vector<I> const & _InputData
               , int const & _rank
               , int const & _size  );

 ~ MinMaxAnalysis(){}
 
 void MinMaxParameter(I const & first
                    , I const & second);

 void perforationStride(int _s);                   
 
 std::vector<O> const & MinMax_Transform_apply();

 void MinMaxUpdate_apply();

 void MinMaxUpdate();

 void computationError();


 I * const getErrorInfo();
 
 std::vector<I> MinMax
  ( std::vector<I> const & Input_Buffer);
 
 void
 updateInput
     ( std::vector<I> const & Input_Buffer
     , I const & totalMin
     , I const & totalMax);

std::vector<I> MinMaxPerforated
      ( std::vector<I> const & Input_Buffer);

 void
 updateInputPerforated
     ( std::vector<I> const & Input_Buffer
     , I const & totalMin
     , I const & totalMax);

std::vector<O> const &
updateInputPerforated_apply
  ( std::vector<I> const & Input_Buffer
  , I const & totalMin
  , I const & totalMax);


 std::vector<O> const & 
 getOutPut();
      
       
 private:
 
  int const  rank;
  int const  size;

  std::vector<I> const  InputData;
  std::vector<O> OutputData;

 
  bool isMinMaxUpdateCall;
  bool ErrorComputed;

  std::unique_ptr< ErrorComputation<I,O> > ErrorCompute;
  I * errorInfo;

  I low_th;
  I high_th;
  int s;
};

///////////////////////////////////
//////// IMPLEMENT
////////////////////////////////////

template<typename I,typename O>
MinMaxAnalysis<I,O>::MinMaxAnalysis
( std::vector<I> const & _InputData
, int const & _rank
, int const & _size)
:InputData(_InputData)
,OutputData(_InputData.size())
,rank(_rank)
,size(_size)
,isMinMaxUpdateCall(false)
,ErrorComputed(true)
,s(1)
{}
 


template<typename I,typename O>
void
MinMaxAnalysis<I,O>::MinMaxParameter
          (I const & first
          ,I const & second)
{         
      low_th  = first;
      high_th = second;    
}          

template<typename I,typename O>
void
MinMaxAnalysis<I,O>::perforationStride(int _s)
{   
   s = _s;         
}          




template<typename I,typename O>
std::vector<O> const &
MinMaxAnalysis<I,O>::MinMax_Transform_apply()
{

   std::vector<I> minMax (MinMaxPerforated( InputData) );
   return updateInputPerforated_apply( InputData
                                     , minMax[0]
                                     , minMax[1]);

}

template<typename I,typename O>
std::vector<I> 
MinMaxAnalysis<I,O>::MinMaxPerforated
  ( std::vector<I> const & Input_Buffer){
 
 int _nThread;
 std::vector<I> rankMinMax(2); std::vector<I> ThreadMinMax(2*_nThread); 
 I tempMax(std::numeric_limits<I>::min()); I tempMin(std::numeric_limits<I>::max());
 for(int i(0); i<Input_Buffer.size(); i+=s){
        if(Input_Buffer[i] >= tempMax){
                tempMax = Input_Buffer[i];}
         if(Input_Buffer[i] <= tempMin){
                tempMin = Input_Buffer[i];}        
 }

   rankMinMax[0] = tempMin;
   rankMinMax[1] = tempMax;

   return rankMinMax;
}


template<typename I,typename O>
std::vector<O> const &
MinMaxAnalysis<I,O>::updateInputPerforated_apply
  ( std::vector<I> const & Input_Buffer
  , I const & totalMin
  , I const & totalMax)
{
   for(int i(0); i< Input_Buffer.size(); i+=s){
        OutputData[i]=static_cast<O>(((Input_Buffer[i]- totalMin )/(totalMax- totalMin)));
    }

  return OutputData;
}





template<typename I, typename O>
void
MinMaxAnalysis<I,O>::computationError(){

   if(isMinMaxUpdateCall){
        ErrorCompute = std::unique_ptr<ErrorComputation<I,O>>
                 (new ErrorComputation<I,O>
                        (rank
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
MinMaxAnalysis<I,O>::MinMaxUpdate_apply()
{
   isMinMaxUpdateCall = true;

   if(ErrorComputed)
   { 
      MinMaxUpdate();
      computationError();
   }else
   {
      MinMaxUpdate();
   }   
  
}




template<typename I,typename O>
I * const 
MinMaxAnalysis<I,O>::getErrorInfo(){
   return errorInfo;
}

template<typename I,typename O>
void
MinMaxAnalysis<I,O>::MinMaxUpdate()
{

//std::vector<I> const & rankMinMax  ( (MinMaxPerforated( InputData) ) );
std::vector<I> const & rankMinMax  ( (MinMax( InputData) ) );
     
            // Gather the each rank Min Max and compute the total min max 
std::vector<I> MaxMinVec(2*size);
MPI_Allgather
      ( & rankMinMax[0]      //send_data,
      , 2                    //send_count
      , mpi_get_type<I>()    //send_datatype
      , & MaxMinVec[0]       //recv_data,
      , 2                    //recv_count,
      , mpi_get_type<I>()    //recv_datatype,
      , MPI_COMM_WORLD);    


        // Compute the total min and Max
std::vector<I> const & totalMinMax ( (MinMax( MaxMinVec) ) );
 

// Update the input in each rank with having min and Max
updateInputPerforated
      ( InputData
      , totalMinMax[0]
      , totalMinMax[1]);


}          

template<typename I,typename O>
std::vector<I> 
MinMaxAnalysis<I,O>::MinMax
  ( std::vector<I> const & Input_Buffer)
{
  int _nThread(1); 
  std::vector<I> rankMinMax(2); std::vector<I> ThreadMinMax(2*_nThread); 
   omp_set_num_threads(_nThread);
   #pragma omp parallel
   {
      int tid = omp_get_thread_num();
      UniformPartition<int> P(0, Input_Buffer.size(), _nThread);
      int Offset = P.begin(tid+0);
      int Endset = P.begin(tid+1);
      
       auto minmaxthread = std::minmax_element(Input_Buffer.begin(), Input_Buffer.end()); 
       ThreadMinMax[2*tid+0] = *minmaxthread.first;   
       ThreadMinMax[2*tid+1] = *minmaxthread.second;
   } 
  
    auto minmax = std::minmax_element(ThreadMinMax.begin(), ThreadMinMax.end()); 
    
    rankMinMax[0] = *minmax.first;   //minimum element: 
    rankMinMax[1] = *minmax.second;  // maximum element: "
   
//   std::cout << "This is " << rankMinMax[0] << "  " << rankMinMax[1] << std::endl;
   return rankMinMax;
}   



template<typename I,typename O>
void
MinMaxAnalysis<I,O>::updateInput
  ( std::vector<I> const & Input_Buffer
  , I const & totalMin
  , I const & totalMax)
{

   int _nThread(1); 
   omp_set_num_threads(_nThread);
   #pragma omp parallel
   {
      int tid = omp_get_thread_num();
      UniformPartition<int> P(0, Input_Buffer.size(), _nThread);
      int Offset = P.begin(tid+0);
      int Endset = P.begin(tid+1);

     for(int i(Offset); i<Endset; i++){
        OutputData[i]=static_cast<O>(((Input_Buffer[i]- totalMin)/(totalMax- totalMin))*(high_th-low_th)+low_th);
     }
  }
}



template<typename I,typename O>
void
MinMaxAnalysis<I,O>::updateInputPerforated
  ( std::vector<I> const & Input_Buffer
  , I const & totalMin
  , I const & totalMax)
{

   for(int i(0); i< Input_Buffer.size(); i+=s){
        OutputData[i]=static_cast<O>(((Input_Buffer[i]- totalMin )/(totalMax- totalMin)));
    }
  
}



template<typename I, typename O>
std::vector<O> const &
MinMaxAnalysis<I,O>::getOutPut(){
   return OutputData;
}


#endif
