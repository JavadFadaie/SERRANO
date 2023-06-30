#ifndef MPI_TYPE_HPP_INCLUDED
#define MPI_TYPE_HPP_INCLUDED

/**
 * \file     mpi_type.hpp
 * \brief    Function for automatically determining MPI data type to a constexpr
 * \mainpage Contains a template function that helps determining the corresponding 
 *           MPI message data type that can be found on
 *           \ref https://www.mpich.org/static/docs/latest/www3/Constants.html
 *           to a constexpr. This way the code may already be simplified at 
 *           compile time.
*/

#include <mpi.h>

#include <cassert>
#include <complex>
#include <cstdint>
#include <type_traits>


/**\fn      mpi_get_type
 * \brief   Small template function to return the correct MPI_DATATYPE
 *          data type need for an MPI message as a constexpr at compile time
 *          https://www.mpich.org/static/docs/latest/www3/Constants.html
 *          Call in a template function with mpi_get_type<T>()
 * 
 * \tparam  T   The C++ data type used in the MPI function
 * \return  The MPI_Datatype belonging to the template C++ data type T
*/
template <typename T>
[[nodiscard]] constexpr MPI_Datatype mpi_get_type() noexcept
{
    MPI_Datatype mpi_type = MPI_DATATYPE_NULL;
    
    if  (std::is_same<T, char>::value)
    {
        mpi_type = MPI_CHAR;
    }
    else if  (std::is_same<T, signed int>::value)
    {
        mpi_type = MPI_INT;
    }
    else if (std::is_same<T, float>::value)
    {
        mpi_type = MPI_FLOAT;
    }
    else if (std::is_same<T, double>::value)
    {
        mpi_type = MPI_DOUBLE;
    }
    
	
    assert(mpi_type != MPI_DATATYPE_NULL);

    return mpi_type;    
}

#endif // MPI_TYPE_HPP_INCLUDED
