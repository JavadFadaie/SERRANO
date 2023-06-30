#ifndef KMEAN_ACCELERATION_HPP
#define KMEAN_ACCELERATION_HPP

#include<iostream>
#include<vector>
#include <cstdlib>
#include <cstring>
#include "signal_Distributor.hpp"
#include "DistributedTimer.hpp"
#include "handelReadWrite.hpp"
#include "DTW.hpp"

template<typename I, typename O>
class KMean_AccelerateSignal{
  
   public :
   

  using  signalContainer = signalContainerAcceleration<I>;

      struct classficationInfo{
        classficationInfo( int _signal_GId
                         , int _signal_LId
                         , int _group_Id
                         , int _class_Id
                         , I   _dist_C0
                         , I   _dist_C1)
         :signal_GId(_signal_GId)
         ,signal_LId(_signal_LId)
         ,group_Id(_group_Id)
         ,class_Id(_class_Id)
         ,dist_C0(_dist_C0)
         ,dist_C1(_dist_C1)
         {}

        classficationInfo()
          :signal_GId(0)
          ,signal_LId(0)
          ,group_Id(0)
          ,class_Id(-1)
          ,dist_C0(0)
          ,dist_C1(0)
          {}

         int signal_GId;
         int signal_LId;
         int group_Id;
         int class_Id;
         I dist_C0;
         I dist_C1;
      };  
 
 public :
  
  KMean_AccelerateSignal(int _rank, int _size); 

  void Data_IO_Path( std::string const & _inputDataPath);

  void Data_WorkSpace( std::string const & _data_Workspace);

  void Profile_WorkSpace( std::string const & _profile_workspace);

  void setFilterSignalPath();

  void benchmarkStatus(bool const _benchmark_state);

  void controlLoop(I _epsilon);

  void apply();

  void loadingSignals();

  void signalDistribution();
 
  void random_centroid(); 

  void choosing_random_centroid_group( std::vector<int> & random_group);

  int const get_group_Id(int const _signal_GId);

  int const get_local_Id(int const _signal_GId);

  void Display( signalContainer const & signal );

  void kmeanClustering();

  void classifySignal();

  std::vector<I> signalDistanceComputation( std::vector<I> const & signal
                                          , std::vector<I> const & centroid_0
                                          , std::vector<I> const & centroid_1);

  void MPI_Point_Data_Type(MPI_Datatype & MPIPointData);

 
  std::vector<classficationInfo>  
  gatherDistance2Centroid(std::vector<classficationInfo> const & signalInfoDist2Centroid);

  std::vector<int> 
  identifyClusteringGroup(std::vector<classficationInfo> const & signalInfoDist2Centroid);

  void assingSignalClasses(std::vector<int> const & clusterofGroup);

  void computeCentroid(std::vector<int> const & clusterofGroup);

  void gatherClassification();

  void computeAvgCentroid();

  void writeClassificationTXT();

/*
  std::vector<Index2SensorName> readTransfIndex();

  std::string findSensorIndex(std::vector<Index2SensorName> const & Index2sensorId
                             ,int const signalIndex);
*/

  void profillingApply();                           
 
 private :

  int const MASTER = 0;

  int rank;
  int size;

  std::string inputDataPath;
  std::string kMeanAccelDataPath;

  std::string data_workspace;
  std::string profile_workspace;

  std::string  index2SensorNameTransform;

  std::unique_ptr<signalDistributor<I>> distribute;
  std::vector<signalContainer>  signalContainerVec;
  std::vector<signalContainer>  CentroidSignal;
  std::vector<classficationInfo> totalclassInfo;

  std::vector<I> sum_local_0_cluster_C_0;
  std::vector<I> sum_local_1_cluster_C_0;
  std::vector<I> sum_local_2_cluster_C_0;
  std::vector<I> sum_local_3_cluster_C_0;
  std::vector<I> sum_local_4_cluster_C_0;
  std::vector<I> sum_local_5_cluster_C_0;

  std::vector<I> sum_local_0_cluster_C_1;
  std::vector<I> sum_local_1_cluster_C_1;
  std::vector<I> sum_local_2_cluster_C_1;
  std::vector<I> sum_local_3_cluster_C_1;
  std::vector<I> sum_local_4_cluster_C_1;
  std::vector<I> sum_local_5_cluster_C_1;

  std::vector<int> groupClassId; 
  
  
  int buffer_size;
  int numGroup;
 
  bool DTW_CLASSIC;
  bool L2_METRIC;
  bool KMean_Applied;
  bool benchmark_state;


  int  controLoopIter;
  I    centeroidError;
  I    epsilon;
  double sizefileGB;
  int loopIteration;

  std::unique_ptr<DistributedTimer<I,O>> Timer;
  std::unique_ptr<ProfileHPC<I,O>>      profile;
  std::vector<Index2Sensor> Index2sensorId;
  readWriteTool<I,O> tool;
  
  int const w = 10;
  int cluster_size_0;  
  int cluster_size_1;  

  int const Class_0 = 0;
  int const Class_1 = 1;
  int const UnClass = -1;
 
};

//////////////////////////////////////////////
///////////IMPLEMANTATION
/////////////////////////////////////////////
template<typename I, typename O>
KMean_AccelerateSignal<I,O>::KMean_AccelerateSignal(int _rank, int _size)
:rank(_rank)
,size(_size)
,DTW_CLASSIC(true)
,L2_METRIC(true)
,buffer_size(0)
,controLoopIter(0)
,centeroidError(0)
,numGroup(0)
,kMeanAccelDataPath("/Output_Data/KMean/")
,cluster_size_0(0)
,cluster_size_1(0)
,loopIteration(0)
{
  Timer = std::unique_ptr<DistributedTimer<I,O> >
               (new DistributedTimer<I,O>(rank,size));
  
  profile = std::unique_ptr< ProfileHPC<I,O> >
            ( new ProfileHPC<I,O>(rank,size) );  
}

