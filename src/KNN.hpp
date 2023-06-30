#ifndef KNN_HPP
#define KNN_HPP

#include <cstdlib>
#include <memory>
#include "ReadWriteMPI_IO.hpp"


template<typename I, typename O>
class KNN{
  
 public:

 struct Tpoint
   {
       Tpoint(  I _x, I _y , int _id)
       :x(_x)
       ,y(_y)
       ,id(_id)
       {}

       Tpoint()
       :x(0)
       ,y(0)
       ,id(0)
       {}
              
       I  x,y;
       int  id;
   };

 struct DistTrainData
   {
       DistTrainData( I _dist , int _id)
       :dist(_dist)
       ,id(_id)
       {}

       DistTrainData()
       :dist(0)
       ,id(0)
       {}
              
       I  dist;
       int  id;    
   };

 

  public:

    KNN( int  _rank
       , int  _size);

    void kthNeighbor(int  _k_nearestNeighbor);
    
    void Data_IO_Path
       ( std::string  _inputDataPath
       , std::string  _knnClustering);

    void profileWorkSpace(std::string _profilePath);

    void DistrbutionData();

    void clusterNum(int const n);
    
    void applyKNN();

    void applyParallelKNN();

    void profillingApply();

    void parallelKNNFunction(std::vector<I> const & trainInputData);

    std::vector<I> const & ReadInputBuffer();

    std::vector<typename::KNN<I,O>::Tpoint> BcastUnclassifiedPoints();

    void clusterPointKNN
           (std::vector<Tpoint> const & unClassifiedVec
           ,std::vector<I>      const & trainInputData);

    void classifyPointParallel
           (std::vector<I> const & InputData);

    std::vector<DistTrainData> distUnclassifiedPoint2TrainData
           ( Tpoint         const & unClassifiedPoint
           , std::vector<I> const & InputData);

    void KNearestNeighbor
           (std::vector<DistTrainData> const & distanceVector);

    void KNearestNeighborOptimazed
           (std::vector<DistTrainData> const & distanceVector);


    I const computeDistance
            (Tpoint const _unSignedPoint, Tpoint const Tp);
 
    void sortingDistance(std::vector<DistTrainData>  & distVec );

    void findNearNeighborGroup(std::vector<DistTrainData> const & distVec);

    void MPI_Point_Data_Type(MPI_Datatype & MPI_TrainPoint);
   
    void MPI_Dist_Data_Type(MPI_Datatype & MPIDistData); 

    void applyKNN_Sequential();
    
    void KNN_Function(Tpoint const p);

    void benchmarkStatus(bool const _benchmarkState);

  private:

  int const MASTER = 0;

  int rank;
  int size;

  std::string knnDataPath;
  std::string knnClustering;
  std::string profilePath;

  int k_nearestNeighbor; 
  int numCluster;
  size_t sizefileGB;
  bool benchmark_state;

  std::vector<Tpoint> vecTrainPoint;
  std::vector<DistTrainData> totalDistanceVec;
  std::unique_ptr<ReadWriteMPI_IO<I,O>> readWriteParallel;

  std::unique_ptr<DistributedTimer<I,O>> Timer;
  std::unique_ptr<ProfileHPC<I,O>>      profile;



  std::vector<size_t> numDataProc;
  std::vector<size_t> offsetVec;
  std::vector<int> offsetDistVec;
  std::vector<int> numDistVec;

  size_t totalNumData;
  Tpoint unClassifiedPoint;
 
};

///////////////////////////////////////////////////////
////////////  IMPLMENTATION
////////////////////////////////////////////////////////

template<typename I, typename O>
KNN<I,O>::KNN( int  _rank
             , int  _size )
:rank(_rank)
,size(_size)
,numDataProc(size)
,offsetVec(size)
,offsetDistVec(size)
,numDistVec(size)
,numCluster(0)     
{
  Timer = std::unique_ptr<DistributedTimer<I,O> >
               (new DistributedTimer<I,O>(rank,size));
  profile = std::unique_ptr< ProfileHPC<I,O> >
            ( new ProfileHPC<I,O>(rank,size) );                
}

