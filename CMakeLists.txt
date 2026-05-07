cmake_minimum_required(VERSION 3.16)

project(IASCommSrv LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)
find_package(CURL REQUIRED)

add_executable(IASCommSrv
  main.cpp
  sys_config.h sys_config.cpp
  struct_define.h
  algo_impl.h
  common_functionality.hpp
  algo_impl.cpp
  qthread.hpp
  http_service_impl.hpp
  macro_define.h
  FtpsClient.cpp FtpsClient.h
)
target_link_libraries(IASCommSrv PRIVATE Qt${QT_VERSION_MAJOR}::Core)
target_link_libraries(IASCommSrv PRIVATE CURL::libcurl)

target_include_directories(IASCommSrv PRIVATE ${PROJECT_SOURCE_DIR}/nlohmann)
target_include_directories(IASCommSrv PRIVATE ${PROJECT_SOURCE_DIR}/httplib)


set_target_properties(IASCommSrv PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin
)


include(GNUInstallDirs)
install(TARGETS IASCommSrv
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