template<typename I,typename O>
void
KMean_AccelerateSignal<I,O>::Data_IO_Path
               ( std::string const & _inputDataPath)
{
   inputDataPath  = _inputDataPath;
}


template<typename I,typename O>
void
KMean_AccelerateSignal<I,O>::setFilterSignalPath()
{
   kMeanAccelDataPath = data_workspace + kMeanAccelDataPath; 
   index2SensorNameTransform =  data_workspace +"/Output_Data/Scalability/signalTransform.txt"; 

}

template<typename I, typename O>
void
KMean_AccelerateSignal<I,O>:: controlLoop(I _epsilon)
{
   epsilon = _epsilon;
}

template<typename I, typename O>
void 
KMean_AccelerateSignal<I,O>::benchmarkStatus(bool const _benchmark_state){
    benchmark_state = _benchmark_state;
}


template<typename I,typename O>
void
KMean_AccelerateSignal<I,O>::Data_WorkSpace( std::string const & _data_workspace){
   data_workspace = _data_workspace;
}

template<typename I,typename O>
void
KMean_AccelerateSignal<I,O>::Profile_WorkSpace( std::string const & _profile_workspace){
   profile_workspace = _profile_workspace;
}

template<typename I,typename O>
void
KMean_AccelerateSignal<I,O>::signalDistribution()
{
   distribute = std::unique_ptr<signalDistributor<I> >(
                         new signalDistributor<I>(rank, size)
                                  );
   distribute->Data_IO_Path( inputDataPath);
   distribute->apply();
}

template<typename I,typename O>
void
KMean_AccelerateSignal<I,O>::apply(){

   //loading the signals
  Timer->start("RKMEAN"); 
    loadingSignals();
  Timer->stop("RKMEAN");
 
   //Kmean Clustering
  Timer->start("KMEAN"); 
    kmeanClustering();
  Timer->stop("KMEAN");
   
    //Write down the classification
  Timer->start("WKMEAN"); 
    writeClassificationTXT();
  Timer->stop("WKMEAN");
 
}

template<typename I,typename O>
void 
KMean_AccelerateSignal<I,O>::profillingApply()
{
  bool KMean_Applied( true );

  profile->Data_WorkSpace(data_workspace); 
  profile->Profile_WorkSpace(profile_workspace);   
  profile->clusteringDataPath(kMeanAccelDataPath);
  profile->applyingKMeanClassification(KMean_Applied);
  profile->setFileSize(tool.numFileInsideDirectory(inputDataPath));
  profile->benchmarkStatus(benchmark_state);
  profile->controlQualityLoop(epsilon);
  profile->controlLoopIteration(loopIteration);
  profile->execuationTimeInformation(Timer->timerInformation());  
  //profile->centroidAccuracy(loopAccuracy); 
  profile->computeEnergy();
  profile->generateProfillingFile();
  
}

template<typename I,typename O>
void
KMean_AccelerateSignal<I,O>::loadingSignals(){
  setFilterSignalPath();
  signalDistribution();int group_Id(0);
  numGroup = distribute->totalNumFiles()/6;  

  for(int i(0); i< distribute->procSignalFiles().size(); i++){
         int  class_Id = -1; 
         std::string fullPath = distribute->procSignalFiles()[i];
         int signal_GId = distribute->procSignalIndex()[i];
         group_Id = get_group_Id(signal_GId);
         int signal_LId = get_local_Id(signal_GId);
         std::vector<I> const & signal_Val( tool.readBinary(fullPath) );
         signalContainer signal(signal_GId, signal_LId, group_Id, class_Id, signal_Val);
         signalContainerVec.push_back(signal);
        // Display( signal );
    }
 
    if(rank == MASTER){
       buffer_size = signalContainerVec[0].signal_Val.size(); 
      }    

     MPI_Bcast
      ( & buffer_size
      , 1
      , MPI_INT
      , MASTER
      , MPI_COMM_WORLD);
  
 //  std::cout << rank << "  " << buffer_size << std::endl;  
}

template<typename I,typename O>
int const 
KMean_AccelerateSignal<I,O>::get_group_Id(int const _signal_GId){
  return _signal_GId/6;
}

template<typename I,typename O>
int const 
KMean_AccelerateSignal<I,O>::get_local_Id(int const _signal_GId){
  return _signal_GId%6;
}


template<typename I,typename O>
void
KMean_AccelerateSignal<I,O>::random_centroid(){
    
    std::vector<int> random_group(2,0);
    choosing_random_centroid_group(random_group);   
    
    for(int j(0); j<random_group.size(); j++){
        int class_Id = j;
        for(int global_Id(0); global_Id<distribute->totalNumFiles(); global_Id++){
           
            int group_Id = get_group_Id(distribute->SignalIndex()[global_Id]);
            int local_Id = get_local_Id(distribute->SignalIndex()[global_Id]);
           
            if( random_group[j] == group_Id){
                std::string signalPath = distribute->SignalFiles()[global_Id];
                std::vector<I> const & signal_Val( tool.readBinary(signalPath) );
                signalContainer signalrandom( global_Id, local_Id, group_Id, class_Id, signal_Val);
                CentroidSignal.push_back(signalrandom);
                //Display( signalrandom );
             }
         //Display( signalrandom );
       }  
    }
   
    
}


