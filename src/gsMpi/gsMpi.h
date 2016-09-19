/** @file gsMpi.h
    
    @brief Helpers for dealing with MPI codes.
    
    This file is part of the G+Smo library.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

    Author(s): C. Hofer, A. Mantzaflaris -- Code based on ideas from the DUNE library
*/


#pragma once

#include <gsCore/gsForwardDeclarations.h>

#ifdef GISMO_WITH_MPI
#include <mpi.h>
#include <gsMpi/gsMpiTraits.h>
#include <gsMpi/gsBinaryFunctions.h>
#endif

#include <gsMpi/gsMpiComm.h>


namespace gismo
{

class gsMpi;

/// Singleton function returning the gsMpi helper object
GISMO_EXPORT gsMpi & gsMpiSingleton(const int& argc = 0, char** argv = 0);

/**
  @brief A helper for initializing MPI within G+Smo code

  Use as:
  \code
  // Initialize the MPI environment and obtain the world communicator
  gsMpiComm comm = gsMpi::init(argc,argv).worldComm();
  \code

  If MPI is available, then gsMpiComm is the the world communicator.

  If MPI is not avaibable during compilation, it defaults to a trivial
  function returning a gsSerialComm object as the communicator.
 
  @ingroup Mpi
 */
class gsMpi
{
public:
      
    friend GISMO_EXPORT gsMpi & gsMpiSingleton(const int& argc, char** argv);
    
#   ifdef GISMO_WITH_MPI
    /**
     * @brief The type of the mpi communicator.
     */
    typedef MPI_Comm Communicator;
#   else
    typedef gsSerialComm Communicator;
#   endif

    /** \brief get the default communicator
     *
     *  Return a communicator to exchange data with all processes
     *
     *  \returns MPI_COMM_WORLD
     */
    static Communicator worldComm()
    {
#   ifdef GISMO_WITH_MPI
        return MPI_COMM_WORLD;
#   else
        return localComm();
#   endif
    }

    /** \brief get a local communicator
     *
     *  Returns a communicator to exchange data with the local process only
     *
     *  \returns MPI_COMM_SELF
     */
    static Communicator localComm()
    {
        return gsSerialComm();
    }
    
    /**
     * @brief Returns the singleton instance of gsMpi
     *
     * This method has to be called with the same arguments
     * that the main method of the program was called:
     * \code
     * int main(int argc, char** argv){
     *   gsMpi::init(argc, argv);
     *   // program code comes here
     *   ...
     * }
     * \endcode
     * @param argc The number of arguments provided to main.
     * @param argv The arguments provided to main.
     */
    static gsMpi& init(const int& argc = 0, char** argv = NULL)
    {
        GISMO_ASSERT( 0 == argc || NULL!=argv, "Need both argc and argv (or none)");
        return gsMpiSingleton(argc,argv);
    }
    
    /**
     * @brief Returns the rank of process
     */
    static int worldRank () { return gsMpiComm(worldComm()).rank(); }
    /**
     * @brief Returns the number of processes
     */
    static int worldSize () { return gsMpiComm(worldComm()).size(); }

private:
    gsMpi();
    
    /// \brief calls MPI_Init with argc and argv as parameters
    gsMpi(const int& argc, char** argv)
    {
        initMpi(const_cast<int*>(&argc), argv);
    }

    void initMpi(int * argc = NULL, char** argv = NULL)
    {
#   ifdef GISMO_WITH_MPI
        int initialized = -1;
        MPI_Initialized( &initialized );
        if( 0 == initialized )
        {
            //Note: valgrind false positive here, see
            // https://www.open-mpi.org/faq/?category=debugging#valgrind_clean
            initialized = MPI_Init(argc, &argv);
            GISMO_ENSURE(MPI_SUCCESS==initialized, "MPI failed to initialize");
        }
#   else  
        GISMO_UNUSED(argc);
        GISMO_UNUSED(argv);
#   endif
        //gsDebug << "Called  MPI_Init on p=" << rank_ << "!" << std::endl;
    }
    
    /// \brief calls MPI_Finalize
    ~gsMpi()
    {
#   ifdef GISMO_WITH_MPI
        int wasFinalized = -1;
        MPI_Finalized( &wasFinalized );
        if( 0 == wasFinalized)
        {
            MPI_Finalize();
        }
#    endif
        //gsDebug << "Called MPI_Finalize on p=" << rank_ << "!" <<std::endl;
    }
    
    gsMpi(const gsMpi&);
    gsMpi& operator=(const gsMpi&);

};

}
