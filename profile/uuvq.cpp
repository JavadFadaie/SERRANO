#include<iostream>
#include<vector>
#include <fstream>
#include <sstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <iomanip>
#include <filesystem>
#include <dirent.h>
#include <sys/types.h>
#include <cstring> 


struct elapsedInfo{
    elapsedInfo
       ( double _iPart
       , double _numproc
       , double _s
       , double _epsilon
       , double _error
       , double _elapsed
       , double _energy
       , std::string _phase
       , std::string _inputPercision
       , std::string _outputPercision)
       :ipart(_iPart)
       ,numproc(_numproc)
       ,s(_s)
       ,epsilon(_epsilon)
       ,error(_error)
       ,elapsed(_elapsed)
       ,energy(_energy)
       ,phase(_phase)
       ,inputPercision(_inputPercision)
       ,outputPercision(_outputPercision)
       {}  
   double ipart;
   double numproc;
   double s;
   double epsilon;
   double error;
   double elapsed;
   double energy;
   std::string   phase;
   std::string   inputPercision;
   std::string   outputPercision;
};


//////////Compute the Elapsed information tools/////////////////
void 
searchFileInDirectory( std::string const & path
                     , std::vector<std::string> & filenameElapsedAll);

std::vector<elapsedInfo> const 
computeElapsedTime(std::vector<std::string> const & filename);

std::vector<elapsedInfo>    
ExtractWithDataPercision(std::vector<elapsedInfo> const & ElapsedDataInfo
                        ,std::string const & inputDataPercision
                        ,std::string const & outputDataPercision );

std::vector<std::string> const 
readingElapsedDataTool(std::vector<std::string> const & filenameElapsed);

std::vector<elapsedInfo> const 
packingElapsedData(std::vector<std::string> const & data);

std::vector<elapsedInfo> const
computeTotalElapsed(std::vector<elapsedInfo> const & data);


////////compute the total Elapsed and Energy tools/////////
std::vector<elapsedInfo> const
computeTotalElapsedEnergy(std::vector<elapsedInfo> const & data);


///////////////Display the information//////////////////////////////
void ElapsedDisplay( std::vector<elapsedInfo> const & ElapsedDataInfo);

void displayInfoUser( auto it
                    , std::string const & kernel);

void displayDetailUser( std::vector<elapsedInfo> const & EnergyDataInfo
                      , auto it
                      , std::string const & kernel);

void findMinimumElapsedEnergy( std::vector<elapsedInfo> const & TelapsedEnergyData
                             , std::vector<elapsedInfo> const & elapsedEnergyData
                             , int const approximation_techniques
                             , int const applied_error_range
                             , std::string const & kernel);

void findMinimumElapsedEnergy( std::vector<elapsedInfo> const & TelapsedEnergyData
                             , std::vector<elapsedInfo> const & elapsedEnergyData
                             , int const applied_error_range
                             , std::string const & kernel);

//////////////////Extract Data with conditioned//////////////////////////
std::vector<elapsedInfo> const elpasedEnergyWithoutApproxnTech
                            ( std::vector<elapsedInfo> const & elapsedEnergyData
                            , std::string const & kernel);

std::vector<elapsedInfo> const
ExtractDataWithErrorRange(std::vector<elapsedInfo> const & TelapsedEnergyData
                         , std::vector<elapsedInfo> const & elapsedEnergyData
                         , double const error_offset
                         , double const error_endset);



void
findErrorRange( std::vector<elapsedInfo> const & TelapsedEnergyData
              , std::string const & kernel);



void
computeScalability( std::vector<elapsedInfo> const & elapsedData
                  , std::string const & kernel);              


/////////////////////////////////////////////
///////////////////////IMPLEMENTATION 
///////////////////////////////////////////