template<typename I,typename O>
void
KMean_AccelerateSignal<I,O>::choosing_random_centroid_group( std::vector<int> & random_group){
    
    
    //choosing the random centroid from the master proc 
  if(rank == 0){   
       random_group[0] = 0;// rand()%(numGroup);
       random_group[1] = 3;// rand()%(numGroup);
  }

   //Broadcast the random centroid to the all processes   
   
   MPI_Bcast
     ( random_group.data()
     , random_group.size()
     , MPI_INT
     , MASTER
     , MPI_COMM_WORLD);
 
}

template<typename I,typename O>
void
KMean_AccelerateSignal<I,O>::kmeanClustering(){

  bool status(true);  int k(10);
  while(status){

     if(loopIteration >= k){
      status = false;
      break;
     }else{

       if(loopIteration==0){
          random_centroid();
       }  
            
       classifySignal();
       
       CentroidSignal.resize(0);
        
       computeAvgCentroid();
        /* 
       if(rank == 0){
           // std::cout <<"This new iteration "<< loopIter << std::endl;
            for(int i(0); i<CentroidSignal.size(); i++){
              signalContainer const & centroid_0(CentroidSignal[i]);
             // Display(centroid_0);
            }
       }*/
       loopIteration++;
    } 
  }    

  
   gatherClassification();
 
}


template<typename I,typename O>
void
KMean_AccelerateSignal<I,O>::classifySignal(){
  
    std::vector<classficationInfo> signalInfoDist2Centroid;
   for(int i(0); i < signalContainerVec.size(); i++){
   
      signalContainer & signal(signalContainerVec[i]);
      
      std::vector<signalContainer> procCentroidSignal;
      for(int i(0); i<CentroidSignal.size(); i++){
        if(signal.signal_LId == CentroidSignal[i].signal_LId ){
            procCentroidSignal.push_back(CentroidSignal[i]);
          }
       }
    
    
      signalContainer const & centroid_0(procCentroidSignal[0]);
      signalContainer const & centroid_1(procCentroidSignal[1]);

      std::vector<I> const & signalBuffer      (signal.signal_Val);
      std::vector<I> const & centroid_0_buffer (centroid_0.signal_Val);
      std::vector<I> const & centroid_1_buffer (centroid_1.signal_Val);

        //Compute the singnal distance to each centroid with local index
      std::vector<I> const & dist2Centroid ( signalDistanceComputation( signalBuffer
                                                                      , centroid_0_buffer
                                                                      , centroid_1_buffer) ); 
  
     
  
     // packing up the distace of the signal with theri local index with local index of the centroid                                                              
     signalInfoDist2Centroid.push_back( classficationInfo
                                            ( signal.signal_GId
                                            , signal.signal_LId 
                                            , signal.group_Id
                                            , signal.class_Id
                                            , dist2Centroid[0]
                                            , dist2Centroid[1])
                                        );
    
     
     }

   //Gather the distance and compute the distance to centroid
  std::vector<classficationInfo> const & totalDistance2Centroid ( gatherDistance2Centroid(signalInfoDist2Centroid) );
  
  //Identify the cluster of each Group
  std::vector<int> clusterofGroup (identifyClusteringGroup(totalDistance2Centroid) );
  
  computeCentroid(clusterofGroup);

  computeAvgCentroid();    
}

template<typename I, typename O>
std::vector<I>  
KMean_AccelerateSignal<I,O>::signalDistanceComputation( std::vector<I> const & signal
                                                      , std::vector<I> const & centroid_0
                                                      , std::vector<I> const & centroid_1)
{
     I min_dist = std::numeric_limits<I>::max(); 
     std::vector<I> dist2Centroid(2,0);
     DTW<I,O> dtw;
     if(DTW_CLASSIC){
   
          DTW<I,O> dtw_0(signal,centroid_0); 
          DTW<I,O> dtw_1(signal,centroid_1); 
         
          I C_0 = dtw_0.DTW_classic();
          I C_1 = dtw_1.DTW_classic();
   
          dist2Centroid[0] = C_0;
          dist2Centroid[1] = C_1;
             
     }
     else{
    
       
         I D_0 = dtw.lBKeoghLowerBound( signal, centroid_0, w);
         I D_1 = dtw.lBKeoghLowerBound( signal, centroid_1, w);

        if( D_0 < min_dist ){
            I C_0 = dtw.DTW_Dist_fast( signal, centroid_0, w); 
            
        }

        if( D_1 < min_dist ){
            I C_1 = dtw.DTW_Dist_fast( signal, centroid_1, w); 
            
        } 
     }

    return dist2Centroid;
}

template<typename I, typename O>
std::vector<typename::KMean_AccelerateSignal<I,O>::classficationInfo>
KMean_AccelerateSignal<I,O>::gatherDistance2Centroid(std::vector<classficationInfo> const & signalInfoDist2Centroid){

  std::vector<classficationInfo> totalDistance2Centroid(distribute->totalNumFiles());
  std::vector<int> const & offsetDisp(distribute->displacementDistribution());
    
  MPI_Datatype MPIPointData;
  MPI_Point_Data_Type( MPIPointData );

  MPI_Gatherv
      ( signalInfoDist2Centroid.data()    //send_data,
      , signalInfoDist2Centroid.size()    //send_count
      , MPIPointData                      //send_datatype
      , totalDistance2Centroid.data()             //recv_data,
      , & distribute->numberDataProc()[0]             
      , & offsetDisp[0]
      , MPIPointData                      //recv_datatype,
      , MASTER                            //root
      , MPI_COMM_WORLD);



 return totalDistance2Centroid;
}

