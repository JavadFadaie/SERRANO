#ifndef IDEKODATA_HPP
#define IDEKODATA_HPP

#include <iostream>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <memory>
#include "signalContainer.hpp"
#include "handelReadWrite.hpp"

//https://towardsdatascience.com/time-series-classification-using-dynamic-time-warping-61dcd9e143f6
//https://stackoverflow.com/questions/44945098/how-can-i-use-knn-k-means-to-clustering-time-series-in-a-dataframe
  //Fast DTW
//https://www.audiolabs-erlangen.de/resources/MIR/FMP/C3/C3S2_DTWbasic.html
template<typename I>
class IDEKO_DATA{
   
 
 public : 

  struct signalContainer{
     signalContainer
        ( std::string _value
        , int _id)
        :value(_value)
        ,id(_id)
        {}

    std::string value;
    int id;
   };

   struct assetContainer{
     assetContainer
        ( std::string _date
        , std::string _assetVal)
        :date(_date)
        ,assetVal(_assetVal)
        {}

    std::string date;
    std::string assetVal;
   };

  
   struct indexDateContainer{
     indexDateContainer
        ( int _signalIndex
        , std::string _date)
        :signalIndex(_signalIndex)
        ,date(_date)
        {}

     int signalIndex;
     std::string date; 
   };

  struct Index2SensorName{
    Index2SensorName(int _signal_Id
                    ,std::string _sensor_name )
       :signal_Id(_signal_Id)
       ,sensor_name(_sensor_name)
       {}

   int signal_Id;
   std::string sensor_name;
  };
  
 IDEKO_DATA(int _rank, int _size);

 void Data_IO_Path( std::string const & _inputDataPath);
 
 void Data_WorkSpace( std::string const & _data_workspace);

 void useCaseKernelApply(int const IDEKO, int const InBestMe);

 void readCSVFilesIDEKO();

 void readCSVFilesAcceleration();

 void Disply(std::vector<I> const & data);

 void writeBinaryFile(std::vector<I> const & data, int ID, std::string const sensorName);

 void applyingFilters
      ( bool _Kalman_FilterApplied
      , bool _FFT_FilterApplied
      , bool _BlackScholes_Applied
      , bool _savitzkyGoly_Transform );
 
 void writeTransformationIndex(std::vector<Index2SensorName> const & data);

 void convertBackToCSVFormate();
 
 std::vector<Index2SensorName> readTransfIndex();

 std::vector<typename::signalContainer<I>> 
 packupBinary2Container(std::vector<std::string> const & outputFiles
                      ,std::string const & output_Filter_Path);


 void readCSVFilesInBestMe();                     

 void writeAssetBuffer(std::vector<assetContainer> const & assetBuffer, int signal_Id);

 void writeDateIndex(std::vector<indexDateContainer> const & data);

private:

int rank;
int size;
 
bool Kalman_FilterApplied;
bool FFT_FilterApplied;
bool BlackScholes_Applied;
bool savitzkyGoly_Transform;
bool benchmarkState;

std::string inputDataPath;
std::string outputDataPath;
std::string data_workspace;

std::vector<std::string> Files; 
std::vector<std::string> ProcSignalFiles; 
readWriteTool<I,I> tool;

 std::vector<size_t> numDataProc;
 std::vector<size_t> offsetVec;
 
 size_t numFile;
 std::vector<signalContainer> signal;
 std::vector<indexDateContainer> indexDateVec;
 std::vector<Index2SensorName> vecSignalIndex2SencsorIndex;
 
};

//////////////////////////////////
/////////////IMPLEMENTATION
///////////////////////////////////

template<typename I>
IDEKO_DATA<I>::IDEKO_DATA(int _rank, int _size)
:rank(_rank)
,size(_size)
{}

template<typename I>
void
IDEKO_DATA<I>::Data_IO_Path
               ( std::string const & _inputDataPath)
{
  inputDataPath  = _inputDataPath;
}

template<typename I>
void
IDEKO_DATA<I>::Data_WorkSpace( std::string const & _data_workspace){
  data_workspace = _data_workspace;
  
  if(sizeof(I) == 8){
    outputDataPath = data_workspace + "/Input_Data/Double_Data_Type/signalFilter";
  }
  if(sizeof(I) == 4){
    outputDataPath = data_workspace + "/Input_Data/Float_Data_Type/signalFilter";
  }
}

template<typename I>
void
IDEKO_DATA<I>::useCaseKernelApply(int const IDEKO, int const InBestMe){

     if(IDEKO){
      std::cout << "The IDEKO Data have been read " << std::endl;
      readCSVFilesIDEKO();
     }

     if(InBestMe){
      std::cout << "The INBestMe Data have been read " << std::endl;
      readCSVFilesInBestMe();    
     }

}


