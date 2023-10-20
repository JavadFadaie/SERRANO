#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include "ReadCSVData.hpp"
#include "ReadCSVDataStruct.hpp"
#include <mpi.h>
#include "AnalysisInterface.hpp"
#include "KNN.hpp"
#include "DistributedTimer.hpp"
#include "UniformPartition.hpp"
#include "kernelFlops.hpp"
#include <cassert>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <DTW.hpp> 
#include <BlackScholes.hpp>
#include <Kmean.hpp>
#include <SavitzkeyGolay.hpp>
#include <signalProcessing.hpp>
#include <IDEKO_Data_Reader.hpp>
#include "KMean_Signal.hpp"
#include "KNN_Signal.hpp"
#include "KMean_Acceleration_Signal.hpp"
#include "KNN_Accelerration_Signal.hpp"
#include <cstring>  
#include "cleaningDirectory.hpp"

int main(int argc, char* argv[])
{
   
    int  icase                  = atoi(argv[1]);
    bool benchmarkState         = static_cast<bool>(atoi(argv[2]));
   

    bool Kalman_Filter          = static_cast<bool>(atoi(argv[3]));                
    bool FFT_Filter             = static_cast<bool>(atoi(argv[4]));                
    bool BlackScholes           = static_cast<bool>(atoi(argv[5]));                
    bool SavitzkyGoly_Transform = static_cast<bool>(atoi(argv[6]));
    bool Wavelet_Filter         = static_cast<bool>(atoi(argv[26]));
    std::string _R              = (argv[7]);
    double R                    = std::atof(_R.c_str());
  
    /////////////KNN Kernel///////////////////////
    int  kNearestNeighbor       = atoi(argv[8]); 
    int  numCluster_KNN         = atoi(argv[9]);

    /////////////KMean Kernel///////////////////
    int  numCluster_kmean       = atoi(argv[10]);
    std::string  _epsilon       = (argv[11]); 
    double epsilon              = std::atof(_epsilon.c_str());
 
    ////////////Perforation Stride for Approximation Technique///////////
    int s = atoi(argv[12]);
   
    /////////////Transprecision Technique for data///////////
    int dataPrecisionCase = atoi(argv[13]);


    /////////////Hardware Architecture/////////// 
    int num_numa      = atoi(argv[14]);
    int num_core_numa = atoi(argv[15]);
    int nThread       = atoi(argv[16]);
   
         
     ////////////Workspace for data and profilling data/////////////
     std::string data_workspace          = (argv[17]);
     std::string profile_workspace       = (argv[18]);

     ////////////Input and Data for the Kernel////////////////////////
     std::string readDataPath          = (argv[19]);   
     std::string inputDataDouble       = (argv[20]);
     std::string inputDataFloat        = (argv[21]);  
     std::string inferenceKNNPath      = (argv[22]);  

     ////////////Use Case providers Statue/////////////////////
     int IDEKO_Kernel       = atoi(argv[23]); 
     int INBestMe_Kernel    = atoi(argv[24]);

     int packing2CSVformate = atoi(argv[25]);
     std::string clustring_label = argv[27];
     
     
     readDataPath          = data_workspace + readDataPath;
     inputDataDouble       = data_workspace + inputDataDouble;
     inputDataFloat        = data_workspace + inputDataFloat;
     inferenceKNNPath      = data_workspace + inferenceKNNPath;
    
     ///////////// MIN MAX CONFIG////////////////////
     double low_th = 0.1;
     double high_th = 0.7;


   
     ///////// KALMAN FILTER CONFIG //////////////////

     double const R_noice_covariance(R); //observation_covariance 
     double const H_measurement_map(1.00);//observaition matrices
     double const Q_init_estimated_covariance(0.01);//transition_covariance
     double       P_init_error_covariance(1);//initial_state_covariance
     double       K_init_kalman_gain(0);//calculated interanlly


    //////////////MPI INIT////////////////////////////
    int rank , size;  
    MPI_Init(NULL, NULL);

    MPI_Comm_rank(MPI_COMM_WORLD, & rank);
    MPI_Comm_size(MPI_COMM_WORLD, & size);
   
    char hostname[HOST_NAME_MAX];
    gethostname(hostname, HOST_NAME_MAX);
    int color = static_cast<int>(hostname[8]);
    //std::cout << hostname <<"  " << color <<  std::endl; 
    
     
   int new_rank, new_size;
   MPI_Comm row_comm;
   MPI_Comm_split(MPI_COMM_WORLD,  color, rank, & row_comm);
   MPI_Comm_rank(row_comm, &new_rank);
   MPI_Comm_size(row_comm, &new_size);
  
   ///////////////// MPI PINNING PATTERN///////////////////
   int num_cores = num_numa*num_core_numa;
   int rank_dist = num_cores/new_size;
   
    
    switch(icase){
       case 0:{
            
           if(rank == 0){  
             IDEKO_DATA<double> data(rank,size);
             data.Data_WorkSpace(data_workspace);
             data.Data_IO_Path( readDataPath);
             data.useCaseKernelApply(IDEKO_Kernel, INBestMe_Kernel);
           }
           if(rank == 0){  
             IDEKO_DATA<float> data(rank,size);
             data.Data_WorkSpace(data_workspace);
             data.Data_IO_Path( readDataPath);
             data.useCaseKernelApply(IDEKO_Kernel, INBestMe_Kernel);
           } 

          break;
        }
       case 1: {
           switch (dataPrecisionCase)
           {
             case 1:{
                   SignalProcessing<double,double> signal_process(rank,size);
                   signal_process.Data_WorkSpace(data_workspace);
                   signal_process.Profile_WorkSpace(profile_workspace);     
                   signal_process.Data_IO_Path( inputDataDouble );
                   signal_process.applyingFilters(Kalman_Filter, FFT_Filter, BlackScholes, SavitzkyGoly_Transform, Wavelet_Filter );      
                   signal_process.set_MinMaxConfiguration( low_th, high_th);
                   signal_process.perforationStride(s);
                   signal_process.benchmarkStatus(benchmarkState);
                   signal_process.set_KalmanFilterConfiguration
                        ( R_noice_covariance
                        , H_measurement_map
                        , Q_init_estimated_covariance
                        , P_init_error_covariance
                        , K_init_kalman_gain); 
                  signal_process.apply();
                   break;
                  }
              case 2:{
                  //std::cout << "double " << "float" << std::endl;
                   SignalProcessing<double,float> signal_process(rank,size);
                   signal_process.Data_WorkSpace(data_workspace);
                   signal_process.Profile_WorkSpace(profile_workspace);     
                   signal_process.Data_IO_Path( inputDataDouble );
                   signal_process.applyingFilters(Kalman_Filter, FFT_Filter, BlackScholes, SavitzkyGoly_Transform, Wavelet_Filter );      
                   signal_process.set_MinMaxConfiguration( low_th, high_th);
                   signal_process.perforationStride(s);
                   signal_process.benchmarkStatus(benchmarkState);
                   signal_process.set_KalmanFilterConfiguration
                        ( R_noice_covariance
                        , H_measurement_map
                        , Q_init_estimated_covariance
                        , P_init_error_covariance
                        , K_init_kalman_gain); 
                  signal_process.apply();
                   break;
                  }
                case 3:{
                  //std::cout << "float" << "double" << std::endl;
                   SignalProcessing<float,double> signal_process(rank,size);
                   signal_process.Data_WorkSpace(data_workspace);
                   signal_process.Profile_WorkSpace(profile_workspace);     
                   signal_process.Data_IO_Path( inputDataFloat );
                   signal_process.applyingFilters(Kalman_Filter, FFT_Filter, BlackScholes, SavitzkyGoly_Transform, Wavelet_Filter );      
                   signal_process.set_MinMaxConfiguration( low_th, high_th);
                   signal_process.perforationStride(s);
                   signal_process.benchmarkStatus(benchmarkState);
                   signal_process.set_KalmanFilterConfiguration
                        ( R_noice_covariance
                        , H_measurement_map
                        , Q_init_estimated_covariance
                        , P_init_error_covariance
                        , K_init_kalman_gain); 
                  signal_process.apply();
                   break;
                  }
                 case 4:{
                  //std::cout << "float" << "float" << std::endl;
                   SignalProcessing<float,float> signal_process(rank,size);
                   signal_process.Data_WorkSpace(data_workspace);
                   signal_process.Profile_WorkSpace(profile_workspace);     
                   signal_process.Data_IO_Path( inputDataFloat );
                   signal_process.applyingFilters(Kalman_Filter, FFT_Filter, BlackScholes, SavitzkyGoly_Transform, Wavelet_Filter );      
                   signal_process.set_MinMaxConfiguration( low_th, high_th);
                   signal_process.perforationStride(s);
                   signal_process.benchmarkStatus(benchmarkState);
                   signal_process.set_KalmanFilterConfiguration
                        ( R_noice_covariance
                        , H_measurement_map
                        , Q_init_estimated_covariance
                        , P_init_error_covariance
                        , K_init_kalman_gain); 
                  signal_process.apply();
                   break;
                  }   
               
           } 
         break;
       }
       case 2:{
               /////////////KMEAN Kernel/////////////
         switch (dataPrecisionCase)
           {
             case 1:{
                  KMeanSignal<double,double> kmean(rank,size);
                  kmean.Data_IO_Path(inputDataDouble);
                  kmean.Profile_WorkSpace(profile_workspace);    
                  kmean.Data_WorkSpace(data_workspace);
                  kmean.controlLoop(epsilon);
                  kmean.benchmarkStatus(benchmarkState);
                  kmean.apply();
                  kmean.profillingApply();
                   break;
                  }
              case 2:{
                  KMeanSignal<double,float> kmean(rank,size);
                  kmean.Data_IO_Path(inputDataDouble);
                  kmean.Profile_WorkSpace(profile_workspace);    
                  kmean.Data_WorkSpace(data_workspace);
                  kmean.controlLoop(epsilon);
                  kmean.benchmarkStatus(benchmarkState);
                  kmean.apply();
                  kmean.profillingApply();
                   break;
                  }
              case 3:{
                  KMeanSignal<float,double> kmean(rank,size);
                  kmean.Data_IO_Path(inputDataFloat);
                  kmean.Profile_WorkSpace(profile_workspace);    
                  kmean.Data_WorkSpace(data_workspace);
                  kmean.controlLoop(epsilon);
                  kmean.benchmarkStatus(benchmarkState);
                  kmean.apply();
                  kmean.profillingApply();
                   break;
                  }
               case 4:{
                  KMeanSignal<float,float> kmean(rank,size);
                  kmean.Data_IO_Path(inputDataFloat);
                  kmean.Profile_WorkSpace(profile_workspace);    
                  kmean.Data_WorkSpace(data_workspace);
                  kmean.controlLoop(epsilon);
                  kmean.benchmarkStatus(benchmarkState);
                  kmean.apply();
                  kmean.profillingApply();
                   break;
                  }         
           }
           break;
         }
        case 3:{
                   /////////////KNN Kernel/////////////
         switch (dataPrecisionCase)
           { 
             case 1:{
                  // std::cout << "double "<<" double " << std::endl;
                  KNN_Signal<double,double> knn(rank,size);
                  knn.Data_IO_Path(inputDataDouble);
                  knn.Profile_WorkSpace(profile_workspace);
                  knn.cluster_label_path(clustring_label);
       	          knn.Data_WorkSpace(data_workspace);
                  knn.getPathUnclassifiedSignal(inferenceKNNPath);   
                  knn.clusterNum(numCluster_KNN);
                  knn.benchmarkStatus(benchmarkState);
                  knn.apply();
                  knn.profillingApply();
                 break;
                 }
             case 2:{
                  // std::cout << "double "<<" float " << std::endl;
                  KNN_Signal<double,float> knn(rank,size);
                  knn.Data_IO_Path(inputDataDouble );
                  knn.Profile_WorkSpace(profile_workspace);
       	          knn.cluster_label_path(clustring_label);
       	          knn.Data_WorkSpace(data_workspace);
                  knn.getPathUnclassifiedSignal(inferenceKNNPath);   
                  knn.clusterNum(numCluster_KNN);
                  knn.benchmarkStatus(benchmarkState);
                  knn.apply();
                  knn.profillingApply();
                  break;
                 }
              case 3:{
                  // std::cout << "float "<<" double " << std::endl;
                  KNN_Signal<float,double> knn(rank,size);
                  knn.Data_IO_Path(inputDataFloat);
                  knn.Profile_WorkSpace(profile_workspace);
       	          knn.cluster_label_path(clustring_label);
       	          knn.Data_WorkSpace(data_workspace);
                  knn.getPathUnclassifiedSignal(inferenceKNNPath);   
                  knn.clusterNum(numCluster_KNN);
                  knn.benchmarkStatus(benchmarkState);
                  knn.apply();
                  knn.profillingApply();
                  break;
                  }
              case 4:{
                  // std::cout << "float "<<"float " << std::endl;
                  KNN_Signal<float,float> knn(rank,size);
                  knn.Data_IO_Path(inputDataFloat );
                  knn.Profile_WorkSpace(profile_workspace);
       	          knn.cluster_label_path(clustring_label);
       	          knn.Data_WorkSpace(data_workspace);
                  knn.getPathUnclassifiedSignal(inferenceKNNPath);   
                  knn.clusterNum(numCluster_KNN);
                  knn.benchmarkStatus(benchmarkState);
                  knn.apply();
                  knn.profillingApply();
                  break;
                  }      
            }
           break;
         }      
       case 4:{
          
          DTW<double,double> dtw;
          dtw.test();

         break; 
       }
       case 8:{
           switch (dataPrecisionCase)
           { 
             case 1:{
                  // std::cout << "double "<<" double " << std::endl;
                 KMean_AccelerateSignal<double,double> kmean(rank,size);
                 kmean.Data_IO_Path(inputDataDouble);
                 kmean.Data_WorkSpace(data_workspace);
                 kmean.Profile_WorkSpace(profile_workspace);     
                 kmean.controlLoop(epsilon);
                 kmean.benchmarkStatus(benchmarkState);
                 kmean.apply();
                 kmean.profillingApply();
                 break;
                 }
             case 2:{
                  // std::cout << "double "<<" float " << std::endl;
                  KMean_AccelerateSignal<double,float> kmean(rank,size);
                 kmean.Data_IO_Path(inputDataDouble);
                 kmean.Data_WorkSpace(data_workspace);
                 kmean.Profile_WorkSpace(profile_workspace);     
                 kmean.controlLoop(epsilon);
                 kmean.benchmarkStatus(benchmarkState);
                 kmean.apply();
                 kmean.profillingApply();
             
                  break;
                 }
              case 3:{
                    // std::cout << "float "<<" double " << std::endl;
                 KMean_AccelerateSignal<float,double> kmean(rank,size);
                 kmean.Data_IO_Path(inputDataDouble);
                 kmean.Data_WorkSpace(data_workspace);
                 kmean.Profile_WorkSpace(profile_workspace);     
                 kmean.controlLoop(epsilon);
                 kmean.benchmarkStatus(benchmarkState);
                 kmean.apply();
                 kmean.profillingApply();
             
                  break;
                  }
              case 4:{
                    // std::cout << "float "<<"float " << std::endl;
                 KMean_AccelerateSignal<float,float> kmean(rank,size);
                 kmean.Data_IO_Path(inputDataDouble);
                 kmean.Data_WorkSpace(data_workspace);
                 kmean.Profile_WorkSpace(profile_workspace);     
                 kmean.controlLoop(epsilon);
                 kmean.benchmarkStatus(benchmarkState);
                 kmean.apply();
                 kmean.profillingApply();
                  break;
                  }      
             }
             break;
           }
       case 9:{
           switch (dataPrecisionCase)
           { 
             case 1:{
                  // std::cout << "double "<<" double " << std::endl;
                 KNN_Acceleration<double,double> knn(rank,size);
                 knn.Data_IO_Path(inputDataDouble);
                 knn.Data_WorkSpace(data_workspace);
                 knn.Profile_WorkSpace(profile_workspace);     
                 knn.getPathUnclassifiedSignal(inferenceKNNPath);   
                 knn.clusterNum(numCluster_KNN);
                 knn.benchmarkStatus(benchmarkState);
                 knn.apply();
                 knn.profillingApply();
                 break;
                 }
             case 2:{
                  // std::cout << "double "<<" float " << std::endl;
                 KNN_Acceleration<double,float> knn(rank,size);
                 knn.Data_IO_Path(inputDataDouble);
                 knn.Data_WorkSpace(data_workspace);
                 knn.Profile_WorkSpace(profile_workspace);     
                 knn.getPathUnclassifiedSignal(inferenceKNNPath);   
                 knn.clusterNum(numCluster_KNN);
                 knn.benchmarkStatus(benchmarkState);
                 knn.apply();
                 knn.profillingApply();
                  break;
                 }
              case 3:{
                    // std::cout << "float "<<" double " << std::endl;
                 KNN_Acceleration<float,double> knn(rank,size);
                 knn.Data_IO_Path(inputDataDouble);
                 knn.Data_WorkSpace(data_workspace);
                 knn.Profile_WorkSpace(profile_workspace);     
                 knn.getPathUnclassifiedSignal(inferenceKNNPath);   
                 knn.clusterNum(numCluster_KNN);
                 knn.benchmarkStatus(benchmarkState);
                 knn.apply();
                 knn.profillingApply();
                  break;
                  }
              case 4:{
                      // std::cout << "float "<<"float " << std::endl;
                  KNN_Acceleration<float,float> knn(rank,size);
                  knn.Data_IO_Path(inputDataDouble);
                  knn.Data_WorkSpace(data_workspace);
                  knn.Profile_WorkSpace(profile_workspace);     
                  knn.getPathUnclassifiedSignal(inferenceKNNPath);   
                  knn.clusterNum(numCluster_KNN);
                  knn.benchmarkStatus(benchmarkState);
                  knn.apply();
                  knn.profillingApply();            
                  break;
                  }      
             }
             
         break; 
       }
       case -1:{
            
           if(rank == 0){ 
             cleaningDirectory act;
             act.Data_WorkSpace( data_workspace);
             act.cleaningInputData(inputDataDouble,inputDataFloat);
             act.applyingFilters( Kalman_Filter
                                 , FFT_Filter
                                 , BlackScholes
                                 , SavitzkyGoly_Transform );
             act.cleaningSignal();
            }
          
          break;
        }
       
      case -2 : {
            
            if(rank == 0){  
             IDEKO_DATA<double> data(rank,size);
             data.Data_WorkSpace(data_workspace);
             data.Data_IO_Path( readDataPath);
             data.applyingFilters( Kalman_Filter
                                 , FFT_Filter
                                 , BlackScholes
                                 , SavitzkyGoly_Transform );
             data.convertBackToCSVFormate(); 
            }
           
           break;   
           }
       
   }
  

   MPI_Finalize(); 

    return 0;
}