template<typename I, typename O>
std::vector<int>
KMean_AccelerateSignal<I,O>::identifyClusteringGroup(std::vector<classficationInfo> const & signalInfoDist2Centroid){
   
   groupClassId.resize(numGroup,-1);
   if(rank == 0)
   {  
        for(int igroup(0); igroup<numGroup; igroup++)
        {
         I dist_C0(0); I dist_C1(0); int closest_clust(-1);
         I min_dist = std::numeric_limits<I>::max(); 
         for(int j(0); j<signalInfoDist2Centroid.size(); j++){
               if(signalInfoDist2Centroid[j].group_Id == igroup ){
                  dist_C0 += signalInfoDist2Centroid[j].dist_C0;
                  dist_C1 += signalInfoDist2Centroid[j].dist_C1;
                }
         }
         
         if(dist_C0 < min_dist){
                closest_clust = 0;
                min_dist = dist_C0;    
         }
         if(dist_C1 < min_dist){
                closest_clust = 1;
                min_dist = dist_C1;    
         }
        
         groupClassId[igroup] = closest_clust; 
        }
   }

   
     //Broadcast the group ID to other process   
     MPI_Bcast
       ( groupClassId.data()
       , groupClassId.size()
       , MPI_INT
       , MASTER
       , MPI_COMM_WORLD);
      
      /*
        if(rank == 4){
         for(int igroup(0); igroup<numGroup; igroup++){
           std::cout << "this is group id " << igroup << "  " << groupClassId[igroup] << std::endl;
           }
         }  
      */  

   return groupClassId;              
}




template<typename I, typename O>
void
KMean_AccelerateSignal<I,O>::computeCentroid(std::vector<int> const & clusterofGroup){

   sum_local_0_cluster_C_0.resize(buffer_size,0); sum_local_1_cluster_C_0.resize(buffer_size,0);
   sum_local_2_cluster_C_0.resize(buffer_size,0); sum_local_3_cluster_C_0.resize(buffer_size,0);
   sum_local_4_cluster_C_0.resize(buffer_size,0); sum_local_5_cluster_C_0.resize(buffer_size,0);

  
   sum_local_0_cluster_C_1.resize(buffer_size,0); sum_local_1_cluster_C_1.resize(buffer_size,0);
   sum_local_2_cluster_C_1.resize(buffer_size,0); sum_local_3_cluster_C_1.resize(buffer_size,0);
   sum_local_4_cluster_C_1.resize(buffer_size,0); sum_local_5_cluster_C_1.resize(buffer_size,0);
   
   for(int i(0); i<buffer_size; i++)
     {
        sum_local_0_cluster_C_0[i]=0;
        sum_local_1_cluster_C_0[i]=0;
        sum_local_2_cluster_C_0[i]=0;
        sum_local_3_cluster_C_0[i]=0;
        sum_local_4_cluster_C_0[i]=0;
        sum_local_5_cluster_C_0[i]=0;
    
        sum_local_0_cluster_C_1[i]=0;
        sum_local_1_cluster_C_1[i]=0;
        sum_local_2_cluster_C_1[i]=0;
        sum_local_3_cluster_C_1[i]=0;
        sum_local_4_cluster_C_1[i]=0;
        sum_local_5_cluster_C_1[i]=0;
    
     }

   for(int i(0); i < signalContainerVec.size(); i++){
       
       signalContainer & signal(signalContainerVec[i]);
       std::vector<I> const & signalBuffer (signal.signal_Val);
     
       signal.class_Id = clusterofGroup[signal.group_Id];
       //std::cout << rank<<" | " << signal.signal_GId << " < " << signal.group_Id << std::endl;
       //Display(signal);

                     // Compute for centroid 0
       if( (signal.class_Id == 0) && (signal.signal_LId == 0) ){
          for(int i(0); i<signalBuffer.size(); i++){
           sum_local_0_cluster_C_0[i] += signalBuffer[i];
          }
       }

       if( (signal.class_Id == 0) && (signal.signal_LId == 1) ){
          for(int i(0); i<signalBuffer.size(); i++){
           sum_local_1_cluster_C_0[i] += signalBuffer[i];
          }
       }


       if( (signal.class_Id == 0) && (signal.signal_LId == 2) ){
          for(int i(0); i<signalBuffer.size(); i++){
           sum_local_2_cluster_C_0[i] += signalBuffer[i];
          }
       }


       if( (signal.class_Id == 0) && (signal.signal_LId == 3) ){
          for(int i(0); i<signalBuffer.size(); i++){
           sum_local_3_cluster_C_0[i] += signalBuffer[i];
          }
       }


       if( (signal.class_Id == 0) && (signal.signal_LId == 4) ){
          for(int i(0); i<signalBuffer.size(); i++){
           sum_local_4_cluster_C_0[i] += signalBuffer[i];
          }
       }


       if( (signal.class_Id == 0) && (signal.signal_LId == 5) ){
          for(int i(0); i<signalBuffer.size(); i++){
           sum_local_5_cluster_C_0[i] += signalBuffer[i];
          }
       }

                 // Compute for centroid 1
       if( (signal.class_Id == 1) && (signal.signal_LId == 0) ){
          for(int i(0); i<signalBuffer.size(); i++){
           sum_local_0_cluster_C_1[i] += signalBuffer[i];
          }
       }

       if( (signal.class_Id == 1) && (signal.signal_LId == 1) ){
          for(int i(0); i<signalBuffer.size(); i++){
           sum_local_1_cluster_C_1[i] += signalBuffer[i];
          }
       }


       if( (signal.class_Id == 1) && (signal.signal_LId == 2) ){
          for(int i(0); i<signalBuffer.size(); i++){
           sum_local_2_cluster_C_1[i] += signalBuffer[i];
          }
       }


       if( (signal.class_Id == 1) && (signal.signal_LId == 3) ){
          for(int i(0); i<signalBuffer.size(); i++){
           sum_local_3_cluster_C_1[i] += signalBuffer[i];
          }
       }


       if( (signal.class_Id == 1) && (signal.signal_LId == 4) ){
          for(int i(0); i<signalBuffer.size(); i++){
           sum_local_4_cluster_C_1[i] += signalBuffer[i];
          }
       }


       if( (signal.class_Id == 1) && (signal.signal_LId == 5) ){
          for(int i(0); i<signalBuffer.size(); i++){
           sum_local_5_cluster_C_1[i] += signalBuffer[i];
          }
       }

   }


}


