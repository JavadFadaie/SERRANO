#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <sstream>
#include <fstream>

template<typename T>
class statistic{

public:

statistic(std::vector<T>  & _inputData, std::string _name);

~statistic(){}

void medianStatistic();

void compute_statistics();

void apply();

void show();

int  floor(int const  _num);

T compute_mean();

T compute_variance(T const mean);

T const Mean();

T const Variance();

T const stdDevation();

T const  Median();

T const twentyFivePercentile();

T const seventyFivePercentile();


private:
std::vector<T> & inputData;
std::vector<T> actualData;
std::string name;
int const length;
T median;
T value_25;
T value_75;
T mean;
T variance;
T sdeviation;
T * _actualData;
};


template<typename T>
statistic<T>::statistic(std::vector<T> & _inputData, std::string _name)
:inputData(_inputData)
,actualData(_inputData)
,length(inputData.size())
,name(_name)
,median(0)
,value_25(0)
,value_75(0)
,mean(0)
,variance(0)
,sdeviation(0)
{
    _actualData = & actualData[0];
   
   std::sort(inputData.begin(), inputData.end());

 
}


template<typename T>
int
statistic<T>::floor(int const _num) 
{
    if( (_num%2) == 0){
         return (_num/2);
    }else{
        return ((_num-1)/2);
    }
}

template<typename T>
void
statistic<T>::apply(){

   medianStatistic();
   compute_statistics();
}

template<typename T>
void 
statistic<T>::show(){

  std::cout <<"          STATISTIC ELAPSED TIME FOR PART " << name << std::endl;
  std::cout << std::endl;
  std::cout << "The median is                " << median << std::endl;
  std::cout << std::endl;
  std::cout << "The 25 Persenctile is        " << value_25 <<  std::endl;
  std::cout << std::endl;
  std::cout << "The 75 Persenctile is        " << value_75 <<  std::endl;
  std::cout << std::endl;
  std::cout << "The mean value is            "<< mean << std::endl;
  std::cout << std::endl;
  std::cout << "The stadard devaition is     "<< sdeviation << std::endl;
  std::cout << std::endl;
  std::cout << "The variance   is            "<< variance << std::endl;
  std::cout << std::endl;
  std::ofstream myfile;
  std::string filename = "../profile/Performance/Performance_";
  filename.append(name); filename.append(".dat");
  myfile.open(filename);


  for(int iparts(0); iparts<length; iparts++){
     myfile << _actualData[iparts] << std::endl;               
  }
    myfile << median << std::endl; 
    myfile << value_25 << std::endl; 
    myfile << value_75 << std::endl; 
    myfile << mean << std::endl; 
    myfile << variance << std::endl; 
    myfile << sdeviation << std::endl; 
   
}
template<typename T>
void
statistic<T>::medianStatistic()
{
       //calculate median
 int idx_median(floor(length)); 
 median = inputData[idx_median];

  //calculate 75 percentile
 int idx_75(floor(idx_median));
 idx_75 += idx_median;
 value_75 = inputData[idx_75];

 //calculate 75 percentile
 int idx_25 = idx_median;
 idx_25 -= floor(idx_median);
 if(idx_25 < 0){
     idx_25 = 0;
 }
 value_25 = inputData[idx_25];
 
}

template<typename T>
T
statistic<T>::compute_mean(){
  
 T  average = std::accumulate( inputData.begin(), inputData.end(), 0.0);
   average /= length;
  
  return average;
}

template<typename T>
T
statistic<T>::compute_variance(T const mean){
 
    T result = 0;
    for(T number : inputData)
    {
        result += (number - mean)*(number - mean);
    }

    return result / (inputData.size() - 1);
}

template<typename T>
void
statistic<T>::compute_statistics(){

  mean       = compute_mean();
  variance   = compute_variance(mean);
  sdeviation = sqrt(variance);

 // std::cout << mean << "  " << variance << "   " << sdeviation << std::endl;
}

template<typename T>
T const
statistic<T>::Mean(){
    return mean;
}

template<typename T>
T const
statistic<T>::Variance(){
    return variance;
}

template<typename T>
T const
statistic<T>::stdDevation(){
     return sdeviation;
}

template<typename T>
T const
statistic<T>::Median(){
    return median;
}

template<typename T>
T const
statistic<T>::twentyFivePercentile(){
    return value_25;
}

template<typename T>
T const
statistic<T>::seventyFivePercentile(){
     return value_75;
}

