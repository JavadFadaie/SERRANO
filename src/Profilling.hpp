#ifndef PROFILLING_HPP
#define PROFILLING_HPP

#include "DistributedTimer.hpp"

template<typename I, typename O>
class ProfileHPC{

  public:
  struct energyData{
     energyData
        ( int _ipart
        , int _numProc
        , I   _power )
      :ipart(_ipart)
      ,numProc(_numProc)
      ,power(_power)
      {}

   int ipart; 
   int numProc;
   I   power;
  };
  
  struct profileData{
   
    profileData
     ( int _ipart
     , int _numproc
     , int _s
     , double _epsilon
     , std::string _partname
     , double _errorAbsMax
     , double _errorAbsL2
     , double _errorRelMax
     , double _errorRelL2
     , double _execuationTime
     , double _energy)
     :ipart(_ipart)
     ,numproc(_numproc)
     ,s(_s)
     ,epsilon(_epsilon)
     ,partname(_partname)
     ,errorAbsMax(_errorAbsMax)
     ,errorAbsL2(_errorAbsL2)
     ,errorRelMax(_errorRelMax)
     ,errorRelL2(_errorRelL2)
     ,execuationTime(_execuationTime)
     ,energy(_energy)
     {}
        
     int ipart;
     int numproc;
     int s;
     double epsilon;
     std::string partname;
     double errorAbsMax;
     double errorAbsL2;
     double errorRelMax;
     double errorRelL2;
     double execuationTime;
     double energy;
   };

     
     
  
  ProfileHPC(int _rank, int _size); 

  void perforationStride(int const _s);

  void controlQualityLoop(double const _epsilon);

  void controlLoopIteration( int const & _controLoopIter);

  void setFileSize(double const _fileSize);

  void signalProcessingDataPath(std::string const & _kalmanDataPath
                               ,std::string const & _blachScholesPath
                               ,std::string const & _FFTFilterPath
                               ,std::string const & _savitzkeyGolayPath);

  void clusteringDataPath(std::string const & _clusterDataPath);  

  void Data_WorkSpace(std::string const & _data_workspace);

  void Profile_WorkSpace(std::string const & _profile_workspace);         

  void applyingFilters
        ( bool _Kalman_FilterApplied
        , bool _FFT_FilterApplied
        , bool _BlackScholes_Applied
        , bool _savitzkyGoly_Transform );    

  
 void 
 applyingKNNClassification(bool _KNN_Applied );

 void 
 applyingKMeanClassification(bool _KMean_Applied );

 
  void 
  benchmarkStatus(bool const _BenchmarkStat);

  void 
  execuationTimeInformation(std::vector<TimerData> const & _timeInfo);    

  void 
  errorInfoFilters( double const & _errorInfo);

  void 
  centroidAccuracy( double const & _centeroidError);        

  void computeEnergy();        

  I approximatePower(std::vector<energyData> const & power);

  std::vector<double> const & 
  computeEnergyInfo( I const  power
                   , TimerData const * timeInfo);

  std::vector<energyData> const &
  computePower();

  void 
  generateProfillingFile();

  std::string const constructFilePathBenchmarking();

  std::string const precisionConstruct(std::string & inputDataPath);

  void 
  writeProfillingInfoBenchmark(std::string const & filePath);

  void 
  writeProfillingInfoOutput();

  void 
  aggregateProfillingInformation();

 
  private:

  int rank;
  int size;
  int nPart;
  int s;
  I epsilon;
  double fileSize;
   
  std::string workspacePath;
  std::string kalmanDataPath;
  std::string blachScholesPath;
  std::string FFTFilterPath;
  std::string savitzkeyGolayPath; 
  std::string clusterDataPath;
 
  std::string data_workspace;
  std::string profile_workspace;


  bool Kalman_FilterApplied;
  bool FFT_FilterApplied;
  bool BlackScholes_Applied;
  bool savitzkyGoly_Transform;
  bool KNN_Applied;
  bool KMean_Applied;