int main(int argc, char * argv[]){
    
    std::string kernel   = argv[1];
    std::string dataSize = argv[2];
    std::string inputDataPercision = argv[3];
    std::string outputDataPercision = argv[4]; 
    int approximation_techniques = atoi(argv[5]);
    std::string errorOffset = (argv[6]);
    std::string errorEndset = (argv[7]);
    int applied_error_range = atoi(argv[8]);
    std::string ScalabilityCriteria = (argv[9]);
    int ExessClusterState   = atoi(argv[10]);
    
    double DataSize= std::atof(dataSize.c_str());
    double error_offset = std::atof(errorOffset.c_str());
    double error_endset = std::atof(errorEndset.c_str());
    
     std::string dirPath;
    if(ExessClusterState){
      dirPath = "/nfs_home/hpcjfada/SERRANO/serrano/profile/";
    }else{	    
      dirPath = "/home/javad/Desktop/Serrano/serrano/profile/";
    }
    std::vector<std::string> filenameElapsed; std::string filenameEnergy;  
    
    
    if( kernel == "KalmanFilter"){
        std::cout << "                                        Kalman Filter kernel " << std::endl;
        dirPath += "KalmanFilter/";
        searchFileInDirectory(dirPath, filenameElapsed);
    }
    else if( kernel == "MinMaxUpdate" ){
        std::cout << "Min Max Update kernel " << std::endl;
        dirPath += "MinMaxUpdate/";
        searchFileInDirectory(dirPath, filenameElapsed);
    }
    else if( kernel == "FFTFilter" ){
        std::cout << "FFT Filter kernel " << std::endl;
        dirPath += "FFTFilter/";
        searchFileInDirectory(dirPath, filenameElapsed);
   
    }
    else if( kernel == "SavitzkyGolyTransform" ){
        std::cout << "Savitzky Goly Transform kernel " << std::endl;
        dirPath += "SavitzkyGolyTransform/";
        searchFileInDirectory(dirPath, filenameElapsed);
    }
    else if( kernel == "KNN" ){
        std::cout << "KNN classification kernel " << std::endl;
        dirPath += "KNN/";
        searchFileInDirectory(dirPath, filenameElapsed);
    }
    else if( kernel == "KMean" ){
        std::cout << "KMean classification kernel " << std::endl;
        dirPath += "KMean/";
        searchFileInDirectory(dirPath, filenameElapsed);
    }
           
                //Reading all the generated filename into one buffer
   
    //ElapsedEnergyDisplay(elapsedEnergyDataInfo);
    //ElapsedEnergyDisplay(TelapsedEnergyDataInfo);
      
        
             //Compute the Elapsed time from the files 
    std::vector<elapsedInfo> const & ElapsedInitDataInfo  
                (computeElapsedTime(filenameElapsed));
    //ElapsedDisplay(ElapsedInitDataInfo);
    
    std::vector<elapsedInfo> const & ElapsedDataInfo  
                (ExtractWithDataPercision
                         ( ElapsedInitDataInfo
                         , inputDataPercision
                         , outputDataPercision)
                );
   // ElapsedDisplay(ElapsedDataInfo);
     if(ScalabilityCriteria == "false"){
        computeScalability(ElapsedDataInfo,kernel);
     }
       
             //Compute the total Elapsed and Energy data info for step
     std::vector<elapsedInfo> const & TelapsedEnergyDataInfo
                (computeTotalElapsedEnergy(ElapsedDataInfo) );
     //ElapsedDisplay(TelapsedEnergyDataInfo);
    
    if(approximation_techniques == 1)
    {   
       findMinimumElapsedEnergy( TelapsedEnergyDataInfo
                               , ElapsedDataInfo
                               , approximation_techniques
                               , applied_error_range
                               , kernel );
    }
    if(approximation_techniques == 0)
    {
        std::vector<elapsedInfo> const & TelapsedEnergyStrideOne
        ( elpasedEnergyWithoutApproxnTech(TelapsedEnergyDataInfo,kernel) ); 
    
        findMinimumElapsedEnergy( TelapsedEnergyStrideOne
                                , ElapsedDataInfo
                                , approximation_techniques
                                , applied_error_range
                                , kernel );
    }   
   
    
    if( applied_error_range )
     {     
       std::vector<elapsedInfo> extractedElapsedEnergy 
       (ExtractDataWithErrorRange
                              ( TelapsedEnergyDataInfo
                              , ElapsedDataInfo
                              , error_offset
                              , error_endset));

      findMinimumElapsedEnergy( extractedElapsedEnergy
                              , ElapsedDataInfo
                              , applied_error_range
                              , kernel );
     }
     
  return 0;
 }

