cmake_minimum_required (VERSION 2.6)
project (libtcod)

# All projects need "include" directory
include_directories(include)

find_package(OpenGL REQUIRED)

SET(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
SET(INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR})
SET(DEPENDENCY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dependencies)
SET(GUI_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/gui)

OPTION(LIBTCOD_SAMPLES "Generate libtcod samples" ON)
OPTION(SWIG_CSHARP "Generate CSharp" OFF)
OPTION(SWIG_PYTHON "Generate Python" OFF)
OPTION(SWIG_LUA "Generate Lua" OFF)

IF(MSVC)
	SET(LIBTCOD_NAME libtcod-VS)
	SET(LIBTCOD_GUI_NAME libtcod-gui-VS)
	SET(ZLIB_DIR msvc)
	IF(CMAKE_SIZEOF_VOID_P EQUAL 8) 
		SET(LIB_DIR msvc64)
	ELSE()
		SET(LIB_DIR msvc32)
	ENDIF()
ELSEIF(MINGW)
	SET(LIBTCOD_NAME tcod-mingw)
	SET(LIBTCOD_GUI_NAME tcod-gui-mingw)
	SET(LIB_DIR mingw)
	SET(ZLIB_DIR mingw)
ELSEIF(APPLE)
	SET(LIBTCOD_NAME tcod)
	SET(LIBTCOD_GUI_NAME tcod-gui)
	SET(LIB_DIR osx)
	SET(ZLIB_DIR osx)
ELSEIF(UNIX)
	SET(LIBTCOD_NAME tcod)
	SET(LIBTCOD_CPP_NAME tcodxx)
	SET(LIBTCOD_GUI_NAME tcod-gui)
	SET(LIB_DIR linux)
	SET(ZLIB_DIR linux)
ENDIF()

#Linux uses system libraries
if(MSVC OR MINGW)
	# MSVC uses special SDL headers.
	IF(MSVC)
		include_directories(${DEPENDENCY_DIR}/SDL-1.2.15/include/msvc)
	ELSE()
		include_directories(${DEPENDENCY_DIR}/SDL-1.2.15/include)
	ENDIF()

	include_directories(${DEPENDENCY_DIR}/zlib-1.2.3/include)

	LINK_DIRECTORIES(${DEPENDENCY_DIR}/SDL-1.2.15/lib/${LIB_DIR}/ 
					${DEPENDENCY_DIR}/zlib-1.2.3/lib/${ZLIB_DIR}/)
ELSE()
	find_package(X11 REQUIRED)
	find_package(SDL REQUIRED)
	find_package(ZLIB REQUIRED)
	find_package(Threads REQUIRED)
	find_library(M_LIB m)
	set(EXTERNAL_LIBS ${X11_LIBRARIES}
			  ${SDL_LIBRARY} # this sucks, it's LIBRARY not LIBRARIES
			  ${ZLIB_LIBRARIES}
			  ${CMAKE_THREAD_LIBS_INIT} # this comes from find_package(Threads ..)
			  ${M_LIB_LIBRARIES}
			  ${OPENGL_LIBRARIES})
	set(EXTERNAL_INCLUDES ${X11_INCLUDE_DIRS}
			      ${SDL_INCLUDE_DIR} # again, singular...
			      ${ZLIB_INCLUDE_DIRS}
			      ${M_LIB_INCLUDE_DIRS}
			      ${X11_INCLUDE_DIRS}
			      ${OPENGL_INCLUDE_DIRS})
	IF(CMAKE_BUILD_TYPE MATCHES "Debug")
		find_library(EFENCE_LIBRARY efence PATHS /usr/lib /usr/local/lib)
	ENDIF()
ENDIF()

add_subdirectory(src) #Needs to come first since samples depend on it

# Use -DLIBTCOD_SAMPLES=OFF if you want do disable the building of the samples (eg. library API changes).
IF(LIBTCOD_SAMPLES)
add_subdirectory(samples)
add_subdirectory(samples/frost) # Frost effect demo
add_subdirectory(samples/navier) # Navier demo
add_subdirectory(samples/rad) # Radiosity demo
add_subdirectory(samples/ripples) # Water ripples demo
add_subdirectory(samples/weather) # Weather + Day/Night demo
add_subdirectory(samples/worldgen) # WorldGen demo
ENDIF()

# For swig to work you either need to set SWIG_PROGRAM_PATH (Windows) or SWIG_SYSTEM (Everywhere else).
# You also need to uncomment the add_subdirectory at the end of the file...

# This needs to be set to the path to a swig executable if you want swig support (and uncommented out).
OPTION(SWIG_ON "Run SWIG" OFF)
SET(SWIG_PROGRAM_PATH "C:\\Users\\donblas\\Programming\\swigwin-1.3.40" CACHE FILEPATH  "Path to SWIG")
IF(SWIG_ON)
	SET(SWIG_SYSTEM TRUE)
	add_subdirectory(swig)
ENDIF()


# BROKEN - These generators will need work to get working...
#SET(SWIG_JAVA TRUE)
#SET(SWIG_PHP TRUE)
#SET(SWIG_RUBY TRUE)
# Under Windows, the ruby find_package doesn't seem to work...
#SET(SWIG_RUBY_HEADER_PATH "C:\\Ruby19\\include\\ruby-1.9.1")
#SET(SWIG_RUBY_LIB_DIR "C:\\Ruby19\\lib\\")
#SET(SWIG_RUBY_LIB "libmsvcrt-ruby191-static.a")
# ENDBROKEN 
