#===============================================================================
# Copyright 2020-2021 Intel Corporation.
#
# This software and the related documents are Intel copyrighted  materials,  and
# your use of  them is  governed by the  express license  under which  they were
# provided to you (License).  Unless the License provides otherwise, you may not
# use, modify, copy, publish, distribute,  disclose or transmit this software or
# the related documents without Intel's prior written permission.
#
# This software and the related documents  are provided as  is,  with no express
# or implied  warranties,  other  than those  that are  expressly stated  in the
# License.
#===============================================================================

include_guard()

function(define_compiler TARGET_COMPILER DEFAULT_COMPILER SUPPORTED_LIST)
  if(NOT DEFINED ${TARGET_COMPILER})
    set(${TARGET_COMPILER} "${${DEFAULT_COMPILER}}" PARENT_SCOPE)
    message(STATUS "${TARGET_COMPILER}: None, set to `${${DEFAULT_COMPILER}}` by default")
  elseif(${TARGET_COMPILER} IN_LIST ${SUPPORTED_LIST})
    message(STATUS "${TARGET_COMPILER}: `${${TARGET_COMPILER}}`")
  elseif(ENABLE_MPI AND ${TARGET_COMPILER} MATCHES "mpi*")
    message(STATUS "${TARGET_COMPILER}: `${${TARGET_COMPILER}}`")
  else()
    foreach(opt ${${SUPPORTED_LIST}})
      set(STR_LIST "${STR_LIST} ${opt}")
    endforeach()
    message(FATAL_ERROR "Unsupported ${TARGET_COMPILER} `${${TARGET_COMPILER}}`, supported: ${STR_LIST}")
  endif()
endfunction()

if(ENABLE_BLACS OR ENABLE_CDFT OR ENABLE_SCALAPACK)
  set(ENABLE_MPI ON)
endif()
# Define C compiler
if("C" IN_LIST TEST_LANG)
  if(ENABLE_OMP_OFFLOAD)
    set(CMAKE_C_COMPILER "icx") # OpenMP Offload is supported only by icx
    message(STATUS "CMAKE_C_COMPILER: `${CMAKE_C_COMPILER}`")
  else()
    if(UNIX)
      set(DEFAULT_C_COMPILER "icc")
      set(C_COMPILER_LIST icc gcc clang pgcc icx)
      if(APPLE)
        set(C_COMPILER_LIST icc clang)
      endif()
    else()
      set(DEFAULT_C_COMPILER "icl")
      set(C_COMPILER_LIST icl cl pgcc icx)
    endif()
    if(ENABLE_MPI)
      list(APPEND C_COMPILER_LIST mpi*)
    endif()
    define_compiler(CMAKE_C_COMPILER DEFAULT_C_COMPILER C_COMPILER_LIST)
  endif() # ENABLE_OMP_OFFLOAD
endif()

# Define Fortran compiler
if("Fortran" IN_LIST TEST_LANG)
  if(ENABLE_OMP_OFFLOAD)
    set(CMAKE_Fortran_COMPILER "ifx") # OpenMP Offload is supported only by ifx
    message(STATUS "CMAKE_Fortran_COMPILER: `${CMAKE_Fortran_COMPILER}`")
  else()
    set(DEFAULT_F_COMPILER "ifort")
    set(F_COMPILER_LIST ifort pgf95 ifx)
    if(UNIX)
      list(APPEND F_COMPILER_LIST "gfortran")
    endif()
    if(ENABLE_MPI)
      list(APPEND F_COMPILER_LIST mpi*)
    endif()

    define_compiler(CMAKE_Fortran_COMPILER DEFAULT_F_COMPILER F_COMPILER_LIST)

  endif() # ENABLE_OMP_OFFLOAD
  # Some Fortran examples need C wrappers to build, define C compiler for them
  if(C_WRAPPERS)
    if(CMAKE_Fortran_COMPILER STREQUAL "ifort")
      if(WIN32)
        set(CMAKE_C_COMPILER "icl")
      else()
        set(CMAKE_C_COMPILER "icc")
      endif()
    elseif(CMAKE_Fortran_COMPILER STREQUAL "pgf95")
      set(CMAKE_C_COMPILER "pgcc")
    elseif(CMAKE_Fortran_COMPILER STREQUAL "ifx")
      set(CMAKE_C_COMPILER "icx")
    elseif(CMAKE_Fortran_COMPILER STREQUAL "gfortran")
      set(CMAKE_C_COMPILER "gcc")
    elseif(CMAKE_Fortran_COMPILER MATCHES "mpi")
      if(WIN32)
        set(CMAKE_C_COMPILER "mpicc.bat")
      else()
        set(CMAKE_C_COMPILER "mpicc")
      endif()
    endif()
  endif()
endif()

if(CMAKE_${TEST_LANG}_COMPILER MATCHES "mpi")
  set(USE_MPI_COMPILER ON)
endif()

# Define C++ compiler
if("CXX" IN_LIST TEST_LANG)
  set(CMAKE_CXX_COMPILER "dpcpp") # Compiler for DPC++ examples
endif()

set(CMAKE_BUILD_TYPE Release)
