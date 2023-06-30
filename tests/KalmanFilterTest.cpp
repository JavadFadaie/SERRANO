#include<iostream>
#include "../src/KalmanFilter.hpp"
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


class Data{

 public :
 Data(){}
 
 void read(){
    FILE* pFile;
  pFile = fopen("../data/AmplituteData", "rb");

  Buffer.resize(5344);
  fread(& Buffer[0], Buffer.size(), sizeof(double), pFile);
  fclose(pFile); 

  
 }
 
 std::vector<double> const & getBuffer(){
   return Buffer;
 }

 private:
 std::vector<double> Buffer;

};

TEST(KalmanFilterTest, MultiThreadKalmanFilter)
{   
  double const R_noice_covariance(20);
  double const H_measurement_map(1.00);
  double const Q_init_estimated_covariance(10);
  double       P_init_error_covariance(0);
  double       K_init_kalman_gain(0);
 
  Data data;
  data.read();

  int rank; int size;   
  MPI_Init(NULL, NULL);

  MPI_Comm_rank(MPI_COMM_WORLD, & rank);
  MPI_Comm_size(MPI_COMM_WORLD, & size);
 
  
  KalmanFilter<double> _kalmanfilter(data.getBuffer()
                                    ,rank
                                    ,size);
  
  
  _kalmanfilter.set_KalmanFilterConfiguration
    ( R_noice_covariance
    , H_measurement_map
    , Q_init_estimated_covariance
    , P_init_error_covariance
    , K_init_kalman_gain);
    
   _kalmanfilter.KalmanFilterSequential();  
    
   std::vector<double> const & outputSequential(_kalmanfilter.getOutPut()); 
   
   
   
  KalmanFilter<double> _kalmanfilterParallel( data.getBuffer()
                                            , rank
                                            , size);
  _kalmanfilterParallel.set_KalmanFilterConfiguration
    ( R_noice_covariance
    , H_measurement_map
    , Q_init_estimated_covariance
    , P_init_error_covariance
    , K_init_kalman_gain);
    
  _kalmanfilterParallel.KalmanFilter_apply();  
    
   std::vector<double> const & outputParallel(_kalmanfilterParallel.getOutPut()); 
 
   int counter(0); 
   for(int i(0); i<data.getBuffer().size(); i++){
     ASSERT_DOUBLE_EQ(outputSequential[i] , outputParallel[i]);
     //std::cout << outputSequential[i] <<"  " <<  outputParallel[i] << std::endl;
    }
    
    MPI_Finalize();  
   
}

TEST(KalmanFilterTest, DistributedKalmanFilter)
{   
  int length(5344);

    FILE* pFileSeq;
    pFileSeq = fopen("../data/SPX_KalmanFilter", "rb");
    std::vector<double> kalmanFilterSeq(5344);
    fread(& kalmanFilterSeq[0], kalmanFilterSeq.size(), sizeof(double), pFileSeq);
    fclose(pFileSeq);
   

    FILE* pFilePar;
    pFilePar = fopen("../data/SPX_KalmanFilterParallel", "rb");
    std::vector<double> KalmanFilterDistributed(5344);
    fread(& KalmanFilterDistributed[0], KalmanFilterDistributed.size(), sizeof(double), pFilePar);
    fclose(pFilePar);
   
    std::vector<double> error(5344);
  

    for(int i(0); i<5344; i++){
     error[i] = abs(KalmanFilterDistributed[i] - kalmanFilterSeq[i]);
    }

   auto minmax = std::minmax_element(error.begin(), error.end());
     
    auto minmaxKalmanSeq = std::minmax_element(kalmanFilterSeq.begin(), kalmanFilterSeq.end());
  
   std::cout << "This is the  error "<<  *minmax.second << std::endl;  
  double  averageSeq = std::accumulate( kalmanFilterSeq.begin(), kalmanFilterSeq.end(), 0.0);
   averageSeq /= length;
  
   double  averageParallel = std::accumulate( KalmanFilterDistributed.begin(), KalmanFilterDistributed.end(), 0.0);
   averageParallel /= length;
   auto minmaxKalmanParallel = std::minmax_element(KalmanFilterDistributed.begin(), KalmanFilterDistributed.end());
  

   std::cout << "this average Seq " << averageSeq << "  " << * minmaxKalmanSeq.first<<"  " << * minmaxKalmanSeq.second << std::endl;
   
   std::cout << "this average Parallel " << averageParallel << "  " << * minmaxKalmanParallel.first<<"  " << * minmaxKalmanParallel.second << std::endl;
   /*
   double abs_error(0.001);
   ASSERT_NEAR(* minmaxKalmanSeq.first , * minmaxKalmanParallel.second, abs_error);
     
   ASSERT_NEAR(* minmaxKalmanSeq.first ,  * minmaxKalmanParallel.second, abs_error);
   
   ASSERT_NEAR( averageSeq ,  averageParallel, abs_error);
    */

}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
