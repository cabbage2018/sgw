CMAKE_MINIMUM_REQUIRED(VERSION 3.5 FATAL_ERROR)

#Set verbose mode for make process
SET(CMAKE_VERBOSE_MAKEFILE ON)

#Project name 
PROJECT(inteagented VERSION "03.07.08.00" LANGUAGES C)
MESSAGE(STATUS "inteagented Version = ${PROJECT_VERSION}")
SET(CMAKE_C_STANDARD 99)
SET(CMAKE_C_STANDARD_REQUIRED ON)

#Required CURL and PahoC versions
#SET(CURL_VERSION_REQUIRED 7.52.1)
#SET(OPENSSL_VERSION_NUMBER_REQUIRED 1.0.2)
#SET(OPENSSL_VERSION_LETTER_REQUIRED "k")

#Set INTEAGENTED_SOURCES to collect source files from different locations
SET(INTEAGENTED_SOURCES "" CACHE INTERNAL "INTEAGENTED_SOURCES" FORCE)
SET(INTEAGENTED_LIBS "" CACHE INTERNAL "INTEAGENTED_LIBS" FORCE)
SET(INTEAGENTED_INCLUDE_DIRECTORIES "" CACHE INTERNAL "INTEAGENTED_INCLUDE_DIRECTORIES" FORCE)

#Set path of INTEAGENTED's root Cmake directory
SET(INTEAGENTED_CMAKE_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

INCLUDE(CheckIncludeFile)

#Check for header files
CHECK_INCLUDE_FILE("stdio.h" HAVE_STDIO_H_)
CHECK_INCLUDE_FILE("stddef.h" HAVE_STDDEF_H_)
CHECK_INCLUDE_FILE("string.h" HAVE_STRING_H_)
CHECK_INCLUDE_FILE("stdlib.h" HAVE_STDLIB_H_)
CHECK_INCLUDE_FILE("stdint.h" HAVE_STDINT_H_)
CHECK_INCLUDE_FILE("stdarg.h" HAVE_STDARG_H_)
CHECK_INCLUDE_FILE("syslog.h" HAVE_SYSLOG_H_)
CHECK_INCLUDE_FILE("time.h" HAVE_TIME_H_)

LIST(APPEND STANDARD_HEADER_MACROS HAVE_STDIO_H_ HAVE_STDDEF_H_ HAVE_STRING_H_ HAVE_STDLIB_H_ HAVE_STDINT_H_ HAVE_STDARG_H_ HAVE_TIME_H_)
FOREACH(STANDARD_HEADER_MACRO ${STANDARD_HEADER_MACROS})
    STRING(REPLACE "HAVE_" "" STANDARD_HEADER_FILE ${STANDARD_HEADER_MACRO})
    STRING(REPLACE "_H_" ".H" STANDARD_HEADER_FILE ${STANDARD_HEADER_FILE})  
    IF(${STANDARD_HEADER_MACRO})
        MESSAGE(STATUS "Standard header ${STANDARD_HEADER_FILE} is found.")  
    ELSE()
        MESSAGE(FATAL_ERROR "FATAL ERROR : Build process terminated because ${STANDARD_HEADER_FILE} is NOT found.")     
    ENDIF()    
ENDFOREACH()

#Option to mark that INTEAGENTED is build
OPTION(INTEAGENTED_BUILD "Option to mark that INTEAGENTED is build. This option is required e.g. to include inteagented_exports or not." ON)
IF(INTEAGENTED_BUILD)
	MESSAGE(STATUS "INTEAGENTED build is ${INTEAGENTED_BUILD}.")
	ADD_DEFINITIONS(-DINTEAGENTED_BUILD=1)
ENDIF()

#Option to set if INTEAGENTED build as static or dynamic
OPTION(INTEAGENTED_STATICLIB "Option to set if INTEAGENTED build as static or dynamic." OFF)
IF(INTEAGENTED_STATICLIB)
	SET(INTEAGENTED_USER_DEFINED_DYNAMIC_OR_STATIC STATIC)
	ADD_DEFINITIONS(-DINTEAGENTED_STATICLIB=1)
ELSE()
	SET(INTEAGENTED_USER_DEFINED_DYNAMIC_OR_STATIC SHARED)
	ADD_DEFINITIONS(-DINTEAGENTED_STATICLIB=0)
ENDIF()
MESSAGE(STATUS "INTEAGENTED will be build as ${INTEAGENTED_USER_DEFINED_DYNAMIC_OR_STATIC} library.")

#Option to enable or disable use of Libcurl
#OPTION(INTEAGENTED_USE_LIBCURL "Use Libcurl for HTTPS." ON)

#Check for PahoC
OPTION(INTEAGENTED_USE_PAHOC "Use PahoC code." ON)
OPTION(CMAKE_USE_PAHOC "Use PahoC code." ${INTEAGENTED_USE_OPENSSL})

#Define storage medium and file system presence on the target
OPTION(HAVE_FILE_SYSTEM_ "The target has a file system." ON)
OPTION(HAVE_STORAGE_MEDIUM_ "The target has a storage medium." ON)

#Set log level if provided during call of cmake
IF(INTEAGENTED_LOG_UTIL_LEVEL)
	MESSAGE(STATUS "Compile log level set to ${INTEAGENTED_LOG_UTIL_LEVEL}")
	ADD_DEFINITIONS(-DINTEAGENTED_LOG_UTIL_LEVEL=${INTEAGENTED_LOG_UTIL_LEVEL})
ENDIF()

#Set flags for coverage output
STRING(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_LOWER)
IF(CMAKE_COMPILER_IS_GNUCC AND (CMAKE_BUILD_TYPE_LOWER MATCHES debug))
    MESSAGE(STATUS "Coverage output with GNU CC generated")
    SET(GCC_COVERAGE_COMPILE_FLAGS "-fprofile-arcs -ftest-coverage")
    SET(GCC_COVERAGE_LINK_FLAGS "-lgcov")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")
ENDIF()

#Use solution folders.
SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)

