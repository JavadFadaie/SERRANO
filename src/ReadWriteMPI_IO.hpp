#ifndef READWRITEMPI_HPP
#define READWRITEMPI_HPP

#include <iostream>
#include <cstring>
#include <vector>
#include <mpi.h>
#include "mpi_type.hpp"

template<typename I,typename O>
class ReadWriteMPI_IO{

 public: 
 ReadWriteMPI_IO
      ( int _rank
      , int _size
      , std::vector<size_t>       & _bufferPorcSize
      , std::vector<size_t> const & _offsetVec);

 void inputFileSetPath(std::string  & _inputDataPath);

 void outputFileSetPath(std::string  & _outputDataPath);

 void readInputBuffer();

 std::vector<I>  & InputBuffer();      

 void writeOutputBuffer
       ( std::vector<O> const & Output_Buffer);
 
 private:

 int rank;
 int size;  
 
 
 size_t bufferProcSize;
 size_t offsetProc;
 
 std::vector<I>   Input_Buffer;

 std::string inputDataPath;
 std::string outputDataPath;
 };

///////////////////////////////////////
////////// IMPLEMENTATION
///////////////////////////////////////

template<typename I,typename O>
ReadWriteMPI_IO<I,O>::ReadWriteMPI_IO
( int _rank
, int _size
, std::vector<size_t>  & _bufferPorcSize
, std::vector<size_t> const & _offsetVec)
:rank(_rank)
,size(_size)
,bufferProcSize(_bufferPorcSize[rank])
,offsetProc(_offsetVec[rank])
,Input_Buffer(bufferProcSize,0)
{}         


template<typename I,typename O>
void
ReadWriteMPI_IO<I,O>::inputFileSetPath(std::string  & _inputDataPath){
    inputDataPath = _inputDataPath;
}


template<typename I,typename O>
void
ReadWriteMPI_IO<I,O>::outputFileSetPath(std::string  & _outputDataPath){
    outputDataPath = _outputDataPath;
}


template<typename I,typename O>
std::vector<I>  &
ReadWriteMPI_IO<I,O>::InputBuffer(){

    return Input_Buffer;
}

template<typename I,typename O>
void
ReadWriteMPI_IO<I,O>::readInputBuffer()
{

MPI_Offset offset;
MPI_File fh;
MPI_Status status;  

char inputData[ inputDataPath.size() + 1];
strcpy(inputData, inputDataPath.c_str()); 


MPI_File_open
    ( MPI_COMM_WORLD
    , inputData
    , MPI_MODE_RDONLY
    , MPI_INFO_NULL
    , &fh
    );

offset = offsetProc*sizeof(I);

MPI_File_set_view
    ( fh
    , offset
    , mpi_get_type<I>()
    , mpi_get_type<I>()
    , "native"
    , MPI_INFO_NULL);    

//Read the file in populate it with local buffer
MPI_File_read_all
    ( fh
    , & Input_Buffer[0]
    , bufferProcSize
    , mpi_get_type<I>()
    , &status
    ); 
    
MPI_File_close(&fh);

}

template<typename I,typename O>
void
ReadWriteMPI_IO<I,O>::writeOutputBuffer
( std::vector<O>  const & Output_Buffer)
{

MPI_Offset offset;
MPI_File fh1;
MPI_Status status;  

char outputData[outputDataPath.size() + 1];
strcpy(outputData, outputDataPath.c_str()); 

MPI_File_open
    ( MPI_COMM_WORLD
    , outputData
    , MPI_MODE_CREATE | MPI_MODE_WRONLY
    , MPI_INFO_NULL
    , &fh1
    );

offset = offsetProc*sizeof(O); 

MPI_File_set_view
    ( fh1
    , offset
    , mpi_get_type<O>()
    , mpi_get_type<O>()
    , "native"
    , MPI_INFO_NULL);

//Write the file in populate it with local buffer
MPI_File_write_all
     ( fh1
     , &Output_Buffer[0]
     , Output_Buffer.size()
     , mpi_get_type<O>()
     , &status);    
    
MPI_File_close(&fh1);
}


#endif