void
searchFileInDirectory( std::string const & dirPath
                     , std::vector<std::string> & filenameElapsedAll){
      
    
  char DirPath[ dirPath.size() + 1];
  strcpy(DirPath, dirPath.c_str()); 
  if (auto dir = opendir(DirPath)) {
  while (auto f = readdir(dir)) {
        if (!f->d_name || f->d_name[0] == '.')
          continue; // Skip everything that starts with a dot
          std::string filesInsideDir = f->d_name;
          std::string fullPath = dirPath + filesInsideDir;
         // std::cout << fullPath << std::endl;
          filenameElapsedAll.push_back(fullPath);
     }
    closedir(dir);
   }


}

std::vector<elapsedInfo> const 
computeElapsedTime(std::vector<std::string> const & filename){

       // Reading the data from the desired file 
    std::vector<std::string>  initiElapsedData ( readingElapsedDataTool(filename) );

       // Packign the data into elapsed time
   return packingElapsedData(initiElapsedData);
}

std::vector<std::string> const 
readingElapsedDataTool(std::vector<std::string> const & filenameElapsed){

 
 std::vector<std::string> dataElapsed;
 for(int i(0); i<filenameElapsed.size(); i++){
 std::string fileline;
 std::string elm;
 std::ifstream InputData;
 InputData.open(filenameElapsed[i]);
 while(InputData)
 {
   getline(InputData,fileline);
   std::istringstream ss(fileline);
  
   while(getline(ss, elm,';') ){
       dataElapsed.push_back(elm);
    }
 }
 }
  return dataElapsed;
}


std::vector<elapsedInfo> const 
packingElapsedData(std::vector<std::string> const & dataElapsed){

 std::vector<elapsedInfo> profileData;
 for(int i(0); i<dataElapsed.size();){
     int j = i;
     double ipart     = std::atof(dataElapsed[j++].c_str());
     double size      = std::atof(dataElapsed[j++].c_str());
     double s         = std::atof(dataElapsed[j++].c_str());
     double epsilon   = std::atof(dataElapsed[j++].c_str());
     double error     = std::atof(dataElapsed[j++].c_str());
     double elapsed   = std::atof(dataElapsed[j++].c_str());
     double energy    = std::atof(dataElapsed[j++].c_str());
  
     std::string name = dataElapsed[j++];
     std::string inputPercision = dataElapsed[j++];
     std::string outputPercision = dataElapsed[j++];
     profileData.push_back(elapsedInfo(ipart,size,s,epsilon,error,elapsed,energy,name,inputPercision,outputPercision));
     i = j;
 }

 return profileData;

}


