#ifndef KMEAN_HPP
#define KMEAN_HPP

#include <cmath>
template<typename I, typename O>
class Kmean {
    
    public:

    struct point{
     
     point(I _x, I _y, int _classId)
     :x(_x)
     ,y(_y)
     ,classId(_classId)
     {}

     point()
     :x(0)
     ,y(0)
     ,classId(0)
     {}

     I x;
     I y;
     int classId;

    };

    public:

      Kmean(int _rank, int _size);

      void Data_IO_Path( std::string _inputDataPath
                       , std::string _outputDataPath);

     void clusterNum(int  _numCluster);
      
     //Parallel Implementation of Kmean 
     void applyKmean();

     void applyParallelKmean();

     void profillingApply();

     void KmeanClustering(std::vector<I> & data);

     std::vector<I> & ReadInputBuffer();

     void writeOutputData();
 
     void DistrbutionData();

     void BroadCastCenteriodParallel(std::vector<point> & centeriod, int iter);

     void BroadCastPoint(std::vector<point> & data);
     
     std::vector<point> const assignClassNewCenteriodParallel
                       ( std::vector<I>       & data
                       , std::vector<point> const & centeriod
                       , int iter);
     
     std::vector<point> calculateCentriodParallel
                      ( std::vector<point>        & newCenteriod
                      , std::vector<point> const  & centeriod
                      , std::vector<int>   const  & classSize
                      , int iter);

     bool assignAndCheckCentroid( std::vector<point>       & centeriod
                                , std::vector<point> const & procCenteriod);

     
     I errorCriteria( point       & centeriod
                   , point const & procCenteriod);  

     void controlLoop(I epsilon);   

     void benchmarkStatus(bool const _benchmark_state);   

     void profileWorkSpace( std::string _profilePath);                                 
    
     void loadDataParallel();

     void MPI_Point_Data_Type(MPI_Datatype & MPIPointData);

     void readFromFile(std::vector<point> & centeriod);
     
     void Display(std::vector<I>  & data, int iter);
     
     void DisplayCluster(std::vector<I>  & data, int iteration);

     void applySeq();

    
      std::vector<point> const
      initializeCenteriod(); 
     
      std::vector<point> const assignClassNewCenteriod
                       ( std::vector<point>       & data
                       , std::vector<point> const & centeriod);
     
      void recalculateCenteroid
                       ( std::vector<point>        & newCenteriod
                       , std::vector<point> const  & centeriod
                       , std::vector<int>   const  & classSize);
   
     I const computeDistance(point const & p, point const & c);


   private:
   
    int const MASTER = 0;

    int rank;
    int size;
    int numCluster;
    double sizefileGB;
  
    std::string inputDataPath;
    std::string outputDataPath;
    std::string profilePath;

    std::vector<size_t> numDataProc;
    std::vector<size_t> offsetVec;
    size_t TotalNumData;

    std::vector<point> data;
    std::vector<int> classSize;

   std::unique_ptr<ReadWriteMPI_IO<I,O>> readWriteParallel;
   std::unique_ptr<DistributedTimer<I,O>> Timer;
   std::unique_ptr<ProfileHPC<I,O>>      profile;

   std::vector<O>  outputBuffer;
   std::vector<point> outputData;
   std::vector<point> centeriod;
   I epsilon;
   bool KMean_Applied;
   bool benchmark_state;
   int  controLoopIter;
   I    centeroidError;

};

template<typename I, typename O>
Kmean<I,O>::Kmean(int _rank, int _size)
:rank(_rank)
,size(_size)
,numDataProc(size)
,offsetVec(size)
,KMean_Applied(false)
,controLoopIter(0)
,centeroidError(0)
{
     Timer = std::unique_ptr<DistributedTimer<I,O> >
               (new DistributedTimer<I,O>(rank,size));

     profile = std::unique_ptr< ProfileHPC<I,O> >
            ( new ProfileHPC<I,O>(rank,size) );  
}

template<typename I,typename O>
void
Kmean<I,O>::Data_IO_Path
               ( std::string _inputDataPath
               , std::string _outputDataPath)
{
  inputDataPath  = _inputDataPath;
  outputDataPath = _outputDataPath;
 }

template<typename I,typename O>
void
Kmean<I,O>::profileWorkSpace( std::string _profilePath){
    profilePath = _profilePath;
}

