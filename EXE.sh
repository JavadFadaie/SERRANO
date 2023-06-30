#!/bin/bash


     #Choosing the Kernel
icase=0

      #Choosing the Kernel Type
IDEKO_Kernel=0
INBestMe_Kernel=1

    #Packing the into CSV format
packing2CSVformat=1

     #Benchmark Status
BenchmarkState=0

   #Applying Filter (1: Filter apply, 0: Filter does not apply)
Kalman_Filter=0    
FFT_Filter=0
BlackScholes=1
SavitzkyGoly_Transform=0
Wavelet_Filter=0

     #Kalman Filter Parameters
R="1"

      #KNN Parameters
K_nearest_neighbor=7; 
Cluster_number_KNN=2;


    #Kmean Parameters
number_cluster_kmean=2
epsilon_criteria=0.0001 

     #Parallelization Parameters
num_MPI_Procs=1
num_Thread=1

     #Approximation Computing techniques
perforation_stride=1

      #Transprecision Techniques
precision_senario=2

    #Hardware Configurationvim 
num_numa=8
num_core_numa=16

    #Setting up the workspace
workspace="/home/javad/Desktop/Serrano/serrano/data"
profillgWorkSpace="/home/javad/Desktop/Serrano/serrano/profile"

 #Setting up the workspace for Hawk compute node
#workspace="/zhome/academic/HLRS/hlrs/hpcjfada/serrano/serrano/data"
#profillgWorkSpace="/zhome/academic/HLRS/hlrs/hpcjfada/serrano/serrano/profile"



     #Path for input and output data
#readInputData="/Init_Data/sample/sample.csv";
#readInputData="/Init_Data/sample/sampleAccel.csv";
#readInputData="/Init_Data/sample/preprocessed_dataset.csv";
readInputData="/Init_Data/raw_data_InBestMe/10_asset_prices.csv";
#readInputData="/Init_Data//inference_data_position/1674644652755.csv";
#readInputData="/Init_Data/raw_data_position/position_110.csv";
#readInputData="/Init_Data/raw_data_input_fft/acceleration_cycle_26.csv";
#readInputData="/Init_Data/raw_data_acceleration_KMean/postprocessed_ffts_26.csv";
inputDataDouble="/Input_Data/Double_Data_Type/signalFilter"
inputDataFloat="/Input_Data/Float_Data_Type/signalFilter"
#inferenceKNNPath="/Init_Data/inference_data_position/Acceleration/"
inferenceKNNPath="/Init_Data/inference_data_position/"
outputDataCSV="data/outputData/FFTFilter/FFT.csv"


EXE=build/SERRANO


mpirun --oversubscribe -np $num_MPI_Procs $EXE $icase $BenchmarkState $Kalman_Filter $FFT_Filter $BlackScholes $SavitzkyGoly_Transform $R $K_nearest_neighbor $Cluster_number_KNN $number_cluster_kmean $epsilon_criteria $perforation_stride $precision_senario $num_numa $num_core_numa $num_Thread $workspace $profillgWorkSpace $readInputData $inputDataDouble $inputDataFloat $inferenceKNNPath $IDEKO_Kernel $INBestMe_Kernel $packing2CSVformat $Wavelet_Filter
