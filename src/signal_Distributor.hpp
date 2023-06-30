#ifndef SIGNALDISTRIBUTOR_HPP
#define SIGNALDISTRIBUTOR_HPP

#include <iostream>
#include <vector>
#include <cstdlib>
#include <cstring>

template<typename I>
class signalDistributor{
     
   public: 
     
     struct FileNameIndex{
         FileNameIndex(std::string _filename, int _index)
         :filename(_filename)
         ,index(_index)
         {}

         std::string filename;
         int index;
       };

      
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

   signalDistributor(int _rank, int _size);  
    
  
  void Data_IO_Path( std::string const & _inputDataPath);
 
  void apply();

  void searchFileInsideDirectory();

  void DistributeFilesProc();

  std::vector<I> readFiles(std::string const & path);

  std::vector<I> readFilesCSV(std::string const & path);

  std::string const  fullpath( std::string const & _signalname);

  std::vector<std::string> const  procSignalFiles();

  std::vector<std::string> const  SignalFiles();

  std::string const  signalName( int const signalIndex);

  std::vector<int> const & procSignalIndex();

  std::vector<int> const & SignalIndex();

  std::vector<int> const & displacementDistribution();

  int const  totalNumFiles();

  std::vector<int> const & numberDataProc();

  size_t const & fileSize();


  private:

  int rank;
  int size;  

  
 std::string inputDataPath;

 std::vector<std::string> Files; 
 std::vector<int>      signalIndex;
 
 std::vector<std::string> ProcSignalFiles; 
 std::vector<int> ProcSignalIndex;

 std::vector<int> numDataProc;
 std::vector<int> offsetVec;
 
 size_t numFiles; 
 size_t sizefile;

 std::vector<FileNameIndex> filenameIndex;
};


//////////////////////////////////
/////////////IMPLEMENTATION
///////////////////////////////////


template<typename I>
signalDistributor<I>::signalDistributor(int _rank, int _size)
:rank(_rank)
,size(_size)
,numDataProc(size,0)
,offsetVec(size,0)
,numFiles(0)
{}


template<typename I>
void
signalDistributor<I>::Data_IO_Path
               ( std::string const & _inputDataPath)
{
  inputDataPath  = _inputDataPath;

}


template<typename I>
void
signalDistributor<I>::apply(){
    
    //look and scan all the files
   searchFileInsideDirectory();

   //Distribute the files in different signals
   DistributeFilesProc();

}


template<typename I>
void
signalDistributor<I>::searchFileInsideDirectory()
{
  char DirPath[ inputDataPath.size() + 1]; 
  strcpy(DirPath, inputDataPath.c_str()); 
  if (auto dir = opendir(DirPath)) {
  while (auto f = readdir(dir)) {
        if (!f->d_name || f->d_name[0] == '.')
          continue; // Skip everything that starts with a dot
          std::string filesInsideDir = f->d_name;
          char signalName[filesInsideDir.size() + 1];
          strcpy(signalName, filesInsideDir.c_str());
          char const * s= & signalName[7];
          int  fileIndex = atoi(s);
          filenameIndex.push_back(FileNameIndex(filesInsideDir,fileIndex));
          numFiles++;
     }
    closedir(dir);
   }
   
  
  std::sort(filenameIndex.begin(), filenameIndex.end(),
                [](FileNameIndex & a, FileNameIndex & b ) -> bool
                {return a.index < b.index;} 
             );
   for(int i(0); i<filenameIndex.size(); i++){
       Files.push_back(fullpath(filenameIndex[i].filename));
       signalIndex.push_back(filenameIndex[i].index);
   }

} 
  

template<typename I>
void
signalDistributor<I>::DistributeFilesProc()  
{  

  size_t rem(numFiles%size);
  size_t displac(0);  
  for(int i(0); i<size; i++){
    numDataProc[i] = numFiles/size;
    offsetVec[i]   = displac; 
    if(rem > 0){
      numDataProc[i]++;
      rem--;      
    }
   displac += numDataProc[i];
  }

   
 for(int i(offsetVec[rank]); i<offsetVec[rank]+numDataProc[rank]; i++){
     std::string fileOffset (fullpath((signalName(signalIndex[i]))));
     
     for(int ifile(0); ifile<numFiles; ifile++){
      if(Files[ifile]==fileOffset){
        ProcSignalFiles.push_back(Files[ifile]);
        ProcSignalIndex.push_back(signalIndex[ifile]);
      }
    }
 }

 
}


template<typename I>
std::vector<I>
signalDistributor<I>::readFiles(std::string const & path){

  FILE *p_file; 
  p_file = fopen(path.c_str(),"rb");
  fseek(p_file,0,SEEK_END);
  sizefile = ftell(p_file);
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



template<typename I>
std::string const 
signalDistributor<I>::fullpath( std::string const & _signalname)
{ 
   return inputDataPath +"/"+ _signalname;
}


template<typename I>
std::string const 
signalDistributor<I>::signalName( int const signalIndex)
{ 
   return "signal_"+ std::to_string(signalIndex);
}

template<typename I>
std::vector<std::string> const 
signalDistributor<I>::procSignalFiles()
{ 
   return ProcSignalFiles;
}

template<typename I>
std::vector<std::string> const 
signalDistributor<I>::SignalFiles()
{ 
   return Files;
}

template<typename I>
std::vector<int> const &
signalDistributor<I>::procSignalIndex()
{ 
   return ProcSignalIndex;
}


template<typename I>
std::vector<int> const &
signalDistributor<I>::SignalIndex()
{ 
   return signalIndex;
}

template<typename I>
std::vector<int> const &
signalDistributor<I>::displacementDistribution()
{ 
   return offsetVec;
}

template<typename I>
int const 
signalDistributor<I>::totalNumFiles()
{ 
   return numFiles;
}


template<typename I>
std::vector<int> const &
signalDistributor<I>::numberDataProc()
{ 
   return numDataProc;
}

template<typename I>
size_t const &
signalDistributor<I>::fileSize()
{ 
   return sizefile;
}

#endif