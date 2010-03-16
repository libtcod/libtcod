cmake_minimum_required (VERSION 2.6)
project (libtcod)

# All projects need "include" directory
include_directories(include)

find_package(OpenGL REQUIRED)

SET(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
SET(INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR})
SET(DEPENDENCY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dependencies)
SET(GUI_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/gui)

IF(MSVC)
	SET(LIBTCOD_NAME libtcod-VS)
	SET(LIBTCOD_GUI_NAME libtcod-gui-VS)
	SET(LIB_DIR msvc)
ELSEIF(MINGW)
	SET(LIBTCOD_NAME tcod-mingw)
	SET(LIBTCOD_GUI_NAME tcod-gui-mingw)
	SET(LIB_DIR mingw)
ELSEIF(APPLE)
	SET(LIBTCOD_NAME tcod)
	SET(LIBTCOD_GUI_NAME tcod-gui)
	SET(LIB_DIR osx)
ELSEIF(UNIX)
	SET(LIBTCOD_NAME tcod)
	SET(LIBTCOD_CPP_NAME tcodxx)
	SET(LIBTCOD_GUI_NAME tcod-gui)
	SET(LIB_DIR linux)
ENDIF()

#Linux uses system libraries
if(MSVC OR MINGW)
	# MSVC uses special SDL headers.
	IF(MSVC)
		include_directories(${DEPENDENCY_DIR}/SDL-1.2.12/include/msvc)
	ELSE()
		include_directories(${DEPENDENCY_DIR}/SDL-1.2.12/include)
	ENDIF()

	include_directories(${DEPENDENCY_DIR}/zlib-1.2.3/include)
	include_directories(${DEPENDENCY_DIR}/libpng-1.2.31/include)

	LINK_DIRECTORIES(${DEPENDENCY_DIR}/libpng-1.2.31/lib/${LIB_DIR}/ ${DEPENDENCY_DIR}/SDL-1.2.12/lib/${LIB_DIR}/ 
					${DEPENDENCY_DIR}/zlib-1.2.3/lib/${LIB_DIR}/)
ELSE()
	find_package(SDL REQUIRED)
	find_package(PNG REQUIRED)
	find_package(ZLIB REQUIRED)
	find_package(Threads REQUIRED)
	find_library(M_LIB m)
	IF(CMAKE_BUILD_TYPE MATCHES "Debug")
		find_library(EFENCE_LIBRARY efence PATHS /usr/lib /usr/local/lib)
	ENDIF()
ENDIF()

add_subdirectory(src) #Needs to come first since samples depend on it
add_subdirectory(samples)


#This needs to be set to the path to a swig executable if you want swig support (and uncommented out).
#SET(SWIG_PROGRAM_PATH "C:\\Users\\donblas\\Programming\\swigwin-1.3.40")
#Python will need to be installed for SWIGing to it to work
#SET(SWIG_PYTHON TRUE)
#add_subdirectory(swig)