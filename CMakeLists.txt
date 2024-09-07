cmake_minimum_required(VERSION 3.28)

project(portagecfg
    VERSION 2.2.2
    LANGUAGES CXX
    DESCRIPTION "Utility to write Portage config files in an easy manner."
)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_definitions(PROJECT_NAME="${PROJECT_NAME}")
add_compile_definitions(PROJECT_VERSION="${PROJECT_VERSION}")
add_compile_definitions(PROJECT_DESCRIPTION="${PROJECT_DESCRIPTION}")
add_compile_definitions(PORTAGE_REPOS_DIR="/var/db/repos/")
add_compile_definitions(RED="\\033[1;31m")
add_compile_definitions(GREEN="\\033[1;32m")
add_compile_definitions(YELLOW="\\033[1;33m")
add_compile_definitions(BLUE="\\033[1;34m")
add_compile_definitions(WHITE="\\033[1;97m")
add_compile_definitions(NO_COLOR="\\033[0m")

find_package(Qt6 REQUIRED COMPONENTS Core LinguistTools)

set(TS_FILES portagecfg_es_US.ts)
list(TRANSFORM TS_FILES PREPEND "i18n/")
qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})

set(PROJECT_SOURCES
    main.cpp
    logger.hpp
    logger.cpp
    writer.hpp
    writer.cpp
    ../resources.qrc
)

list(TRANSFORM PROJECT_SOURCES PREPEND "src/")

qt_add_executable(${PROJECT_NAME}
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    ${QM_FILES}
)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Core)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(FILES ${QM_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME}/translations)

qt_finalize_executable(${PROJECT_NAME})
