################################################### PROJECT ###################################################
set(CMAKE_CXX_STANDARD 20)
cmake_minimum_required(VERSION 3.16)
project(XBee VERSION 0.1 DESCRIPTION "Communication XBee pour raspberry pi")

################################################### LIBRARY ###################################################
add_library(${PROJECT_NAME} src/xbee.cpp src/serialib.cpp)

set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "include/xbee.h;include/define_xbee.h;include/serialib.h")

find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} Threads::Threads)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/robotech
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/robotech)

configure_file(Config.cmake.in ${PROJECT_NAME}Config.cmake @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/${PROJECT_NAME}Config.cmake DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

################################################### TESTING ###################################################
add_executable(${PROJECT_NAME}_test src/main.cpp src/xbee.cpp src/serialib.cpp)
target_link_libraries(${PROJECT_NAME}_test Threads::Threads)
target_include_directories(${PROJECT_NAME}_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