template<typename I, typename O>
void 
Kmean<I,O>::benchmarkStatus(bool const _benchmark_state){
    benchmark_state = _benchmark_state;
} 

template<typename I,typename O>
void 
Kmean<I,O>::DistrbutionData(){

FILE * p_file;
p_file = fopen(inputDataPath.c_str(),"rb");
fseek(p_file,0,SEEK_END);
sizefileGB = ftell(p_file);
TotalNumData = sizefileGB/(3*sizeof(I));
fclose(p_file);

size_t rem(TotalNumData%size);
size_t displac(0);
for(int i(0); i<size; i++){
    numDataProc[i] = TotalNumData/size;
    offsetVec[i]   = displac; 
    if(rem > 0){
      numDataProc[i]++;
      rem--;      
    }
   numDataProc[i] = 3*numDataProc[i];
   displac += numDataProc[i];
 }

 
} 

template<typename I, typename O>
void
Kmean<I,O>::clusterNum(int  _numCluster)
{
   numCluster = _numCluster;
}


template<typename I, typename O>
void
Kmean<I,O>:: controlLoop(I _epsilon)
{
   epsilon = _epsilon;
}

template<typename I, typename O>
void
Kmean<I,O>::applyKmean(){
  
   DistrbutionData();

   applyParallelKmean();
   
 }   


template<typename I,typename O>
void 
Kmean<I,O>::profillingApply()
{
  bool KMean_Applied( true );

  profile->pathProfileSettingClustering
             ( profilePath
             , outputDataPath);

  profile->applyingKMeanClassification(KMean_Applied);
  profile->setFileSize(sizefileGB);
  profile->benchmarkStatus(benchmark_state);
  profile->controlQualityLoop(epsilon);
  profile->controlLoopIteration(controLoopIter);
  profile->centroidAccuracy(centeroidError);
  profile->execuationTimeInformation(Timer->timerInformation());   
  profile->computeEnergy();
  profile->generateProfillingFile();
  
}

template<typename I, typename O>
void
Kmean<I,O>::applyParallelKmean(){
  
  
  //Read the input data 
  Timer->start("RKMEAN"); 
    std::vector<I> & data (ReadInputBuffer());
  Timer->stop("RKMEAN");
 
  Timer->start("KMEAN"); 
     KmeanClustering(data);
  Timer->stop("KMEAN");
  //Display( data);
  
  //Writeout the cluster data
  Timer->start("WKMEAN"); 
     writeOutputData();
  Timer->stop("WKMEAN");
  
}


template<typename I, typename O>
void
Kmean<I,O>::KmeanClustering(std::vector<I> & data){
  
  int iter(0); bool status(true); int k(9);  centeriod.resize(numCluster);
  while(status)
  {

       if( iter >= k ){
           status = false;
           break;
        }else{

                  //Broadcast for the random centroid point to the worker  process in first iteration
            if(iter == 0){
                BroadCastCenteriodParallel(centeriod, iter);
            }  
                  //Calculate the new Centroid point
            std::vector<point>  procCenteriod( assignClassNewCenteriodParallel(data,centeriod, iter) );
            
            iter++;

                  //Broacast the new Centroid poitns of cluster to the worker process
            BroadCastCenteriodParallel(procCenteriod, iter);  

                  //Check that the new centroid change with previous centroid
            bool checkCenteroid = assignAndCheckCentroid(centeriod, procCenteriod);           
            
             // Display( data, iter);

             if( checkCenteroid ){
                status =  false;
             }

            if(rank == 0){
                  controLoopIter = iter;
                  //std::cout << "This is iter " << iter << "  " << size <<"  " << centeroidError << std::endl;
                for(int i(0); i<numCluster; i++){
                 //  std::cout <<" Avg "<< centeriod[i].x << "  " << centeriod[i].y << "  "<< centeriod[i].classId << std::endl;
                  }
            }
          
        }
  }
 
}

