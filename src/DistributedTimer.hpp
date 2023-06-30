#ifndef DISTRIBUTEDTIMER_HPP
#define DISTRIBUTEDTIMER_HPP

#include <chrono>
#include <sys/time.h>
#include <sstream>
#include <fstream>
#include "mpi_type.hpp"
#include <dirent.h>
#include <iomanip>

 struct TimerData{

    TimerData( double _exeTime
             , std::string _partname)
      :exeTime(_exeTime)
      ,partname(_partname)
       {}       
     
    
   double exeTime;
   std::string partname;
 };

template<typename I, typename O>
class DistributedTimer
{
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



  public :
  
 DistributedTimer(int _nparts, int rank, int size);

 DistributedTimer(int _rank, int _size);
  
 void FlopsInfo(int ipart, flops _info, int DataSize);
 
 void startSys(int ipart, std::string name);
 
 void stopSys(int ipart);
 
 void timeProfilSys();
 
 void elapsedTimeProfil(); 
 
 void computeFloatPointPerSec();
 
 void AggregateTimerInfo();

 void profileElapsedTime();

 std::vector<double> const & elapsedTime();

 std::vector<TimerData> const & timerInformation();

 void applyProfilling();

 void start(std::string const phaseName);
 
 void stop(std::string const  phaseName);

 
 private :
 
 int   nparts;

 using Clock = std::chrono::high_resolution_clock;
 
// std::vector<Clock::time_point> startTime; 
 std::vector<long long> startSysTime;
 std::vector<long long> stopSysTime;
 std::vector<long long> startTime;
 std::vector<long long> stopTime;

 //Clock::time_point startTime;
 std::vector<double>    elapsed;
 std::vector<std::string> partname;
 std::vector<flops>     executionInfo;
 
 int rank;
 int size;

 struct TimeInfo{
   int    _ipart;
   double _elapsed;
   double _flops;
   double _readBytes;
   double _writeBytes;
   double _bandwidth;
 };


std::vector<TimeInfo> timerInfo;
std::vector<TimeInfo> aggregatedTimerInfo;
I * errorInfo;
double sizeFile;
int s;
std::string workSpaceProfilling;
std::string kalmanDataPath;
std::vector<TimerData> timerInform;
};

////////////////////////////
//////// IMPLEMENTATION
////////////////////////////

template<typename I, typename O>
DistributedTimer<I,O>::DistributedTimer(int _nparts, int _rank, int _size)
:nparts(_nparts)
,startSysTime(2*_nparts)
,stopSysTime(2*_nparts)
,executionInfo(_nparts)
,elapsed(_nparts)
,partname(_nparts)
,rank(_rank)
,size(_size)
,timerInfo(_nparts)
,aggregatedTimerInfo(size*nparts)
{}

template<typename I, typename O>
DistributedTimer<I,O>::DistributedTimer(int _rank, int _size)
:rank(_rank)
,size(_size)
,startTime(2)
,stopTime(2)
{}


template<typename I, typename O>
void
DistributedTimer<I,O>::start(std::string const name){
    struct timespec time_now; clock_gettime(CLOCK_REALTIME,&time_now);
    startTime[0]=time_now.tv_sec; 
    startTime[1]=time_now.tv_nsec;
}

template<typename I, typename O>
void
DistributedTimer<I,O>::stop(std::string const name){
  struct timespec time_now; 
  clock_gettime(CLOCK_REALTIME,&time_now);
 
  stopTime[0]=time_now.tv_sec; 
  stopTime[1]=time_now.tv_nsec;

  double elapsed = static_cast<double>(stopTime[0]) + static_cast<double>(stopTime[1])*1.0e-9 ;
  elapsed -= static_cast<double>(startTime[0]) + static_cast<double>(startTime[1])*1.0e-9 ;

  timerInform.push_back(TimerData(elapsed,name));   
  
}

template<typename I, typename O>
std::vector<TimerData> const &
DistributedTimer<I,O>::timerInformation(){
   return timerInform;
}

