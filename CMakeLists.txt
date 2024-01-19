cmake_minimum_required(VERSION 3.27)

set(PROJECT_NAME portagecfg)
project(${PROJECT_NAME} LANGUAGES CXX)

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(PROJECT_SOURCES
	main.cpp
	portagecfg.hpp
	portagecfg.cpp
)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

install(
	TARGETS ${PROJECT_NAME}
)