  TimerData const * timerInfo;
  double errorAbsMax;
  double errorAbsL2;
  double errorRelMax;
  double errorRelL2;

  std::vector<double> energy;
  
  std::vector<energyData> powerConsumeInfo;
  std::string             inputDataPath;
  bool                    BenchmarkState;
  std::vector<profileData> profileInfo; 
  int  controLoopIter;
  double    centeroidError;

};

  
///////////////////////////////////
//////// IMPLEMENT
////////////////////////////////////


template<typename I, typename O>
ProfileHPC<I,O>::ProfileHPC(int _rank, int _size)
:rank(_rank)
,size(_size)
,Kalman_FilterApplied(false)
,FFT_FilterApplied(false)
,BlackScholes_Applied(false) 
,savitzkyGoly_Transform(false)
,KNN_Applied(false)
,KMean_Applied(false)
,BenchmarkState(false)
,timerInfo(nullptr)
,s(1)
,epsilon(0)
,energy(0,0)
,centeroidError(0)
,controLoopIter(1)
{}




template<typename I, typename O>
void
ProfileHPC<I,O>::controlQualityLoop(double const _epsilon){
    epsilon = _epsilon;
}

template<typename I, typename O>
void
ProfileHPC<I,O>::setFileSize(double const _fileSize){
    fileSize = _fileSize;
}

template<typename I, typename O>
void
ProfileHPC<I,O>::applyingFilters
    ( bool _Kalman_FilterApplied
    , bool _FFT_FilterApplied
    , bool _BlackScholes_Applied
    , bool _savitzkyGoly_Transform )
{
   Kalman_FilterApplied    = _Kalman_FilterApplied;
   FFT_FilterApplied       = _FFT_FilterApplied;
   BlackScholes_Applied    = _BlackScholes_Applied; 
   savitzkyGoly_Transform  = _savitzkyGoly_Transform;
}

template<typename I, typename O>
void
ProfileHPC<I,O>::perforationStride(int const _s){
    s = _s;
  if((FFT_FilterApplied)||(BlackScholes_Applied)){
    s=1;
  }
}

template<typename I, typename O>
void
ProfileHPC<I,O>::applyingKNNClassification( bool _KNN_Applied )
{
   KNN_Applied   = _KNN_Applied;
}

template<typename I, typename O>
void
ProfileHPC<I,O>::applyingKMeanClassification( bool _KMean_Applied )
{
   KMean_Applied   = _KMean_Applied;
}



template<typename I, typename O>
void 
ProfileHPC<I,O>::signalProcessingDataPath(std::string const & _kalmanDataPath
                                         ,std::string const & _blachScholesPath
                                         ,std::string const & _FFTFilterPath
                                         ,std::string const & _savitzkeyGolayPath)
{
    kalmanDataPath     = _kalmanDataPath;
    blachScholesPath   = _blachScholesPath;
    FFTFilterPath      = _FFTFilterPath;
    savitzkeyGolayPath = _savitzkeyGolayPath;
}


template<typename I,typename O>
void
ProfileHPC<I,O>::Data_WorkSpace( std::string const & _data_workspace){
  data_workspace = _data_workspace;
}

template<typename I,typename O>
void
ProfileHPC<I,O>::Profile_WorkSpace( std::string const & _profile_workspace){
  profile_workspace = _profile_workspace;
}


template<typename I, typename O>
void 
ProfileHPC<I,O>::clusteringDataPath(std::string const & _clusterDataPath){
    clusterDataPath = _clusterDataPath;
 }


template<typename I, typename O>
void 
ProfileHPC<I,O>::benchmarkStatus(bool const _BenchmarkState){
    BenchmarkState = _BenchmarkState;
}


template<typename I, typename O>
void 
ProfileHPC<I,O>::execuationTimeInformation(std::vector<TimerData> const & _timerInfo){
   timerInfo = _timerInfo.data();
   nPart     = _timerInfo.size();   
}