std::vector<elapsedInfo>    
ExtractWithDataPercision(std::vector<elapsedInfo> const & ElapsedDataInfo
                        ,std::string const & inputDataPercision
                        ,std::string const & outputDataPercision)
{

   std::vector<elapsedInfo> profileData;
   for(int i(0); i<ElapsedDataInfo.size(); i++){
      
      if( (inputDataPercision == "NULL") && (outputDataPercision == "NULL")  ){
         
               profileData.push_back(elapsedInfo(ElapsedDataInfo[i].ipart
                                          ,ElapsedDataInfo[i].numproc
                                          ,ElapsedDataInfo[i].s
                                          ,ElapsedDataInfo[i].epsilon
                                          ,ElapsedDataInfo[i].error
                                          ,ElapsedDataInfo[i].elapsed
                                          ,ElapsedDataInfo[i].energy
                                          ,ElapsedDataInfo[i].phase
                                          ,ElapsedDataInfo[i].inputPercision
                                          ,ElapsedDataInfo[i].outputPercision)
                                 ); 
                                 
        }
       else{
       
           if( (ElapsedDataInfo[i].inputPercision == inputDataPercision)
             &&(ElapsedDataInfo[i].outputPercision == outputDataPercision) ){

               profileData.push_back(elapsedInfo(ElapsedDataInfo[i].ipart
                                          ,ElapsedDataInfo[i].numproc
                                          ,ElapsedDataInfo[i].s
                                          ,ElapsedDataInfo[i].epsilon
                                          ,ElapsedDataInfo[i].error
                                          ,ElapsedDataInfo[i].elapsed
                                          ,ElapsedDataInfo[i].energy
                                          ,ElapsedDataInfo[i].phase
                                          ,ElapsedDataInfo[i].inputPercision
                                          ,ElapsedDataInfo[i].outputPercision)
                                 ); 
             }
            
            if( (ElapsedDataInfo[i].inputPercision == inputDataPercision)
                && (outputDataPercision == "NULL") ){

               profileData.push_back(elapsedInfo(ElapsedDataInfo[i].ipart
                                          ,ElapsedDataInfo[i].numproc 
                                          ,ElapsedDataInfo[i].s
                                          ,ElapsedDataInfo[i].epsilon
                                          ,ElapsedDataInfo[i].error
                                          ,ElapsedDataInfo[i].elapsed
                                          ,ElapsedDataInfo[i].energy
                                          ,ElapsedDataInfo[i].phase
                                          ,ElapsedDataInfo[i].inputPercision
                                          ,ElapsedDataInfo[i].outputPercision)
                                 ); 
             }

            if( (ElapsedDataInfo[i].outputPercision == outputDataPercision)
                && (inputDataPercision == "NULL") ){

               profileData.push_back(elapsedInfo(ElapsedDataInfo[i].ipart
                                          ,ElapsedDataInfo[i].numproc
                                          ,ElapsedDataInfo[i].s
                                          ,ElapsedDataInfo[i].epsilon
                                          ,ElapsedDataInfo[i].error
                                          ,ElapsedDataInfo[i].elapsed
                                          ,ElapsedDataInfo[i].energy
                                          ,ElapsedDataInfo[i].phase
                                          ,ElapsedDataInfo[i].inputPercision
                                          ,ElapsedDataInfo[i].outputPercision)
                                 ); 
             } 

        } 
     }
 
   return profileData;

}


void findMinimumElapsedEnergy( std::vector<elapsedInfo> const & TelapsedEnergyData
                             , std::vector<elapsedInfo> const & elapsedEnergyData
                             , int const approximation_techniques
                             , int const applied_error_range
                             , std::string const & kernel)
 {
    
    auto it = max_element(TelapsedEnergyData.begin (), TelapsedEnergyData.end(),
                            []( const elapsedInfo & a, const elapsedInfo & b)
                               {return a.elapsed > b.elapsed ;}
                          );
     
    if( approximation_techniques == 1 )
    {    
         displayInfoUser(it,kernel);
         findErrorRange(TelapsedEnergyData, kernel);
         displayDetailUser(elapsedEnergyData, it,kernel);
    }
    if( approximation_techniques == 0 ){
         displayInfoUser(it,kernel);
         displayDetailUser(elapsedEnergyData, it,kernel);
    }
} 

void findMinimumElapsedEnergy( std::vector<elapsedInfo> const & TelapsedEnergyData
                             , std::vector<elapsedInfo> const & elapsedEnergyData
                             , int const applied_error_range
                             , std::string const & kernel)
 {
    
    auto it = max_element(TelapsedEnergyData.begin (), TelapsedEnergyData.end(),
                            []( const elapsedInfo & a, const elapsedInfo & b)
                               {return a.elapsed > b.elapsed ;}
                          );
     
    displayInfoUser(it, kernel);
    displayDetailUser(elapsedEnergyData, it, kernel);
} 



