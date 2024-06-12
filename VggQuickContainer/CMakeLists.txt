cmake_minimum_required(VERSION 3.16)
project(VggQuickContainer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

option(VGG_USE_QT_6 "Use Qt6 instead of Qt5" ON)
if(VGG_USE_QT_6)
  set(VGG_QT_NAME "Qt6")
else()
  set(VGG_QT_NAME "Qt5")
endif()

find_package(${VGG_QT_NAME} COMPONENTS Quick REQUIRED)

add_library(VggQuickContainer STATIC
  QVggQuickItem.cpp
  QVggEventAdapter.cpp
)

if(VGG_USE_QT_6)
  target_compile_definitions(VggQuickContainer PUBLIC VGG_USE_QT_6)
endif()

target_link_directories(VggQuickContainer PUBLIC external/lib)

target_link_libraries(VggQuickContainer
  PUBLIC ${VGG_QT_NAME}::Quick
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

  if(MSVC)
    add_custom_command(TARGET VggQtQuickDemo POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${CMAKE_CURRENT_SOURCE_DIR}/external/lib/node.exe"
      "${CMAKE_CURRENT_SOURCE_DIR}/external/lib/libnode.dll"
      "${CMAKE_CURRENT_SOURCE_DIR}/external/lib/icudtl.dat"
      "${CMAKE_CURRENT_SOURCE_DIR}/external/lib/skia.dll"
      "${CMAKE_CURRENT_SOURCE_DIR}/external/lib/skparagraph.dll"
      "${CMAKE_CURRENT_SOURCE_DIR}/external/lib/skshaper.dll"
      "${CMAKE_CURRENT_SOURCE_DIR}/external/lib/sktext.dll"
      "${CMAKE_CURRENT_SOURCE_DIR}/external/lib/skunicode.dll"
      "${CMAKE_CURRENT_SOURCE_DIR}/external/lib/vgg_container.dll"
      "${CMAKE_CURRENT_SOURCE_DIR}/external/lib/SDL2.dll"
      "${CMAKE_BINARY_DIR}"
    )
  endif()

endif()