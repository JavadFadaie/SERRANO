#ifndef KNNSIGNAL_HPP
#define KNNSIGNAL_HPP

#include<iostream>
#include<vector>
#include <cstdlib>
#include <cstring>
#include "signal_Distributor.hpp"
#include "DistributedTimer.hpp"

template<typename I, typename O>
class KNN_Signal{
 
 public:
   
    struct classficationInfo{
         
         classficationInfo(int _signalId, int _classID, I _dist)
         :signalId(_signalId)
         ,classID(_classID)
         ,dist(_dist)
         {}

         classficationInfo()
         :signalId(0)
         ,classID(0)
         ,dist(0)
         {}

         int signalId;
         int classID; 
         I dist;
    };  
   
    using  KMeanClassificationContanier = typename::readWriteTool<I,O>::KMeanClassificationContanier;
    using  Index2SensorName =  typename::readWriteTool<I,O>::Index2SensorName;
 
  public:
   
    KNN_Signal(int _rank, int _size);

    void Data_IO_Path( std::string const & _inputDataPath);

    void Data_WorkSpace(std::string const & _data_workspace);

    void Profile_WorkSpace(std::string const & _profile_workspace);         

    void getTrainingSignal( std::string _classification_training);
   
    void setFilterSignalPath();

    void cluster_label_path( std::string const & _clustring_label);
    
    void signalDistribution();

    void signalClassificationReader();

    void loadingSignals();

    void apply(); 

    void profillingApply();

    void KNN_Classification(std::string const & unclassified_path_signal);

    void KNN_Classification_Distributed();

    void receiveUnclassifiedSignal();

    void
    getPathUnclassifiedSignal(std::string const & _unclassified_path_signal);
    
    typename::signalContainer<I> 
    receiveUnclassifiedSignal(std::string const & unclassified_path_signal);
    
    std::vector<classficationInfo> 
    computeDistance2UnClassifiedSignal(typename::signalContainer<I> const & UnclassifiedSignal);

    void computeNearestCluter(std::vector<classficationInfo> const & signalInfo
                              ,std::string const & unclassifiedSignal );

    void  MPI_Point_Data_Type(MPI_Datatype & MPIPointData);

    void kthNeighbor(int _k_nearestNeighbor);

    void benchmarkStatus(bool const _benchmark_state);
    
    void clusterNum(int const  n);
/*
    void readTransfIndex();

    std::string sensorNameFinder(int  signalIndex);

    int classIDFinder(std::string  sensorName);

    std::vector<I> readInferenceSignalCSV(std::string const & path);
 */
    void writeClassificationSignal();


    private :

    int const MASTER = 0;

    int rank;
    int size;

    int k_nearestNeighbor; 
  
    std::string inputDataPath;
    std::string KNNDataPath;
    std::string unclassified_path_signal;
    std::string clustring_label;
    std::string index2SensorNameTransform;
   
    std::string data_workspace;
    std::string profile_workspace;

/*
    std::vector<KMeanClassificationReader> signalClassId; 
    std::vector<Index2SensorName> Index2sensorId;
*/
    
    std::vector<KMeanClassificationContanier> outputSignalClassification;


    std::unique_ptr<signalDistributor<I>>      distribute;
    std::vector<typename::signalContainer<I>>  signalContainerVec;
    std::vector<classficationInfo> totalSignalInfo;
    std::unique_ptr<DistributedTimer<I,O>> Timer;
    std::unique_ptr<ProfileHPC<I,O>>      profile;
    readWriteTool<I,O> tool;


    int buffer_size;
    int k;
    bool benchmark_state;
    int numCluster;
    int numUnclassifiedSignal;
    
   
};

//////////////////////////////////////////////
///////////IMPLEMANTATION
/////////////////////////////////////////////
template<typename I, typename O>
KNN_Signal<I,O>::KNN_Signal(int _rank, int _size)
:rank(_rank)
,size(_size)
,buffer_size(0)
,k(3)
//,clustring_label("/Output_Data/KMean/KMean_cluster.csv")
,KNNDataPath("/Output_Data/KNN/")
,numUnclassifiedSignal(0)
{
   Timer = std::unique_ptr<DistributedTimer<I,O> >
               (new DistributedTimer<I,O>(rank,size));

   profile = std::unique_ptr< ProfileHPC<I,O> >
            ( new ProfileHPC<I,O>(rank,size) );

   
}



