set(CMAKE_CXX_STANDARD 23)
cmake_minimum_required(VERSION 3.24)
project(Xbee VERSION 0.1 DESCRIPTION "Communication Xbee pour raspberry pi")

add_library(${PROJECT_NAME}
        include/xbee_vars.h
        src/xbee.cpp include/xbee.h
        src/serialib.cpp include/serialib.h)
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(${PROJECT_NAME} PROPERTIES SOVERSION 1)
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "include/xbee.h;include/xbee_vars.h")

target_include_directories(${PROJECT_NAME} PRIVATE include)
target_include_directories(${PROJECT_NAME} PRIVATE src)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/robotech
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/robotech)

configure_file(${PROJECT_NAME}.pc.in ${PROJECT_NAME}.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LOGS REQUIRED Logs)
include_directories(${PROJECT_NAME}_test ${LOGS_INCLUDE_DIR})

add_executable(${PROJECT_NAME}_test
        src/main.cpp include/xbee_vars.h
        src/xbee.cpp include/xbee.h
        src/serialib.cpp include/serialib.h)

target_link_libraries(${PROJECT_NAME}_test ${LOGS_LIBRARIES})
target_compile_options(${PROJECT_NAME}_test PRIVATE ${LOGS_CFLAGS_OTHER})
