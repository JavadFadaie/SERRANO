#ifndef SAVI_GOLAY_HPP
#define SAVI_GOLAY_HPP


#include<iostream>
#include<vector>
#include <omp.h>
#include "DistributedErrorComputation.hpp"
#include "signalContainer.hpp"


template<typename I, typename O>
class SavitzkyGoly{

  public :

    SavitzkyGoly( std::vector<I> const & _InputData 
                , int _rank
                , int _size);


    void Data_IO_Path( std::string _inputDataPath
                     , std::string _outputDataPath);
    

    void Savitzky_Goly_apply();

    void Savitzky_Goly_function(std::vector<I> const & signal);

    void Savitzky_Goly_function_elevel_windeow(std::vector<I> const & signal);

    double const getError();

    std::vector<O> const & getOutPut();

    void perforationStride(int _s);

    void savitzkyGolay_apply();

    void computationError();
    
    void parallelSavitzkyGoly();

    I * const getErrorInfo();


    void comminucateInfrustracture
                   (std::vector<I> const & inputData
                   ,std::vector<I> & upperHalo
                   ,std::vector<I> & downHalo);
    
    void SavitzkyGolyFunc
              ( std::vector<I> const & data
              , std::vector<I> & upperHallo
              , std::vector<I> & downHallo);

    void savitzkey_funct(std::vector<I> const & signal);
  
    void Display(std::vector<I> const & inputData);

  private : 

   
    int rank;
    int size;
    int s;

    std::string inputDataPath;
    std::string outputDataPath;

    std::string outputpath;
    
    std::vector<size_t> numDataProcExt;
    std::vector<size_t> offsetVecExt;
    
 
    std::unique_ptr< ErrorComputation<I,O> > ErrorCompute;
    I * errorInfo;

    bool isSavGolCall;
    bool ErrorComputed;

    double  relErrorInfo;

    int const m;
    std::vector<O> OutputData;
    std::vector<I> const  InputData;
};

///////////////////////////////////////
///////////// IMPLEMENTATION
///////////////////////////////////////



template<typename I, typename O>
SavitzkyGoly<I,O>::SavitzkyGoly(std::vector<I> const & _InputData ,int _rank, int _size)
:InputData(_InputData)
,rank(_rank)
,size(_size)
,OutputData(_InputData.size())
,m(5)
,s(1)
,isSavGolCall(false)
,ErrorComputed(true)
{}



template<typename I,typename O>
void
SavitzkyGoly<I,O>::Data_IO_Path( std::string _inputDataPath
                               , std::string _outputDataPath)
{
  inputDataPath  = _inputDataPath;
  outputDataPath = _outputDataPath;
}

template<typename I,typename O>
void
SavitzkyGoly<I,O>::perforationStride(int _s)
{   
   s = _s;         
}


template<typename I,typename O>
void
SavitzkyGoly<I,O>::Savitzky_Goly_apply()
{

   
    Savitzky_Goly_function_elevel_windeow( InputData);

}





template<typename I, typename O>
void
SavitzkyGoly<I,O>::Savitzky_Goly_function_elevel_windeow(std::vector<I> const & signal){

   
   return savitzkey_funct(signal);
 
}


template<typename I, typename O>
void
SavitzkyGoly<I,O>::savitzkey_funct(std::vector<I> const & signal){

  std::vector<I> coff{-0.08391608,  0.02097902,  0.1025641 ,  0.16083916,  0.1958042 ,
                    0.20745921,  0.1958042 ,  0.16083916,  0.1025641 ,  0.02097902, -0.08391608};
 
  int window_size = coff.size();
  int half_window = window_size / 2;
   
  int numberOfInput(signal.size());  
  for(int i(0); i<numberOfInput; i+=s){
       I sum(0); I value(0);
       for(int j= -half_window ; j<= half_window; j++){
          
          if(i+j < 0){
             value = signal[0]; 
          }
          if(i+j >= numberOfInput ){
             value = signal[numberOfInput-1]; 
          }
          if((i+j >= 0) || (i+j < numberOfInput))
          {
              value = coff[j+half_window]*signal[i+j];
          }
          sum += value;
       }
       OutputData[i] = sum;
     //  relErrorInfo += std::fabs(OutputData[i] - signal[i]);
     //std::cout << OutputData[i] << std::endl;   
  }

 for(int i(0); i<numberOfInput; i++){
       relErrorInfo += std::fabs(OutputData[i] - signal[i]);
  }

}   




template<typename I, typename O>
double const 
SavitzkyGoly<I,O>::getError(){
  return sqrt(relErrorInfo);
}

template<typename I,typename O>
std::vector<O> const &
SavitzkyGoly<I,O>::getOutPut(){
 
   return OutputData;
}

