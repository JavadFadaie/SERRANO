#!/bin/bash

#1:KalmanFilter 2:MinMaxUpdate 3:FFTFilter 4:SavitzkyGolyTransform 5:KNN 6:KMean

     #Choosing Filter
kernel="KalmanFilter"

     #Input data size
Data_size="110"

      #Data percision for Input and Output data (transprecision techniques)
Input_data_precision="NULL"
Output_data_precision="NULL"


     #Applying the approximate techniques
Apply_approximation_techniques=-1

 
      #Applying the affordable error 
Apply_error_range=1
error_offset="50"
error_endset="150"

      #compute Scalability 
computeScalability="false"


      #Exess cluster state 
ExessClusterState=0
 
  
profile/uuvq $kernel $Data_size $Input_data_precision $Output_data_precision $Apply_approximation_techniques $error_offset $error_endset $Apply_error_range $computeScalability $ExessClusterState


#compile for the auto 
#g++ -fconcepts uuvq.cpp -o uuvq

