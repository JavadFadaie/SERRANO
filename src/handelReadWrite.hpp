#ifndef READWRITEHENDEL_HPP
#define READWRITEHENDEL_HPP

#include <iostream>
#include <vector>
#include <cstdlib>
#include <cstring>

template<typename I, typename O>
class readWriteTool{

 public :

  struct Index2SensorName{
    Index2SensorName(int _signal_Id
                    ,std::string _sensor_name )
       :signal_Id(_signal_Id)
       ,sensor_name(_sensor_name)
       {}

   int signal_Id;
   std::string sensor_name;
  };
  
  
  struct KMeanClassificationContanier{
       
    KMeanClassificationContanier(std::string _sensor_name
                                ,int _class_Id )
      :sensor_name(_sensor_name)
      ,class_Id(_class_Id)
      {}
         
    std::string sensor_name; 
    int class_Id;
  };

  readWriteTool();

  std::vector<I> readBinary(std::string const & path);

  std::vector<I> readTxtFile(std::string const & path);

  void writeBuffer2Binary( std::vector<O> const & buffer 
                         , std::string const  bufferPath);

  void writeBuffer2Text( std::vector<O> const & buffer 
                       , std::string const  bufferPath);

  std::vector<std::string> searchFileInDirectory(std::string const & dirPath);

  size_t  numFileInsideDirectory(std::string const & dirPath);

  size_t const  fileSize(std::string const & path);

  std::vector<Index2SensorName> readTransfIndex(std::string const & Path);

  std::vector<KMeanClassificationContanier> signalClassificationReader(std::string const & classification_training);

  std::string findSensorNameId(std::vector<Index2SensorName> const & Index2sensorId
                              ,int const signalIndex);

  int classIDFinder( std::vector<KMeanClassificationContanier> const & KMeanClassificationVec
                   , std::string sensorName);

  std::vector<I> readInferenceSignalCSV(std::string const & path);                            

  
private:

};

///////////////////////////////////
////////// Implementation
//////////////////////////////////
template<typename I, typename O>
readWriteTool<I,O>::readWriteTool()
{}


template<typename I, typename O>
std::vector<I>
readWriteTool<I,O>::readBinary(std::string const & path){

  FILE *p_file; 
  p_file = fopen(path.c_str(),"rb");
  fseek(p_file,0,SEEK_END);
  size_t sizefile = ftell(p_file);
  size_t totalNumData= sizefile/sizeof(I);
  fclose(p_file);

  char dataPath[path.size() + 1];
  strcpy(dataPath, path.c_str());
  FILE* pFileSeq;
  pFileSeq = fopen(dataPath, "rb"); 
  std::vector<I> buffer(totalNumData);
  fread(& buffer[0], totalNumData, sizeof(I), pFileSeq);
  fclose(pFileSeq);

 return buffer;

}



template<typename I, typename O>
size_t 
readWriteTool<I,O>::numFileInsideDirectory(std::string const & dirPath)
{
   size_t numFilesInsideDirectory(0); 
  char DirPath[ dirPath.size() + 1];
  strcpy(DirPath, dirPath.c_str()); 
  if (auto dir = opendir(DirPath)) {
  while (auto f = readdir(dir)) {
        if (!f->d_name || f->d_name[0] == '.')
          continue; // Skip everything that starts with a dot
          numFilesInsideDirectory ++ ;
     }
    closedir(dir);
   }

  return numFilesInsideDirectory;
}


template<typename I, typename O>
std::vector<std::string>
readWriteTool<I,O>::searchFileInDirectory(std::string const & dirPath){

  std::vector<std::string>  fileInsideDir;    
  char DirPath[ dirPath.size() + 1];
  strcpy(DirPath, dirPath.c_str()); 
  if (auto dir = opendir(DirPath)) {
  while (auto f = readdir(dir)) {
        if (!f->d_name || f->d_name[0] == '.')
          continue; // Skip everything that starts with a dot
          std::string file = f->d_name;
          fileInsideDir.push_back(file);
     }
    closedir(dir);
   }

 return fileInsideDir;
}

template<typename I, typename O>
void 
readWriteTool<I,O>::writeBuffer2Binary( std::vector<O> const & buffer 
                                       , std::string const  bufferPath){

  char Path[ bufferPath.size() + 1];
  strcpy(Path, bufferPath.c_str());
  FILE* pFile;
  pFile = fopen(Path, "wb");
  fwrite(& buffer[0], buffer.size(), sizeof(O), pFile);
  fclose(pFile);
  
}


template<typename I, typename O>
void 
readWriteTool<I,O>::writeBuffer2Text( std::vector<O> const & buffer 
                                    , std::string const  bufferPath){

  std::string path = bufferPath;
  char Path[ path.size() + 1];
  strcpy(Path, path.c_str());
  std::ofstream cluster_output;
  cluster_output.open(Path);
  for(int i(0); i<buffer.size(); i++){
    cluster_output << i << " " << buffer[i] << std::endl; 
   } 

}

template<typename I,typename O>
size_t const 
readWriteTool<I,O>::fileSize(std::string const & path)
{ 
   
  FILE *p_file; 
  p_file = fopen(path.c_str(),"rb");
  fseek(p_file,0,SEEK_END);
  size_t sizefile = ftell(p_file);
  size_t totalNumData= sizefile/sizeof(I);
  fclose(p_file);
  std::cout << path << std::endl;
  std::cout << sizefile << std::endl;

   return sizefile;
}


template<typename I,typename O>
std::vector<typename::readWriteTool<I,O>::Index2SensorName>
readWriteTool<I,O>::readTransfIndex(std::string const & path){
  
 //std::string path =  data_workspace +"/Output_Data/Scalability/signalTransform.txt"; 
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
readWriteTool<I,O>::findSensorNameId(std::vector<Index2SensorName> const & Index2sensorId
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

template<typename I,typename O>
std::vector<typename::readWriteTool<I,O>::KMeanClassificationContanier>
readWriteTool<I,O>::signalClassificationReader(std::string const & classification_training){

    std::string fileline;
    std::string elm;
    std::ifstream InputData;
    InputData.open(classification_training);
    std::vector<std::string> data;
    std::vector<KMeanClassificationContanier> KMeanClassificationVec;
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
        KMeanClassificationVec.push_back(KMeanClassificationContanier(sensor_name,classID));
    }

   return KMeanClassificationVec;
    
}

template<typename I,typename O>
int 
readWriteTool<I,O>::classIDFinder( std::vector<KMeanClassificationContanier> const & KMeanClassificationVec
                                 , std::string sensorName){
   
     auto it = std::find_if(KMeanClassificationVec.begin(), KMeanClassificationVec.end(), 
                       [sensorName] (const KMeanClassificationContanier & signal) { 
                          return signal.sensor_name == sensorName; 
                       });

    return it->class_Id;
 }

template<typename I, typename O>
std::vector<I>
readWriteTool<I,O>::readInferenceSignalCSV(std::string const & path){
 
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

 /*  
   for(int i(0); i<signalBuffer.size(); i++){
    std::cout << signalBuffer[i] << std::endl;
   } 
  */ 

 return signalBuffer;
} 
#endif