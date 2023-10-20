#ifndef WAVELET_HPP
#define WAVELET_HPP

#include<iostream>
#include<vector>
#include <complex>
#include <cmath>
#include "signalContainer.hpp"
#include <vector.h>
#include <wavelet.h>

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
      std::cout << "this is wavelet " << std::endl;
     waveletFilterFunction(InputData);     
}

 
template<typename I,typename O>
void
Wavelet_Filter<I,O>::waveletFilterFunction(std::vector<I> const & _InputData){ 
  
  int num_levels = 4;

  const double inv_sqrt2 = 1.0 / std::sqrt(2.0);
  const std::vector<double> Lo_D = {inv_sqrt2, inv_sqrt2};
  const std::vector<double> Hi_D = {-inv_sqrt2, inv_sqrt2};
  const std::vector<double> Lo_R = {inv_sqrt2, inv_sqrt2};
  const std::vector<double> Hi_R = {inv_sqrt2, -inv_sqrt2};

  // Instantiate a Wavelet object with Haar filters
  const Wavelet<double> haar(Lo_D, Hi_D, Lo_R, Hi_R);


  const std::vector<double> vector = {1.2, 1.5, 1.8, 1.7, 1.6, 1.3, 1.1, 1.0};

  const Decomposition1D<double> dec1D = haar.Wavedec(vector, num_levels);
  //const std::vector<double> vector_rec = haar.Waverec(dec1D, vector.size());

}

template<typename I, typename O>
std::vector<O> const &
Wavelet_Filter<I,O>::getOutPut()
{
   return OutputData;
}





#endif