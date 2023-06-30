#ifndef KMEANSIGNAL_HPP
#define KMEANSIGNAL_HPP


#include<iostream>
#include<vector>
#include <cstdlib>
#include <cstring>
#include "signal_Distributor.hpp"
#include "DistributedTimer.hpp"
#include "DTW.hpp"
#include "signalContainer.hpp"
#include "handelReadWrite.hpp"

template<typename I, typename O>
class KMeanSignal{
 
  public:

  
    struct classficationInfo{
     classficationInfo(int _signalId, int _classID)
     :signalId(_signalId)
     ,classID(_classID)
     {}

     classficationInfo()
     :signalId(0)
     ,classID(0)
     {}

     int signalId;
     int classID; 
    };
 
 
  KMeanSignal(int _rank, int _size); 
  
  void Data_IO_Path( std::string const & _inputDataPath);
  
  void Data_WorkSpace( std::string const & _data_Workspace);

  void Profile_WorkSpace( std::string const & _profile_workspace);

  void setFilterSignalPath();

  void benchmarkStatus(bool const _benchmark_state);

  void controlLoop(double const _epsilon);

  void profillingApply();

  void apply();

  void loadingSignals();

  void kmeanClustering();
  
  void signalDistribution();

  void random_centroid();

  void choosing_random_centroid_indices( std::vector<int> & random_index);

  void classifySignal();

  void recomputeCenteroid();

  void DisplaySignalClassification();

  int const signal_classification( typename::signalContainer<I> const & signal); 
 
  void Display( typename::signalContainer<I> const & signal );

  void gatherClassification();

  void writeClassification();

  void writeClassificationTXT();

  void MPI_Point_Data_Type(MPI_Datatype & MPIPointData);

  void computeError(std::vector<I> const & centroid_0_old_buffer
                   ,std::vector<I> const & centroid_1_old_buffer 
                   ,std::vector<I> const & centroid_0_new_buffer
                   ,std::vector<I> const & centroid_1_new_buffer);

  std::vector<I> computeInitialRelativeError( std::vector<I> const & OutputData 
                                            , std::vector<I> const & InputData);                 
  
  private:
  
  int const MASTER = 0;

  int rank;
  int size;

  std::string inputDataPath;
  std::string kMeanDataPath;

  std::string data_workspace;
  std::string profile_workspace;

  std::string  index2SensorNameTransform;

  std::unique_ptr<signalDistributor<I>> distribute;
  std::vector<typename::signalContainer<I>>  signalContainerVec;
  std::vector<typename::signalContainer<I>>  CentroidSignal;
  std::vector<typename::signalContainer<I>>  PreCentroidSignal;
 
  std::vector<classficationInfo> totalclassInfo;
  
  
  std::vector<O> sum_local_cluster_0;
  std::vector<O> sum_local_cluster_1;
 
  std::vector<int> cluster_local_size;
  readWriteTool<I,O> tool;

 
  int buffer_size;
 
  bool DTW_CLASSIC;
  bool L2_METRIC;
  bool KMean_Applied;
  bool benchmark_state;

  int  controLoopIter;
  I    centeroidError;
  I    epsilon;
  double sizefileGB;
   