template<typename I,typename O>
void
KNN<I,O>::kthNeighbor(int  _k_nearestNeighbor)
{
  k_nearestNeighbor = _k_nearestNeighbor; 
}


template<typename I,typename O>
void
KNN<I,O>
   ::Data_IO_Path
    ( std::string  _knnDataPath
    , std::string  _knnClustering)
{
    knnDataPath   = _knnDataPath;
    knnClustering = _knnClustering;
}

template<typename I, typename O>
void 
KNN<I,O>::benchmarkStatus(bool const _benchmark_state){
    benchmark_state = _benchmark_state;
}

template<typename I,typename O>
void
KNN<I,O>::profileWorkSpace( std::string _profilePath){
    profilePath = _profilePath;
}

template<typename I,typename O>
void 
KNN<I,O>::DistrbutionData(){
     
FILE *p_file;
p_file = fopen(knnDataPath.c_str(),"rb");
fseek(p_file,0,SEEK_END);
sizefileGB = ftell(p_file);
totalNumData= sizefileGB/(3*sizeof(I));
fclose(p_file);

size_t displac(0);totalDistanceVec.resize(totalNumData); int dispDist(0);
size_t rem(totalNumData%size);
for(int i(0); i<size; i++){
  numDataProc[i] = totalNumData/size;   
  offsetVec[i] = displac;
  offsetDistVec[i] = dispDist;
  if(rem > 0){
    numDataProc[i]++;
    rem--;      
   }
   numDistVec[i] = numDataProc[i];
   dispDist += numDataProc[i];
   numDataProc[i] = 3*numDataProc[i];
   displac += numDataProc[i]; 
 }

 
}

template<typename I, typename O>
void
KNN<I,O>::clusterNum(int const  n)
{
   numCluster = n;
}


template<typename I, typename O>
void
KNN<I,O>::applyKNN()
{
    //Distributiong the Data and Intializing the buffers
     DistrbutionData();
 
     applyParallelKNN();
}

template<typename I,typename O>
void 
KNN<I,O>::profillingApply()
{
  bool KNN_Applied( true );
  profile->pathProfileSettingClustering
             ( profilePath
             , knnClustering);
  
  profile->applyingKNNClassification(KNN_Applied);
  profile->setFileSize(sizefileGB);
  profile->benchmarkStatus(benchmark_state);
  profile->execuationTimeInformation(Timer->timerInformation());   
  profile->computeEnergy();
  profile->generateProfillingFile();
  
}

template<typename I, typename O>
void
KNN<I,O>::applyParallelKNN(){

       //Read Training data in Parallel and load them into local Buffer
  Timer->start("RKNN");     
    std::vector<I> const & trainInputData  ( ReadInputBuffer() );
  Timer->stop("RKNN"); 

      
  Timer->start("KNN");
     parallelKNNFunction(trainInputData);
  Timer->stop("KNN"); 
}



template<typename I, typename O>
void
KNN<I,O>::parallelKNNFunction(std::vector<I> const & trainInputData){

    //Get Unclassified Point and Broadcast from master Proc into all Proc
  std::vector<Tpoint> const & unClassifiedVec(BcastUnclassifiedPoints());

     //clustering the Unclassified Vec
  clusterPointKNN(unClassifiedVec, trainInputData); 
  
}  

template<typename I, typename O>
std::vector<typename::KNN<I,O>::Tpoint>
KNN<I,O>::BcastUnclassifiedPoints(){

 std::vector<Tpoint> unClassifiedVec(1); 
if(rank == MASTER)
{
    unClassifiedVec[0].x = 0.5;
    unClassifiedVec[0].y = 0.3;
    unClassifiedVec[0].id = -2;
}

MPI_Datatype MPIPointData;
MPI_Point_Data_Type( MPIPointData );


 MPI_Bcast
    ( & unClassifiedVec[0]
    , 1
    , MPIPointData
    , MASTER
    , MPI_COMM_WORLD);

 return unClassifiedVec;    

}


