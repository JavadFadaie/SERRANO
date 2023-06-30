#ifndef CLEANINGDIRECTORY_HPP
#define CLEANINGDIRECTORY_HPP

#include <iostream>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
class cleaningDirectory{

 public:
 cleaningDirectory()
 {}

 void cleaningInputData(std::string const & inputDataDouble
                      , std::string const & inputDataFloat)
 {

  cleanFileDirectory(inputDataDouble);
  cleanFileDirectory(inputDataFloat);
  std::string signalintextformat = data_workspace + "/Input_Data/Double_Data_Type/signalFilter_plot/";
  cleanFileDirectory(signalintextformat);
 }


void Data_WorkSpace( std::string const & _data_workspace){
  data_workspace = _data_workspace;
}


void applyingFilters
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


void cleaningSignal(){

    std::string output_Filter_Path; 
   if(Kalman_FilterApplied){
    output_Filter_Path = data_workspace +"/Output_Data/kalmanFilter";
  }
  if(MinMax_TransformApplied ){
    output_Filter_Path = data_workspace +"/Output_Data/MinMax";
  }
  if(FFT_FilterApplied ){
    output_Filter_Path = data_workspace + "/Output_Data/FFTFilter";
  }
  if(savitzkyGoly_Transform){
    output_Filter_Path = data_workspace +"/Output_Data/SavitzkeyGolay";
  }

   
  cleanFileDirectory(output_Filter_Path); 
  

}


void cleanFileDirectory(std::string const & path){
  
  for (const auto& entry : std::filesystem::directory_iterator(path)) 
            std::filesystem::remove_all(entry.path());
  
}

 private: 

  std::string data_workspace;
  std::string profile_path;

  bool Kalman_FilterApplied;
  bool FFT_FilterApplied; 
  bool MinMax_TransformApplied;
  bool savitzkyGoly_Transform;
  bool benchmarkState;


};




#endif