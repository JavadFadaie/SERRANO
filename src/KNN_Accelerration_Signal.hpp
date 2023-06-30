#ifndef KNN_ACCELERATION_HPP
#define KNN_ACCELERATION_HPP

#include<iostream>
#include<vector>
#include <cstdlib>
#include <cstring>
#include "signal_Distributor.hpp"
#include "DistributedTimer.hpp"


template<typename I, typename O>
class KNN_Acceleration{

  public :

    struct classficationInfo{
        classficationInfo( int _signal_GId
                         , int _signal_LId
                         , int _group_Id
                         , int _class_Id
                         , I   _dist)
         :signal_GId(_signal_GId)
         ,signal_LId(_signal_LId)
         ,group_Id(_group_Id)
         ,class_Id(_class_Id)
         ,dist(_dist)
         {}

        classficationInfo()
          :signal_GId(0)
          ,signal_LId(0)
          ,group_Id(0)
          ,class_Id(-1)
          ,dist(0)
          {}

         int signal_GId;
         int signal_LId;
         int group_Id;
         int class_Id;
         I dist;
      };   
    
  
 using  KMeanClassificationContanier = typename::readWriteTool<I,O>::KMeanClassificationContanier;
 using  Index2SensorName =  typename::readWriteTool<I,O>::Index2SensorName;
 using  signalContainer = signalContainerAcceleration<I>;

 struct InferenceSignalContainer{
     InferenceSignalContainer
        ( std::string _value
        , int _id)
        :value(_value)
        ,id(_id)
        {}

    std::string value;
    int id;
   };


  public:
   
    KNN_Acceleration(int _rank, int _size);

    void Data_IO_Path( std::string const & _inputDataPath);

    void Data_WorkSpace( std::string const & _data_Workspace);

    void Profile_WorkSpace( std::string const & _profile_workspace);

    void getTrainingSignal( std::string _classification_training);
   
    void setFilterSignalPath();
    
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
    
    
    std::vector<signalContainer>
    receiveUnclassifiedSignal(std::string const & unclassified_path_signal);
    
    std::vector<classficationInfo> 
    computeDistance2UnClassifiedSignal(std::vector<signalContainer> const & UnclassifiedSignal);

    void computeNearestCluter(std::vector<classficationInfo> const & signalInfo
                              ,std::string const & unclassifiedSignal );

    void  MPI_Point_Data_Type(MPI_Datatype & MPIPointData);

    void kthNeighbor(int _k_nearestNeighbor);

    void benchmarkStatus(bool const _benchmark_state);
    
    void clusterNum(int const  n);
/*
    void searchFileInDirectory();

    void readTransfIndex();

    std::string sensorNameFinder(int  signalIndex);

    int classIDFinder(std::string  sensorName);
*/
    void writeClassificationSignal();

    std::vector<I> readInferenceSignalCSV(std::string const & path);
 
    int const get_group_Id(int const _signal_GId); 

    int const get_local_Id(int const _signal_GId);

    void Display( signalContainer const & signal );


    private :

    int const MASTER = 0;

    int rank;
    int size;

    int k_nearestNeighbor; 
  
    std::string inputDataPath;
    std::string KNNAccelDataPath;
    std::string unclassified_path_signal;
    std::string classification_training;
    std::string index2SensorNameTransform;
  
    
    std::string data_workspace;
    std::string profile_workspace;

   /*
    std::vector<KMeanClassificationReader> signalClassId; 
    std::vector<Index2SensorName> Index2sensorId;
  */
    std::vector<KMeanClassificationContanier> outputSignalClassification;
    std::unique_ptr<signalDistributor<I>> distribute;
    std::vector<signalContainer>  signalContainerVec;
    std::vector<classficationInfo> totalSignalInfo;
    std::unique_ptr<DistributedTimer<I,O>> Timer;
    std::unique_ptr<ProfileHPC<I,O>>      profile;
    readWriteTool<I,O> tool;
  

