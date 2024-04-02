cmake_minimum_required(VERSION 3.16)

project(VggQuickContainer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(QT_QML_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR})

find_package(Qt6 6.2 COMPONENTS Quick REQUIRED)

qt_add_library(VggQuickContainer STATIC)
qt_add_qml_module(VggQuickContainer
    URI VggQuickContainer
    VERSION 1.0
    #QML_FILES VggQuickContainerControls.qml
    SOURCES vggquickcontainer.cpp vggquickcontainer.h
)

set_target_properties(VggQuickContainer PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER qtquick.verygoodgraphics.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_compile_definitions(VggQuickContainer
    PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(VggQuickContainer
    PRIVATE Qt6::Quick)

target_include_directories(VggQuickContainer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Example Project
qt_add_executable(VggQtQuickDemo example/main.cpp)
qt_add_qml_module(VggQtQuickDemo
    URI VggQtQuickDemoApp
    VERSION 1.0
    QML_FILES example/main.qml
)
target_link_libraries(VggQtQuickDemo PRIVATE Qt6::Quick VggQuickContainerplugin)
target_compile_definitions(VggQtQuickDemo PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
