cmake_minimum_required(VERSION 3.16)

project(IASCommSrv LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
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

#已经添加了子项目了，链接时，直接包含就行
add_subdirectory(patroller_log)

target_link_libraries(IASCommSrv PRIVATE Qt${QT_VERSION_MAJOR}::Core)
target_link_libraries(IASCommSrv PRIVATE CURL::libcurl)
target_link_libraries(IASCommSrv PRIVATE patroller_log)

target_include_directories(IASCommSrv PRIVATE ${CMAKE_SOURCE_DIR}/nlohmann)
target_include_directories(IASCommSrv PRIVATE ${CMAKE_SOURCE_DIR}/httplib)
target_include_directories(IASCommSrv PRIVATE ${CMAKE_SOURCE_DIR}/patroller_log)

#设置执行文件生成目录
set_target_properties(IASCommSrv PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)


include(GNUInstallDirs)
install(TARGETS IASCommSrv
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

