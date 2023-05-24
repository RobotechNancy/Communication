################################################### PROJECT ###################################################
set(CMAKE_CXX_STANDARD 20)
cmake_minimum_required(VERSION 3.16)
project(CAN VERSION 0.1 DESCRIPTION "Communication CAN pour raspberry pi")

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h")

################################################### LIBRARY ###################################################
add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})

set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(${PROJECT_NAME} PROPERTIES SOVERSION 1)
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "include/can.h;include/define_can.h")

find_package(PkgConfig)
find_package(Threads REQUIRED)
pkg_check_modules(Logs REQUIRED Logs)

target_include_directories(${PROJECT_NAME} PRIVATE include src)
target_include_directories(${PROJECT_NAME} PUBLIC ${Logs_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${Logs_LIBRARIES} Threads::Threads)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/robotech
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/robotech)

configure_file(${PROJECT_NAME}.pc.in ${PROJECT_NAME}.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)

################################################### TESTING ###################################################
add_executable(${PROJECT_NAME}_test ${SOURCES} ${HEADERS})

include_directories(${PROJECT_NAME}_test ${LOGS_INCLUDE_DIR} include)
target_link_libraries(${PROJECT_NAME}_test ${Logs_LIBRARIES} Threads::Threads)
