cmake_minimum_required(VERSION 3.5)

project(Counter VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS OpenGLWidgets Widgets)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        Counter.cpp
        Counter.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Counter
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Counter APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(Counter SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(Counter
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(Counter PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::OpenGLWidgets
)

# libs
add_subdirectory(../../VggContainer VggContainerBin)
target_link_libraries(Counter PRIVATE
    VggContainer
)
target_include_directories(Counter PRIVATE
  external
)


# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Counter)
endif()
set_target_properties(Counter PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS Counter
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Counter)
endif()

if(MSVC)
    add_custom_command(TARGET Counter POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different
        #"${VggContainer_BINARY_DIR}/${CMAKE_BUILD_TYPE}/VggContainer.dll"
        "${VggContainer_SOURCE_DIR}/external/lib/icudtl.dat"
        "${VggContainer_SOURCE_DIR}/external/lib/libnode.dll"
        "${VggContainer_SOURCE_DIR}/external/lib/node.exe"
        "${VggContainer_SOURCE_DIR}/external/lib/vgg_container.dll"
        "${VggContainer_SOURCE_DIR}/external/lib/skia.dll"
        "${VggContainer_SOURCE_DIR}/external/lib/skparagraph.dll"
        "${VggContainer_SOURCE_DIR}/external/lib/skshaper.dll"
        "${VggContainer_SOURCE_DIR}/external/lib/sktext.dll"
        "${VggContainer_SOURCE_DIR}/external/lib/skunicode.dll"
        "${VggContainer_SOURCE_DIR}/external/lib/SDL2.dll"
        "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}"
    )
endif()
