#ifndef WAVELET_HPP
#define WAVELET_HPP

#include<iostream>
#include<vector>
#include <complex>
#include <cmath>
#include "signalContainer.hpp"

template<typename I,typename O>
class Wavelet_Filter {

 public:
 
 Wavelet_Filter
   ( std::vector<I> const & _InputData
   , int _rank
   , int _size);


void Wavelet_Filter_Apply();

void waveletFilterFunction(std::vector<I> const & _InputData);
 
std::vector<O> const & getOutPut();

 
private:

 int rank;
 int size;
 
 std::vector<I>  InputData;
 std::vector<O>  OutputData;
 
 bool ErrorComputed; 
};

//////////////////////////////////////////
///////////// IMPLEMENTATION
//////////////////////////////////////////

template<typename I, typename O>
Wavelet_Filter<I,O>::Wavelet_Filter
          ( std::vector<I> const & _InputData
          , int _rank
          , int _size)
:InputData(_InputData)
,rank(_rank)
,size(_size)
,OutputData(_InputData.size())
{}



template<typename I, typename O>
void
Wavelet_Filter<I,O>::Wavelet_Filter_Apply(){
}

 
template<typename I,typename O>
void
Wavelet_Filter<I,O>::waveletFilterFunction(std::vector<I> const & _InputData){ 
  

}

template<typename I, typename O>
std::vector<O> const &
Wavelet_Filter<I,O>::getOutPut()
{
   return OutputData;
}





#endif
