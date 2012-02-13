IF(MSVC)
	# msvc wants to "deprecate" standard C. Turn those warnings off.
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)
ENDIF()

IF(MSVC)
	SET(UPX ${DEPENDENCY_DIR}/upx/win/upx.exe)
	SET(PROGRAM_ENDING .exe)
ELSEIF(MINGW)
	SET(UPX ${DEPENDENCY_DIR}/upx/win/upx.exe)
	SET(PROGRAM_ENDING .exe)
ELSEIF(APPLE)
	SET(PROGRAM_ENDING )
ELSEIF(UNIX)
	SET(UPX upx)
	SET(PROGRAM_ENDING )
ENDIF()

SET(LIBTCOD_RADIOSITY_SRC main.cpp bsp_helper.cpp rad_shader.cpp rad_shader_photon.cpp rad_shader_standard.cpp)

add_executable(rad ${LIBTCOD_RADIOSITY_SRC})
target_link_libraries(rad ${LIBTCOD_NAME} ${LIBTCOD_CPP_NAME}) 

IF(UNIX)
	SET(CMAKE_EXE_LINKER_FLAGS "-Wl,-rpath,. ${CMAKE_EXE_LINKER_FLAGS}")
ENDIF()

set_property(TARGET rad PROPERTY DEBUG_POSTFIX "_debug")

IF(CMAKE_BUILD_TYPE MATCHES "Release")
	IF(NOT APPLE)
		ADD_CUSTOM_COMMAND(
	  	 TARGET rad
	  	 POST_BUILD
	   	 COMMAND ${UPX} rad${PROGRAM_ENDING}
		    VERBATIM
		)
	ENDIF()
ENDIF()

INSTALL(TARGETS rad
  RUNTIME DESTINATION ${INSTALL_DIR}
  LIBRARY DESTINATION ${INSTALL_DIR}
)
