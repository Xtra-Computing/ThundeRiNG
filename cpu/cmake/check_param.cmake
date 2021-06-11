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

function(define_param TARGET_PARAM DEFAULT_PARAM SUPPORTED_LIST)
  if(NOT DEFINED ${TARGET_PARAM} AND NOT DEFINED ${DEFAULT_PARAM})
    message(STATUS "${TARGET_PARAM}: Undefined")
  elseif(NOT DEFINED ${TARGET_PARAM} AND DEFINED ${DEFAULT_PARAM})
    set(${TARGET_PARAM} "${${DEFAULT_PARAM}}" CACHE STRING "Choose ${TARGET_PARAM} options are: ${${SUPPORTED_LIST}}")
    foreach(opt ${${DEFAULT_PARAM}})
      set(STR_LIST "${STR_LIST} ${opt}")
    endforeach()
    message(STATUS "${TARGET_PARAM}: None, set to `${STR_LIST}` by default")
  elseif(${SUPPORTED_LIST})
    set(ITEM_FOUND 1)
    foreach(opt ${${TARGET_PARAM}})
      if(NOT ${opt} IN_LIST ${SUPPORTED_LIST})
        set(ITEM_FOUND 0)
      endif()
    endforeach()
    if(ITEM_FOUND EQUAL 0)
      foreach(opt ${${SUPPORTED_LIST}})
        set(STR_LIST "${STR_LIST} ${opt}")
      endforeach()
      message(FATAL_ERROR "Invalid ${TARGET_PARAM} `${${TARGET_PARAM}}`, options are: ${STR_LIST}")
    else()
      message(STATUS "${TARGET_PARAM}: ${${TARGET_PARAM}}")
    endif()
  else()
    message(STATUS "${TARGET_PARAM}: ${${TARGET_PARAM}}")
  endif()
endfunction()

if(ENABLE_BLACS OR ENABLE_CDFT OR ENABLE_SCALAPACK)
  set(ENABLE_MPI ON)
endif()
# Check/define MKL_TARGET_ARCH
set(DEFAULT_TARGET_ARCH "intel64")
# DPC++, OpenMP offload, and MPI support only intel64
if(CMAKE_CXX_COMPILER MATCHES ".*dpcpp[.exe]*" OR CMAKE_${TEST_LANG}_COMPILER_ID STREQUAL "PGI"
        OR ENABLE_OMP_OFFLOAD OR ENABLE_MPI)
  set(TARGET_ARCH_LIST intel64)
else()
  set(TARGET_ARCH_LIST ia32 intel64)
endif()
define_param(MKL_TARGET_ARCH DEFAULT_TARGET_ARCH TARGET_ARCH_LIST)

if(ENABLE_OMP_OFFLOAD)
  set(DEFAULT_TARGET_OFFLOAD_PRECISION sp dp)
  set(TARGET_OFFLOAD_PRECISION_LIST sp dp)
  define_param(TARGET_OFFLOAD_PRECISION DEFAULT_TARGET_OFFLOAD_PRECISION TARGET_OFFLOAD_PRECISION_LIST)
endif()

# Define additional parameters for selected compiler based on arch
if(CMAKE_${TEST_LANG}_COMPILER_ID STREQUAL "PGI")
  message("CMAKE_${TEST_LANG}_FLAGS")
    set(CMAKE_${TEST_LANG}_FLAGS -m64)
elseif(CMAKE_${TEST_LANG}_COMPILER_ID STREQUAL "GNU")
  if(MKL_TARGET_ARCH STREQUAL "ia32")
    set(CMAKE_${TEST_LANG}_FLAGS -m32)
  else()
    set(CMAKE_${TEST_LANG}_FLAGS -m64)
  endif()
endif()

if(NOT MKL_HEADERS_ONLY)

# Check/define TARGET_LINK
set(DEFAULT_TARGET_LINK dynamic)
if(CMAKE_CXX_COMPILER MATCHES ".*dpcpp[.exe]*" OR ENABLE_MPI)
  set(TARGET_LINK_LIST static dynamic)
else()
  set(TARGET_LINK_LIST static dynamic sdl)
endif()
define_param(TARGET_LINK DEFAULT_TARGET_LINK TARGET_LINK_LIST)