template<typename I, typename O>
void 
ProfileHPC<I,O>::errorInfoFilters( double const & _errorInfo){
     errorRelL2 = _errorInfo;

}

template<typename I, typename O>
void 
ProfileHPC<I,O>::centroidAccuracy( double const & _centeroidError){
    centeroidError = _centeroidError;
}

template<typename I, typename O>
void 
ProfileHPC<I,O>::controlLoopIteration( int const & _controLoopIter){
   controLoopIter = _controLoopIter;
}

template<typename I, typename O>
void
ProfileHPC<I,O>::computeEnergy(){

    if(rank == 0){
      std::vector<energyData> const & powerInfo( computePower() );
      I power (approximatePower(powerInfo));
      computeEnergyInfo( power, timerInfo); 
    }
}


template<typename I, typename O>
void
ProfileHPC<I,O>::generateProfillingFile(){
 
  if(rank==0){
         aggregateProfillingInformation();
    if(BenchmarkState){
        std::string const filePath( constructFilePathBenchmarking() ); 
        writeProfillingInfoBenchmark(filePath);
      }else{
         writeProfillingInfoOutput();
      }
  }
 
} 



template<typename I, typename O>
void 
ProfileHPC<I,O>::aggregateProfillingInformation(){
 
  if(KNN_Applied)
  {  
     for(int i(0); i<nPart; i++){
          profileInfo.push_back(profileData(i,size,s,epsilon,timerInfo[i].partname,0,0,0,0,timerInfo[i].exeTime, energy[i]));
         //std::cout<< profileInfo[i].numproc <<std::setw(20)<< profileInfo[i].partname <<std::setw(27)<<profileInfo[i].execuationTime<<std::setw(20)<< profileInfo[i].energy <<std::endl; 
       }
  }
  else if(KMean_Applied)
  { 
     for(int i(0); i<nPart; i++){
	 profileInfo.push_back(profileData(i,size,controLoopIter,epsilon,timerInfo[i].partname,0,0,centeroidError,0,timerInfo[i].exeTime, energy[i]));
         //std::cout<< profileInfo[i].numproc <<std::setw(20)<< profileInfo[i].partname <<std::setw(27)<<profileInfo[i].execuationTime<<std::setw(20)<< profileInfo[i].energy <<std::endl; 
       }
  }
  else{
      for(int i(0); i<nPart; i++){
         // std::cout << timerInfo[i].partname<<"  " <<  timerInfo[i].exeTime << "  " << errorInfo[1] << "  " << errorInfo[2] << std::endl;
         profileInfo.push_back(profileData(i,size,s,epsilon,timerInfo[i].partname,errorRelL2,errorRelL2,errorRelL2,errorRelL2,timerInfo[i].exeTime, energy[i]));
       }
   }    
}



template<typename I, typename O>
std::string const 
ProfileHPC<I,O>::constructFilePathBenchmarking(){

   if(Kalman_FilterApplied){
      inputDataPath = profile_workspace + "/KalmanFilter";
   }
   else if(BlackScholes_Applied ){
      inputDataPath = profile_workspace + "/BlackScholes";
   }
   else if(FFT_FilterApplied ){
      inputDataPath = profile_workspace + "/FFTFilter";
   }
   else if(savitzkyGoly_Transform){
      inputDataPath = profile_workspace + "/SavitzkyGolyTransform";
    }
   else if(KNN_Applied){
      inputDataPath = profile_workspace + "/KNN";
   }
   else if(KMean_Applied){
      inputDataPath = profile_workspace + "/KMean";
   } 
   
  if((sizeof(I) == 8) && (sizeof(O) == 8)){
    inputDataPath = inputDataPath+"/Double_Double_"+ std::to_string(fileSize) + ".dat";
  }
  else if((sizeof(I) == 4) && (sizeof(O) == 8)){
      inputDataPath = inputDataPath+"/Float_Double_"+ std::to_string(fileSize) + ".dat";
  }
  else if((sizeof(I) == 8) && (sizeof(O) == 4)){
      inputDataPath = inputDataPath+"/Double_Float_"+ std::to_string(fileSize) + ".dat";
  }
  else if((sizeof(I) == 4) && (sizeof(O) == 4)){
      inputDataPath = inputDataPath+"/Float_Float_"+ std::to_string(fileSize) + ".dat";
  }   
   
   return inputDataPath;
}