template<typename I, typename O>
void
KMean_AccelerateSignal<I,O>::computeAvgCentroid(){
  
  std::vector<I> centroid_local_0_cluster_0(buffer_size,0);
  MPI_Reduce(
     sum_local_0_cluster_C_0.data()
    ,centroid_local_0_cluster_0.data()
    ,buffer_size
    ,mpi_get_type<I>()
    ,MPI_SUM
    ,MASTER
    ,MPI_COMM_WORLD);

  std::vector<I> centroid_local_1_cluster_0(buffer_size,0);
  MPI_Reduce(
     sum_local_1_cluster_C_0.data()
    ,centroid_local_1_cluster_0.data()
    ,buffer_size
    ,mpi_get_type<I>()
    ,MPI_SUM
    ,MASTER
    ,MPI_COMM_WORLD);

   
  std::vector<I> centroid_local_2_cluster_0(buffer_size,0);
  MPI_Reduce(
     sum_local_2_cluster_C_0.data()
    ,centroid_local_2_cluster_0.data()
    ,buffer_size
    ,mpi_get_type<I>()
    ,MPI_SUM
    ,MASTER
    ,MPI_COMM_WORLD);

   
  std::vector<I> centroid_local_3_cluster_0(buffer_size,0);
  MPI_Reduce(
     sum_local_3_cluster_C_0.data()
    ,centroid_local_3_cluster_0.data()
    ,buffer_size
    ,mpi_get_type<I>()
    ,MPI_SUM
    ,MASTER
    ,MPI_COMM_WORLD);    

  
  std::vector<I> centroid_local_4_cluster_0(buffer_size,0);
  MPI_Reduce(
     sum_local_4_cluster_C_0.data()
    ,centroid_local_4_cluster_0.data()
    ,buffer_size
    ,mpi_get_type<I>()
    ,MPI_SUM
    ,MASTER
    ,MPI_COMM_WORLD);
  
  std::vector<I> centroid_local_5_cluster_0(buffer_size,0);
  MPI_Reduce(
     sum_local_5_cluster_C_0.data()
    ,centroid_local_5_cluster_0.data()
    ,buffer_size
    ,mpi_get_type<I>()
    ,MPI_SUM
    ,MASTER
    ,MPI_COMM_WORLD);

   // compute Average for centroid 2
    std::vector<I> centroid_local_0_cluster_1(buffer_size,0);
  MPI_Reduce(
     sum_local_0_cluster_C_1.data()
    ,centroid_local_0_cluster_1.data()
    ,buffer_size
    ,mpi_get_type<I>()
    ,MPI_SUM
    ,MASTER
    ,MPI_COMM_WORLD);

  std::vector<I> centroid_local_1_cluster_1(buffer_size,0);
  MPI_Reduce(
     sum_local_1_cluster_C_1.data()
    ,centroid_local_1_cluster_1.data()
    ,buffer_size
    ,mpi_get_type<I>()
    ,MPI_SUM
    ,MASTER
    ,MPI_COMM_WORLD);

   
  std::vector<I> centroid_local_2_cluster_1(buffer_size,0);
  MPI_Reduce(
     sum_local_2_cluster_C_1.data()
    ,centroid_local_2_cluster_1.data()
    ,buffer_size
    ,mpi_get_type<I>()
    ,MPI_SUM
    ,MASTER
    ,MPI_COMM_WORLD);

   
  std::vector<I> centroid_local_3_cluster_1(buffer_size,0);
  MPI_Reduce(
     sum_local_3_cluster_C_1.data()
    ,centroid_local_3_cluster_1.data()
    ,buffer_size
    ,mpi_get_type<I>()
    ,MPI_SUM
    ,MASTER
    ,MPI_COMM_WORLD);    

  
  std::vector<I> centroid_local_4_cluster_1(buffer_size,0);
  MPI_Reduce(
     sum_local_4_cluster_C_1.data()
    ,centroid_local_4_cluster_1.data()
    ,buffer_size
    ,mpi_get_type<I>()
    ,MPI_SUM
    ,MASTER
    ,MPI_COMM_WORLD);
  
  std::vector<I> centroid_local_5_cluster_1(buffer_size,0);
  MPI_Reduce(
     sum_local_5_cluster_C_1.data()
    ,centroid_local_5_cluster_1.data()
    ,buffer_size
    ,mpi_get_type<I>()
    ,MPI_SUM
    ,MASTER
    ,MPI_COMM_WORLD); 

  //compute the Avg of centroid
  if(rank == MASTER)
    {  

       std::vector<int> cluster_size(2);
       for(int i(0); i< groupClassId.size(); i++){
          if(groupClassId[i] == 0){
            cluster_size[0]++;
           }
          if(groupClassId[i] == 1){
            cluster_size[1]++;
          }
        }

        if(cluster_size[0] == 0){
          cluster_size[0] = 1;
        }
        if(cluster_size[1] == 0){
          cluster_size[1] = 1;
        }

        cluster_size_0 = cluster_size[0];
        cluster_size_1 = cluster_size[1];

       for(int i(0); i<centroid_local_0_cluster_0.size(); i++)
       {
          centroid_local_0_cluster_0[i] /= static_cast<I>(cluster_size[0]);
          centroid_local_1_cluster_0[i] /= static_cast<I>(cluster_size[0]);
          centroid_local_2_cluster_0[i] /= static_cast<I>(cluster_size[0]);
          centroid_local_3_cluster_0[i] /= static_cast<I>(cluster_size[0]);
          centroid_local_4_cluster_0[i] /= static_cast<I>(cluster_size[0]);
          centroid_local_5_cluster_0[i] /= static_cast<I>(cluster_size[0]); 

          centroid_local_0_cluster_1[i] /= static_cast<I>(cluster_size[1]);
          centroid_local_1_cluster_1[i] /= static_cast<I>(cluster_size[1]);
          centroid_local_2_cluster_1[i] /= static_cast<I>(cluster_size[1]);
          centroid_local_3_cluster_1[i] /= static_cast<I>(cluster_size[1]);
          centroid_local_4_cluster_1[i] /= static_cast<I>(cluster_size[1]);
          centroid_local_5_cluster_1[i] /= static_cast<I>(cluster_size[1]);  
       }

    }
   // Broadcast the centroid to each rank
    MPI_Bcast
      ( centroid_local_0_cluster_0.data()
      , buffer_size
      , mpi_get_type<I>()
      , MASTER
      , MPI_COMM_WORLD); 

     MPI_Bcast
      ( centroid_local_1_cluster_0.data()
      , buffer_size
      , mpi_get_type<I>()
      , MASTER
      , MPI_COMM_WORLD);   
  
     MPI_Bcast
      ( centroid_local_2_cluster_0.data()
      , buffer_size
      , mpi_get_type<I>()
      , MASTER
      , MPI_COMM_WORLD); 
     
      MPI_Bcast
      ( centroid_local_3_cluster_0.data()
      , buffer_size
      , mpi_get_type<I>()
      , MASTER
      , MPI_COMM_WORLD);

      MPI_Bcast
      ( centroid_local_4_cluster_0.data()
      , buffer_size
      , mpi_get_type<I>()
      , MASTER
      , MPI_COMM_WORLD);

      MPI_Bcast
      ( centroid_local_5_cluster_0.data()
      , buffer_size
      , mpi_get_type<I>()
      , MASTER
      , MPI_COMM_WORLD);     

     //Broad cast the centroid 1 to each ranks
      MPI_Bcast
      ( centroid_local_0_cluster_1.data()
      , buffer_size
      , mpi_get_type<I>()
      , MASTER
      , MPI_COMM_WORLD); 

     MPI_Bcast
      ( centroid_local_1_cluster_1.data()
      , buffer_size
      , mpi_get_type<I>()
      , MASTER
      , MPI_COMM_WORLD);   
   
     MPI_Bcast
      ( centroid_local_2_cluster_1.data()
      , buffer_size
      , mpi_get_type<I>()
      , MASTER
      , MPI_COMM_WORLD); 
     
      MPI_Bcast
      ( centroid_local_3_cluster_1.data()
      , buffer_size
      , mpi_get_type<I>()
      , MASTER
      , MPI_COMM_WORLD);

      MPI_Bcast
      ( centroid_local_4_cluster_1.data()
      , buffer_size
      , mpi_get_type<I>()
      , MASTER
      , MPI_COMM_WORLD);

      MPI_Bcast
      ( centroid_local_5_cluster_1.data()
      , buffer_size
      , mpi_get_type<I>()
      , MASTER
      , MPI_COMM_WORLD);  

     //reconstructing the centroid C_0 for the next iteration  
     int centroid_global_Id_C0 = 0; int centroid_local_Id_C0 = 0;
     int centroid_group_Id_C0 = -1; int centroid_class_Id_C0 = 0;
    
     signalContainer centroid_local_0_C0( signalContainer(centroid_global_Id_C0
                                                         ,centroid_local_Id_C0 
                                                         ,centroid_group_Id_C0
                                                         ,centroid_class_Id_C0
                                                         ,centroid_local_0_cluster_0 ) );
     CentroidSignal.push_back(centroid_local_0_C0);                                                    
     
     centroid_global_Id_C0 = 0;  centroid_local_Id_C0 = 1;
     centroid_group_Id_C0 = -1;  centroid_class_Id_C0 = 0;
     signalContainer centroid_local_1_C0( signalContainer(centroid_global_Id_C0
                                                         ,centroid_local_Id_C0 
                                                         ,centroid_group_Id_C0
                                                         ,centroid_class_Id_C0
                                                         ,centroid_local_1_cluster_0 ) );
     CentroidSignal.push_back(centroid_local_1_C0);

     centroid_global_Id_C0 = 0;  centroid_local_Id_C0 = 2;
     centroid_group_Id_C0 = -1;  centroid_class_Id_C0 = 0;
     signalContainer centroid_local_2_C0( signalContainer(centroid_global_Id_C0
                                                         ,centroid_local_Id_C0 
                                                         ,centroid_group_Id_C0
                                                         ,centroid_class_Id_C0
                                                         ,centroid_local_2_cluster_0 ) );
     CentroidSignal.push_back(centroid_local_2_C0); 

     centroid_global_Id_C0 = 0;  centroid_local_Id_C0 = 3;
     centroid_group_Id_C0 = -1;  centroid_class_Id_C0 = 0;
     signalContainer centroid_local_3_C0( signalContainer(centroid_global_Id_C0
                                                         ,centroid_local_Id_C0 
                                                         ,centroid_group_Id_C0
                                                         ,centroid_class_Id_C0
                                                         ,centroid_local_3_cluster_0 ) );
     
     CentroidSignal.push_back(centroid_local_3_C0);

     centroid_global_Id_C0 = 0;  centroid_local_Id_C0 = 4;
     centroid_group_Id_C0 = -1;  centroid_class_Id_C0 = 0;
     signalContainer centroid_local_4_C0( signalContainer(centroid_global_Id_C0
                                                         ,centroid_local_Id_C0 
                                                         ,centroid_group_Id_C0
                                                         ,centroid_class_Id_C0
                                                         ,centroid_local_4_cluster_0 ) );
     CentroidSignal.push_back(centroid_local_4_C0);

     centroid_global_Id_C0 = 0;  centroid_local_Id_C0 = 5;
     centroid_group_Id_C0 = -1;  centroid_class_Id_C0 = 0;
     signalContainer centroid_local_5_C0( signalContainer(centroid_global_Id_C0
                                                         ,centroid_local_Id_C0 
                                                         ,centroid_group_Id_C0
                                                         ,centroid_class_Id_C0
                                                         ,centroid_local_5_cluster_0 ) );
    CentroidSignal.push_back(centroid_local_5_C0);
     

    //reconstructing the centroid C_1 for the next iteration  
    int centroid_global_Id_C1 = 0;  int centroid_local_Id_C1 = 0;
    int centroid_group_Id_C1 = -1;  int centroid_class_Id_C1 = 1;
    signalContainer centroid_local_0_C1( signalContainer(centroid_global_Id_C1
                                                         ,centroid_local_Id_C1 
                                                         ,centroid_group_Id_C1
                                                         ,centroid_class_Id_C1
                                                         ,centroid_local_0_cluster_1 ) );
    CentroidSignal.push_back(centroid_local_0_C1);
    

    centroid_global_Id_C1 = 0;  centroid_local_Id_C1 = 1;
    centroid_group_Id_C1 = -1;  centroid_class_Id_C1 = 1;
    signalContainer centroid_local_1_C1( signalContainer(centroid_global_Id_C1
                                                         ,centroid_local_Id_C1 
                                                         ,centroid_group_Id_C1
                                                         ,centroid_class_Id_C1
                                                         ,centroid_local_1_cluster_1 ) );
     CentroidSignal.push_back(centroid_local_1_C1);
   
    
    centroid_global_Id_C1 = 0;  centroid_local_Id_C1 = 2;
    centroid_group_Id_C1 = -1;  centroid_class_Id_C1 = 1;
    signalContainer centroid_local_2_C1( signalContainer(centroid_global_Id_C1
                                                         ,centroid_local_Id_C1 
                                                         ,centroid_group_Id_C1
                                                         ,centroid_class_Id_C1
                                                         ,centroid_local_2_cluster_1 ) );
    CentroidSignal.push_back(centroid_local_2_C1);
    
    centroid_global_Id_C1 = 0;  centroid_local_Id_C1 = 3;
    centroid_group_Id_C1 = -1;  centroid_class_Id_C1 = 1;
    signalContainer centroid_local_3_C1( signalContainer(centroid_global_Id_C1
                                                         ,centroid_local_Id_C1 
                                                         ,centroid_group_Id_C1
                                                         ,centroid_class_Id_C1
                                                         ,centroid_local_3_cluster_1 ) );
    CentroidSignal.push_back(centroid_local_3_C1);
   
    centroid_global_Id_C1 = 0;  centroid_local_Id_C1 = 4;
    centroid_group_Id_C1 = -1;  centroid_class_Id_C1 = 1;
    signalContainer centroid_local_4_C1( signalContainer(centroid_global_Id_C1
                                                         ,centroid_local_Id_C1 
                                                         ,centroid_group_Id_C1
                                                         ,centroid_class_Id_C1
                                                         ,centroid_local_3_cluster_1 ) );
    CentroidSignal.push_back(centroid_local_4_C1);
   
    centroid_global_Id_C1 = 0;  centroid_local_Id_C1 = 5;
    centroid_group_Id_C1 = -1;  centroid_class_Id_C1 = 1;
    signalContainer centroid_local_5_C1( signalContainer(centroid_global_Id_C1
                                                         ,centroid_local_Id_C1 
                                                         ,centroid_group_Id_C1
                                                         ,centroid_class_Id_C1
                                                         ,centroid_local_5_cluster_1 ) );
    CentroidSignal.push_back(centroid_local_5_C1);
   
}