template<typename I, typename O>
void
KNN<I,O>::clusterPointKNN
        (std::vector<Tpoint> const & unClassifiedVec
        ,std::vector<I>      const & trainInputData){

  for(int i(0); i<unClassifiedVec.size(); i++){ 

      //computing the distrance of Unclassified point to Training point
  std::vector<DistTrainData> distVec( 
                                 distUnclassifiedPoint2TrainData(unClassifiedVec[i],trainInputData) 
                              );
      //Sorting the Distance vector locally inside each Proc
   sortingDistance(distVec);
    
     //Gather the Distance from all Proc to Master Proc and sorting them and
     //Find k nearest Neighbor 
    KNearestNeighborOptimazed(distVec);
   // KNearestNeighbor(distVec);
  }

}


template<typename I, typename O>
std::vector<typename::KNN<I,O>::DistTrainData>
KNN<I,O>::distUnclassifiedPoint2TrainData
          ( Tpoint              const & unClassifiedVec
          , std::vector<I>      const & InputData)
{
    
    std::vector<DistTrainData> distVecPoint2TrainData;
    int iter(0); Tpoint Tp; 
 for(int i(0); i<InputData.size(); ){
     Tp.x = InputData[iter++]; Tp.y = InputData[iter++]; 
     Tp.id= InputData[iter++];
     i = iter;
     I dist( computeDistance( unClassifiedVec, Tp));
     distVecPoint2TrainData.push_back(DistTrainData(dist,Tp.id));
  }

  return distVecPoint2TrainData;
}




template<typename I, typename O>
void
KNN<I,O>::KNearestNeighborOptimazed
        (std::vector<DistTrainData> const & distanceVector)
{

  //Initializing the MPI struct Data type for Distance
 MPI_Datatype  MPIDistData;
 MPI_Dist_Data_Type( MPIDistData);

std::vector<int> numDistVecOp(size,k_nearestNeighbor);
std::vector<int> offsetDistVecOp(size);
std::vector<DistTrainData> totalDistanceVecOp(size*k_nearestNeighbor);
for(int i(0); i<size; i++){
 offsetDistVecOp[i]=i*k_nearestNeighbor;
}

MPI_Gatherv
      ( distanceVector.data()  //send_data,
      , k_nearestNeighbor             //send_count
      , MPIDistData                   //send_datatype
      , totalDistanceVecOp.data()      //recv_data,
      , & numDistVecOp[0]              
      , & offsetDistVecOp[0]
      , MPIDistData                 //recv_datatype,
      , MASTER                      //root
      , MPI_COMM_WORLD);

   if(rank == MASTER){  
     sortingDistance(totalDistanceVecOp);
     findNearNeighborGroup(totalDistanceVecOp); 
   }
        
}


template<typename I, typename O>
I const
KNN<I,O>::computeDistance(Tpoint const _unClassifiedPoint, Tpoint const Tp)
{
    return sqrt( (_unClassifiedPoint.x - Tp.x)*(_unClassifiedPoint.x - Tp.x) 
               + (_unClassifiedPoint.y - Tp.y)*(_unClassifiedPoint.y - Tp.y)
               ); 
}


template<typename I, typename O>
void
KNN<I,O>::sortingDistance(std::vector<DistTrainData> & distVec ){
   
      std::sort
         ( distVec.begin(), distVec.end(),
                [](const DistTrainData & a, const DistTrainData & b) -> bool
                { return a.dist < b.dist; }
         );
   
    
}

template<typename I, typename O>
void
KNN<I,O>::findNearNeighborGroup(std::vector<DistTrainData> const & distVec)
{
   
   std::vector<int> countKneighbor(numCluster); 
   for(int i(0); i<k_nearestNeighbor; i++){
       countKneighbor[distVec[i].id]++;
    }
   int maxElement = std::max_element(countKneighbor.begin(),countKneighbor.end()) - countKneighbor.begin();
   
   if(!benchmark_state)
   { 
     char outputFile[ knnClustering.size() + 1];
     strcpy(outputFile, knnClustering.c_str()); 
     std::ofstream outputPath;
     outputPath.open (outputFile);

     outputPath<<"                                Profilling of the parallel KNN Clustering" <<std::endl;
     outputPath<<"<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"<<std::endl;
     outputPath << "This is the most common class in nearest neighbor " << maxElement <<"  " << countKneighbor[maxElement]<< std::endl;

     for(int i(0); i<numCluster; i++){
         outputPath << i <<"  " <<  countKneighbor[i] << std::endl;
        // countKneighbor[distVec[i].id]++;
       }  
   
      outputPath.close();
   }
}


