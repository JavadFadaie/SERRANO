#ifndef READ_CSV_HPP
#define READ_CSV_HPP

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cstdlib>


template<typename T>
class readCSV{

public:

readCSV();

void readCSV_Seprated();

void BigData(int _nth); 

int numofData();

protected:
int countData;
int rank;
int size;

};

///////////////////////////
//////
/////////////////////////

template<typename T>
readCSV<T>::readCSV()
{}

template<typename T>
void 
readCSV<T>::readCSV_Seprated(){
 
std::string fileline;
std::string elm;
std::ifstream spxData;
spxData.open("../data/SPX.csv");

std::vector<std::string> SPXdate;
std::vector<T> SPXopen;
std::vector<T> SPXhigh;
std::vector<T> SPXlow;
std::vector<T> SPXclose;
std::vector<size_t> SPXvolume;

while(spxData){
  getline(spxData,fileline);
  std::istringstream ss(fileline);
  int elmId(0); 
  while(getline(ss, elm,',') ){
        
        if((elmId == 0)&&( elm != "date" )){
           SPXdate.push_back(elm);
         }
         
        //T SPXelm = std::atof(elm.c_str());
        T SPXelm = std::strtod(elm.c_str(), 0);
        if(elmId == 1){
           if(SPXelm !=0 ){
           SPXopen.push_back(SPXelm);
           }
         }
        if(elmId == 2){
           if(SPXelm !=0 ){
           SPXhigh.push_back(SPXelm);
           }
         }
        if(elmId == 3){
           if(SPXelm !=0 ){
           SPXlow.push_back(SPXelm);
           }
         }
        if(elmId == 4){
           if(SPXelm !=0 ){
           SPXclose.push_back(SPXelm);
           }
         }
        if(elmId == 5){
           if(SPXelm !=0 ){
           SPXvolume.push_back(static_cast<size_t>(SPXelm));
           }
         }      
       elmId++;
   }
 }
   
   // std::cout << countData << "  " << SPXopen.size() << std::endl;
 for(int i(0); i<SPXopen.size(); i++){
      std::cout << i <<"  " << SPXlow[i]<<  std::endl;
   }



 countData=SPXopen.size();  
//Save the open columns of SPX in unformated file   
FILE* pFileOpen;
pFileOpen = fopen("../data/SPX_Low", "wb");
 
fwrite(& SPXlow[0], SPXlow.size(), sizeof(T), pFileOpen);
fclose(pFileOpen); 

/*
//Save the High columns of SPX in unformated file   
FILE* pFileHigh;
pFileHigh = fopen("../data/SPX_High", "wb");
 
fwrite(& SPXhigh[0], SPXhigh.size(), sizeof(T), pFileHigh);
fclose(pFileHigh); 


//Save the Low columns of SPX in unformated file   
FILE* pFileLow;
pFileLow = fopen("../data/SPX_Low", "wb");
 
fwrite(& SPXlow[0], SPXlow.size(), sizeof(T), pFileLow);
fclose(pFileLow); 


//Save the Close columns of SPX in unformated file   
FILE* pFileClose;
pFileClose = fopen("../data/SPX_Close", "wb");
 
fwrite(& SPXclose[0], SPXlow.size(), sizeof(T), pFileClose);
fclose(pFileClose); 

//Save the Volume columns of SPX in unformated file   
FILE* pFileVolume;
pFileVolume = fopen("../data/SPX_Volume", "wb");
 
fwrite(& SPXvolume[0], SPXvolume.size(), sizeof(size_t), pFileVolume);
fclose(pFileVolume); 
*/

/*
FILE* pFilee;
pFilee = fopen("../data/SPX_Open", "rb");

std::vector<T> Buffer(SPXopen.size());
fread(& Buffer[0], SPXopen.size(), sizeof(T), pFilee);


for(int i(0); i<SPXopen.size(); i++){
   std::cout << i <<"  " << Buffer[i] <<" " << SPXopen[i] << std::endl;
}
fclose(pFilee); 
*/
 
}

 

 


template<typename T>
int 
readCSV<T>::numofData(){
   return countData;
}
 




#endif