std::vector<elapsedInfo> const
elpasedEnergyWithoutApproxnTech
          (std::vector<elapsedInfo> const & elapsedEnergyData
          , std::string const & kernel){

  std::vector<elapsedInfo> dataWithoutApprox;
  for(int i(0); i<elapsedEnergyData.size();i++){
       
     if(kernel == "KMean"){
        if(elapsedEnergyData[i].epsilon == 0.001){
          dataWithoutApprox.push_back(elapsedInfo
                                     ( elapsedEnergyData[i].ipart
                                     , elapsedEnergyData[i].numproc
                                     , elapsedEnergyData[i].s
                                     , elapsedEnergyData[i].epsilon
                                     , elapsedEnergyData[i].error
                                     , elapsedEnergyData[i].elapsed
                                     , elapsedEnergyData[i].energy
                                     , elapsedEnergyData[i].phase
                                     , elapsedEnergyData[i].inputPercision
                                     , elapsedEnergyData[i].outputPercision)
                                     );
        }
     }
     else{
       if(elapsedEnergyData[i].s == 1){
          dataWithoutApprox.push_back(elapsedInfo
                                     ( elapsedEnergyData[i].ipart
                                     , elapsedEnergyData[i].numproc
                                     , elapsedEnergyData[i].s
                                     , elapsedEnergyData[i].epsilon
                                     , elapsedEnergyData[i].error
                                     , elapsedEnergyData[i].elapsed
                                     , elapsedEnergyData[i].energy
                                     , elapsedEnergyData[i].phase
                                     , elapsedEnergyData[i].inputPercision
                                     , elapsedEnergyData[i].outputPercision)
                                     );
        }
     } 
  }

  return dataWithoutApprox;
}


void
computeScalability( std::vector<elapsedInfo> const & elapsedData
                  , std::string const & kernel){
  
  double elpasedProc1(0); double elpasedProcN(0);
  double energyProc1(0); double energyProcN(0);

  if(kernel == "KMean"){
       for(int i(0); i<elapsedData.size();i++){
  
         if( (elapsedData[i].numproc == 1) && (elapsedData[i].epsilon == 0.001) && (elapsedData[i].ipart == 1) ){
           elpasedProc1 = elapsedData[i].elapsed;
           energyProc1 = elapsedData[i].energy;
          }
         if( (elapsedData[i].numproc == 8) && (elapsedData[i].epsilon == 0.001) && (elapsedData[i].ipart == 1) ){

           elpasedProcN = elapsedData[i].elapsed;
           energyProcN = elapsedData[i].energy;
       }
      }
  }  
  else{
     for(int i(0); i<elapsedData.size();i++){
         if( (elapsedData[i].numproc == 1) && (elapsedData[i].s == 1) && (elapsedData[i].ipart == 1) ){
           elpasedProc1 = elapsedData[i].elapsed;
           energyProc1 = elapsedData[i].energy;
         }
         if( (elapsedData[i].numproc == 8) && (elapsedData[i].s == 1) && (elapsedData[i].ipart == 1) ){
           elpasedProcN = elapsedData[i].elapsed;
           energyProcN = elapsedData[i].energy;
         }
      }
 
  }
  /*
  std::cout << elpasedProc1<< "  " << energyProc1 << std::endl;
   std::cout << elpasedProcN<< "  " << energyProcN << std::endl;
 
  std::cout << "This is the elapsed sppedup  " << elpasedProc1/elpasedProcN << std::endl;
  std::cout << "This is the energy sppedup  " << energyProc1/energyProcN << std::endl;
  */
}

void
findErrorRange( std::vector<elapsedInfo> const & TelapsedEnergyData
              , std::string const & kernel)
{
    
    auto minmax = std::minmax_element(TelapsedEnergyData.begin(), TelapsedEnergyData.end()
                                    , [](elapsedInfo const & lhs, elapsedInfo const & rhs )
                                      {
                                         return lhs.error < rhs.error;
                                      }
                                      );
     if(kernel == "KMean"){
     std::cout << "\n\n\n";                              
     std::cout << std::setw(53) << "Loop iteration variation" << std::endl;
     std::cout << "                     <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << std::endl;                              
     std::cout << std::setw(40) << "minimum iteration"  << std::setw(20) << "maximum iteration" << std::endl;
     std::cout << std::setw(30) << (minmax.first->s) << std::setw(18)<<  (minmax.second->s)<< std::endl;
     std::cout << "\n\n";                              
     std::cout << std::setw(60) << "Control loop parameter variation" << std::endl;
     std::cout << "                      <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << std::endl;                              
     std::cout << std::setw(40) << "minimum epsilon"  << std::setw(20) << "maximum epsilon" << std::endl;
     std::cout << std::setw(30) << (minmax.first->epsilon) << std::setw(18)<<  (minmax.second->epsilon)<< std::endl;
     
     }else{    
     std::cout << "\n\n\n";                              
     std::cout << std::setw(50) << "Error variation" << std::endl;
     std::cout << "                       <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << std::endl;                              
     std::cout << std::setw(40) << "minimum error"  << std::setw(20) << "maximum error" << std::endl;
     std::cout << std::setw(38) << (minmax.first->error) << std::setw(18)<<  (minmax.second->error) << std::endl;
     }
    
}