template<typename I, typename O>
void
KMean_AccelerateSignal<I,O>::gatherClassification(){

   std::vector<classficationInfo> classInfo;
   totalclassInfo.resize(distribute->totalNumFiles());
   std::vector<int> const & offsetDisp(distribute->displacementDistribution());
  
  for(int i(0); i<signalContainerVec.size(); i++){
    int signal_GId = signalContainerVec[i].signal_GId;
    int signal_LId = signalContainerVec[i].signal_LId;
    int group_Id = signalContainerVec[i].group_Id;
    int class_Id = signalContainerVec[i].class_Id;
    I dist_C0=0; I dist_C1=0;
    classInfo.push_back(classficationInfo(signal_GId,signal_LId,group_Id,class_Id,dist_C0,dist_C1));
   } 
  
  MPI_Datatype MPIPointData;
  MPI_Point_Data_Type( MPIPointData );

  MPI_Gatherv
      ( classInfo.data()        //send_data,
      , classInfo.size()        //send_count
      , MPIPointData            //send_datatype
      , totalclassInfo.data()   //recv_data,
      , & distribute->numberDataProc()[0]             
      , & offsetDisp[0]
      , MPIPointData            //recv_datatype,
      , MASTER                  //root
      , MPI_COMM_WORLD);

               

}