template<typename I, typename O>
void
KNN<I,O>::KNearestNeighbor(std::vector<DistTrainData> const & distanceVector )
{

  //Initializing the MPI struct Data type for Distance
 MPI_Datatype  MPIDistData;
 MPI_Dist_Data_Type( MPIDistData);


  MPI_Gatherv
      ( distanceVector.data()     //send_data,
      , distanceVector.size()     //send_count
      , MPIDistData               //send_datatype
      , totalDistanceVec.data()   //recv_data,
      , & numDistVec[0]              
      , & offsetDistVec[0]
      , MPIDistData                 //recv_datatype,
      , MASTER                      //root
      , MPI_COMM_WORLD);

   if(rank == MASTER){
     
     sortingDistrance(totalDistanceVec);

     findNearNeighborGroup(totalDistanceVec); 
   }
        
}

template<typename I,typename O>
std::vector<I> const & 
KNN<I,O>::ReadInputBuffer()
{

  readWriteParallel = std::unique_ptr< ReadWriteMPI_IO<I,O> >
           ( new ReadWriteMPI_IO<I,O>(rank, size, numDataProc, offsetVec) );
 
  readWriteParallel->inputFileSetPath(knnDataPath);
  readWriteParallel->readInputBuffer();

  return readWriteParallel->InputBuffer();
}



template<typename I, typename O>
void
KNN<I,O>::applyKNN_Sequential()
{

   if(size == 1){
      unClassifiedPoint.x = 0.5;
      unClassifiedPoint.y = 0.3;
      DistrbutionData(); 
      KNN_Function(unClassifiedPoint);
   }else{
      std::cout << "This is just sequential function " << std::endl;
   }
}

template<typename I, typename O>
void
KNN<I,O>::KNN_Function(Tpoint const p)
{
         //Read Training data in  and load them into local Buffer
   std::vector<I> const & trainInputData  ( ReadInputBuffer() );

      //computing the distrance of Unclassified point to Training point
    std::vector<DistTrainData> distVec (distUnclassifiedPoint2TrainData(unClassifiedPoint,trainInputData) );
 
     // sorting Distrance 
   sortingDistrance(distVec);

     //Find the nearest neighbor with the first K elment
   findNearNeighborGroup(distVec);
}



template<typename I, typename O>
void
KNN<I,O>::MPI_Point_Data_Type(MPI_Datatype & MPIPointData){

const int    nItems=3;
int          blocklengths[nItems] = { 1, 1, 1 };
MPI_Datatype types[nItems] = { mpi_get_type<I>(), mpi_get_type<I>(), MPI_INT};
MPI_Aint     offsets[nItems];

offsets[0] = offsetof(Tpoint, x);
offsets[1] = offsetof(Tpoint, y);
offsets[2] = offsetof(Tpoint, id);

MPI_Type_create_struct(nItems, blocklengths, offsets, types, & MPIPointData);
MPI_Type_commit(& MPIPointData);

}

template<typename I, typename O>
void
KNN<I,O>::MPI_Dist_Data_Type(MPI_Datatype & MPIDistData){

const int    nItems=2;
int          blocklengths[nItems] = { 1, 1 };
MPI_Datatype types[nItems] = { mpi_get_type<I>(), MPI_INT};
MPI_Aint     offsets[nItems];

offsets[0] = offsetof(DistTrainData, dist);
offsets[1] = offsetof(DistTrainData, id);

MPI_Type_create_struct(nItems, blocklengths, offsets, types, & MPIDistData);
MPI_Type_commit(& MPIDistData);

}


#endif
