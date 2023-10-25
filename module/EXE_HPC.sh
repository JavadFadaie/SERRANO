#!/bin/bash

     #Choosing the Kernel
icase=2

     #Signal ID for Filtering
id=3

     #Benchmark Status
BenchmarkState=0

     #Applying Filter (1: Filter apply, 0: Filter does not apply)
Kalman_Filter=1
MinMax_Transform=0
FFT_Filter=0
SavitzkyGoly_Transform=0


     #Kalman Filter Parameters
R="1"
R_range_offset=0;
R_range_endset=100;

      #KNN Parameters
K_nearest_neighbor=7; 
Cluster_number_KNN=2;

    #Kmean Parameters
number_cluster_kmean=2
epsilon_criteria=0.005 
           

     #Approximation Computing techniques
perforation_stride=1

     #Transprecision Computing techniques 
precision_senario=4

     #Parallelization Parameters
num_MPI_Procs=8
numCommunicatoi=1
num_Thread=1


      #Hardware Configuration
num_numa=4
num_core_numa=16


     #Setting up the workspace
workspace="/nfs_home/hpcjfada/SERRANO/serrano/data"
profillgWorkSpace="/nfs_home/hpcjfada/SERRANO/serrano/profile"


     #Path for input and output data
readInputData="/Input_Data/Init_Data/IDEKO.csv"
inputDataDouble="/Input_Data/Double_Data_Type/signalFilter/signal"
inputDataFloat="/Input_Data/Float_Data_Type/signalFilter/signal"
binaryDataDoubleKNN="/Input_Data/Double_Data_Type/KNN/knn"
binaryDataFloatKNN="/Input_Data/Float_Data_Type/KNN/knn"
binaryDataDoubleKmean="/Input_Data/Double_Data_Type/KMean/kmean"    
binaryDataFloatKmean="/Input_Data/Float_Data_Type/KMean/kmean"  
binaryDataDTWx="/Input_Data/DTWx";    
binaryDataDTWy="/Input_Data/DTWy";   
binaryStockPrice="/Input_Data/DTWx";  
binaryBSholes="/Output_Data/BlackScholes";  

   #Path For Output Data
kalmanDataPath="/Output_Data/kalmanFilter/KalmanFilter";
fftDataPath="/Output_Data/FFTFilter/FFTFilter";
minMaxDataPath="/Output_Data/MinMax/MinMaxUpdate";
SavitzkeyGoly="/Output_Data/SavitzkeyGolay/SavitzkeyGolyFilter";    
KmeanCluster="/Output_Data/KMean/KmeanCluter";    
KNN="/Output_Data/KNN/KNN";



module load compiler/gcc/11.2.0
module load mpi/openmpi/4.1.1-gcc-11.2.0

EXE=/nfs_home/hpcjfada/SERRANO/serrano/build/SERRANO

freq_stp=0
sudo /opt/power/rome-freq/bin/set_node01_user_frequency.sh $freq_stp

mpirun --mca pml ob1 --mca btl tcp,self --bind-to core -n $num_MPI_Procs $EXE $icase $id $R $num_Thread $num_numa $num_core_numa $Kalman_Filter $FFT_Filter $MinMax_Transform $workspace $readInputData $inputDataDouble $kalmanDataPath $fftDataPath $minMaxDataPath $binaryDataDoubleKNN $binaryDataDTWx $binaryDataDTWy $binaryStockPrice $binaryBSholes $binaryDataDoubleKmean $KmeanCluster $SavitzkeyGoly $perforation_stride $precision_senario $inputDataFloat $SavitzkyGoly_Transform $profillgWorkSpace $BenchmarkState $K_nearest_neighbor $Cluster_number_KNN $KNN $number_cluster_kmean $epsilon_criteria $binaryDataFloatKNN $binaryDataFloatKmean
 


sudo /opt/power/rome-freq/bin/set_node01_default_frequency.sh
