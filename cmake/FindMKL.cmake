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
include(FindPackageHandleStandardArgs)

if(NOT DEFINED MKL_ROOT AND NOT DEFINED ENV{MKLROOT})
  message(STATUS "MKLROOT env variable and MKL_ROOT are not defined, set to `${CMAKE_CURRENT_SOURCE_DIR}/../..` by default")
  set(MKL_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../..")
else()
  if(NOT DEFINED MKL_ROOT AND DEFINED ENV{MKLROOT})
    set(MKL_ROOT $ENV{MKLROOT})
  endif()
  string(REPLACE "\\" "/" MKL_ROOT ${MKL_ROOT})
  message(STATUS "MKL_ROOT ${MKL_ROOT}")
endif()

if(CMAKE_CXX_COMPILER MATCHES ".*dpcpp[.ex]*" OR ENABLE_OMP_OFFLOAD)
  set(MKL_DPCPP_INTERFACE ON)
endif()

set(MKL_SYCL              mkl_sycl)
set(MKL_IFACE_lp64        mkl_intel_lp64)
set(MKL_IFACE_ilp64       mkl_intel_ilp64)
set(MKL_IFACE_GNUlp64     mkl_gf_lp64)
set(MKL_IFACE_GNUilp64    mkl_gf_ilp64)
if(WIN32)
  set(MKL_IFACE_intel       mkl_intel_c)
else()
  set(MKL_IFACE_intel       mkl_intel)
endif()
set(MKL_IFACE_GNU         mkl_gf)
set(MKL_CORE              mkl_core)
set(MKL_THREAD_sequential mkl_sequential)
set(MKL_THREAD_intel_omp  mkl_intel_thread)
set(MKL_THREAD_pgi_omp    mkl_pgi_thread)
set(MKL_THREAD_gnu_omp    mkl_gnu_thread)
set(MKL_THREAD_tbb        mkl_tbb_thread)
set(MKL_SDL               mkl_rt)
if(MKL_TARGET_ARCH STREQUAL "ia32")
  set(MKL_BLAS95            mkl_blas95)
  set(MKL_LAPACK95          mkl_lapack95)
elseif(TARGET_INTERFACE)
  set(MKL_BLAS95            mkl_blas95_${TARGET_INTERFACE})
  set(MKL_LAPACK95          mkl_lapack95_${TARGET_INTERFACE})
endif()
if(TARGET_MPI AND TARGET_INTERFACE)
  set(MKL_BLACS             mkl_blacs_${TARGET_MPI}_${TARGET_INTERFACE})
  if(UNIX AND NOT APPLE AND TARGET_MPI MATCHES "mpich")
    # MPICH is compatible with INTELMPI Wrappers on Linux
    set(MKL_BLACS             mkl_blacs_intelmpi_${TARGET_INTERFACE})
  endif()
  if(WIN32 AND TARGET_MPI STREQUAL "msmpi")
    # Old and new names of Microsoft MPI are supported with one BLACS lib
    set(MKL_BLACS             mkl_blacs_msmpi_${TARGET_INTERFACE})
  endif()
  if(WIN32 AND NOT TARGET_LINK STREQUAL "static")
    set(MKL_BLACS             mkl_blacs_${TARGET_INTERFACE})
    if(TARGET_MPI STREQUAL "msmpi")
      set(MKL_BLACS_ENV MSMPI)
    elseif(TARGET_MPI STREQUAL "intelmpi")
      set(MKL_BLACS_ENV INTELMPI)
    endif()
  endif()


endif()
set(MKL_CDFT              mkl_cdft_core)
set(MKL_SCALAPACK         mkl_scalapack_${TARGET_INTERFACE})

if(UNIX)
  set(LIB_PREFIX "lib")
  set(LIB_EXT ".a")
  set(DLL_EXT ".so")
  if(APPLE)
    set(DLL_EXT ".dylib")
  endif()
  set(LINK_PREFIX "-l")
  set(LINK_SUFFIX "")
else()
  set(LIB_PREFIX "")
  set(LIB_EXT ".lib")
  set(DLL_EXT "_dll.lib")
  set(LINK_PREFIX "")
  set(LINK_SUFFIX ".lib")
endif()

set(MKL_COPT "")
set(MKL_LOPT "")
set(MKL_INCLUDE "")
set(MKL_LINK_PREFIX "")
set(SUPP_LINK "")
set(MKL_ENV "")
set(MKL_ENV_PATH "")

set(OLD_PATH $ENV{PATH})
string(REPLACE ";" "\;" OLD_PATH "${OLD_PATH}")

# Define additional compilation options
if(MKL_DPCPP_INTERFACE)
  if(MKL_HEADERS_ONLY)
    list(APPEND MKL_COPT -fsycl-unnamed-lambda)
  else()
    if(TARGET_LINK STREQUAL "static")
      list(APPEND MKL_LOPT "-fsycl-device-code-split=per_kernel")
    endif()
  endif()
endif()

if(ENABLE_OMP_OFFLOAD)
  list(APPEND MKL_ENV "OMP_TARGET_OFFLOAD=MANDATORY")
  if(WIN32)
    list(APPEND MKL_COPT /MD /Qiopenmp /Qopenmp-targets:spir64)
    list(APPEND MKL_LOPT /Qiopenmp /Qopenmp-targets:spir64 -fsycl)
    set(SKIP_LIBPATH ON)
  else()
    list(APPEND MKL_COPT -fiopenmp -fopenmp-targets=spir64)
    list(APPEND MKL_LOPT -fiopenmp -fopenmp-targets=spir64 -fsycl)
    if(APPLE)
      list(APPEND SUPP_LINK -lc++)
    else()
      list(APPEND SUPP_LINK -lstdc++)
    endif()
  endif()
endif()

# Define MKL headers
find_path(MKL_H mkl.h
  HINTS ${MKL_ROOT}
  PATH_SUFFIXES include)
list(APPEND MKL_INCLUDE ${MKL_H})

# Add pre-built F95 Interface Modules
if(CMAKE_Fortran_COMPILER_ID STREQUAL "Intel")
  if(ENABLE_F95_BLAS OR ENABLE_F95_LAPACK)
    if(MKL_TARGET_ARCH STREQUAL "intel64")
      list(APPEND MKL_INCLUDE "${MKL_ROOT}/include/${MKL_TARGET_ARCH}/${TARGET_INTERFACE}")
    else()
      list(APPEND MKL_INCLUDE "${MKL_ROOT}/include/${MKL_TARGET_ARCH}")
    endif()
  endif()
endif()
find_package_handle_standard_args(MKL REQUIRED_VARS MKL_INCLUDE)

# Interface selection
if(TARGET_INTERFACE)
  set(MKL_IFACE MKL_IFACE_)

  if(MKL_TARGET_ARCH STREQUAL "ia32")
    if(CMAKE_Fortran_COMPILER_ID STREQUAL "GNU")
      set(MKL_IFACE MKL_IFACE_GNU)
      set(MKL_IFACE_ENV "GNU,")
    else()
      set(MKL_IFACE MKL_IFACE_intel)
    endif()
    set(MKL_IFACE_ENV ${MKL_IFACE_ENV}"LP64")
  else()
    if(CMAKE_Fortran_COMPILER_ID STREQUAL "GNU")
      set(MKL_IFACE MKL_IFACE_GNU)
      set(MKL_IFACE_ENV "GNU,")
    endif()
    set(MKL_IFACE "${MKL_IFACE}${TARGET_INTERFACE}")
    set(MKL_IFACE_ENV "${MKL_IFACE_ENV}${TARGET_INTERFACE}")
    if(TARGET_INTERFACE STREQUAL "ilp64")
      if(TEST_LANG STREQUAL "Fortran")
        if(CMAKE_Fortran_COMPILER_ID STREQUAL "GNU")
          list(APPEND MKL_COPT "-fdefault-integer-8")
        else()
          if(WIN32 AND CMAKE_Fortran_COMPILER_ID STREQUAL "Intel")
            list(APPEND MKL_COPT "/4I8")
          else()
            list(APPEND MKL_COPT "-i8")
          endif()
        endif()
      else()
      list(APPEND MKL_COPT "-DMKL_ILP64")
    endif()
  endif()
  endif()

  set(MKL_IFACE ${${MKL_IFACE}})
  if(MKL_IFACE_ENV)
    string(TOUPPER ${MKL_IFACE_ENV} MKL_IFACE_ENV)
  endif()
endif() # TARGET_INTERFACE

# Threading selection
if(TARGET_THREADING)
  if(TARGET_THREADING STREQUAL "tbb")
    find_package(TBB MODULE REQUIRED)
    list(APPEND SUPP_LINK ${TBB_LINK})
    if(UNIX)
      if(APPLE)
        list(APPEND SUPP_LINK -lc++)
      else()
        list(APPEND SUPP_LINK -lstdc++)
      endif()
    endif()
    if(WIN32)
      set(MKL_ENV_PATH "${TBB_DLL_DIR}")
    endif()
  endif()
  if(TARGET_THREADING STREQUAL "intel_omp")
    find_package(IOMP MODULE REQUIRED)
    list(APPEND SUPP_LINK ${IOMP_LINK})
    if(WIN32)
      set(MKL_ENV_PATH "${IOMP_DLL_DIR}")
    endif()
  endif()
  if(TARGET_THREADING STREQUAL "gnu_omp")
    list(APPEND SUPP_LINK -lgomp)
  endif()
  if(TARGET_THREADING STREQUAL "pgi_omp")
    list(APPEND MKL_COPT -Mnokeepobj )
    list(APPEND SUPP_LINK -mp -pgf90libs)
  endif()

  set(MKL_THREAD ${MKL_THREAD_${TARGET_THREADING}})
  set(MKL_THREAD_ENV ${TARGET_THREADING})
  if(MKL_THREAD_ENV)
    string(REPLACE "_omp" "" MKL_THREAD_ENV ${MKL_THREAD_ENV})
    string(TOUPPER ${MKL_THREAD_ENV} MKL_THREAD_ENV)
  endif()
endif() # TARGET_THREADING

# Define Link Line

if(NOT MKL_HEADERS_ONLY)
  # Define link options
  if (UNIX)
    if(NOT APPLE AND TARGET_LINK STREQUAL "static")
      set(START_GROUP "-Wl,--start-group")
      set(END_GROUP "-Wl,--end-group")
      if(MKL_DPCPP_INTERFACE)
        set(EXPORT_DYNAMIC "-Wl,-export-dynamic")
      endif()
    endif()
    if(NOT APPLE AND TARGET_LINK STREQUAL "dynamic")
      if(CMAKE_Fortran_COMPILER_ID STREQUAL "GNU" OR CMAKE_Fortran_COMPILER_ID STREQUAL "PGI")
        set(NO_AS_NEEDED -Wl,--no-as-needed)
      endif()
    endif()
  endif()

  # Add DPC++ Interface library
  if(MKL_DPCPP_INTERFACE)
    list(APPEND MKL_LIBRARIES ${MKL_SYCL})
    list(APPEND MKL_LINK_PREFIX ${MKL_SYCL}_file)
    list(APPEND SUPP_LINK ${LINK_PREFIX}sycl${LINK_SUFFIX} ${LINK_PREFIX}OpenCL${LINK_SUFFIX})
  endif()

  # Add pre-built F95 Interface libraries
  if(CMAKE_Fortran_COMPILER_ID STREQUAL "Intel")
    if(ENABLE_F95_BLAS)
      list(APPEND MKL_LIBRARIES ${MKL_BLAS95})
      list(APPEND MKL_LINK_PREFIX ${MKL_BLAS95}_file)
    endif()
    if(ENABLE_F95_LAPACK)
      list(APPEND MKL_LIBRARIES ${MKL_LAPACK95})
      list(APPEND MKL_LINK_PREFIX ${MKL_LAPACK95}_file)
    endif()
  endif()

  # Add MPI Interface libraries
  set(MKL_BLACS_FILE "")
  set(MKL_CDFT_FILE "")
  if(ENABLE_BLACS OR ENABLE_CDFT OR ENABLE_SCALAPACK)
    # Custom BLACS library should be added manually
    if(MKL_BLACS AND NOT TARGET_MPI STREQUAL "custom")
      set(MKL_BLACS_FILE "${MKL_BLACS}_file")
      list(APPEND MKL_LIBRARIES ${MKL_BLACS})
    endif()
    if(ENABLE_CDFT)
      set(MKL_CDFT_FILE "${MKL_CDFT}_file")
      list(APPEND MKL_LIBRARIES ${MKL_CDFT})
    endif()
    if(ENABLE_SCALAPACK)
      list(APPEND MKL_LIBRARIES ${MKL_SCALAPACK})
      list(APPEND MKL_LINK_PREFIX ${MKL_SCALAPACK}_file)
    endif()
    if(WIN32 AND NOT TARGET_LINK STREQUAL "static")
      list(APPEND MKL_ENV "MKL_BLACS_MPI=${MKL_BLACS_ENV}")
    endif()
  endif()

  # Add main 3 libraries or single dynamic library
  if(TARGET_LINK STREQUAL "sdl")
    list(APPEND MKL_LIBRARIES ${MKL_SDL})
    list(APPEND MKL_LINK_PREFIX ${MKL_SDL}_file)
    list(APPEND MKL_ENV "MKL_INTERFACE_LAYER=${MKL_IFACE_ENV};MKL_THREADING_LAYER=${MKL_THREAD_ENV}")
  else()
    list(APPEND MKL_LIBRARIES ${MKL_IFACE} ${MKL_THREAD} ${MKL_CORE})
    list(APPEND MKL_LINK_PREFIX ${EXPORT_DYNAMIC} ${NO_AS_NEEDED} ${START_GROUP} ${MKL_CDFT_FILE} ${MKL_IFACE}_file ${MKL_THREAD}_file ${MKL_CORE}_file ${MKL_BLACS_FILE} ${END_GROUP})
  endif()

  # Find MKL libraries
  foreach(lib ${MKL_LIBRARIES})
    unset(${lib}_file CACHE)
    if(TARGET_LINK STREQUAL "static")
      find_library(${lib}_file ${LIB_PREFIX}${lib}${LIB_EXT}
        PATHS ${MKL_ROOT}
        PATH_SUFFIXES "lib" "lib/${MKL_TARGET_ARCH}")
    else()
      find_library(${lib}_file NAMES ${LIB_PREFIX}${lib}${DLL_EXT} ${lib}
        PATHS ${MKL_ROOT}
        PATH_SUFFIXES "lib" "lib/${MKL_TARGET_ARCH}")
    endif()
    find_package_handle_standard_args(MKL REQUIRED_VARS ${lib}_file)

    # ifx doesn't support full lib path on Windows
    if(WIN32 AND ENABLE_OMP_OFFLOAD AND CMAKE_Fortran_COMPILER MATCHES "ifx.exe")
	  get_filename_component(${lib}_name ${${lib}_file} NAME)
      list(TRANSFORM MKL_LINK_PREFIX REPLACE ${lib}_file ${${lib}_name})
    else()
      list(TRANSFORM MKL_LINK_PREFIX REPLACE ${lib}_file ${${lib}_file})
    endif()
  endforeach()

  # Add MKL dynamic libraries if RPATH is not defined on Unix
  if(UNIX AND CMAKE_SKIP_BUILD_RPATH)
    get_filename_component(MKL_LIB_DIR ${${MKL_CORE}_file} DIRECTORY)
    if(APPLE)
      list(APPEND MKL_ENV "DYLD_LIBRARY_PATH=${MKL_LIB_DIR}:$ENV{DYLD_LIBRARY_PATH}")
    else()
      list(APPEND MKL_ENV "LD_LIBRARY_PATH=${MKL_LIB_DIR}:$ENV{LD_LIBRARY_PATH}")
    endif()
  endif()

  # Add MKL dynamic libraries to PATH on Windows
  if(WIN32 AND NOT TARGET_LINK STREQUAL "static")
    file(GLOB MKL_CORE_DLL_FILE ${MKL_ROOT}/redist/${MKL_TARGET_ARCH}/mkl_core*.dll)
    if(NOT EXISTS ${MKL_CORE_DLL_FILE})
      # Old structure
      file(GLOB MKL_CORE_DLL_FILE ${MKL_ROOT}/../redist/${MKL_TARGET_ARCH}/mkl/mkl_core*.dll)
    endif()
    find_package_handle_standard_args(MKL REQUIRED_VARS MKL_CORE_DLL_FILE)
    get_filename_component(MKL_DLL_DIR ${MKL_CORE_DLL_FILE} DIRECTORY)
    set(MKL_ENV_PATH "${MKL_DLL_DIR}\;${MKL_ENV_PATH}")
  endif()

  if(UNIX)
    list(APPEND SUPP_LINK -lm -lpthread -ldl)
  endif()

  list(APPEND MKL_LINK_PREFIX ${SUPP_LINK})

  if(MKL_ENV_PATH)
    list(APPEND MKL_ENV "PATH=${MKL_ENV_PATH}\;${OLD_PATH}")
  endif()

endif() # NOT MKL_HEADERS_ONLY