template<typename I,typename O>
void
KNN_Signal<I,O>::Data_IO_Path
               ( std::string const & _inputDataPath)
{
  inputDataPath  = _inputDataPath;
} 

template<typename I,typename O>
void
KNN_Signal<I,O>::kthNeighbor(int  _k_nearestNeighbor)
{
  k_nearestNeighbor = _k_nearestNeighbor; 
}


template<typename I,typename O>
void
KNN_Signal<I,O>::Data_WorkSpace( std::string const & _data_workspace){
  data_workspace = _data_workspace;
}

template<typename I,typename O>
void
KNN_Signal<I,O>::Profile_WorkSpace( std::string const & _profile_workspace){
  profile_workspace = _profile_workspace;
}

template<typename I,typename O>
void
KNN_Signal<I,O>::cluster_label_path( std::string const & _clustring_label){
  clustring_label = _clustring_label;
}


template<typename I,typename O>
void
KNN_Signal<I,O>::setFilterSignalPath(){

   KNNDataPath = data_workspace + KNNDataPath; 
   clustring_label = data_workspace + clustring_label;
   index2SensorNameTransform =  data_workspace +"/Output_Data/Scalability/signalTransform.txt"; 
 

}


template<typename I, typename O>
void 
KNN_Signal<I,O>::benchmarkStatus(bool const _benchmark_state){
    benchmark_state = _benchmark_state;
}

template<typename I, typename O>
void
KNN_Signal<I,O>::clusterNum(int const  n)
{
   numCluster = n;
}

template<typename I,typename O>
void
KNN_Signal<I,O>::getPathUnclassifiedSignal(std::string const & _unclassified_path_signal){
  unclassified_path_signal = _unclassified_path_signal;
}


template<typename I,typename O>
void
KNN_Signal<I,O>::signalDistribution()
{
    distribute = std::unique_ptr<signalDistributor<I> >(
                         new signalDistributor<I>(rank, size)
                                  );
    distribute->Data_IO_Path( inputDataPath);
    distribute->apply();
}