# Check/define TARGET_INTERFACE
if(MKL_TARGET_ARCH STREQUAL "intel64" AND NOT CMAKE_CXX_COMPILER MATCHES ".*dpcpp[.exe]*")
  set(DEFAULT_TARGET_INTERFACE ilp64)
  set(TARGET_INTERFACE_LIST ilp64 lp64)
  define_param(TARGET_INTERFACE DEFAULT_TARGET_INTERFACE TARGET_INTERFACE_LIST)
elseif(CMAKE_CXX_COMPILER MATCHES ".*dpcpp[.exe]*")
 set(TARGET_INTERFACE ilp64)
else()
 set(TARGET_INTERFACE lp64)
endif()

# Check/define TARGET_THREADING
if(CMAKE_CXX_COMPILER MATCHES ".*dpcpp[.exe]*")
  set(DEFAULT_TARGET_THREADING tbb)
  set(TARGET_THREADING_LIST sequential tbb)
else()
  if(WIN32 AND CMAKE_${TEST_LANG}_COMPILER_ID STREQUAL "PGI")
    set(DEFAULT_TARGET_THREADING pgi_omp)
  else()
    set(DEFAULT_TARGET_THREADING intel_omp)
  endif()
  if(ENABLE_OMP_OFFLOAD)
    set(TARGET_THREADING_LIST sequential intel_omp)
  else()
    set(TARGET_THREADING_LIST sequential)
    if(CMAKE_${TEST_LANG}_COMPILER_ID STREQUAL "PGI")
      list(APPEND TARGET_THREADING_LIST pgi_omp)
      if(NOT WIN32)
        list(APPEND TARGET_THREADING_LIST intel_omp)
      endif()
    else()
      list(APPEND TARGET_THREADING_LIST tbb intel_omp)
    endif()
    if(CMAKE_${TEST_LANG}_COMPILER_ID STREQUAL "GNU")
      list(APPEND TARGET_THREADING_LIST gnu_omp)
    endif()
  endif()
endif()
define_param(TARGET_THREADING DEFAULT_TARGET_THREADING TARGET_THREADING_LIST)

# Check/define TARGET_MPI
if(ENABLE_MPI)
  if(APPLE)
    set(DEFAULT_TARGET_MPI mpich)
    set(TARGET_MPI_LIST mpich)
  else()
    set(DEFAULT_TARGET_MPI intelmpi)
    if(WIN32)
      set(TARGET_MPI_LIST intelmpi mshpc msmpi)
    elseif(UNIX)
      set(TARGET_MPI_LIST intelmpi openmpi mpich mpich2)
    endif()
  endif()
  define_param(TARGET_MPI DEFAULT_TARGET_MPI TARGET_MPI_LIST)
  if(TARGET_MPI STREQUAL "mshpc")
    set(TARGET_MPI msmpi)
  endif()
endif()

endif() # MKL_HEADERS_ONLY

# Check/define TARGET_DEVICES
if(CMAKE_CXX_COMPILER MATCHES ".*dpcpp[.exe]*")
  if(TARGET_DEVICES)
    # Make sure the input was converted to list
    string(REPLACE " " ";" TARGET_DEVICES ${TARGET_DEVICES})
  endif()
  set(DEFAULT_TARGET_DEVICES cpu gpu)
  set(TARGET_DEVICES_LIST host cpu gpu)
  define_param(TARGET_DEVICES DEFAULT_TARGET_DEVICES TARGET_DEVICES_LIST)
endif()

# Check/define TARGET_DOMAINS
# TARGET_DOMAINS_LIST can be any, all by default
define_param(TARGET_DOMAINS DEFAULT_TARGET_DOMAINS TARGET_DOMAINS_LIST)
# Make sure the input was converted to list
if(TARGET_DOMAINS)
    string(REPLACE " " ";" TARGET_DOMAINS ${TARGET_DOMAINS})
endif()

# Define TARGET_FUNCTIONS
# TARGET_FUNCTIONS_LIST can be any, all by default
define_param(TARGET_FUNCTIONS DEFAULT_TARGET_FUNCTIONS TARGET_FUNCTIONS_LIST)
# Make sure the input was converted to list
if(TARGET_FUNCTIONS)
    string(REPLACE " " ";" TARGET_FUNCTIONS ${TARGET_FUNCTIONS})
endif()