template<typename I, typename O>
void 
ProfileHPC<I,O>::writeProfillingInfoBenchmark(std::string const & filePath){
   
   char filename[ filePath.size() + 1];
   strcpy(filename, filePath.c_str()); 
  
    bool is_existed=false;
    FILE* perf_file=(FILE*)0;

    if((perf_file = fopen(filename, "r")))
    {
      fclose(perf_file);
      is_existed=true;
    }

    perf_file = fopen(filename, "a");
    char lable[64]; char inputPercision[64]; char outputPercision[64]; std::string inputData; std::string outputData;
    if(sizeof(I) == 8){
       inputData = "Double";
    }
    if(sizeof(I) == 4){
       inputData = "Float";
    }
    if(sizeof(O) == 8){
       outputData = "Double"; 
    }
    if(sizeof(O) == 4){
       outputData = "Float"; 
    }
      
    strcpy(inputPercision, inputData.c_str());
    strcpy(outputPercision, outputData.c_str()); 
  
    for(int i(0); i<nPart; i++){
       strcpy(lable, profileInfo[i].partname.c_str());
      // std::cout << profileInfo[i].errorAbsL2 << std::endl;
       fprintf(perf_file, "%d;%d;%d;%lf;%lf;%lf;%lf;%s;%s;%s\n", profileInfo[i].ipart, profileInfo[i].numproc, profileInfo[i].s , profileInfo[i].epsilon , profileInfo[i].errorRelL2, profileInfo[i].execuationTime, profileInfo[i].energy,lable,inputPercision,outputPercision);
      }
   
  
} 