std::vector<elapsedInfo> const
ExtractDataWithErrorRange(std::vector<elapsedInfo> const & TelapsedEnergyData
                         , std::vector<elapsedInfo> const & elapsedEnergyData
                         , double const error_offset
                         , double const error_endset)
{  
    std::vector<elapsedInfo> elapsedEnergyErrorResitrict;
    for(int i(0); i<TelapsedEnergyData.size();i++){   
      
       if(  (TelapsedEnergyData[i].error >= error_offset) 
         && (TelapsedEnergyData[i].error <= error_endset))
         {
            elapsedEnergyErrorResitrict.push_back(elapsedInfo
                                            ( TelapsedEnergyData[i].ipart
                                            , TelapsedEnergyData[i].numproc
                                            , TelapsedEnergyData[i].s
                                            , TelapsedEnergyData[i].epsilon
                                            , TelapsedEnergyData[i].error
                                            , TelapsedEnergyData[i].elapsed
                                            , TelapsedEnergyData[i].energy
                                            , TelapsedEnergyData[i].phase
                                            , elapsedEnergyData[i].inputPercision
                                            , elapsedEnergyData[i].outputPercision)
                                            );
         }     
    }

    if(elapsedEnergyErrorResitrict.size() == 0){
        throw std::runtime_error("The Error range does not include and data " );
    }
   
   return elapsedEnergyErrorResitrict;
 }




std::vector<elapsedInfo> const
computeTotalElapsedEnergy(std::vector<elapsedInfo> const & data){
  
    std::vector<elapsedInfo> profileData;
    for(int i(0); i<data.size();){
       int j = i; 
       double Tenergyfreq1(0); 
       double Telapsed;
    
       double RenergyFreq1   = data[j].energy;
       double Relapsed       = data[j].elapsed;  

       double ipart          = data[j].ipart;
       double s              = data[j].s;
       double epsilon        = data[j].epsilon;
       double numproc        = data[j].numproc;
       double error          = data[j].error;

       std::string inputPercision = data[j].inputPercision;
       std::string outputPercision = data[j].outputPercision;

       double KenergyFreq1   = data[j++].energy;
       double Kelapsed       = data[j].elapsed;  
       std::string iphase    = data[j].phase;
       
       double WenergyFreq1   = data[j++].energy;
       double Welapsed       = data[j].elapsed;  
       

       Tenergyfreq1   = RenergyFreq1 + KenergyFreq1 + WenergyFreq1;
       Telapsed = Relapsed     + Kelapsed     + Welapsed;
      // std::cout << Telapsed <<" = " << Relapsed << "  " << Kelapsed << "  " << Welapsed << std::endl;
       
       profileData.push_back(elapsedInfo(ipart, numproc, s, epsilon, error, Telapsed, Tenergyfreq1, iphase, inputPercision, outputPercision ));
       i = j;
    }

  return profileData;
}


void ElapsedDisplay( std::vector<elapsedInfo> const & ElapsedDataInfo)
{
     std::cout << " This is Elpased Data " << ElapsedDataInfo.size() << std::endl;
     std::cout << "ipart"<<std::setw(13) << "num proc"<<std::setw(9)<<"s"<<std::setw(13)<<"epsilon"<<std::setw(13)<< "error "<<std::setw(13)<< "elapsed"<<std::setw(13)<< "energy"<<std::setw(23)<<"input percision"<<std::setw(23)<< "output percision"<< std::endl;
    for(int i(0); i<ElapsedDataInfo.size(); i++){
        std::cout << ElapsedDataInfo[i].ipart<< std::setw(13) << ElapsedDataInfo[i].numproc <<std::setw(13)<< ElapsedDataInfo[i].s<<std::setw(13)<< ElapsedDataInfo[i].epsilon<<std::setw(13)<< 100*ElapsedDataInfo[i].epsilon <<std::setw(13)<< 100*ElapsedDataInfo[i].error<<"%"<<std::setw(17)<< ElapsedDataInfo[i].elapsed<<std::setw(17)<< ElapsedDataInfo[i].energy<<std::setw(17) << ElapsedDataInfo[i].inputPercision<<std::setw(23)<<ElapsedDataInfo[i].outputPercision<<std::endl;
    }
 
}