    int buffer_size;
    int numGroup;

  
    int k;
    bool benchmark_state;
    int numCluster;
    std::vector<std::string>  unClassifiedSignalVec;  
    int numUnclassifiedSignal;

};

/////////////////////////////////
///////////IMPLEMENTATION////////
/////////////////////////////////

template<typename I, typename O>
KNN_Acceleration<I,O>::KNN_Acceleration(int _rank, int _size)
:rank(_rank)
,size(_size)
,buffer_size(0)
,k(3)
,classification_training("/Output_Data/KMean/KMean_cluster.csv")
,KNNAccelDataPath("/Output_Data/KNN/")
,numUnclassifiedSignal(0)
{
   Timer = std::unique_ptr<DistributedTimer<I,O> >
               (new DistributedTimer<I,O>(rank,size));

   profile = std::unique_ptr< ProfileHPC<I,O> >
            ( new ProfileHPC<I,O>(rank,size) );

   
}



template<typename I,typename O>
void
KNN_Acceleration<I,O>::Data_IO_Path
               ( std::string const & _inputDataPath)
{
  inputDataPath  = _inputDataPath;
} 

template<typename I,typename O>
void
KNN_Acceleration<I,O>::kthNeighbor(int  _k_nearestNeighbor)
{
  k_nearestNeighbor = _k_nearestNeighbor; 
}

template<typename I, typename O>
void 
KNN_Acceleration<I,O>::benchmarkStatus(bool const _benchmark_state){
    benchmark_state = _benchmark_state;
}


template<typename I,typename O>
void
KNN_Acceleration<I,O>::Data_WorkSpace( std::string const & _data_workspace){
   data_workspace = _data_workspace;
}

template<typename I,typename O>
void
KNN_Acceleration<I,O>::Profile_WorkSpace( std::string const & _profile_workspace){
   profile_workspace = _profile_workspace;
}


template<typename I,typename O>
void
KNN_Acceleration<I,O>::setFilterSignalPath(){

   KNNAccelDataPath = data_workspace + KNNAccelDataPath; 
   classification_training = data_workspace + classification_training;
   index2SensorNameTransform =  data_workspace +"/Output_Data/Scalability/signalTransform.txt"; 
 

}


template<typename I, typename O>
void
KNN_Acceleration<I,O>::clusterNum(int const  n)
{
   numCluster = n;
}

template<typename I,typename O>
void
KNN_Acceleration<I,O>::getPathUnclassifiedSignal(std::string const & _unclassified_path_signal){
  unclassified_path_signal = _unclassified_path_signal;
}


template<typename I,typename O>
void
KNN_Acceleration<I,O>::signalDistribution()
{
    distribute = std::unique_ptr<signalDistributor<I> >(
                         new signalDistributor<I>(rank, size)
                                  );
    distribute->Data_IO_Path( inputDataPath);
    distribute->apply();
}