template<typename I, typename O>
void 
ProfileHPC<I,O>::writeProfillingInfoOutput(){

   std::string output_Filter_Path; std::string FilterName;
  if(Kalman_FilterApplied){
    output_Filter_Path = data_workspace +"/Output_Data/profilling/kalman_filter_profilling.txt";
    FilterName = "Kalman Filter";
  }
  if(BlackScholes_Applied ){
    output_Filter_Path = data_workspace +"/Output_Data/profilling/BlackScholes_profilling.txt";
    FilterName = "Min Max Transform";
  }
  if(FFT_FilterApplied ){
    output_Filter_Path = data_workspace + "/Output_Data/profilling/FFT_filter_profilling.txt";
    FilterName = "FFT Filter";
  }
  if(savitzkyGoly_Transform){
    output_Filter_Path = data_workspace +"/Output_Data/profilling/SavitzkyGoly_profilling.txt";
    FilterName = "Savitzkey Golay";
  }
  if(KNN_Applied){
    output_Filter_Path = data_workspace +"/Output_Data/profilling/KNN_Profilling.txt";
    FilterName = "KNN Classification";
  }
  if(KMean_Applied){
    output_Filter_Path = data_workspace + "/Output_Data/profilling/KMean_Profilling.txt";
    FilterName = "KMean Classification";
  }
 
   char outputFile[ output_Filter_Path.size() + 1];
   strcpy(outputFile, output_Filter_Path.c_str()); 
   std::ofstream outputPath;
   outputPath.open (outputFile);
   
   double sumElapsed(0); double sumEnergy(0);
   for(int i(0); i<nPart; i++){
       sumElapsed += profileInfo[i].execuationTime;
       sumEnergy  += profileInfo[i].energy;
    }
  
    outputPath<<"                       Profilling of the parallel Kernel "<< FilterName << " " <<  std::endl;
    outputPath<<"<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"<<std::endl;
    
   if(KNN_Applied)
   {
     outputPath<<"num proc"<<std::setw(25)<<"execuation Time/s"<<std::setw(17)<<"energy/Joule"<<std::endl; 
     outputPath<<"   "<< profileInfo[0].numproc <<std::setw(23)<<sumElapsed<<std::setw(20)<< sumEnergy <<std::endl; 
     outputPath<<std::endl;
     outputPath<<std::endl;
     outputPath<<"                       Profilling the components of parallel "<< FilterName << " " <<  std::endl;
     outputPath<<"<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << std::endl;
     outputPath<<"ipart"<<std::setw(20)<<"num proc"<<std::setw(23)<<"part name"<<std::setw(27)<<"execuation Time/s"<<std::setw(17)<<"energy/Joule"<<std::endl; 
   
     for(int i(0); i<nPart; i++){
        outputPath<<i<<std::setw(20)<< profileInfo[i].numproc <<std::setw(20)<< profileInfo[i].partname <<std::setw(27)<<profileInfo[i].execuationTime<<std::setw(20)<< profileInfo[i].energy <<std::endl; 
     }
   }   
   else if(KMean_Applied)
   { 
     outputPath<<"num proc"<<std::setw(25)<<"Quality Control loop"<<std::setw(20)<<"execuation Time/s"<<std::setw(17)<<"energy/Joule"<<std::endl; 
     outputPath<<"   "<< profileInfo[0].numproc <<std::setw(23)<<profileInfo[0].epsilon<<std::setw(20)<<sumElapsed<<std::setw(20)<< sumEnergy <<std::endl; 
     outputPath<<std::endl;
     outputPath<<std::endl;
     outputPath<<"                       Profilling the components of parallel "<< FilterName << " " <<  std::endl;
     outputPath<<"<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << std::endl;
     outputPath<<"ipart"<<std::setw(20)<<"num proc"<<std::setw(25)<<"Quality Control loop"<<std::setw(19)<<"part name"<<std::setw(27)<<"execuation Time/s"<<std::setw(17)<<"energy/Joule"<<std::endl; 
   
     for(int i(0); i<nPart; i++){
        outputPath<<i<<std::setw(20)<< profileInfo[i].numproc <<std::setw(20)<<std::setw(23)<<profileInfo[0].epsilon<<std::setw(23)<< profileInfo[i].partname <<std::setw(27)<<profileInfo[i].execuationTime<<std::setw(20)<< profileInfo[i].energy <<std::endl; 
      } 
   }else
   {
     outputPath<<"num proc"<<std::setw(23)<<"perforation stride"<<std::setw(20)<<"absoulute error"<<std::setw(23)<<"relative error"<<std::setw(27)<<"execuation Time/s"<<std::setw(17)<<"energy/Joule"<<std::endl; 
     outputPath<<"   "<< profileInfo[0].numproc <<std::setw(20)<< profileInfo[0].s <<std::setw(20)<< profileInfo[0].errorAbsL2 <<std::setw(27)<< profileInfo[0].errorRelL2 <<std::setw(23)<<sumElapsed<<std::setw(20)<< sumEnergy <<std::endl; 
     outputPath<<std::endl;
     outputPath<<std::endl;
     outputPath<<"                            Profilling the components of parallel "<< FilterName << " " <<  std::endl;
     outputPath<<"<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << std::endl;
     outputPath<<"ipart"<<std::setw(20)<<"num proc"<<std::setw(23)<<"part name"<<std::setw(27)<<"execuation Time/s"<<std::setw(17)<<"energy/Joule"<<std::endl; 

     for(int i(0); i<nPart; i++){
       outputPath<<i<<std::setw(20)<< profileInfo[i].numproc <<std::setw(20)<< profileInfo[i].partname <<std::setw(27)<<profileInfo[i].execuationTime<<std::setw(20)<< profileInfo[i].energy <<std::endl; 
      // std::cout<<i<<std::setw(20)<< profileInfo[i].numproc <<std::setw(20)<< profileInfo[i].partname <<std::setw(27)<<profileInfo[i].execuationTime<<std::setw(20)<< profileInfo[i].energy <<std::endl;

     }
  

   } 
   outputPath.close();
   
}