template<typename I>
void
IDEKO_DATA<I>::readCSVFilesIDEKO(){
  
   std::string fileline;
   std::string elm;
   std::ifstream InputData;
   std::vector<I> buffer;
   InputData.open(inputDataPath);
   while(InputData)
   {  
     getline(InputData,fileline);
     std::istringstream ss(fileline);
     size_t elmId(0); I val(0); 
     double time; int signalId(0);
     while(getline(ss, elm,';') ){
           I SPXelm = std::atof(elm.c_str());
           signal.push_back(signalContainer(elm,signalId));
           signalId++;
       }
   }

    auto it = max_element(signal.begin (), signal.end(),
                            []( const signalContainer & a, const signalContainer & b)
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
       writeBinaryFile(signalBuffer, signal_Id , sensor_name);
       vecSignalIndex2SencsorIndex.push_back(Index2SensorName(signal_Id,sensor_name));
    }
    
    writeTransformationIndex(vecSignalIndex2SencsorIndex);

}

template<typename I>
void
IDEKO_DATA<I>::writeBinaryFile(std::vector<I> const & data, int ID, std::string const sensorName)
 { 
    bool write2BinFile(true);  
    bool write2TextFile(true);  

    if(write2BinFile){
       std::string signalPath = outputDataPath + "/signal_";
       signalPath = signalPath + std::to_string(ID);
       tool.writeBuffer2Binary( data, signalPath);
     }
     if(write2TextFile){
        std::string signalPath = data_workspace+"/Input_Data/Double_Data_Type/signalFilter_plot/signal_";
        signalPath = signalPath + std::to_string(ID);
        tool.writeBuffer2Text( data, signalPath);
     }
    
    
 }

template<typename I>
void
IDEKO_DATA<I>::Disply(std::vector<I> const & data){
    std::cout << "this is tere  "<< data.size() << std::endl;
    for(int i(0); i<data.size(); i++){
        std::cout<< rank << "  " <<i<<"  " << data[i] << std::endl;
    }
}

template<typename I>
void
IDEKO_DATA<I>::writeTransformationIndex(std::vector<Index2SensorName> const & data){
  
    std::string signalPath = data_workspace+ "/Output_Data/Scalability/signalTransform.txt";
     char filename[ signalPath.size() + 1];
     strcpy(filename, signalPath.c_str());
     std::ofstream outputPath;
     outputPath.open(filename);
   
     for(int i(0);i<data.size(); i++){
        outputPath <<data[i].signal_Id<< "  "<<data[i].sensor_name << std::endl;
        // std::cout <<data[i].id<< " - " <<  data[i].sensor_Id << "  " << sensor_type[i] << std::endl;
        }
   
}

template<typename I>
void
IDEKO_DATA<I>::applyingFilters
    ( bool _Kalman_FilterApplied
    , bool _FFT_FilterApplied
    , bool _BlackScholes_Applied
    , bool _savitzkyGoly_Transform )
{
   Kalman_FilterApplied    = _Kalman_FilterApplied;
   FFT_FilterApplied       = _FFT_FilterApplied;
   BlackScholes_Applied = _BlackScholes_Applied; 
   savitzkyGoly_Transform  = _savitzkyGoly_Transform;
}


template<typename I>
void
IDEKO_DATA<I>::convertBackToCSVFormate(){

   std::string output_Filter_Path; std::string filtername;
   if(Kalman_FilterApplied){
    output_Filter_Path = data_workspace +"/Output_Data/kalmanFilter";
    filtername = "Kalman_Filter";
   }
   if(BlackScholes_Applied ){
    output_Filter_Path = data_workspace +"/Output_Data/BlackScholes";
    filtername = "BlackScholes";
   }
  if(FFT_FilterApplied ){
    output_Filter_Path = data_workspace + "/Output_Data/FFTFilter";
    filtername = "FFT_Filter";
  }
  if(savitzkyGoly_Transform){
    output_Filter_Path = data_workspace +"/Output_Data/SavitzkeyGolay";
    filtername = "SavitzkeyGolay";
  }
  
  
  //search and read all the files in the output directory
  std::vector<std::string> outputFiles ( tool.searchFileInDirectory(output_Filter_Path) );

  //packing up all the binary into cotainer
  
  std::vector<typename::signalContainer<I>>  outputSignalContainerVec (packupBinary2Container(outputFiles, output_Filter_Path));
   
   //read the transfer index to sensor name file
  std::vector<Index2SensorName> Index2sensorId(readTransfIndex());

  
  std::string cluster = data_workspace+"/Output_Data/CSVFormate/"+filtername+"_output.csv";
  char Cluster[ cluster.size() + 1];
  strcpy(Cluster, cluster.c_str());
  std::ofstream cluster_output;
  cluster_output.open(Cluster);
  
  //generating the csv file for the output 
  for(int i(0); i <outputSignalContainerVec.size(); i++){
       cluster_output << Index2sensorId[i].sensor_name<< ";";
     }
  cluster_output << std::endl;
  for(int index(0); index <outputSignalContainerVec[0].signalVal.size(); index++){
  
    for(int i(0); i <outputSignalContainerVec.size(); i++){
       std::vector<I> const & outputSignal (outputSignalContainerVec[i].signalVal);
       cluster_output << outputSignal[index] <<";" ; 
       std::cout << outputSignal[index] << std::endl;
    }
    cluster_output << std::endl;
  }


}