template<typename I,typename O>
void
KNN_Acceleration<I,O>::loadingSignals(){

  setFilterSignalPath();
  /*
  signalClassificationReader();
  readTransfIndex();
   */
  std::vector<KMeanClassificationContanier> KMeanClassificationVec(tool.signalClassificationReader(classification_training));
  std::vector<Index2SensorName> Index2sensorId(tool.readTransfIndex(index2SensorNameTransform));
  

  signalDistribution();int group_Id(0);
  numGroup = distribute->totalNumFiles()/6;  
  for(int i(0); i< distribute->procSignalFiles().size(); i++){
         std::string fullPath = distribute->procSignalFiles()[i];     
    	   int signal_GId = distribute->procSignalIndex()[i];
         group_Id = get_group_Id(signal_GId);
         int signal_LId = get_local_Id(signal_GId);
         //std::string  sensorName ( sensorNameFinder(signal_GId) );
         //int class_Id = classIDFinder(sensorName);
         std::string sensorName ( tool.findSensorNameId(Index2sensorId, signal_GId) );
         int class_Id (tool.classIDFinder(KMeanClassificationVec,sensorName));
         //std::cout << signal_GId << " <  " << sensorName <<  " < "<< class_Id << std::endl;
         std::vector<I> const & signal_Val( distribute->readFiles(fullPath) );
         signalContainer Signal(signal_GId, signal_LId, group_Id, class_Id, signal_Val);
	       signalContainerVec.push_back(Signal); 
        // Display( Signal );  
	 }

  
  if(rank == MASTER){
         buffer_size = signalContainerVec[0].signal_Val.size(); 
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
int const 
KNN_Acceleration<I,O>::get_group_Id(int const _signal_GId){
  return _signal_GId/6;
}

template<typename I,typename O>
int const 
KNN_Acceleration<I,O>::get_local_Id(int const _signal_GId){
  return _signal_GId%6;
}

template<typename I,typename O>
void
KNN_Acceleration<I,O>::apply(){
    
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
KNN_Acceleration<I,O>::KNN_Classification_Distributed()
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
KNN_Acceleration<I,O>::profillingApply()
{
  bool KNN_Applied( true );
  profile->Data_WorkSpace(data_workspace); 
  profile->Profile_WorkSpace(profile_workspace);   
  profile->clusteringDataPath(KNNAccelDataPath);
  profile->applyingKNNClassification(KNN_Applied);
  profile->setFileSize(tool.numFileInsideDirectory(inputDataPath));
  profile->benchmarkStatus(benchmark_state);
  profile->execuationTimeInformation(Timer->timerInformation());   
  profile->computeEnergy();
  profile->generateProfillingFile();
  
}


/*
*/

template<typename I,typename O>
void
KNN_Acceleration<I,O>::KNN_Classification(std::string const & unclassified_signal){
    
     //Receving the unclassfied signal to all procs
   std::string unclassified_ful_lpath_signal = unclassified_path_signal + unclassified_signal;
   std::vector<signalContainer> const & UnclassifiedSignal ( receiveUnclassifiedSignal(unclassified_ful_lpath_signal) );
/*
   for(int i(0); i<UnclassifiedSignal.size(); i++){
     signalContainer const & Unsignal ( UnclassifiedSignal[i] );
    // Display(Unsignal);
   }  
*/
     //Compute the distance of the traiing data to unclassfied data
   std::vector<classficationInfo> signalInfo ( computeDistance2UnClassifiedSignal(UnclassifiedSignal) );

     //Compute the nearest neighbour
   computeNearestCluter(signalInfo, unclassified_signal);

}


template<typename I, typename O>
std::vector<typename::KNN_Acceleration<I,O>::signalContainer>
KNN_Acceleration<I,O>::receiveUnclassifiedSignal
                    (std::string const & unclassified_path_signal){
   
   std::vector<signalContainer>  UnclassifiedSignal;
   std::string fileline;
   std::string elm;
   std::ifstream InputData;
   std::vector<I> buffer;
   InputData.open(unclassified_path_signal);
   std::vector<InferenceSignalContainer> signal;
   while(InputData)
   {  
     getline(InputData,fileline);
     std::istringstream ss(fileline);
     size_t elmId(0); I val(0); 
     double time; int signalId(0);
     while(getline(ss, elm,';') ){
           I SPXelm = std::atof(elm.c_str());
           signal.push_back(InferenceSignalContainer(elm,signalId));
           signalId++;
       }
   }

   auto it = max_element(signal.begin (), signal.end(),
                            []( const InferenceSignalContainer & a, const InferenceSignalContainer & b)
                               {return a.id < b.id ;}
                          );
   
  int numSignal = it->id+1;                        
  for(int signal_Id(0); signal_Id<numSignal; signal_Id++){
       std::vector<I> signalBuffer; std::string sensor_name;  bool headerStatus(true);
      for(int i(0); i<signal.size(); i++){
          if( signal[i].id == signal_Id ){
              I value =  std::atof(signal[i].value.c_str());
              //just save the header
              if(headerStatus){
                   sensor_name = signal[i].value.c_str();
                   headerStatus = false;
              }else
              {
                    signalBuffer.push_back(value);
              }
          }
      }
       int class_Id(-1);
       int group_Id(-1);
       UnclassifiedSignal.push_back(signalContainer(signal_Id,signal_Id,group_Id,class_Id,signalBuffer));
    }
   
    
   return UnclassifiedSignal;
}



template<typename I, typename O>
std::vector<typename::KNN_Acceleration<I,O>::classficationInfo>
KNN_Acceleration<I,O>::computeDistance2UnClassifiedSignal(std::vector<signalContainer> const & UnclassifiedSignal){
       
  std::vector<classficationInfo> signalInfo;
  for(int j(0); j<UnclassifiedSignal.size(); j++){ 

     for(int i(0); i<signalContainerVec.size(); i++)
     {

        signalContainer const & unClassifiedsignal ( UnclassifiedSignal[j] ); 
        signalContainer const & trainingSignal     ( signalContainerVec[i] ); 
  
        if( trainingSignal.signal_LId == unClassifiedsignal.signal_LId ){
         
           std::vector<I> const & signalBuffer        (trainingSignal.signal_Val);
           std::vector<I> const & unclassified_buffer (unClassifiedsignal.signal_Val);
           DTW<I,O> dtw(signalBuffer , unclassified_buffer);
           I dist = dtw.DTW_classic();
           signalInfo.push_back(classficationInfo( trainingSignal.signal_GId
                                                 , trainingSignal.signal_LId
                                                 , trainingSignal.group_Id
                                                 , trainingSignal.class_Id
                                                 , dist )); 
         }
      }
   } 

  return signalInfo;
}

template<typename I, typename O>
void
KNN_Acceleration<I,O>::computeNearestCluter(std::vector<classficationInfo> const & signalInfo
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
  
  /*
    for(int j(0); j<totalSignalInfo.size(); j++){
      std::cout << totalSignalInfo[j].signal_GId <<"  "<< totalSignalInfo[j].signal_LId<< "  " << totalSignalInfo[j].group_Id << "  " << totalSignalInfo[j].dist << std::endl;  
    }
  */

    std::vector<I> groupTotalDist(numGroup,0);
    for(int iGroup(0); iGroup<numGroup; iGroup++){
        I groupDist(0);
        for(int j(0); j<totalSignalInfo.size(); j++){
           if(totalSignalInfo[j].group_Id == iGroup){
              groupDist += totalSignalInfo[j].dist ;
            }
        }
       groupTotalDist[iGroup] = groupDist;  
    }
    
   for(int j(0); j<totalSignalInfo.size(); j++){
      int iGroup =  totalSignalInfo[j].group_Id;
      totalSignalInfo[j].dist = groupTotalDist[iGroup]; 
    }

   
    
    
    std::sort(totalSignalInfo.begin(), totalSignalInfo.end(),
                []( classficationInfo & a, classficationInfo & b ) -> bool
                {return a.dist < b.dist;} 
             );
    
    int cluster_0(0); int cluster_1(0);
    for(int i(0); i<k*6;  i++){
         if(totalSignalInfo[i].class_Id == 0){
            cluster_0++;
         }
          if(totalSignalInfo[i].class_Id == 1){
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
    
    /*
   std::cout <<cluster_0 << "   "<<cluster_1<< std::endl;
   std::cout << std::endl;
   for(int j(0); j<totalSignalInfo.size(); j++){
      std::cout << totalSignalInfo[j].signal_GId <<"  "<< totalSignalInfo[j].signal_LId<< "  " << totalSignalInfo[j].group_Id << "  " << totalSignalInfo[j].dist <<"  " << totalSignalInfo[j].class_Id<< std::endl;  
    }

   std::cout << std::endl;
   std::cout << std::endl;
    */
   }   

}


template<typename I, typename O>
void
KNN_Acceleration<I,O>::writeClassificationSignal(){

  if(rank == 0){
    
      std::string cluster = data_workspace + "KNN_timesseries_label.csv";
      char Cluster[ cluster.size() + 1];
      strcpy(Cluster, cluster.c_str());
      std::ofstream cluster_output;
      cluster_output.open(Cluster);

        cluster_output<<"timeseries;label" << std::endl;
     for(int i(0); i<outputSignalClassification.size(); i++){ 
        // std::cout << totalSignalInfo[i].signal_GId<<" | "<< totalSignalInfo[i].class_Id << std::endl;
         cluster_output << outputSignalClassification[i].sensor_name<<";"<< outputSignalClassification[i].class_Id << std::endl; 
     }

 
   } 

}

template<typename I, typename O>
void
KNN_Acceleration<I,O>::MPI_Point_Data_Type(MPI_Datatype & MPIPointData){


    const int    nItems=5;
    int          blocklengths[nItems] = { 1, 1, 1, 1 ,1 };
    MPI_Datatype types[nItems] = { MPI_INT, MPI_INT, MPI_INT, MPI_INT, mpi_get_type<I>()};
    MPI_Aint     offsets[nItems];

    offsets[0] = offsetof(classficationInfo, signal_GId);
    offsets[1] = offsetof(classficationInfo, signal_LId);
    offsets[2] = offsetof(classficationInfo, group_Id);
    offsets[3] = offsetof(classficationInfo, class_Id);
    offsets[4] = offsetof(classficationInfo, dist);

    MPI_Type_create_struct(nItems, blocklengths, offsets, types, & MPIPointData);
    MPI_Type_commit(& MPIPointData);

}

/*
template<typename I,typename O>
void
KNN_Acceleration<I,O>::searchFileInDirectory()
{
  char DirPath[ unclassified_path_signal.size() + 1];
  strcpy(DirPath, unclassified_path_signal.c_str()); 
  if (auto dir = opendir(DirPath)) {
  while (auto f = readdir(dir)) {
        if (!f->d_name || f->d_name[0] == '.')
          continue; // Skip everything that starts with a dot
          std::string signalInsideDir = f->d_name;
          unClassifiedSignalVec.push_back(signalInsideDir);
     }
    closedir(dir);
   }

}



template<typename I, typename O>
void
KNN_Acceleration<I,O>::readTransfIndex(){
  
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
KNN_Acceleration<I,O>::signalClassificationReader(){

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
KNN_Acceleration<I,O>::sensorNameFinder(int signalIndex){
   
     auto it = std::find_if(Index2sensorId.begin(), Index2sensorId.end(), 
                       [signalIndex] (const Index2SensorName & signal) { 
                          return signal.signal_Id == signalIndex; 
                       });

    return it->sensor_name;
}  

template<typename I,typename O>
int 
KNN_Acceleration<I,O>::classIDFinder(std::string sensorName){
   
     auto it = std::find_if(signalClassId.begin(), signalClassId.end(), 
                       [sensorName] (const KMeanClassificationReader & signal) { 
                          return signal.sensor_name == sensorName; 
                       });

    return it->class_Id;
 }  
*/


template<typename I, typename O>
void
KNN_Acceleration<I,O>::Display( signalContainer const & signal ){

  
  std::cout << std::endl;
  std::cout << std::endl;
  std::cout << "signal group ID  " << signal.group_Id<<  std::endl;
  std::cout << "signal global ID  " << signal.signal_GId<<  std::endl;
  std::cout << "signal local ID  " << signal.signal_LId<<  std::endl;
  std::cout << "signal class ID  " << signal.class_Id<<  std::endl;
  
  for(int i(0); i<signal.signal_Val.size(); i++){
    std::cout << signal.signal_Val[i] << "  ";
  }

   std::cout << std::endl;
   std::cout << std::endl;
 }

#endif