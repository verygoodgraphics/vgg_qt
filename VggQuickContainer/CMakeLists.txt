cmake_minimum_required(VERSION 3.16)
project(VggQuickContainer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# TODO Qt6 unfinished
find_package(Qt5 COMPONENTS Quick REQUIRED)

add_library(VggQuickContainer STATIC
  QVggQuickItem.cpp
  QVggEventAdapter.cpp
)

target_link_directories(VggQuickContainer PUBLIC external/lib)

target_link_libraries(VggQuickContainer
  PUBLIC Qt5::Quick
  PRIVATE vgg_container)

target_include_directories(VggQuickContainer PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/external
)

# Example Project
option(ENABLE_VGG_QUICK_DEMO "Enable vgg quick demo" ON)
if(ENABLE_VGG_QUICK_DEMO)
  add_executable(VggQtQuickDemo example/main.cpp example/main.qrc)
  target_link_libraries(VggQtQuickDemo PRIVATE VggQuickContainer)
endif()