template<typename I, typename O>
std::vector<typename::ProfileHPC<I,O>::energyData> const & 
ProfileHPC<I,O>::computePower(){

 std::string filename = profile_workspace + "/EnergySystem/energy.dat";   
 std::string fileline;
 std::string elm;
 std::ifstream InputData;
 InputData.open(filename);
 std::vector<double> bufferPower;    
 while(InputData)
 {
   getline(InputData,fileline);
   std::istringstream ss(fileline);
   int  elmId(0); double val(0); 
   bool phase(true);  
   int  ith(0);
   double power(0);
   double energy(0);
   double time(0); 
   
   while(getline(ss, elm,';') )
   {
     double SPXelm = std::atof(elm.c_str());
     ith++;         
     if(ith == 2){
          time = SPXelm;
     }

         // power = D + J + P
     if( (ith == 4) || (ith == 10) || (ith == 16) ){
         power += SPXelm;
     }
          
     if(ith == 20){
       // power = (power +50);
       // energy = power*time;
        if(power){
              power = (power +50);
              bufferPower.push_back(power);
           }
     }
         
     elmId++;
    }
  }
  std::vector<int> procNum = {1,2,4,8,16,32,64};
  std::vector<int> phases  = {0,1,2};
  int iter(0); std::vector<energyData> powerData;
 
  for(int i(0); i<procNum.size()*phases.size(); i++){
     
     int numproc = procNum[iter];
     int ipart = i%3;
     if( (ipart) == 2 ){
              iter++;
     }
     powerData.push_back(energyData(ipart, numproc , bufferPower[i]));
  }

   double sumPower(0); 
  for(int i(0); i<powerData.size(); i++){
        sumPower += powerData[i].power;
        if( powerData[i].ipart == 2 ){
            powerConsumeInfo.push_back(energyData
                                   ( powerData[i].ipart 
                                   , powerData[i].numProc
                                   , sumPower/phases.size())
                                 );
            sumPower = 0;                   
         }
  }

/*
  for(int i(0); i<powerConsumeInfo.size(); i++){
      std::cout << powerConsumeInfo[i].numProc<<"  " <<  powerConsumeInfo[i].power << std::endl; 
  }
*/

  return  powerConsumeInfo; 
}


template<typename I, typename O>
I 
ProfileHPC<I,O>::approximatePower(std::vector<energyData> const & power){

std::vector<int> x;
std::vector<I> y;

for(int i(0); i<power.size(); i++){
   x.push_back(power[i].numProc);
   y.push_back(power[i].power);
}

 double sumy(0);double sumx(0); double sumxy(0); double sumxx(0);
 int n = x.size();
 for(int i(0); i<n; i++)
  {
       sumy  += y[i];
       sumx  += x[i];
       sumxy += y[i]*x[i];
       sumxx += x[i]*x[i];
  }
   
 double a  = (sumx*sumxy-sumy*sumxx)/(sumx*sumx-n*sumxx);
 double b = (sumy*sumx-n*sumxy)/(sumx*sumx-n*sumxx);

 I ApproxPower;
 if (size <128){  
   ApproxPower =  (a + b*size);
 }else{
   int perpSize = size%128;
   I   perpPower = (a + b*perpSize);
   
   int numberComputeNode = (size - perpSize)/128;
   I computeNodePower = (a + b*128);
   computeNodePower = numberComputeNode*computeNodePower;
   
   ApproxPower = computeNodePower + perpPower; 
 }

  
  return ApproxPower;
}



template<typename I, typename O>
std::vector<double> const &
ProfileHPC<I,O>::computeEnergyInfo
                      ( I const  power
                      , TimerData const * timer)
{

  energy.resize(nPart); 
  for(int j(0); j<nPart; j++)
  { 
      energy[j] = power*timer[j].exeTime;
     // std::cout << size <<"   "<< timer[j].exeTime << "  " << energy[j] << std::endl;
  }

  return energy;
  
}


#endif