template<typename I, typename O>
void
KMean_AccelerateSignal<I,O>::writeClassificationTXT(){


   if(rank == 0){
      std::string cluster = kMeanAccelDataPath + "KMean_cluster.csv";
      char Cluster[ cluster.size() + 1];
      strcpy(Cluster, cluster.c_str());
      std::ofstream cluster_output;
      cluster_output.open(Cluster);
       
      std::vector<typename::readWriteTool<I,O>::Index2SensorName> Index2sensorId(tool.readTransfIndex(index2SensorNameTransform));
      cluster_output<< "timeseries;labels" <<std::endl; 
      for(int i(0); i<totalclassInfo.size(); i++){ 
         //std::cout << totalclassInfo[i].signalId<<" | "<< totalclassInfo[i].classID << std::endl;
         int classID =UnClass;
           std::string sensorName ( tool.findSensorNameId(Index2sensorId, totalclassInfo[i].signal_GId) );
         // std::string sensorName ( findSensorIndex(Index2sensorId, totalclassInfo[i].signal_GId) );
         if(cluster_size_0 > cluster_size_1){
            classID =  totalclassInfo[i].class_Id ; 
         }else
         {
            if(totalclassInfo[i].class_Id == Class_0){
              classID = Class_1;
            }

            if(totalclassInfo[i].class_Id == Class_1){
              classID = Class_0;
            }
         }
         cluster_output << sensorName<<";"<< classID  << std::endl; 
     }

 
   }            

} 