  int const w = 10;
  std::unique_ptr<DistributedTimer<I,O>> Timer;
  std::unique_ptr<ProfileHPC<I,O>>      profile;
  std::vector<Index2Sensor> Index2sensorId;
  double loopAccuracy;
  int loopIteration;
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
KMeanSignal<I,O>::KMeanSignal(int _rank, int _size)
:rank(_rank)
,size(_size)
,cluster_local_size(2,0)
,DTW_CLASSIC(true)
,L2_METRIC(true)
,buffer_size(0)
,controLoopIter(0)
,centeroidError(0)
,kMeanDataPath("/Output_Data/KMean/")
{
  Timer = std::unique_ptr<DistributedTimer<I,O> >
               (new DistributedTimer<I,O>(rank,size));
  
  profile = std::unique_ptr< ProfileHPC<I,O> >
            ( new ProfileHPC<I,O>(rank,size) );  
}


template<typename I,typename O>
void
KMeanSignal<I,O>::Data_IO_Path
               ( std::string const & _inputDataPath)
{
  inputDataPath  = _inputDataPath;
}


template<typename I,typename O>
void
KMeanSignal<I,O>::setFilterSignalPath()
{
  kMeanDataPath = data_workspace + kMeanDataPath; 
  index2SensorNameTransform =  data_workspace +"/Output_Data/Scalability/signalTransform.txt"; 

}

template<typename I, typename O>
void
KMeanSignal<I,O>::controlLoop(double const _epsilon)
{
   epsilon = _epsilon;
}

template<typename I, typename O>
void 
KMeanSignal<I,O>::benchmarkStatus(bool const _benchmark_state){
    benchmark_state = _benchmark_state;
}



template<typename I,typename O>
void
KMeanSignal<I,O>::Data_WorkSpace( std::string const & _data_workspace){
    data_workspace = _data_workspace;
}

template<typename I,typename O>
void
KMeanSignal<I,O>::Profile_WorkSpace( std::string const & _profile_workspace){
    profile_workspace = _profile_workspace;
}

template<typename I,typename O>
void
KMeanSignal<I,O>::apply(){

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
    writeClassification();
  Timer->stop("WKMEAN");
 
}



template<typename I,typename O>
void
KMeanSignal<I,O>::loadingSignals(){
  setFilterSignalPath();
  signalDistribution();
  for(int i(0); i< distribute->procSignalFiles().size(); i++){
         int  classID = UnClass; 
         std::string fullPath = distribute->procSignalFiles()[i];
         int signalIndex = distribute->procSignalIndex()[i];
         std::vector<I> const & signalBuffer( tool.readBinary(fullPath) );
         typename::signalContainer<I> Signal(signalIndex,signalBuffer,classID);
         signalContainerVec.push_back(Signal);
    }

  if(rank == MASTER){
         buffer_size = signalContainerVec[0].signalVal.size(); 
   }
 
  //Broadcasting the buffer size to other procs
  MPI_Bcast
      ( & buffer_size
      , 1
      , MPI_INT
      , MASTER
      , MPI_COMM_WORLD);
  
}

template<typename I,typename O>
void
KMeanSignal<I,O>::signalDistribution()
{
    distribute = std::unique_ptr<signalDistributor<I> >(
                         new signalDistributor<I>(rank, size)
                                  );
    distribute->Data_IO_Path( inputDataPath);
    distribute->apply();
}

template<typename I,typename O>
void
KMeanSignal<I,O>::random_centroid(){
    
    std::vector<int> random_index(2,0);
    choosing_random_centroid_indices(random_index);   
    
    std::vector<int> random_centroid(2);
    random_centroid[0] = distribute->SignalIndex()[random_index[0]];
    random_centroid[1] = distribute->SignalIndex()[random_index[1]];
   
    for(int i(0); i<random_centroid.size(); i++){
        int classID = i;
        std::string randomSignalPath = distribute->SignalFiles()[random_centroid[i]];
        std::vector<I> const & signal( tool.readBinary(randomSignalPath) );
        typename::signalContainer<I> signalrandom(random_centroid[i], signal, classID);
        CentroidSignal.push_back(signalrandom);
        //Display( signalrandom );
    }  
    
}


template<typename I,typename O>
void
KMeanSignal<I,O>::choosing_random_centroid_indices( std::vector<int> & random_index){
    
    
    //choosing the random centroid from the master proc 
  if(rank == 0){   
       random_index[0] = 0;// rand()%(distribute->totalNumFiles());
       random_index[1] = 3;// rand()%(distribute->totalNumFiles());
   }

   //Broadcast the random centroid to the all processes   
   
   MPI_Bcast
     ( random_index.data()
     , random_index.size()
     , MPI_INT
     , MASTER
     , MPI_COMM_WORLD);
 
}





template<typename I,typename O>
void 
KMeanSignal<I,O>::profillingApply()
{
  bool KMean_Applied( true );
 
  
  profile->Data_WorkSpace(data_workspace); 
  profile->Profile_WorkSpace(profile_workspace);   
  profile->clusteringDataPath(kMeanDataPath);
  profile->applyingKMeanClassification(KMean_Applied);
  profile->setFileSize(tool.numFileInsideDirectory(inputDataPath));
  profile->benchmarkStatus(benchmark_state);
  profile->controlQualityLoop(epsilon);
  profile->controlLoopIteration(loopIteration);
  profile->execuationTimeInformation(Timer->timerInformation());  
  profile->centroidAccuracy(loopAccuracy); 
  profile->computeEnergy();
  profile->generateProfillingFile();
  
}


template<typename I,typename O>
void
KMeanSignal<I,O>::kmeanClustering(){

   bool status(true); int k(10); loopAccuracy=std::numeric_limits<I>::max();loopIteration=0;
   while(status){
      
      if((loopIteration >= k) || (loopAccuracy < epsilon)){
        status=false;
        break;
      }else{
     
         if(loopIteration == 0){
            random_centroid();
         }
          
            
        classifySignal();
       
        recomputeCenteroid();
        
        loopIteration++;
            
      }
   }
   
   gatherClassification();
         
   
}


template<typename I,typename O>
void
KMeanSignal<I,O>::classifySignal(){
  
     sum_local_cluster_0.resize(buffer_size,0);
     sum_local_cluster_1.resize(buffer_size,0);
  
     for(int i(0); i<buffer_size; i++)
     {
        sum_local_cluster_0[i]=0;
        sum_local_cluster_1[i]=0;
     }
   
       cluster_local_size[0]=0;
       cluster_local_size[1]=0;
   for(int i(0); i< signalContainerVec.size(); i++){
      
      typename::signalContainer<I> & signal(signalContainerVec[i]);
      std::vector<I> const & signalBuffer(signal.signalVal);
   
     
      int closest_clust ( signal_classification( signal) ); 

      if(closest_clust == 0){
         signal.classID = Class_0;
         cluster_local_size[0]++;
         for(int i(0); i<signalBuffer.size(); i++){
           sum_local_cluster_0[i] += signalBuffer[i];
         }
      }


      if(closest_clust == 1){
         signal.classID = Class_1;
         cluster_local_size[1]++;
         for(int i(0); i<signalBuffer.size(); i++){
           sum_local_cluster_1[i] += signalBuffer[i];
         }
      }
    
   }

}


template<typename I, typename O>
int const 
KMeanSignal<I,O>::signal_classification( typename::signalContainer<I> const & signal)
{
    typename::signalContainer<I> centroid_0(CentroidSignal[0]);
    typename::signalContainer<I> centroid_1(CentroidSignal[1]);
     
    std::vector<I> const & signalBuffer      (signal.signalVal);
    std::vector<I> const & centroid_0_buffer (centroid_0.signalVal);
    std::vector<I> const & centroid_1_buffer (centroid_1.signalVal);

     
    I min_dist = std::numeric_limits<I>::max();
    int closest_clust = -1;
     DTW<I,O> dtw; I C_0; I C_1;
     if(DTW_CLASSIC){

          DTW<I,O> dtw_0(signalBuffer,centroid_0_buffer); 
          DTW<I,O> dtw_1(signalBuffer,centroid_1_buffer); 

          C_0 = dtw_0.DTW_classic();
          C_1 = dtw_1.DTW_classic();
     
        if( C_0 < min_dist){
          closest_clust = 0;
          min_dist = C_0;
         }

        if( C_1 < min_dist){
          closest_clust = 1;
          min_dist = C_1;
         }  
     }
     else{
    
       
         I D_0 = dtw.lBKeoghLowerBound( signalBuffer,centroid_0_buffer, w);
         I D_1 = dtw.lBKeoghLowerBound( signalBuffer,centroid_1_buffer, w);

        if( D_0 < min_dist ){
            I C_0 = dtw.DTW_Dist_fast( signalBuffer,centroid_0_buffer, w); 
            if( C_0  < min_dist){
                  closest_clust = 0;
                  min_dist = C_0;
            }
        }

        if( D_1 < min_dist ){
            I C_1 = dtw.DTW_Dist_fast( signalBuffer,centroid_1_buffer, w); 
            if( C_1  < min_dist){
                  closest_clust = 1;
                  min_dist = C_1;
            }
        } 
     }

    
    return closest_clust;
}

template<typename I, typename O>
void
KMeanSignal<I,O>::recomputeCenteroid(){
  
  std::vector<O> sum_cluster_0(buffer_size,0);
  MPI_Reduce(
     sum_local_cluster_0.data()
    ,sum_cluster_0.data()
    ,buffer_size
    ,mpi_get_type<O>()
    ,MPI_SUM
    ,MASTER
    ,MPI_COMM_WORLD);


  std::vector<O> sum_cluster_1(buffer_size,0);
  MPI_Reduce(
     sum_local_cluster_1.data()
    ,sum_cluster_1.data()
    ,buffer_size
    ,mpi_get_type<O>()
    ,MPI_SUM
    ,MASTER
    ,MPI_COMM_WORLD);

  std::vector<int> cluster_size(2,0);
  MPI_Reduce(
     cluster_local_size.data()
    ,cluster_size.data()
    ,cluster_size.size()
    ,MPI_INT
    ,MPI_SUM
    ,MASTER
    ,MPI_COMM_WORLD);
   

    if(rank == MASTER)
    {
       
       if(cluster_size[0] == 0)
           cluster_size[0] = 1;
     
       if(cluster_size[1] == 0)
           cluster_size[1] = 1;
     
        
       for(int i(0); i<sum_cluster_0.size(); i++)
        {
           sum_cluster_0[i] /= static_cast<I>(cluster_size[0]);
           sum_cluster_1[i] /= static_cast<I>(cluster_size[1]);
        }
       
        cluster_size_0 = cluster_size[0];
        cluster_size_1 = cluster_size[1];
    }

  MPI_Bcast
      ( sum_cluster_0.data()
      , buffer_size
      , mpi_get_type<O>()
      , MASTER
      , MPI_COMM_WORLD);

    
  MPI_Bcast
     ( sum_cluster_1.data()
     , buffer_size
     , mpi_get_type<O>()
     , MASTER
     , MPI_COMM_WORLD);
     


    std::vector<I> sum_cluster_0_new(buffer_size,0);  std::vector<I> sum_cluster_1_new(buffer_size,0);
    for(int i(0); i<buffer_size; i++)
    {
        sum_cluster_0_new[i]  = sum_cluster_0[i]; 
        sum_cluster_1_new[i]  = sum_cluster_1[i]; 
    }    
     
      std::vector<I> const & centroid_0_old_buffer (CentroidSignal[0].signalVal);
      std::vector<I> const & centroid_1_old_buffer (CentroidSignal[1].signalVal);
    
     
     computeError(centroid_0_old_buffer
                 ,centroid_1_old_buffer 
                 ,sum_cluster_0_new
                 ,sum_cluster_1_new);
     
     CentroidSignal.resize(0);

       
    

     typename::signalContainer<I> centroid_0( signalContainer(CentroidSignal[0].signalId, sum_cluster_0_new, CentroidSignal[0].classID) );
     typename::signalContainer<I> centroid_1( signalContainer(CentroidSignal[1].signalId, sum_cluster_1_new, CentroidSignal[1].classID) );

    
     CentroidSignal.push_back(centroid_0);
     CentroidSignal.push_back(centroid_1);
  
     
}

                 

 
template<typename I, typename O>
void
KMeanSignal<I,O>::gatherClassification(){

   std::vector<classficationInfo> classInfo;
   totalclassInfo.resize(distribute->totalNumFiles());
   std::vector<int> const & offsetDisp(distribute->displacementDistribution());
  
  for(int i(0); i<signalContainerVec.size(); i++){
    int signalId = signalContainerVec[i].signalId;
    int classID = signalContainerVec[i].classID;
    classInfo.push_back(classficationInfo(signalId,classID));
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
KMeanSignal<I,O>::computeError(std::vector<I> const & centroid_0_old_buffer
                              ,std::vector<I> const & centroid_1_old_buffer 
                              ,std::vector<I> const & centroid_0_new_buffer
                              ,std::vector<I> const & centroid_1_new_buffer){

   bool DTWdistanceType(false);
   if(DTWdistanceType){
     DTW<I,O> dtw_0(centroid_0_old_buffer,centroid_0_new_buffer); 
     DTW<I,O> dtw_1(centroid_1_old_buffer,centroid_1_new_buffer); 

     I C_0 = dtw_0.DTW_classic();
     I C_1 = dtw_1.DTW_classic();
   }else{
       
   
     std::vector<I> errorInfo_C0 (computeInitialRelativeError( centroid_0_old_buffer , centroid_0_new_buffer));
     std::vector<I> errorInfo_C1 (computeInitialRelativeError( centroid_1_old_buffer , centroid_1_new_buffer));
  
     loopAccuracy = errorInfo_C0[0] + errorInfo_C1[0]; 
   }
     
  }
     
    
template<typename I, typename O>
void
KMeanSignal<I,O>::writeClassification(){


   if(rank == 0){
      std::string cluster = kMeanDataPath + "KMean_cluster.csv";
      char Cluster[ cluster.size() + 1];
      strcpy(Cluster, cluster.c_str());
      std::ofstream cluster_output;
      cluster_output.open(Cluster);
       
      std::vector<typename::readWriteTool<I,O>::Index2SensorName> Index2sensorId(tool.readTransfIndex(index2SensorNameTransform));
      cluster_output<< "timeseries;labels" <<std::endl; 
     for(int i(0); i<totalclassInfo.size(); i++){ 
     
         int classID =UnClass;
         std::string sensorName ( tool.findSensorNameId(Index2sensorId, totalclassInfo[i].signalId) );
         if(cluster_size_0 > cluster_size_1){
            classID =  totalclassInfo[i].classID ; 
         }else
         {
            if(totalclassInfo[i].classID == 0){
              classID = Class_1;
            }

            if(totalclassInfo[i].classID == 1){
              classID = Class_0;
            }
         }
         cluster_output << sensorName<<";"<< classID  << std::endl; 
     }
      
     writeClassificationTXT();

   }            

}



template<typename I, typename O>
void
KMeanSignal<I,O>::writeClassificationTXT(){


   if(rank == 0){
      std::string cluster = kMeanDataPath + "KMean_cluster.txt";
      char Cluster[ cluster.size() + 1];
      strcpy(Cluster, cluster.c_str());
      std::ofstream cluster_output;
      cluster_output.open(Cluster);
       
     for(int i(0); i<totalclassInfo.size(); i++){ 
         // std::cout << totalclassInfo[i].signalId<<" | "<< totalclassInfo[i].classID << std::endl;
         cluster_output << totalclassInfo[i].signalId<<";"<< totalclassInfo[i].classID << std::endl; 
     }

 
   }            

}

                              

template<typename I, typename O>
void
KMeanSignal<I,O>::MPI_Point_Data_Type(MPI_Datatype & MPIPointData){

const int    nItems=2;
int          blocklengths[nItems] = { 1, 1 };
MPI_Datatype types[nItems] = {  MPI_INT, MPI_INT};
MPI_Aint     offsets[nItems];

offsets[0] = offsetof(classficationInfo, signalId);
offsets[1] = offsetof(classficationInfo, classID);

MPI_Type_create_struct(nItems, blocklengths, offsets, types, & MPIPointData);
MPI_Type_commit(& MPIPointData);

}



template<typename I, typename O>
void
KMeanSignal<I,O>::DisplaySignalClassification(){
    std::cout << " id | class " << std::endl;   
   for(int i(0); i<signalContainerVec.size(); i++){
    std::cout << " "<< signalContainerVec[i].signalId<< "  |    "<< signalContainerVec[i].classID << std::endl; 
   } 
   std::cout << std::endl;
 }                               


template<typename I, typename O>
void
KMeanSignal<I,O>::Display( typename::signalContainer<I> const & signal ){

  std::cout << "signal Id  " << signal.signalId << " |  class ID " << signal.classID<<  std::endl;
  
 
  for(int i(0); i<signal.signalVal.size(); i++){
    std::cout << signal.signalVal[i] << "  ";
  }

   std::cout << std::endl;
 }

template<typename I,typename O>
std::vector<I>
KMeanSignal<I,O>::computeInitialRelativeError( std::vector<I> const & OutputData 
                                              , std::vector<I> const & InputData)
{
 std::vector<I> errorInfo(2,0); 
 std::vector<I> AbsError(InputData.size()); I AbsL2Norm(0);
  for(int i(0); i< AbsError.size(); i+=1){
     AbsError[i] = fabs(OutputData[i] - InputData[i]);
     AbsL2Norm += (AbsError[i]*AbsError[i]); 
     }

auto minmaxAbsError = std::minmax_element(AbsError.begin(), AbsError.end());

I AbsMax = * minmaxAbsError.second;

errorInfo[0]= AbsMax;
errorInfo[1]= sqrt(AbsL2Norm);

return errorInfo;
}

#endif
