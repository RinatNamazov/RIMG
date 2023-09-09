# *****************************************************************************
# *
# *  PROJECT:       RIMG
# *  LICENSE:       See LICENSE in the top level directory
# *  FILE:          CMakeLists.txt
# *  DESCRIPTION:   Main cmake
# *  COPYRIGHT:     (c) 2020 RINWARES <rinwares.com>
# *  AUTHOR:        Rinat Namazov <namazov.rinat@rinwares.com>
# *
# *****************************************************************************

cmake_minimum_required(VERSION 3.16)
project("RIMG")

find_package(unofficial-tiny-aes-c CONFIG REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/src)

set(RIMG_APP_SOURCES
	src/app/main.cpp
	src/app/main.hpp

	src/misc/keys.hpp
	src/misc/types.hpp
)

add_executable(rimg_app ${RIMG_APP_SOURCES})
target_link_libraries(rimg_app PRIVATE unofficial::tiny-aes-c::tiny-aes-c)

set(RIMG_PLUGIN_SOURCES
	src/plugin/dllmain.cpp
	src/plugin/dllmain.hpp
	src/plugin/gta_misc.cpp
	src/plugin/gta_misc.hpp
	src/plugin/Plugin.cpp
	src/plugin/Plugin.hpp
	src/plugin/utils.hpp
	src/plugin/utils.cpp

	src/misc/keys.hpp
	src/misc/types.hpp
)

add_library(rimg_plugin SHARED ${RIMG_PLUGIN_SOURCES})
target_link_libraries(rimg_plugin PRIVATE unofficial::tiny-aes-c::tiny-aes-c)
set_target_properties(
	rimg_plugin
	PROPERTIES
		SUFFIX ".asi"
)

install(TARGETS rimg_app rimg_plugin DESTINATION .)
