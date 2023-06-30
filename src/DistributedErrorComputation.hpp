#ifndef DISTERRORCOMPUTE_HPP
#define DISTERRORCOMPUTE_HPP

#include <iostream>
#include <vector>
#include "kernelFlops.hpp"

 

template<typename I,typename O>
class ErrorComputation{

 public:
 ErrorComputation
       ( I const & _R
       , int const & _rank
       , int const & _size);

 ErrorComputation( int const & _rank
                 , int const & _size);

 void computeError( std::vector<O> const & OutputData 
                  , std::vector<I> const & InputData);

 void DistributedErrorComputation();

 void
 perforationStride(int _s);

 void 
 noiseCovarianceKalmanFilter(I const _R);

 void 
 computeInitialRelativeError
      ( std::vector<O> const & OutputData 
      , std::vector<I> const & InputData);

 void TotalError();

 std::vector<double> const & getErrorInfo();

 private:
  
  int  rank;
  int  size;

  int  s; 
  I    R;
 
 struct ErrorElm{
  I AbsErrorMax;
  I AbsErrorL2;
  I RelErrorMax;
  I RelErrorL2;
  };

 
 ErrorElm AbsouluteError;
 ErrorElm AbsouluteTotalError;
 
 std::vector<double> errorInfo;
 bool DistributedErrorComputationStatus;
};

template<typename I,typename O>
ErrorComputation<I,O>:: ErrorComputation( I const & _R
                                        , int const & _rank
                                        , int const & _size)
:R(_R)
,rank(_rank)
,size(_size)
,errorInfo(4)
,DistributedErrorComputationStatus(false)
{}


template<typename I,typename O>
ErrorComputation<I,O>:: ErrorComputation( int const & _rank
                                        , int const & _size)
:rank(_rank)
,size(_size)
,errorInfo(4,0)
{}

template<typename I,typename O>
void
ErrorComputation<I,O>::perforationStride(int _s)
{   
   s = _s;         
}



template<typename I,typename O>
void
ErrorComputation<I,O>::noiseCovarianceKalmanFilter(I const _R){
   R = _R;
}

template<typename I,typename O>
void
ErrorComputation<I,O>::computeError
      ( std::vector<O> const & OutputData 
      , std::vector<I> const & InputData)
{
   if(!DistributedErrorComputationStatus){
     computeInitialRelativeError( OutputData, InputData);
   }
   else
   {
     computeInitialRelativeError( OutputData, InputData);
     DistributedErrorComputation();  
   }
}


template<typename I,typename O>
void
ErrorComputation<I,O>::computeInitialRelativeError
      ( std::vector<O> const & OutputData 
      , std::vector<I> const & InputData
      )
{
 std::vector<I> AbsError(InputData.size()); I AbsL2Norm(0);
 std::vector<I> RelError(InputData.size()); I RelL2Norm(0);
  for(int i(0); i< AbsError.size(); i+=1){
     AbsError[i] = fabs(OutputData[i] - InputData[i]);
     RelError[i] = fabs(InputData[i]);
     AbsL2Norm += (AbsError[i]*AbsError[i]); 
     RelL2Norm += RelError[i]*RelError[i]; 
     }

auto minmaxAbsError = std::minmax_element(AbsError.begin(), AbsError.end());
auto minmaxRelError = std::minmax_element(RelError.begin(), RelError.end());

I AbsMax = * minmaxAbsError.second;
I AbsRel = * minmaxRelError.second;

errorInfo[0]= AbsMax;
errorInfo[1]= sqrt(AbsL2Norm);
errorInfo[2]= AbsMax/AbsRel;
errorInfo[3]= sqrt(AbsL2Norm)/sqrt(RelL2Norm);


}

template<typename I,typename O>
std::vector<double> const & 
ErrorComputation<I,O>::getErrorInfo(){
   return  errorInfo;
}


template<typename I,typename O>
void
ErrorComputation<I,O>::DistributedErrorComputation()
{
 
const int    nItems=4;
int          blocklengths[nItems] = { 1, 1, 1 ,1};
MPI_Datatype types[nItems] = { mpi_get_type<I>(), mpi_get_type<I>(), mpi_get_type<I>(), mpi_get_type<I>()};
MPI_Datatype MPI_KalmanErrorElm;
MPI_Aint     offsets[nItems];

offsets[0] = offsetof(ErrorElm, AbsErrorMax);
offsets[1] = offsetof(ErrorElm, AbsErrorL2);
offsets[2] = offsetof(ErrorElm, RelErrorMax);
offsets[3] = offsetof(ErrorElm, RelErrorL2);

MPI_Type_create_struct(nItems, blocklengths, offsets, types, & MPI_KalmanErrorElm);
MPI_Type_commit(& MPI_KalmanErrorElm);


std::vector<ErrorElm> ErrorVec(size);
MPI_Gather
      ( & AbsouluteError    //send_data,
      , 1                   //send_count
      , MPI_KalmanErrorElm  //send_datatype
      , & ErrorVec[0]       //recv_data,
      , 1                   //recv_count,
      , MPI_KalmanErrorElm  //recv_datatype,
      , 0
      , MPI_COMM_WORLD);    //Comminucation group

if(rank == 0 )
  {
     ErrorElm totalError; 
     totalError.AbsErrorL2 = 0; totalError.RelErrorL2 = 0;
     totalError.AbsErrorMax = 0; totalError.RelErrorMax = 0;
     I AbsMax(0); I RelMax(0);
     for(int i(0); i<size; i++){
        if( ErrorVec[i].AbsErrorMax > AbsMax ){
           AbsMax = ErrorVec[i].AbsErrorMax;
        }
        if( ErrorVec[i].RelErrorMax > RelMax ){
           RelMax = ErrorVec[i].RelErrorMax;
        }
        totalError.AbsErrorL2  += ErrorVec[i].AbsErrorL2; 
        totalError.RelErrorL2  += ErrorVec[i].RelErrorL2;
       
     }
     
     totalError.AbsErrorMax =  AbsMax;
     totalError.RelErrorMax =  RelMax;
     totalError.AbsErrorL2 = sqrt(totalError.AbsErrorL2);
     totalError.RelErrorL2 = sqrt(totalError.RelErrorL2);
     
      errorInfo[0]= totalError.AbsErrorMax;
      errorInfo[1]= totalError.AbsErrorL2;
      errorInfo[2]= totalError.AbsErrorMax/totalError.RelErrorMax;
      errorInfo[3]= totalError.AbsErrorL2/totalError.RelErrorL2;
      //std::cout << "perforation s "<< s << " |  Absoulute error L2_norm : " <<  errorInfo[1] << "  | Relative error L2_norm : " << errorInfo[3] << std::endl;
     
  }
   

}




#endif