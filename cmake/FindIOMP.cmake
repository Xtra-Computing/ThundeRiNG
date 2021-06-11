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

if(UNIX)
  if(APPLE)
    set(OMP_LIBNAME libiomp5.dylib)
  else()
    set(OMP_LIBNAME libiomp5.so)
  endif()
else()
  set(OMP_LIBNAME libiomp5md.lib)
  set(OMP_DLLNAME libiomp5md.dll)
endif()

find_path(IOMP_LIB_DIR ${OMP_LIBNAME}
      HINTS $ENV{LIB} $ENV{LIBRARY_PATH} $ENV{MKLROOT} ${MKL_ROOT} ${CMPLR_ROOT}
      PATH_SUFFIXES "lib" "lib/${MKL_TARGET_ARCH}" "lib/${MKL_TARGET_ARCH}_win"
               "linux/compiler/lib/${MKL_TARGET_ARCH}"
               "windows/compiler/lib/${MKL_TARGET_ARCH}_win"
               "../compiler/lib/${MKL_TARGET_ARCH}" "../compiler/lib"
               "../../compiler/latest/linux/compiler/lib/${MKL_TARGET_ARCH}"
               "../../compiler/latest/windows/compiler/lib/${MKL_TARGET_ARCH}_win"
               "../../compiler/latest/mac/compiler/lib"
)

if(WIN32)
  find_path(IOMP_DLL_DIR ${OMP_DLLNAME}
      HINTS $ENV{LIB} $ENV{LIBRARY_PATH} $ENV{MKLROOT} ${MKL_ROOT} ${CMPLR_ROOT}
      PATH_SUFFIXES "redist/${MKL_TARGET_ARCH}"
              "redist/${MKL_TARGET_ARCH}_win" "redist/${MKL_TARGET_ARCH}_win/compiler"
               "../redist/${MKL_TARGET_ARCH}/compiler" "../compiler/lib"
               "../../compiler/latest/windows/compiler/redist/${MKL_TARGET_ARCH}_win"
               "../../compiler/latest/windows/compiler/redist/${MKL_TARGET_ARCH}_win/compiler"
  )
endif()

find_library(IOMP_LIBRARIES ${OMP_LIBNAME}
      HINTS $ENV{LIB} $ENV{LIBRARY_PATH} $ENV{MKLROOT} ${MKL_ROOT} ${CMPLR_ROOT}
      PATH_SUFFIXES "lib/${MKL_TARGET_ARCH}" "lib/${MKL_TARGET_ARCH}_win"
               "linux/compiler/lib/${MKL_TARGET_ARCH}"
               "windows/compiler/lib/${MKL_TARGET_ARCH}_win"
               "../compiler/lib/${MKL_TARGET_ARCH}" "../compiler/lib"
               "../../compiler/latest/linux/compiler/lib/${MKL_TARGET_ARCH}"
               "../../compiler/latest/windows/compiler/lib/${MKL_TARGET_ARCH}_win"
               "../../compiler/latest/mac/compiler/lib"
)

#Workaround for ref problem
if(UNIX)
  if(CMAKE_SKIP_BUILD_RPATH)
    set(IOMP_LINK "-L${IOMP_LIB_DIR} -liomp5")
  else()
    set(IOMP_LINK "-Wl,-rpath,${IOMP_LIB_DIR} -L${IOMP_LIB_DIR} -liomp5")
  endif()
else()
  if(SKIP_LIBPATH)
    set(IOMP_LINK "libiomp5md.lib")
  else()
    set(IOMP_LINK "-LIBPATH:\"${IOMP_LIB_DIR}\" libiomp5md.lib")
  endif()
endif()
include(FindPackageHandleStandardArgs)
if(WIN32)
  find_package_handle_standard_args(IOMP REQUIRED_VARS IOMP_LIBRARIES IOMP_LINK IOMP_DLL_DIR)
else()
  find_package_handle_standard_args(IOMP REQUIRED_VARS IOMP_LIBRARIES IOMP_LINK)
endif()

