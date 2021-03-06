# Copyright 2006-2012 The FLWOR Foundation.
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
# http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

IF (PHP5_FOUND)
  MESSAGE (STATUS "SWIG: Generating PHP5 API")

  SET_SOURCE_FILES_PROPERTIES (zorba_api.i PROPERTIES CPLUSPLUS ON)
  INCLUDE_DIRECTORIES (${PHP5_INCLUDES})
  IF (WIN32)
    INCLUDE_DIRECTORIES(${PHP5_MAIN_INCLUDE_DIR})
    INCLUDE_DIRECTORIES(${PHP5_TSRM_INCLUDE_DIR})
    INCLUDE_DIRECTORIES(${PHP5_ZEND_INCLUDE_DIR})
    ADD_DEFINITIONS(-DZEND_WIN32 -DPHP_WIN32 -DZEND_INCLUDE_FULL_WINDOWS_HEADERS -DZEND_DEBUG=0 -DZTS=1)
  ENDIF (WIN32)

  SWIG_ADD_MODULE (zorba_api_php php5 zorba_api.i)

  IF (APPLE)
    SWIG_LINK_LIBRARIES (zorba_api_php zorba_simplestore ${CMAKE_SHARED_MODULE_CREATE_C_FLAGS})
  ELSE (APPLE)
    SWIG_LINK_LIBRARIES (zorba_api_php zorba_simplestore)
  ENDIF (APPLE)

  #fix for library path, using windows backslash causes error
  IF (WIN32)
    STRING(REGEX REPLACE "\\\\" "/" PHP5_LIBRARY "${PHP5_LIBRARY}")
    SWIG_LINK_LIBRARIES (zorba_api_php ${PHP5_LIBRARY})
  ENDIF (WIN32)

  #Copy test to the build folder
  CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/test.xq" "${CMAKE_CURRENT_BINARY_DIR}/test.xq" COPYONLY)
  
  #Copy alternate files
  CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/XQueryProcessor.php" "${CMAKE_CURRENT_BINARY_DIR}/Zorba/XQueryProcessor.php" COPYONLY)
  #fix for swig error with wrapper name
  ADD_CUSTOM_COMMAND(
    TARGET zorba_api_php
    POST_BUILD
    COMMAND cmake -E echo "Fixing PHP wrapper..."
    COMMAND cmake -P "${CMAKE_CURRENT_SOURCE_DIR}/postfix.cmake"
  )

  FILE(GLOB ZORBA_API_HEADERS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/../" "${CMAKE_CURRENT_SOURCE_DIR}/../*.h")
  FOREACH(ZORBA_API_HEADER ${ZORBA_API_HEADERS})
    CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/../${ZORBA_API_HEADER}"  "${CMAKE_CURRENT_BINARY_DIR}/${ZORBA_API_HEADER}")
  ENDFOREACH(ZORBA_API_HEADER ${ZORBA_API_HEADERS})

  #set project folder location for IDEs
  SET_TARGET_PROPERTIES(zorba_api_php PROPERTIES PROJECT_LABEL "PHP" FOLDER "APIs")

  IF ( APPLE )
    SET_TARGET_PROPERTIES(zorba_api_php PROPERTIES OUTPUT_NAME "zorba_api_php")
  ELSE ( APPLE )
    SET_TARGET_PROPERTIES(zorba_api_php PROPERTIES OUTPUT_NAME "zorba_api")
  ENDIF ( APPLE )
  
  IF (NOT WIN32 OR CYGWIN)
    STRING (REPLACE ${CMAKE_INSTALL_PREFIX} "" PHP5_INSTALL_PATH ${PHP5_EXTENSION_DIR})
    SET (PHP5_INSTALL_PATH "${CMAKE_INSTALL_PREFIX}/share/php5")

    INSTALL (
      FILES
        ${CMAKE_CURRENT_BINARY_DIR}/Zorba/XQueryProcessor.php
        ${CMAKE_CURRENT_BINARY_DIR}/Zorba/zorba_api_wrapper.php
        ${CMAKE_CURRENT_BINARY_DIR}/libzorba_api_php.so
      DESTINATION ${PHP5_INSTALL_PATH}
    )
  ELSE (NOT WIN32 OR CYGWIN)
    IF (MSVC_IDE)
      INSTALL ( FILES 
        ${CMAKE_CURRENT_BINARY_DIR}/Zorba/XQueryProcessor.php
        ${CMAKE_CURRENT_BINARY_DIR}/Zorba/zorba_api_wrapper.php
        ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/zorba_api_php.dll
        COMPONENT "php_swig"
        DESTINATION share/php )
    ELSE (MSVC_IDE)
      INSTALL ( FILES 
        ${CMAKE_CURRENT_BINARY_DIR}/Zorba/XQueryProcessor.php
        ${CMAKE_CURRENT_BINARY_DIR}/Zorba/zorba_api_wrapper.php
        ${CMAKE_CURRENT_BINARY_DIR}/zorba_api_php.dll
        COMPONENT "php_swig"
        DESTINATION share/php )
    ENDIF (MSVC_IDE)
  ENDIF (NOT WIN32 OR CYGWIN)
  ADD_SUBDIRECTORY (tests)

ELSE (PHP5_FOUND)
  MESSAGE (STATUS "SWIG: PHP5 binding not generated because library and include file are not installed.")
ENDIF (PHP5_FOUND)