template<typename I,typename O>
void
KNN_Signal<I,O>::loadingSignals(){

  setFilterSignalPath();
  
  std::vector<KMeanClassificationContanier> KMeanClassificationVec(tool.signalClassificationReader(clustring_label));
  std::vector<Index2SensorName> Index2sensorId(tool.readTransfIndex(index2SensorNameTransform));
     
     
  signalDistribution();
  for(int i(0); i< distribute->procSignalFiles().size(); i++){
         std::string fullPath = distribute->procSignalFiles()[i];     
    	   int signalIndex = distribute->procSignalIndex()[i];
         std::string sensorName ( tool.findSensorNameId(Index2sensorId, signalIndex) );
         int classID (tool.classIDFinder(KMeanClassificationVec,sensorName));
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
KNN_Signal<I,O>::apply(){
    
   //loading the signals
   Timer->start("RKNN");
    loadingSignals();
   Timer->stop("RKNN");

   
   //KNN Clustering
   Timer->start("KNN");
   KNN_Classification_Distributed(); 
   Timer->stop("KNN");

   
   Timer->start("WKNN");
   writeClassificationSignal();
   Timer->stop("WKNN");

   
}

template<typename I,typename O>
void 
KNN_Signal<I,O>::KNN_Classification_Distributed()
{
   
    std::vector<std::string> const & unClassifiedSignalVec(  tool.searchFileInDirectory(unclassified_path_signal) );
    for(int i(0); i<unClassifiedSignalVec.size(); i++){
      
      std::string const & unclassfiedSignal(unClassifiedSignalVec[i]);
      MPI_Barrier;
      KNN_Classification(unclassfiedSignal);
   }
  
  
}


template<typename I,typename O>
void 
KNN_Signal<I,O>::profillingApply()
{
  bool KNN_Applied( true );

  profile->Data_WorkSpace(data_workspace); 
  profile->Profile_WorkSpace(profile_workspace);   
  profile->clusteringDataPath(KNNDataPath);
  profile->applyingKNNClassification(KNN_Applied);
  profile->setFileSize(tool.numFileInsideDirectory(inputDataPath));
  profile->benchmarkStatus(benchmark_state);
  profile->execuationTimeInformation(Timer->timerInformation());   
  profile->computeEnergy();
  profile->generateProfillingFile();
  
}





template<typename I,typename O>
void
KNN_Signal<I,O>::KNN_Classification(std::string const & unclassified_signal){
    
     //Receving the unclassfied signal to all procs
   std::string unclassified_ful_lpath_signal = unclassified_path_signal + unclassified_signal;
   typename::signalContainer<I> const & UnclassifiedSignal( receiveUnclassifiedSignal(unclassified_ful_lpath_signal) );
     
     //Compute the distance of the traiing data to unclassfied data
   std::vector<classficationInfo> signalInfo ( computeDistance2UnClassifiedSignal(UnclassifiedSignal) );

     //Compute the nearest neighbour
   computeNearestCluter(signalInfo, unclassified_signal);

}


template<typename I, typename O>
typename::signalContainer<I>
KNN_Signal<I,O>::receiveUnclassifiedSignal
                    (std::string const & unclassified_path_signal){

   std::vector<I> const & signal( tool.readInferenceSignalCSV(unclassified_path_signal) );
   int signalIndex(-1);
   int classID(-1);
   typename::signalContainer<I> UnclassifiedSignal(signalIndex,signal,classID);

  return  UnclassifiedSignal;
}



template<typename I, typename O>
std::vector<typename::KNN_Signal<I,O>::classficationInfo>
KNN_Signal<I,O>::computeDistance2UnClassifiedSignal(typename::signalContainer<I> const & UnclassifiedSignal){
       
    std::vector<classficationInfo> signalInfo;
   for(int i(0); i<signalContainerVec.size(); i++){
        
       std::vector<I> const & signalBuffer (signalContainerVec[i].signalVal);
       std::vector<I> const & unclassified_buffer (UnclassifiedSignal.signalVal);
       DTW<I,O> dtw(signalBuffer , unclassified_buffer);
       I dist = dtw.DTW_classic();
       signalInfo.push_back(classficationInfo( signalContainerVec[i].signalId
                                             , signalContainerVec[i].classID
                                             , dist )
                            ); 
      
   }
  
  return signalInfo;
}

template<typename I, typename O>
void
KNN_Signal<I,O>::computeNearestCluter(std::vector<classficationInfo> const & signalInfo
                                     ,std::string const & unclassifiedSignal ){
       
    MPI_Datatype  MPIDistData;
    MPI_Point_Data_Type(MPIDistData);

  totalSignalInfo.resize(distribute->totalNumFiles());
  std::vector<int> const & offsetDisp(distribute->displacementDistribution());
  std::vector<int> const & numDistVec(distribute->numberDataProc());

  MPI_Gatherv
      ( signalInfo.data()       //send_data,
      , signalInfo.size()       //send_count
      , MPIDistData             //send_datatype
      , totalSignalInfo.data()  //recv_data,
      , & numDistVec[0]              
      , & offsetDisp[0]
      , MPIDistData             //recv_datatype,
      , MASTER                  //root
      , MPI_COMM_WORLD);



   if(rank == MASTER){

    std::sort(totalSignalInfo.begin(), totalSignalInfo.end(),
                []( classficationInfo & a, classficationInfo & b ) -> bool
                {return a.dist < b.dist;} 
             );
    
    int cluster_0(0); int cluster_1(0);
    for(int i(0); i<k;  i++){
         if(totalSignalInfo[i].classID == 0){
            cluster_0++;
         }
          if(totalSignalInfo[i].classID == 1){
            cluster_1++;
         }
    }

   
    if(cluster_0 > cluster_1){
      int cluster = 0;
      outputSignalClassification.push_back(KMeanClassificationContanier(unclassifiedSignal,cluster));
    }else{
      int cluster = 1;
      outputSignalClassification.push_back(KMeanClassificationContanier(unclassifiedSignal,cluster));
    }

    
    for(int i(0); i<totalSignalInfo.size(); i++){
     //   std::cout << totalSignalInfo[i].signalId << "  " << totalSignalInfo[i].classID << "  " << totalSignalInfo[i].dist << std::endl;
    }
    

   }   

}


template<typename I, typename O>
void
KNN_Signal<I,O>::writeClassificationSignal(){

  if(rank == 0){
  
      std::string cluster = KNNDataPath + "KNN_timesseries_label.csv";
      char Cluster[ cluster.size() + 1];
      strcpy(Cluster, cluster.c_str());
      std::ofstream cluster_output;
      cluster_output.open(Cluster);

        cluster_output<<"timeseries;label" << std::endl;
     for(int i(0); i<outputSignalClassification.size(); i++){ 
        // std::cout << totalclassInfo[i].signalId<<" | "<< totalclassInfo[i].classID << std::endl;
         cluster_output << outputSignalClassification[i].sensor_name<<";"<< outputSignalClassification[i].class_Id << std::endl; 
     }

 
   } 

}

template<typename I, typename O>
void
KNN_Signal<I,O>::MPI_Point_Data_Type(MPI_Datatype & MPIPointData){

    const int    nItems=3;
    int          blocklengths[nItems] = { 1, 1, 1 };
    MPI_Datatype types[nItems] = { MPI_INT, MPI_INT, mpi_get_type<I>()};
    MPI_Aint     offsets[nItems];

    offsets[0] = offsetof(classficationInfo, signalId);
    offsets[1] = offsetof(classficationInfo, classID);
    offsets[2] = offsetof(classficationInfo, dist);

    MPI_Type_create_struct(nItems, blocklengths, offsets, types, & MPIPointData);
    MPI_Type_commit(& MPIPointData);

}

/*
template<typename I, typename O>
void
KNN_Signal<I,O>::readTransfIndex(){
  
 std::string path =  data_workspace +"/Output_Data/Scalability/signalTransform.txt"; 
 char filename[ path.size() + 1];
 strcpy(filename, path.c_str());
 std::ifstream ifs(filename);
 std::string line;  
 // read one line from ifs
 while(std::getline(ifs, line)){
    std::istringstream iss(line); 
    int signal_Id;
    std::string sensor_name;
    
    iss >> signal_Id >> sensor_name ; 
    Index2sensorId.push_back(Index2SensorName(signal_Id,sensor_name));
    }

}



template<typename I,typename O>
void
KNN_Signal<I,O>::signalClassificationReader(){

    std::string fileline;
    std::string elm;
    std::ifstream InputData;
    InputData.open(classification_training);
    std::vector<std::string> data;
 
    while(InputData)
    {
        getline(InputData,fileline);
        std::istringstream ss(fileline);
  
        while(getline(ss, elm,';') ){
          data.push_back(elm);
        }
    }

    for(int i(2); i<data.size(); i+=2){
        std::string sensor_name = data[i];
        int classID  = static_cast<int>(std::atof(data[i+1].c_str()));
        signalClassId.push_back(KMeanClassificationReader(sensor_name,classID));
    }

    
}


template<typename I,typename O>
std::string 
KNN_Signal<I,O>::sensorNameFinder(int signalIndex){
   
     auto it = std::find_if(Index2sensorId.begin(), Index2sensorId.end(), 
                       [signalIndex] (const Index2SensorName & signal) { 
                          return signal.signal_Id == signalIndex; 
                       });

    return it->sensor_name;
}  

template<typename I,typename O>
int 
KNN_Signal<I,O>::classIDFinder(std::string sensorName){
   
     auto it = std::find_if(signalClassId.begin(), signalClassId.end(), 
                       [sensorName] (const KMeanClassificationReader & signal) { 
                          return signal.sensor_name == sensorName; 
                       });

    return it->class_Id;
 }  


template<typename I, typename O>
std::vector<I>
KNN_Signal<I,O>::readInferenceSignalCSV(std::string const & path){
 
   std::string fileline;
   std::string elm;
   std::ifstream InputData;
   std::vector<I> buffer;
   InputData.open(path);
   std::vector<std::string> signal; std::vector<I> signalBuffer; int iter(0);
   while(InputData)
   {  
     getline(InputData,fileline);
     std::istringstream ss(fileline);
     size_t elmId(0); I val(0); 
     double time; int signalId(0);
     while(getline(ss, elm,';') ){
           // signal.push_back(elm);
           I SPXelm = std::atof(elm.c_str());
           if(iter){
             signalBuffer.push_back(SPXelm);
           }
           //std::cout<<iter << "  " << elm << std::endl;
       }
     iter++;  
   }

   for(int i(0); i<signalBuffer.size(); i++){
    std::cout << signalBuffer[i] << std::endl;
   } 

 return signalBuffer;
}
*/
#endif