/*
template<typename I, typename O>
std::vector<typename::KMean_AccelerateSignal<I,O>::Index2SensorName>
KMean_AccelerateSignal<I,O>::readTransfIndex(){
  
 std::string path =  data_workspace +"/Output_Data/Scalability/signalTransform.txt"; 
 char filename[ path.size() + 1];
 strcpy(filename, path.c_str());
 std::ifstream ifs(filename);
 std::string line;  std::vector<Index2SensorName> Index2sensorId; 
 // read one line from ifs
 while(std::getline(ifs, line)){
    std::istringstream iss(line); 
    int signal_Id;
    std::string sensor_name;
    
    iss >> signal_Id >> sensor_name ; 
    Index2sensorId.push_back(Index2SensorName(signal_Id,sensor_name));
    }
 return Index2sensorId;

}

template<typename I, typename O>
std::string
KMean_AccelerateSignal<I,O>::findSensorIndex(std::vector<Index2SensorName> const & Index2sensorId
                                 ,int const signalIndex){
  
  auto it = std::find_if( Index2sensorId.begin()
                        , Index2sensorId.end()
                        , [&](Index2SensorName const & obj)
                            {
                                 return obj.signal_Id == signalIndex;
                            }
                 );     
   
   return it->sensor_name;
}

*/


template<typename I, typename O>
void
KMean_AccelerateSignal<I,O>::MPI_Point_Data_Type(MPI_Datatype & MPIPointData){
  
    const int    nItems=6;
    int          blocklengths[nItems] = { 1, 1, 1, 1, 1, 1 };
    MPI_Datatype types[nItems] = { MPI_INT, MPI_INT, MPI_INT, MPI_INT, mpi_get_type<I>(), mpi_get_type<I>() };
    MPI_Aint     offsets[nItems];

    offsets[0] = offsetof(classficationInfo, signal_GId);
    offsets[1] = offsetof(classficationInfo, signal_LId);
    offsets[2] = offsetof(classficationInfo, group_Id);
    offsets[3] = offsetof(classficationInfo, class_Id);
    offsets[4] = offsetof(classficationInfo, dist_C0);
    offsets[5] = offsetof(classficationInfo, dist_C1);

    MPI_Type_create_struct(nItems, blocklengths, offsets, types, & MPIPointData);
    MPI_Type_commit(& MPIPointData);
}

template<typename I, typename O>
void
KMean_AccelerateSignal<I,O>::Display( signalContainer const & signal ){

  
  std::cout << std::endl;
  std::cout << std::endl;
  std::cout << "signal group ID  " << signal.group_Id<<  std::endl;
  std::cout << "signal global ID  " << signal.signal_GId<<  std::endl;
  std::cout << "signal local ID  " << signal.signal_LId<<  std::endl;
  std::cout << "signal class ID  " << signal.class_Id<<  std::endl;
  
 /*
  for(int i(0); i<signal.signal_Val.size(); i++){
    std::cout << signal.signal_Val[i] << "  ";
  }
*/
   std::cout << std::endl;
   std::cout << std::endl;
 }
#endif