template<typename I>
std::vector<typename::signalContainer<I>> 
IDEKO_DATA<I>::packupBinary2Container(std::vector<std::string> const & outputFiles
                                     ,std::string const & output_Filter_Path ){

   std::vector<typename::signalContainer<I>>  outputSignalContainerVec; 
   for(int index(0); index < outputFiles.size(); index++){
      std::string filename = "signal_filter_" + std::to_string(index);
      int ClassID(-1);
      for(int i(0); i<outputFiles.size(); i++){
        if(outputFiles[i] == filename ){
          std::string filePath  = output_Filter_Path + "/" + filename;  
          //std::vector<I> const & signalBuffer (readFiles(filePath));
          std::vector<I> const & signalBuffer (tool.readBinary(filePath));
          typename::signalContainer<I> signal(index,signalBuffer,ClassID); 
          outputSignalContainerVec.push_back(signal); 
         }
      }
   }

  return outputSignalContainerVec;
}
   

template<typename I>
std::vector<typename::IDEKO_DATA<I>::Index2SensorName>
IDEKO_DATA<I>::readTransfIndex(){
  
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

template<typename I>
void
IDEKO_DATA<I>::readCSVFilesInBestMe(){
  
   std::string fileline;
   std::string elm;
   std::ifstream InputData;
   std::vector<I> buffer;
   InputData.open(inputDataPath);
   while(InputData)
   {  
     getline(InputData,fileline);
     std::istringstream ss(fileline);
     size_t elmId(0); I val(0); 
     double time; int signalId(0);
     while(getline(ss, elm,',') ){
           signal.push_back(signalContainer(elm,signalId));
           signalId++;
       }
   }
   
 
   auto it = max_element(signal.begin (), signal.end(),
                            []( const signalContainer & a, const signalContainer & b)
                               {return a.id < b.id ;}
                          );

  int numSignal = it->id+1; 
  for(int signal_Id(0); signal_Id<numSignal/2; signal_Id++ ){
     
       std::vector<assetContainer> assetBuffer; int iter(0); 
       for(int i(0); i<signal.size(); i++ ){
          iter = i;
          if( (signal[i].id == 2*signal_Id) )
          {            
               std::string  Date = signal[iter++].value.c_str();
               std::string  Value = signal[iter++].value.c_str();
               assetBuffer.push_back(assetContainer(Date,Value)); 
          }
       }
        writeAssetBuffer(assetBuffer, signal_Id); 
   }
  
  writeTransformationIndex(vecSignalIndex2SencsorIndex);
  writeDateIndex(indexDateVec);

}

template<typename I>
void
IDEKO_DATA<I>::writeAssetBuffer(std::vector<assetContainer> const & assetBuffer, int signal_Id){

 std::string signalPath = data_workspace+"/Input_Data/Double_Data_Type/signalFilter_plot/signal_";
 signalPath = signalPath + std::to_string(signal_Id);
          
 std::string path = signalPath;
 char Path[ path.size() + 1];
 strcpy(Path, path.c_str());
 std::ofstream cluster_output;
 cluster_output.open(Path);
 std::vector<I> buffer;
 for(int i(0); i<assetBuffer.size(); i++){

    I value = std::atof(assetBuffer[i].assetVal.c_str());
   
    if(i==0){
      std::string assetName = assetBuffer[i].date.c_str();
      vecSignalIndex2SencsorIndex.push_back(Index2SensorName(signal_Id, assetName ));
    }
    if(i>2){
      cluster_output << i-3 << " "<< assetBuffer[i].date << " " << value << std::endl; 
 
      buffer.push_back(value);
    }
   
   } 
  
  indexDateVec.push_back(indexDateContainer(signal_Id, assetBuffer[assetBuffer.size()-1].date));

  std::string asssetPath = outputDataPath + "/signal_";
  asssetPath = asssetPath + std::to_string(signal_Id);
  tool.writeBuffer2Binary( buffer, asssetPath);

}

template<typename I>
void
IDEKO_DATA<I>::writeDateIndex(std::vector<indexDateContainer> const & data){

    std::string signalPath = data_workspace+ "/Output_Data/Scalability/BlackScholesLastDate.txt";
     char filename[ signalPath.size() + 1];
     strcpy(filename, signalPath.c_str());
     std::ofstream outputPath;
     outputPath.open(filename);
   
     for(int i(0);i<data.size(); i++){
        outputPath <<data[i].signalIndex<< "  "<<data[i].date << std::endl;
        // std::cout <<data[i].id<< " - " <<  data[i].sensor_Id << "  " << sensor_type[i] << std::endl;
        }
   
}


#endif