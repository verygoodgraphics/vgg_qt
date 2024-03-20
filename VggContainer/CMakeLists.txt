cmake_minimum_required(VERSION 3.14)

project(VggContainer LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS OpenGLWidgets Widgets)

set(CONTAINER_SOURCE
  include/VggContainer/QVggOpenGLWidget.hpp
  include/VggContainer/QVggEventAdapter.hpp
  src/QVggOpenGLWidget.cpp
  src/QVggEventAdapter.cpp
)

add_library(VggContainer STATIC ${CONTAINER_SOURCE})

target_include_directories(VggContainer PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/external
)

target_link_libraries(VggContainer PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::OpenGLWidgets
)

target_compile_definitions(VggContainer PRIVATE VGGCONTAINER_LIBRARY)

target_link_libraries(VggContainer PRIVATE vgg_container)

target_link_directories(VggContainer PUBLIC external/lib)