template<typename I, typename O>
std::vector<typename::Kmean<I,O>::point> const
Kmean<I,O>::assignClassNewCenteriodParallel( std::vector<I>       & data
                                           , std::vector<point> const & centeriod
                                           , int iter)
{
  std::vector<point> newCenteriod(numCluster); std::vector<int> classSize(numCluster,0);
  int iterr(0); outputBuffer.resize(data.size());
  for(int i(0); i<data.size();){
      I mindist(std::numeric_limits<I>::max());
      point   p; int j1=iterr++; int j2=iterr++; int j3=iterr++; 
      outputBuffer[j1]=p.x = data[j1]; 
      outputBuffer[j2]=p.y = data[j2]; 
      outputBuffer[j3]=p.classId= data[j3]; 
      i = iterr;     

      for(int j(0); j<centeriod.size(); j++){
          point const & c (centeriod[j]);
          I dist ( computeDistance(p,c) );
          if(dist < mindist){
              mindist = dist;
              p.classId = c.classId;
              outputBuffer[j3] = data[j3] = p.classId;
                
          }
       }
     
       classSize[p.classId]++;
       newCenteriod[p.classId].x += p.x;
       newCenteriod[p.classId].y += p.y;
       newCenteriod[p.classId].classId = p.classId;
    }

   return  calculateCentriodParallel(  newCenteriod , centeriod , classSize, iter);
}

template<typename I, typename O>
std::vector<typename::Kmean<I,O>::point>
Kmean<I,O>::calculateCentriodParallel
                ( std::vector<point>        & newCenteriod
                , std::vector<point> const  & centeriod
                , std::vector<int>   const  & classSize
                , int iter)
{
   
MPI_Datatype MPIPointData;
MPI_Point_Data_Type( MPIPointData );

//compute the coordinate of new Centriod
std::vector<point> totalNewCenteriod(size*newCenteriod.size());
MPI_Gather
      ( newCenteriod.data()      //send_data,
      , newCenteriod.size()      //send_count
      , MPIPointData             //send_datatype
      , totalNewCenteriod.data() //recv_data,
      , newCenteriod.size()      //recv_size,        
      , MPIPointData             //recv_datatype,
      , MASTER                    //root
      , MPI_COMM_WORLD);

//compute the size of each classes
std::vector<int> totalClassSize(size*newCenteriod.size(),0);
MPI_Gather
      ( classSize.data()        //send_data,
      , classSize.size()        //send_count
      , MPI_INT                 //send_datatype
      , totalClassSize.data()   //recv_data,
      , classSize.size()        //recv_size,        
      , MPI_INT                 //recv_datatype,
      , MASTER                  //root
      , MPI_COMM_WORLD);

std::vector<point> Tcenteriod(numCluster);

if(rank == 0)
{
    //Compute the total size of each class
    std::vector<int> classSize(numCluster,0);
    for(int j(0); j<size; j++){
       for(int i(0); i<numCluster ; i++){
          int ii = centeriod[i].classId;
          classSize[ii] += totalClassSize[j*numCluster + ii];
        }
    }

    //Compute the center of each Cluster of each class
    for(int j(0); j<size; j++){
       for(int i(0); i<numCluster ; i++){
          int ii = centeriod[i].classId;
          Tcenteriod[ii].x += totalNewCenteriod[j*numCluster + ii].x;
          Tcenteriod[ii].y += totalNewCenteriod[j*numCluster + ii].y;
          Tcenteriod[ii].classId = ii;
        }
    }

    //Compute the average center of each Cluster
    for(int i(0); i<numCluster ; i++){
        int ii = centeriod[i].classId;
        if( classSize[ii] != 0) {
            Tcenteriod[ii].x = Tcenteriod[ii].x/static_cast<I>(classSize[ii]);
            Tcenteriod[ii].y = Tcenteriod[ii].y/static_cast<I>(classSize[ii]);
        }
    }

}

return Tcenteriod;

}

template<typename I, typename O>
void
Kmean<I,O>::BroadCastCenteriodParallel(std::vector<point> & centeriod, int iter){
  
    if(iter == 0){
        if(rank == 0){
           readFromFile(centeriod);
         }
         BroadCastPoint(centeriod);
     }else{
         BroadCastPoint(centeriod);  
      }
}


template<typename I, typename O>
void
Kmean<I,O>::BroadCastPoint(std::vector<point> & data){


     MPI_Datatype MPIPointData;
     MPI_Point_Data_Type( MPIPointData );

       MPI_Bcast
        ( data.data()
        , numCluster
        , MPIPointData
        , MASTER
        , MPI_COMM_WORLD);

}



