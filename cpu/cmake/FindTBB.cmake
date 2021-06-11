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

if(NOT DEFINED TBB_ROOT AND DEFINED ENV{TBBROOT})
  set(TBB_ROOT $ENV{TBBROOT})
  message(TBB_ROOT=${TBB_ROOT})
endif()

if(UNIX)
  if(APPLE)
    set(TBB_LIBNAME libtbb.dylib)
  else()
    set(TBB_LIBNAME libtbb.so)
  endif()
else()
  set(TBB_LIBNAME tbb.lib)
  set(TBB_DLLNAME tbb[0-9][0-9].dll)
endif()

find_path(TBB_LIB_DIR ${TBB_LIBNAME}
      HINTS ${TBB_ROOT} ${MKL_ROOT}
      PATH_SUFFIXES "lib" "lib/${MKL_TARGET_ARCH}/gcc4.8" "lib/${MKL_TARGET_ARCH}/vc14"
          "../../tbb/latest/lib/${MKL_TARGET_ARCH}/gcc4.8" "../tbb/lib/${MKL_TARGET_ARCH}/gcc4.8"
          "../../tbb/latest/lib/${MKL_TARGET_ARCH}/vc14" "../tbb/lib/${MKL_TARGET_ARCH}/vc14"
          "../../tbb/latest/lib" "../tbb/lib"
)

if(WIN32)
  # search for tbb*.dll in TBB_ROOT first
  if(TBB_ROOT)
    file(GLOB TBB_DLL_FILE ${TBB_ROOT}/redist/${MKL_TARGET_ARCH}/vc14/${TBB_DLLNAME})
    if(NOT EXISTS ${TBB_DLL_FILE})
      file(GLOB TBB_DLL_FILE ${TBB_ROOT}/../redist/${MKL_TARGET_ARCH}/tbb/vc14/${TBB_DLLNAME})
    endif()
  endif()
  # if not found search in MKL_ROOT
  if(NOT EXISTS ${TBB_DLL_FILE})
    file(GLOB TBB_DLL_FILE ${MKL_ROOT}/../../tbb/latest/redist/${MKL_TARGET_ARCH}/vc14/${TBB_DLLNAME})
    if(NOT EXISTS ${TBB_DLL_FILE})
      file(GLOB TBB_DLL_FILE ${MKL_ROOT}/../redist/${MKL_TARGET_ARCH}/tbb/vc14/${TBB_DLLNAME})
    endif()
  endif()
  get_filename_component(TBB_DLL_DIR ${TBB_DLL_FILE} DIRECTORY)
  message(TBB_DLL_DIR=${TBB_DLL_DIR})
endif()

find_library(TBB_LIBRARIES NAMES tbb
        HINTS $ENV{TBBROOT} $ENV{MKLROOT} ${MKL_ROOT} ${TBB_ROOT}
        PATH_SUFFIXES "lib" "lib/${MKL_TARGET_ARCH}/gcc4.8" "lib/${MKL_TARGET_ARCH}/vc14"
          "../../tbb/latest/lib/${MKL_TARGET_ARCH}/gcc4.8" "../tbb/lib/${MKL_TARGET_ARCH}/gcc4.8"
          "../../tbb/latest/lib/${MKL_TARGET_ARCH}/vc14" "../tbb/lib/${MKL_TARGET_ARCH}/vc14"
          "../../tbb/latest/lib" "../tbb/lib"
)

if(UNIX)
  if(CMAKE_SKIP_BUILD_RPATH)
    set(TBB_LINK "-L${TBB_LIB_DIR} -ltbb")
  else()
    set(TBB_LINK "-Wl,-rpath,${TBB_LIB_DIR} -L${TBB_LIB_DIR} -ltbb")
  endif()
else()
  set(TBB_LINK "${TBB_LIBRARIES}")
endif()
include(FindPackageHandleStandardArgs)
if(WIN32)
  find_package_handle_standard_args(TBB REQUIRED_VARS TBB_LIBRARIES TBB_LINK TBB_DLL_DIR)
else()
  find_package_handle_standard_args(TBB REQUIRED_VARS TBB_LIBRARIES TBB_LINK)
endif()