template<typename I, typename O>
void
DistributedTimer<I,O>::FlopsInfo(int ipart, flops _info , int DataSize)
{

  executionInfo[ipart].mult =_info.mult;
  executionInfo[ipart].div  =_info.div;
  executionInfo[ipart].add  =_info.add;
  executionInfo[ipart].rbyte=_info.rbyte;
  executionInfo[ipart].wbyte=_info.wbyte;
  
  
  timerInfo[ipart]._flops  += (executionInfo[ipart].mult
                           +  executionInfo[ipart].div
                           +  executionInfo[ipart].add
                           +  executionInfo[ipart].rbyte
                           +  executionInfo[ipart].wbyte)*DataSize;

  timerInfo[ipart]._readBytes     += (executionInfo[ipart].rbyte)*DataSize;
  timerInfo[ipart]._writeBytes    += (executionInfo[ipart].wbyte)*DataSize;
  timerInfo[ipart]._bandwidth     += (executionInfo[ipart].rbyte 
                                  +  2*executionInfo[ipart].wbyte)*DataSize;
}

template<typename I, typename O>
void
DistributedTimer<I,O>::startSys(int ipart, std::string name ){

  struct timespec time_now; clock_gettime(CLOCK_REALTIME,&time_now);
  startSysTime[ipart*2]=time_now.tv_sec; 
  startSysTime[ipart*2+1]=time_now.tv_nsec;
  partname[ipart]=name;
}


template<typename I, typename O>
void
DistributedTimer<I,O>::stopSys(int ipart){
 struct timespec time_now; 
 clock_gettime(CLOCK_REALTIME,&time_now);
 
 stopSysTime[ipart*2]=time_now.tv_sec; 
 stopSysTime[ipart*2+1]=time_now.tv_nsec;
}


template<typename I, typename O>
void
DistributedTimer<I,O>::elapsedTimeProfil(){
  

  for(int i(0); i<nparts; i++)
  {
     double elaps = static_cast<double>(stopSysTime[i*2]) + static_cast<double>(stopSysTime[i*2+1])*1.0e-9 ;
     elaps -= static_cast<double>(startSysTime[i*2]) + static_cast<double>(startSysTime[i*2+1])*1.0e-9 ;
     timerInfo[i]._ipart   = i;
     timerInfo[i]._elapsed = elaps;
     elapsed[i]=elaps;
     timerInform.push_back(TimerData(elaps,partname[i]));
    }

}

template<typename I, typename O>
std::vector<double> const &
DistributedTimer<I,O>::elapsedTime(){
   return elapsed;
}



template<typename I, typename O>
void
DistributedTimer<I,O>::timeProfilSys(){
  /*
    std::string  inputDataPath;
if((sizeof(I) == 8) && (sizeof(O) == 8)){
 
    std::string  inputDataPath = "/home/javad/Desktop/SERRANO/serrano/serrano/serrano/profile/EnergySystem/DoubleDouble/SysTime_rank_"+ std::to_string(sizeFile) + "_GB_"+ std::to_string(rank) + "_.dat";
    writeEnergyExecutionTimeFile(inputDataPath);
}
if((sizeof(I) == 4) && (sizeof(O) == 8)){
 
    std::string  inputDataPath = "/home/javad/Desktop/SERRANO/serrano/serrano/serrano/profile/EnergySystem/FloatDouble/SysTime_rank_"+ std::to_string(sizeFile) + "_GB_"+ std::to_string(rank) + "_.dat";
    writeEnergyExecutionTimeFile(inputDataPath);
}
if((sizeof(I) == 8) && (sizeof(O) == 4)){

    std::string  inputDataPath = "/home/javad/Desktop/SERRANO/serrano/serrano/serrano/profile/EnergySystem/DoubleFloat/SysTime_rank_"+ std::to_string(sizeFile) + "_GB_"+ std::to_string(rank) + "_.dat";
    writeEnergyExecutionTimeFile(inputDataPath);
}
if((sizeof(I) == 4) && (sizeof(O) == 4)){
    std::string  inputDataPath = "/home/javad/Desktop/SERRANO/serrano/serrano/serrano/profile/EnergySystem/FloatFloat/SysTime_rank_"+ std::to_string(sizeFile) + "_GB_"+ std::to_string(rank) + "_.dat";
    writeEnergyExecutionTimeFile(inputDataPath);

}
*/

   
}