template<typename I,typename O>
std::vector<I> & 
Kmean<I,O>::ReadInputBuffer()
{
  readWriteParallel = std::unique_ptr< ReadWriteMPI_IO<I,O> >
           ( new ReadWriteMPI_IO<I,O>(rank, size, numDataProc, offsetVec) );
  
  readWriteParallel->inputFileSetPath(inputDataPath);
  readWriteParallel->readInputBuffer();

  return readWriteParallel->InputBuffer();
}

template<typename I,typename O>
bool 
Kmean<I,O>::assignAndCheckCentroid(std::vector<point>       & centeriod
                                 , std::vector<point> const & procCenteriod){

    int rep(0); bool CentriodCheck(false);
    for(int i(0); i<numCluster; i++){
        if( errorCriteria(centeriod[i] , procCenteriod[i] ) < epsilon ){
                rep++;
           }
         centeriod[i]=procCenteriod[i];             
    }
    
    if(rep == numCluster)
           CentriodCheck = true;


    return CentriodCheck;
}

template<typename I,typename O>
void
Kmean<I,O>::writeOutputData()
{
    readWriteParallel->outputFileSetPath( outputDataPath);
    readWriteParallel->writeOutputBuffer( outputBuffer );
}    

template<typename I, typename O>
void
Kmean<I,O>::MPI_Point_Data_Type(MPI_Datatype & MPIPointData){

const int    nItems=3;
int          blocklengths[nItems] = { 1, 1, 1 };
MPI_Datatype types[nItems] = { mpi_get_type<I>(), mpi_get_type<I>(), MPI_INT};
MPI_Aint     offsets[nItems];

offsets[0] = offsetof(point, x);
offsets[1] = offsetof(point, y);
offsets[2] = offsetof(point, classId);

MPI_Type_create_struct(nItems, blocklengths, offsets, types, & MPIPointData);
MPI_Type_commit(& MPIPointData);

}

template<typename I, typename O>
I
Kmean<I,O>::errorCriteria( point       & centeriod
                         , point const & procCenteriod){

 I error(0); 
 error = sqrt(   (centeriod.x - procCenteriod.x)*(centeriod.x - procCenteriod.x) 
               + (centeriod.y - procCenteriod.y)*(centeriod.y - procCenteriod.y)
             );
  centeroidError = error;             
  return error;
}

template<typename I, typename O>
void
Kmean<I,O>::readFromFile(std::vector<point> & centeriod){

    FILE* pFileSeq;
    char inputData[ inputDataPath.size() + 1];
    strcpy(inputData, inputDataPath.c_str()); 
    pFileSeq = fopen(inputData, "rb");

    for(int i(0); i<numCluster; i++){
        
        int z = rand()%(TotalNumData);
        std::vector<I> randBuffer(3);
        int offset = 3*z*sizeof(I);

        fseek(pFileSeq, offset, SEEK_SET); 
        fread(&randBuffer[0], sizeof(double), 3, pFileSeq);
        centeriod[i].x=randBuffer[0] ; centeriod[0].y=randBuffer[1]; centeriod[0].classId=i;
       // std::cout <<"This " << randBuffer[0] << "  " << randBuffer[1] << "   " << randBuffer[2] << std::endl; 
    }

        //centeriod[0].x=0.512932 ; centeriod[0].y=0.839112 ; centeriod[0].classId=0 ;
        //centeriod[1].x=0.23828 ; centeriod[1].y=0.970634 ; centeriod[1].classId=1 ;
    

    fclose(pFileSeq);
}

template<typename I, typename O>
void 
Kmean<I,O>::Display(std::vector<I>  & data, int iteration){
   
   
    int iterr(0); int iter(0);
    for(int i(0); i<data.size();){
         point   p;
         p.x = data[iterr++]; p.y = data[iterr++];  p.classId= data[iterr++];
         std::cout << p.x <<"  " << p.y<<"  "<< p.classId << std::endl; 
         i = iterr;     
    }

   // DisplayCluster(data,iteration);
}