template<typename I, typename O>
void
SavitzkyGoly<I,O>::computationError(){

   if(isSavGolCall){
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



template<typename I, typename O>
void
SavitzkyGoly<I,O>::Savitzky_Goly_function(std::vector<I> const & signal){

    int numberOfInput(signal.size());  
    for(int i(0); i<numberOfInput; i++){
       
       if( (i>2) || (i<numberOfInput-2) ){
         OutputData[i] = (1./35.)*( -3*signal[i-2] +12*signal[i-1] + 17*signal[i] + 12*signal[i+1] -3*signal[i+2] );
       }else{
          OutputData[i] = signal[i];
       }
       
    }
    
}

template<typename I,typename O>
void
SavitzkyGoly<I,O>::savitzkyGolay_apply()
{
    isSavGolCall = true;

   if(ErrorComputed)
   { 
      parallelSavitzkyGoly();
      computationError();
   }else
   {
      parallelSavitzkyGoly();
   }   
  
}


template<typename I, typename O>
void 
SavitzkyGoly<I,O>::parallelSavitzkyGoly(){

  std::vector<I> upperHalo(2,std::numeric_limits<I>::max());
  std::vector<I> downHalo(2,std::numeric_limits<I>::max());

       //Exchange the Halo data by comminucation point to point
  comminucateInfrustracture(InputData, upperHalo, downHalo);

         //The Savitzky Goly function
   SavitzkyGolyFunc(InputData, upperHalo, downHalo);
}

template<typename I, typename O>
void
SavitzkyGoly<I,O>::comminucateInfrustracture
                   (std::vector<I> const & inputData
                   ,std::vector<I>       & upperHalo
                   ,std::vector<I>       & downHalo){

MPI_Status status;

// send down Halo from proc i to i + 1
if( rank < size-1){

 MPI_Send
    ( &inputData[inputData.size()-2] //data
    , 2                              //count
    , mpi_get_type<I>()              //datatype
    , (rank+1)                       //destination
    , 0                              //tag
    , MPI_COMM_WORLD                 //communicator  
    ); 
}
if( rank > 0){

 MPI_Recv
     ( &downHalo[0]             //data
     , 2                        //count 
     , mpi_get_type<I>()        //datatype 
     , rank-1                   //source
     , 0                        //tag
     , MPI_COMM_WORLD           //communicator 
     , MPI_STATUS_IGNORE        // status
     );
}

// send upper Halo from proc i to i - 1
if( rank > 0){

 MPI_Send
    ( &inputData[0]       //data
    , 2                   //count
    , mpi_get_type<I>()   //datatype
    , rank-1              //destination
    , 1                   //tag
    , MPI_COMM_WORLD      //communicator  
    ); 

}

if( rank < size - 1){

 MPI_Recv
     ( &upperHalo[0]       //data
     , 2                   //count 
     , mpi_get_type<I>()   //datatype 
     , rank+1              //source
     , 1                   //tag
     , MPI_COMM_WORLD      //communicator 
     , & status            // status
     );
}



}

template<typename I, typename O>
void
SavitzkyGoly<I,O>::SavitzkyGolyFunc
              ( std::vector<I> const & data
              , std::vector<I> & upperHallo
              , std::vector<I> & downHallo)
 {

   int n = data.size();
  for(int i(0); i<data.size(); i+=s){
  
       I upperHallo1(0);I upperHallo2(0); I downHallo1(0); I downHallo2(0);
       
       if( (i+1 >= n) || (i+2 >= n) || (i-1 < 0) || (i-2 < 0) )  
       { 
           if( (i+1 == n) && (upperHallo[0] != std::numeric_limits<I>::max() ) ){
            upperHallo1 = upperHallo[0];
           }
           if( (i+2 == n) && (upperHallo[0] != std::numeric_limits<I>::max() ) ){
            upperHallo2 = upperHallo[0];
           } 
           if( (i+2 == (n+1)) && (upperHallo[1] != std::numeric_limits<I>::max() ) ){
            upperHallo2 = upperHallo[1];
           }


           if( (i-1 == -1) && (downHallo[1] != std::numeric_limits<I>::max() ) ){
            downHallo1 = downHallo[1];
           }
           if( (i-2 == -1) && (downHallo[1] != std::numeric_limits<I>::max() ) ){
            downHallo2 = downHallo[1];
           } 
           if( (i-2 == -2) && (downHallo[0] != std::numeric_limits<I>::max() ) ){
            downHallo2 = downHallo[0];
           }
       }
           if( (i-2) >= 0 ){
              downHallo2 = data[i-2];
           }
           if( (i-1) >= 0 ){
              downHallo1 = data[i-1];
           }
           if( (i+1 < n) ){
              upperHallo1 = data[i+1];
           } 
           if( (i+2 < n) ){
              upperHallo2 = data[i+2];
          } 
        I approx  = (1./35.)*(-3*downHallo2 +12*downHallo1 +17*data[i] +12*upperHallo1 -3*upperHallo2);
        OutputData[i] = static_cast<O>(approx);
 }

}




template<typename I,typename O>
void
SavitzkyGoly<I,O>::Display(std::vector<I> const & inputData){

  for(int i(0); i<inputData.size(); i++){
    std::cout << i<< "  " << inputData[i]<< "  " << OutputData[i]<< std::endl; 
  }
}
#endif
