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

cmake_minimum_required(VERSION 3.13)
enable_testing()

# Add cmake scripts and modules to CMake search path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Define language and compiler
set(TEST_LANG CXX)
set(TEST_EXT cpp)
include(setup_compiler)

project(MKL_Examples LANGUAGES ${TEST_LANG})

# Parse input parameters
include(check_param)
# Generate domainList and ${domain}_funcList
include(generate_examples_list)

# Not supported in DLL
if(WIN32 AND TARGET_LINK STREQUAL "dynamic")
  list(REMOVE_ITEM domainList "dft" "rng" "sparse_blas" "stats" "vml")
endif()

find_package(MKL MODULE REQUIRED)



# Override default compile/link lines
if(WIN32)
  set(CMAKE_CXX_STANDARD 14)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-function -w")
  foreach (flag_var
           CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
           CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
    string(REPLACE "/MD" "" ${flag_var} "${${flag_var}}")
  endforeach()
  set(CMAKE_CXX_COMPILE_OBJECT "<CMAKE_CXX_COMPILER> <DEFINES> <INCLUDES> ${DPCPP_COMPILE_FLAGS} <FLAGS> /Fo<OBJECT> -c <SOURCE>")
  set(CMAKE_CXX_CREATE_STATIC_LIBRARY "lib <OBJECTS> /out:<TARGET>")
  set(CMAKE_CXX_LINK_EXECUTABLE "<CMAKE_CXX_COMPILER> <OBJECTS> -o <TARGET> ${MKL_LOPT} <LINK_LIBRARIES>")
endif()



#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopenmp -lpthread ")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp -lpthread -I../../  -march=core-avx2 -O3")

if(FAIL_ON_MISSING_DEVICES)
  list(APPEND DEVICE_OPT -DFAIL_ON_MISSING_DEVICES)
endif()

foreach(device IN LISTS TARGET_DEVICES)
  list(APPEND DEVICE_OPT -DSYCL_DEVICES_${device})
endforeach()

# Define target for each function from each domain
if(domainList)
foreach(domain IN LISTS domainList)
  set(TEST_INCLUDE "")
  set(TEST_LOPT "")
  set(TEST_COPT "")

  list(APPEND TEST_INCLUDE "${PROJECT_SOURCE_DIR}/common" "${PROJECT_SOURCE_DIR}/${domain}")
  list(APPEND TEST_COPT ${DEVICE_OPT})
  # Some tests need this option as well
  if(NOT TARGET_LINK STREQUAL "static")
    list(APPEND TEST_LOPT "-fsycl-device-code-split=per_kernel")
  endif()

  # Build target for each example
  message(STATUS "Functions list ${domain}: ${${domain}_funcList}")
  foreach(func IN LISTS ${domain}_funcList)
    string(STRIP ${func} func_name)
    set(func "${domain}-${func_name}")

    if(func_name STREQUAL "soboluserdirnums-joe-kuo")
      configure_file(${PROJECT_SOURCE_DIR}/${domain}/data/${func_name}.dat ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
    endif()

    file(GLOB_RECURSE ${domain}_${func}_SRC ${PROJECT_SOURCE_DIR}/${domain}/*/${func_name}.${TEST_EXT})
    if(NOT ${domain}_${func}_SRC)
      message(FATAL_ERROR "${domain} source file ${func_name}.${TEST_EXT} was not found")
    endif()

    add_executable(${func} ${${domain}_${func}_SRC})
    target_include_directories(${func} PUBLIC ${TEST_INCLUDE} ${MKL_INCLUDE})
    target_compile_options(${func} PUBLIC ${TEST_COPT} ${MKL_COPT})
    target_link_options(${func} PUBLIC ${MKL_LOPT})
    target_link_libraries(${func} PUBLIC ${TEST_LOPT} ${MKL_LINK_PREFIX})

    # Register example as ctest
    add_test(NAME ${func} COMMAND ${func})

    # Add Environment variables
    if(MKL_ENV)
      set_tests_properties(${func} PROPERTIES ENVIRONMENT "${MKL_ENV}")
    endif()
  endforeach() #${domain}_funcList
endforeach() #domainList
endif() #not empty domainList