#Set output folder for all targets
#First for the generic no-config case (e.g. with mingw)
SET(INTEAGENTED_OUTPUT_DIR ${CMAKE_BINARY_DIR}/build/${CMAKE_BUILD_TYPE})
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${INTEAGENTED_OUTPUT_DIR})
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${INTEAGENTED_OUTPUT_DIR})
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${INTEAGENTED_OUTPUT_DIR})
#Second, for multi-config builds (e.g. msvc)
FOREACH(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    STRING(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
    SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${INTEAGENTED_OUTPUT_DIR})
    SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${INTEAGENTED_OUTPUT_DIR})
    SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${INTEAGENTED_OUTPUT_DIR})
ENDFOREACH()

#Copy required libs to output folder
IF(WIN32 OR WIN64)
    MESSAGE(STATUS "INTEAGENTED_LIBS = ${INTEAGENTED_LIBS}")
    FOREACH(INTEAGENTED_LIB ${INTEAGENTED_LIBS})
		STRING(REPLACE ".lib" ".dll" INTEAGENTED_LIB ${INTEAGENTED_LIB})
		#STRING(REPLACE "lib/libcurl_imp" "bin/libcurl" INTEAGENTED_LIB ${INTEAGENTED_LIB})		
        IF(EXISTS ${INTEAGENTED_LIB})
			MESSAGE(STATUS "    Copying ${INTEAGENTED_LIB} to ${INTEAGENTED_OUTPUT_DIR}")
			FILE(COPY ${INTEAGENTED_LIB} DESTINATION ${INTEAGENTED_OUTPUT_DIR})
		ENDIF()
    ENDFOREACH()
ENDIF()

ADD_SUBDIRECTORY(lib/cJSON)
ADD_SUBDIRECTORY(lib/zf_log)

#INTEAGENTED Core sources
ADD_SUBDIRECTORY(src)

#Set variables for distribution package destination
SET(PACKAGE_DESTINATION_BIN "bin")
SET(PACKAGE_DESTINATION_INCLUDE "include")
SET(PACKAGE_DESTINATION_LIB "lib")

SET(CPACK_ARCHIVE_COMPONENT_INSTALL ON)
SET(BINARY_COMPONENT_NAME "binary")
SET(SOURCE_COMPONENT_NAME "source")
       
#Install directory for the header files
INSTALL(DIRECTORY "${INTEAGENTED_CMAKE_ROOT_DIR}/include/itag" DESTINATION ${PACKAGE_DESTINATION_INCLUDE} COMPONENT ${BINARY_COMPONENT_NAME})

#Install README document
INSTALL(FILES "${INTEAGENTED_CMAKE_ROOT_DIR}/README.md" DESTINATION . COMPONENT ${BINARY_COMPONENT_NAME})

#Install PahoC libraries if they are found
IF(PAHOC_FOUND)    
    IF(WIN32 OR WIN64)
        FOREACH(OPENSSL_LIB ${PAHOC_LIBRARIES})
    		STRING(REPLACE ".lib" ".dll" OPENSSL_LIB ${OPENSSL_LIB})
            IF(EXISTS ${OPENSSL_LIB})
    			INSTALL(FILES ${OPENSSL_LIB} DESTINATION ${PACKAGE_DESTINATION_BIN} COMPONENT ${BINARY_COMPONENT_NAME})
    		ENDIF()
        ENDFOREACH()
    ELSE()
        INSTALL(FILES ${PAHOC_LIBRARIES} DESTINATION ${PACKAGE_DESTINATION_LIB} COMPONENT ${BINARY_COMPONENT_NAME})    
    ENDIF()
ENDIF()

#Install top directory for source distribution
INSTALL(DIRECTORY "${INTEAGENTED_CMAKE_ROOT_DIR}" DESTINATION . COMPONENT ${SOURCE_COMPONENT_NAME})

#Set CPack configuration        
SET(CPACK_GENERATOR "ZIP")
SET(CPACK_PACKAGE_NAME "INTEAGENTED")
SET(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
SET(CPACK_OUTPUT_FILE_PREFIX ${CMAKE_BINARY_DIR}/dist)

SET(CPACK_COMPONENTS_ALL ${SOURCE_COMPONENT_NAME} ${BINARY_COMPONENT_NAME})
                                  
#Include CPack to create 'package' target
INCLUDE(CPack)
