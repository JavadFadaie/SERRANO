#include<iostream>
#include <gtest/gtest.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <string>
#include <sstream>
#include <omp.h>
#include <mpi.h>
#include <cmath>
#include <algorithm>
#include <numeric>
#include "../src/signalContainer.hpp"
  


std::vector<double> const  readTheCSVformate(std::string const inputDataPath){
  
   struct signalContainer{
     signalContainer
        ( std::string _value
        , int _id)
        :value(_value)
        ,id(_id)
        {}

    std::string value;
    int id;
    };  
   std::vector<signalContainer> signal;
   std::string fileline;
   std::string elm;
   std::ifstream InputData;
   std::vector<double> buffer;
   InputData.open(inputDataPath);
   while(InputData)
   {  
     getline(InputData,fileline);
     std::istringstream ss(fileline);
     size_t elmId(0); double val(0); 
     double time; int signalId(0);
     while(getline(ss, elm,';') ){
           double SPXelm = std::atof(elm.c_str());
           signal.push_back(signalContainer(elm,signalId));
           signalId++;
       }
   }

    auto it = max_element(signal.begin (), signal.end(),
                            []( const signalContainer & a, const signalContainer & b)
                               {return a.id < b.id ;}
                          );

    int numSignal = it->id+1; std::vector<double> signalBuffer; 
    for(int signal_Id(0); signal_Id<1; signal_Id++){
        std::string sensor_name;  bool headerStatus(true);
      for(int i(0); i<signal.size(); i++){
          if( signal[i].id == signal_Id ){
              double value =  std::atof(signal[i].value.c_str());
              //just save the header
              if(headerStatus){
                   sensor_name = signal[i].value.c_str();
                   headerStatus = false;
              }else
              {
                    signalBuffer.push_back(value);
                    //std::cout << value << std::endl;
              }
          }
      }

    }
  
   return signalBuffer;

}

TEST(FFT_FilterTest, Test26Signals )
{ 
    
   std::string IDEKO_FFT_Path = "/home/javad/Desktop/interview/SERRANO/data/Init_Data/acceleration/ffts_output/ffts_26.csv";  
   std::string HPC_FFT_Path = "/home/javad/Desktop/interview/SERRANO/data/Output_Data/CSVFormate/FFT_Filter_output.csv";
   std::vector<double> IDEKO_FFT_output( readTheCSVformate(IDEKO_FFT_Path) );
   std::vector<double> HPC_FFT_output( readTheCSVformate(HPC_FFT_Path) );
   
   double tolerance = 0.00001;
   for(int i(0); i<IDEKO_FFT_output.size(); i++){
    EXPECT_NEAR(IDEKO_FFT_output[i], HPC_FFT_output[i], tolerance);
   }   
    
   
}


int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

