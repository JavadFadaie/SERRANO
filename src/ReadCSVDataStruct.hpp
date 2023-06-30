#ifndef READ_CSV_STRUCT_HPP
#define READ_CSV_STRUCT_HPP

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <mpi.h>
#include "kernelFlops.hpp"
#include <cmath>
#include <cstring>
template<typename I>
class readCSVStruct{

public:

readCSVStruct(int _id, int _rank, int _size);

void readCSV();

void amplituteData(int nth);

void cutData2PowerTwo();

void generatePowerTwo(int power2);

void Data_IO_Path
    ( std::string _originalDataPath
    , std::string _inputDataDouble
    , std::string _inputDataFloat);

void writingBinaryFile(std::vector<I> const & data);


protected:
 
int id;
int rank;
int size;
size_t counter;
std::vector<I> buffer;
std::string   originalDataPath;
std::string   inputDataDouble;
std::string   inputDataFloat;
};

////////////////////////////////////////
////// IMPLEMENTATION
/////////////////////////////////////////

template<typename I>
readCSVStruct<I>::readCSVStruct(int _id, int _rank, int _size)
:id(_id)
,rank(_rank)
,size(_size)
,counter(0)
{}


template<typename I>
void
readCSVStruct<I>::Data_IO_Path
    ( std::string _originalDataPath
    , std::string _inputDataDouble
    , std::string _inputDataFloat)
{
  originalDataPath   = _originalDataPath;
  inputDataDouble    = _inputDataDouble;
  inputDataFloat     = _inputDataFloat; 
}

template<typename I>
void 
readCSVStruct<I>::readCSV(){

std::string fileline;
std::string elm;
std::ifstream InputData;

InputData.open(originalDataPath);

while(InputData)
{

  getline(InputData,fileline);
  std::istringstream ss(fileline);
  size_t elmId(0); I val(0); 
  
  while(getline(ss, elm,',') ){
      I SPXelm = std::atof(elm.c_str());
      if(elmId == id){
         if(SPXelm !=0 ){
            val = SPXelm;
            buffer.push_back(val);
            counter++;
           }
      }
      elmId++;
   }
 }

int numberToMakeFiveHounderedKiloByte(11300);
for(int i(0); i<numberToMakeFiveHounderedKiloByte; i++)
    buffer.push_back(buffer[i]);

counter = buffer.size();

}

template<typename I>
void
readCSVStruct<I>::amplituteData( int nth){
 
 std::vector<I> bigBuffer;
 for(size_t i(0); i<nth; i++){
    for(size_t j(0); j<counter;j++){
       bigBuffer.push_back(buffer[j]);
    }
 }

 writingBinaryFile(bigBuffer);

}



template<typename I>
void
readCSVStruct<I>::cutData2PowerTwo(){
   std::string dataPath;
   if(sizeof(I) == 8){
      dataPath = inputDataDouble;
   }
   if(sizeof(I) == 4){
      dataPath = inputDataFloat;
   } 
   
   FILE *p_file;
   p_file = fopen(dataPath.c_str(),"rb");
   fseek(p_file,0,SEEK_END);
   size_t sizefile = ftell(p_file);
   size_t N= sizefile/sizeof(I);
   fclose(p_file);

   bool isPowtwo(true);
   while(true)
   {
      size_t res (N%2);
      if(res != 0){
         isPowtwo = false;
         break;
      }

      N=N/2;
      if(N==1){
          break;
       }
   }
  
   if(isPowtwo == false)
   {
      int i(1); 
      while(true)
      {

      int pow2N = pow(2,i);
     
        if(pow2N > N){
           break;
         }
         i++;
      }
    
      size_t nearPow2(pow(2,i-1)); 
      char _dataPath[dataPath.size() + 1];
      strcpy(_dataPath, dataPath.c_str());

       //load up the buffer with the nearest power two
      FILE* pFileSeq;
      pFileSeq = fopen(_dataPath, "rb"); 
      std::vector<I> bufferPowTwo(nearPow2);
      fread(& bufferPowTwo[0], bufferPowTwo.size(), sizeof(I), pFileSeq);
      fclose(pFileSeq);

        //writing the buffer to the file 
      writingBinaryFile(bufferPowTwo);
   }

}

template<typename I>
void
readCSVStruct<I>::generatePowerTwo(int power2){

 size_t nearPow2(pow(2,power2)); 
 std::vector<I> bufferPowTwo(nearPow2);
 
 for(size_t iter(0); iter<bufferPowTwo.size(); iter++){
    size_t i = iter%counter ;
    bufferPowTwo[iter] = buffer[i];
 }
 
 writingBinaryFile(bufferPowTwo);

   
}



template<typename I>
void
readCSVStruct<I>::writingBinaryFile(std::vector<I> const & data){

std::string dataPath;
if(sizeof(I) == 8){
   dataPath = inputDataDouble;
}
if(sizeof(I) == 4){
   dataPath = inputDataFloat;
}

char outputData[dataPath.size() + 1];
strcpy(outputData, dataPath.c_str()); 

FILE* filename;
filename = fopen(outputData, "wb");
 
fwrite(& data[0], data.size(), sizeof(I), filename);
fclose(filename); 

}



#endif
