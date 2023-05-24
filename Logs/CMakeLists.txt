################################################### PROJECT ###################################################
set(CMAKE_CXX_STANDARD 20)
cmake_minimum_required(VERSION 3.16)
project(Logs VERSION 0.1 DESCRIPTION "Librairie de logs")

################################################### LIBRARY ###################################################
add_library(${PROJECT_NAME} lib/logs.h)
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER lib/logs.h)
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/robotech
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/robotech)

configure_file(${PROJECT_NAME}.pc.in ${PROJECT_NAME}.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)

################################################### TESTING ###################################################
add_executable(${PROJECT_NAME}_test lib/logs.h lib/main.cpp)