void displayInfoUser( auto it, std::string const & kernel)
 {  
     std::cout << "\n\n";   
     std::cout << std::setw(50) << "                   Minimum execution time and energy consumption of the kernel" << std::endl;
     std::cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"<< std::endl;                             
     
     if(kernel == "KNN"){
     std::cout << "number of proc" << std::setw(23) <<std::setw(23) << "execuation time/s"<<std::setw(16) << "error"<<std::setw(20)<<"energy/Joule"<<std::setw(27) << "input data percision"<<std::setw(27)<< "output data percision " << std::endl;
     std::cout << std::setw(8) << it->numproc <<std::setw(27) << it->elapsed <<std::setw(19)<< it->error << std::setw(15)<< it->energy <<std::setw(25) << it->inputPercision<< std::setw(27)<< it->outputPercision <<std::endl;
     }
     else if(kernel == "KMean"){
     std::cout << "number of proc" << std::setw(20) << " quality loop"<<std::setw(28) << "execuation time/s"<<std::setw(16) << "Iteration"<<std::setw(20)<<"energy/Joule"<<std::setw(27) << "input data percision"<<std::setw(27)<< "output data percision " << std::endl;
     std::cout << std::setw(8) << it->numproc <<std::setw(20)<< it->epsilon <<std::setw(27) << it->elapsed <<std::setw(19)<< it->s << std::setw(20)<< it->energy <<std::setw(25) << it->inputPercision<< std::setw(27)<< it->outputPercision <<std::endl;
     }
     else{ 
     std::cout << "number of proc" << std::setw(23) << " perforation stride s"<<std::setw(23) << "execuation time/s"<<std::setw(16) << "error"<<std::setw(20)<<"energy/Joule"<<std::setw(27) << "input data percision"<<std::setw(27)<< "output data percision " << std::endl;
     std::cout << std::setw(8) << it->numproc <<std::setw(20)<< it->s <<std::setw(27) << it->elapsed <<std::setw(22)<< it->error << std::setw(20)<< it->energy <<std::setw(25) << it->inputPercision<< std::setw(27)<< it->outputPercision <<std::endl;
     }    
 }


void displayDetailUser(std::vector<elapsedInfo> const & EnergyDataInfo, auto it, std::string const & kernel)
{
    double numproc = it->numproc;
    double s       =    it->s;
    double epsilon =    it->epsilon;
    std::string inputPercision = it->inputPercision;
    std::string outputPercision = it->outputPercision;
    
    std::cout << "\n\n";                              
    std::cout <<"               Detail of execution time and energy consumption of each of the component " << std::endl;
    std::cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"<< std::endl;                             
      
    std::cout << "phase name"<<std::setw(23)<<"execuation time/s"<<std::setw(20)<<"energy/Joule"<<std::setw(32)<<"input data percision"<<std::setw(32)<< "output data percision " << std::endl;
 
    for(int i(0); i<EnergyDataInfo.size(); i++)
    {
        if( (EnergyDataInfo[i].numproc == numproc) && 
            (EnergyDataInfo[i].s == s) &&
            (EnergyDataInfo[i].epsilon == epsilon) && 
            (EnergyDataInfo[i].inputPercision == inputPercision) && 
            (EnergyDataInfo[i].outputPercision == outputPercision) 
          ){
         std::cout <<std::setw(3) << EnergyDataInfo[i].phase <<std::setw(23)<< EnergyDataInfo[i].elapsed  <<std::setw(23)<< EnergyDataInfo[i].energy<<std::setw(29)<< EnergyDataInfo[i].inputPercision <<std::setw(28)<< EnergyDataInfo[i].outputPercision<< std::endl;
           }
    }


}