template<typename I, typename O>
void
DistributedTimer<I,O>::computeFloatPointPerSec(){

  for(int ipart(0); ipart<nparts; ipart++)
   {
    
     timerInfo[ipart]._flops        /=  timerInfo[ipart]._elapsed;    
     timerInfo[ipart]._readBytes    /=  timerInfo[ipart]._elapsed;
     timerInfo[ipart]._writeBytes   /=  timerInfo[ipart]._elapsed;
     timerInfo[ipart]._bandwidth    /=  timerInfo[ipart]._elapsed;
  }

}


template<typename I, typename O>
void
DistributedTimer<I,O>::AggregateTimerInfo(){

  const int     nItems=6;
  int          blocklengths[nItems] = {1, 1, 1, 1, 1, 1};
  MPI_Datatype types[nItems] = {MPI_INT, mpi_get_type<double>(), mpi_get_type<double>(), mpi_get_type<double>(), mpi_get_type<double>(), mpi_get_type<double>()};
  MPI_Datatype MPI_TimeInfo;
  MPI_Aint     offsets[nItems];

  offsets[0] = offsetof(TimeInfo, _ipart);
  offsets[1] = offsetof(TimeInfo, _elapsed);
  offsets[2] = offsetof(TimeInfo, _flops);
  offsets[3] = offsetof(TimeInfo, _readBytes);
  offsets[4] = offsetof(TimeInfo, _writeBytes);
  offsets[5] = offsetof(TimeInfo, _bandwidth);

  MPI_Type_create_struct(nItems, blocklengths, offsets, types, & MPI_TimeInfo);
  MPI_Type_commit(& MPI_TimeInfo);
  
 MPI_Gather
      ( & timerInfo[0]             //send_data,
      , nparts                     //send_count
      , MPI_TimeInfo               //send_datatype
      , & aggregatedTimerInfo[0]   //recv_data,
      , nparts                     //recv_count,
      , MPI_TimeInfo               //recv_datatype,
      , 0                          //Master Proc  
      , MPI_COMM_WORLD);           //Comminucation group


}

template<typename I, typename O>
void
DistributedTimer<I,O>::applyProfilling(){
  elapsedTimeProfil();
  timeProfilSys();
  AggregateTimerInfo();  
  profileElapsedTime();
  
}

template<typename I, typename O>
void
DistributedTimer<I,O>::profileElapsedTime(){

 if(rank == 0)
 {
     double iPartFlops(0) , iPartElapsed(0), iPartReadBandwidth(0), iPartWriteBandwidth(0), iPartTotalBandwidth(0);
     std::vector<double> Elapsed(nparts);
    for(int iPart(0); iPart<nparts; iPart++)
    {
     for(int i(0); i<aggregatedTimerInfo.size(); i++)
     {
         if(aggregatedTimerInfo[i]._ipart == iPart)
          {
             iPartElapsed         += aggregatedTimerInfo[i]._elapsed;
             iPartFlops           += aggregatedTimerInfo[i]._flops;
             iPartReadBandwidth   += aggregatedTimerInfo[i]._readBytes;
             iPartWriteBandwidth  += aggregatedTimerInfo[i]._writeBytes;
             iPartTotalBandwidth  += aggregatedTimerInfo[i]._bandwidth;
         }
        
     }
     Elapsed[iPart] = iPartElapsed/static_cast<double>(size);
    //std::cout << " Elapsed time   " << " Flops     " << "Read Bandwidth   " << "  Write Bandwidth    " << "   total Bandwidth   " << std::endl;
    //std::cout << iPartElapsed/static_cast<double>(size) << "      " << iPartFlops <<"       " << iPartReadBandwidth << "   " << iPartWriteBandwidth <<"        " << iPartTotalBandwidth << std::endl;
    //std::cout <<" size " << size<< " Execuation Time part " << iPart << "  " <<  iPartElapsed/static_cast<double>(size) << std::endl;
    }

     
 }
}






#endif
