cmake_minimum_required(VERSION 3.5)

project(lvnai VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Multimedia)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Multimedia)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(lvnai
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        transcribemodule.h transcribemodule.cpp
        utils.h
        llamamodule.h llamamodule.cpp

        audionormalizer.h audionormalizer.cpp
        parser.h parser.cpp
        lexer.cpp










    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET lvnai APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(lvnai SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(lvnai
            ${PROJECT_SOURCES}
        )
    endif()
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(lvnai PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(lvnai PRIVATE Qt${QT_VERSION_MAJOR}::Multimedia)
target_link_libraries(lvnai PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libs/whisper.lib)
target_link_libraries(lvnai PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libs/llama.lib)
target_link_libraries(lvnai PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libs/avcodec.lib)
target_link_libraries(lvnai PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libs/avdevice.lib)
target_link_libraries(lvnai PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libs/avfilter.lib)
target_link_libraries(lvnai PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libs/avformat.lib)
target_link_libraries(lvnai PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libs/avutil.lib)
target_link_libraries(lvnai PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libs/postproc.lib)
target_link_libraries(lvnai PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libs/swresample.lib)
target_link_libraries(lvnai PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libs/swscale.lib)
target_link_libraries(lvnai PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libs/portaudio.lib)


# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.lvnai)
endif()
set_target_properties(lvnai PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS lvnai
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(lvnai)
endif()