template<typename I, typename O>
void 
Kmean<I,O>::DisplayCluster(std::vector<I>  & data, int iteration){

   std::ofstream cluster_0;
   std::string filenameCluster_0 ="/home/javad/Desktop/SERRANO/serrano/serrano/serrano/data/Output_Data/Kmean/ClusterZero"+ std::to_string(iteration) +".txt";
   cluster_0.open(filenameCluster_0);
  
   std::ofstream cluster_1;
   std::string filenameCluster_1 ="/home/javad/Desktop/SERRANO/serrano/serrano/serrano/data/Output_Data/Kmean/ClusterOne"+ std::to_string(iteration) +".txt";
   cluster_1.open(filenameCluster_1);
   
    int iterr(0); int iter(0);
    for(int i(0); i<data.size();){
         point   p;
         p.x = data[iterr++]; p.y = data[iterr++];  p.classId= data[iterr++]; 
        if(p.classId == 0){
          cluster_0<< p.x << "  " << p.y << std::endl;
        }else{
             cluster_1<< p.x << "  " << p.y << std::endl;
        }
         i = iterr;     
    }

}

template<typename I, typename O>
void
Kmean<I,O>::applySeq(){
   
  data.push_back(point(1,2,-1));
  data.push_back(point(1,2.2,-1));
  //data.push_back(point(1,2.5,-1));
  //data.push_back(point(1.5,2.5,-1));
  data.push_back(point(2,2,-1));
  data.push_back(point(2,2.2,-1));
  data.push_back(point(2,2.5,-1));
  data.push_back(point(1.8,2.5,-1));


   
  std::vector<point> const centeriod(initializeCenteriod());

  bool status(true); int k(0); int iter(3);
  while(status){
 
  std::vector<point> const & newCenteriod( assignClassNewCenteriod(data,centeriod) );
     
  std::vector<point> const & centeriod(newCenteriod);   
  
  for(int i(0); i<numCluster; i++){
   //  std::cout<< centeriod[i].x << "  " << centeriod[i].y <<"  " << centeriod[i].classId<< std::endl;
  }  
    
  k++;
  std::cout << std::endl;
  if(k>iter){
      status = false;
      break;
    }

  } 
}

template<typename I, typename O>
std::vector<typename::Kmean<I,O>::point> const
Kmean<I,O>::assignClassNewCenteriod
  ( std::vector<point>       & data
  , std::vector<point> const & centeriod)
{
  std::vector<point> newCenteriod(numCluster); std::vector<int> classSize(numCluster,0);
  for(int i(0); i<data.size(); i++){
      I mindist(std::numeric_limits<I>::max());
      point       & p (data[i]);

      for(int j(0); j<centeriod.size(); j++){
          point const & c (centeriod[j]);
          I dist ( computeDistance(p,c) );
          if(dist < mindist){
              mindist = dist;
              p.classId = c.classId;
          }
       }

       classSize[p.classId]++;
       newCenteriod[p.classId].x += p.x;
       newCenteriod[p.classId].y += p.y;
     }

   recalculateCenteroid(  newCenteriod , centeriod , classSize);
   
  for(int i(0); i<data.size(); i++){
      std::cout << data[i].x << "  " << data[i].y << "  " << data[i].classId << std::endl;
  }
 return  newCenteriod;
}


template<typename I, typename O>
void 
Kmean<I,O>::recalculateCenteroid( std::vector<point>        & newCenteriod
                                , std::vector<point> const  & centeriod
                                , std::vector<int>   const  & classSize)
{

  
  for(int i(0); i<numCluster; i++){
    //std::cout<< newCenteriod[i].x << "  " << newCenteriod[i].y <<"  " << newCenteriod[i].classId<< std::endl;
    int ii = centeriod[i].classId;
     newCenteriod[ii].x = newCenteriod[ii].x/static_cast<I>(classSize[ii]);
     newCenteriod[ii].y = newCenteriod[ii].y/static_cast<I>(classSize[ii]);
     newCenteriod[ii].classId = ii;
  }
 
  
}

template<typename I, typename O>
std::vector<typename::Kmean<I,O>::point> const
Kmean<I,O>::initializeCenteriod(){
    
  
  std::vector<point> centeriod(numCluster); 
  
  centeriod.push_back(point(1,2.2,0));
  centeriod.push_back(point(1.8,2.5,1));

  

 return centeriod;
}

template<typename I, typename O>
I const 
Kmean<I, O>::computeDistance(point const & p, point const & c){

   return sqrt( (p.x-c.x)*(p.x-c.x) + (p.y-c.y)*(p.y-c.y) );
}

#endif
