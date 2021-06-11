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

# Define list of examples to run

# Custom list of examples
if (TARGET_FUNCTIONS)
  foreach(domain_func IN LISTS TARGET_FUNCTIONS)
    if(NOT domain_func MATCHES "/")
      message(FATAL_ERROR "All functions in TARGET_FUNCTIONS should be defined as domain/function")
    endif()
    string(REPLACE "/" ";" domain_func ${domain_func})
    list(GET domain_func 0 domain)
    list(GET domain_func 1 func)
    list(APPEND domainList "${domain}")
    list(APPEND ${domain}_funcList "${func}")
  endforeach()
else()
# Define default list of examples for selected domains
  # Select all domains in the directory if they are not defined by user
  if(NOT TARGET_DOMAINS)
    file(GLOB TARGET_DOMAINS RELATIVE ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/*)
    list(REMOVE_ITEM TARGET_DOMAINS build)
    foreach(dir IN LISTS TARGET_DOMAINS)
      # If folder contains domain_name.lst it will be included to domainList
      file(GLOB ${dir}_LISTS ${PROJECT_SOURCE_DIR}/${dir}/${dir}*.lst)
      if(${dir}_LISTS STREQUAL "")
        list(REMOVE_ITEM TARGET_DOMAINS ${dir})
      endif()
    endforeach()
  endif()
  set(domainList ${TARGET_DOMAINS})
  foreach(DOMAIN IN LISTS TARGET_DOMAINS)
    if(EXISTS ${PROJECT_SOURCE_DIR}/${DOMAIN}/${DOMAIN}.lst)
      file(STRINGS ${PROJECT_SOURCE_DIR}/${DOMAIN}/${DOMAIN}.lst ${DOMAIN}_funcList)
    endif()
    if(ENABLE_OMP_OFFLOAD)
      # Add examples lists for particular TARGET_OFFLOAD_PRECISION if they exist
      foreach(OFFLOAD_PRECISION IN LISTS TARGET_OFFLOAD_PRECISION)
        if(EXISTS ${PROJECT_SOURCE_DIR}/${DOMAIN}/${DOMAIN}_${OFFLOAD_PRECISION}.lst)
          file(STRINGS ${PROJECT_SOURCE_DIR}/${DOMAIN}/${DOMAIN}_${OFFLOAD_PRECISION}.lst ${DOMAIN}_${OFFLOAD_PRECISION}_funcList)
          list(APPEND ${DOMAIN}_funcList "${${DOMAIN}_${OFFLOAD_PRECISION}_funcList}")
        endif()
      endforeach()
    endif()
    # Add examples lists for particular TARGET_LINK if they exist
    if(EXISTS ${PROJECT_SOURCE_DIR}/${DOMAIN}/${DOMAIN}_${TARGET_LINK}.lst)
      file(STRINGS ${PROJECT_SOURCE_DIR}/${DOMAIN}/${DOMAIN}_${TARGET_LINK}.lst ${DOMAIN}_${TARGET_LINK}_funcList)
      list(APPEND ${DOMAIN}_funcList "${${DOMAIN}_${TARGET_LINK}_funcList}")
    endif()
    if(WIN32 AND EXISTS ${PROJECT_SOURCE_DIR}/${DOMAIN}/${DOMAIN}_win.lst)
      file(STRINGS ${PROJECT_SOURCE_DIR}/${DOMAIN}/${DOMAIN}_win.lst ${DOMAIN}_win_funcList)
      list(APPEND ${DOMAIN}_funcList "${${DOMAIN}_win_funcList}")
    endif()
    if(UNIX AND NOT APPLE AND EXISTS ${PROJECT_SOURCE_DIR}/${DOMAIN}/${DOMAIN}_lnx.lst)
      file(STRINGS ${PROJECT_SOURCE_DIR}/${DOMAIN}/${DOMAIN}_lnx.lst ${DOMAIN}_lnx_funcList)
      list(APPEND ${DOMAIN}_funcList "${${DOMAIN}_lnx_funcList}")
    endif()
    string(REPLACE "\\" "" ${DOMAIN}_funcList ${${DOMAIN}_funcList})
    list(FILTER ${DOMAIN}_funcList  EXCLUDE REGEX "=")
  endforeach()
endif()

# Cleanup duplications
list(REMOVE_DUPLICATES domainList)
foreach(d IN LISTS domainList)
  list(REMOVE_DUPLICATES ${d}_funcList)
endforeach()
