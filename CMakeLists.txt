cmake_minimum_required(VERSION 3.16)

project(Leno VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools)

find_package(OpenCV REQUIRED)

add_subdirectory(3rdparty/nodeeditor)
add_subdirectory(3rdparty/ElaWidgetTools)

set(BUILD_TESTING ON)

set(TS_FILES
    res/i18n/${PROJECT_NAME}_zh_CN.ts
    res/i18n/${PROJECT_NAME}_en_US.ts
)

set(QRC_FILES
    ${PROJECT_NAME}.qrc
)

set(SRC_FILES
    src/main.cpp
    # src/PluginManager.h src/PluginManager.cpp src/PluginInterface.h
    src/nodes/ImageData.hpp
    src/nodes/VideoLoaderNode.hpp
    src/nodes/ImageLoaderNode.hpp
    src/nodes/ImageShowNode.hpp
    src/nodes/ImageProcessBaseNode.hpp
    src/nodes/base/ThresholdNode.hpp
    src/nodes/yolo/YOLONode.hpp
    src/models/NodeModel.hpp
)

set(UI_FILES
    ui/mainwindow.cpp
    ui/LWindow/LWindow.cpp ui/LWindow/LWindowPrivate.cpp
)

set(PROJECT_SOURCES
    ${SRC_FILES} ${UI_FILES} ${TS_FILES} ${QRC_FILES}
)

find_program(QT_LUPDATE NAMES lupdate lupdate-qt6)
find_program(QT_LRELEASE NAMES lrelease lrelease-qt6)

add_custom_target(
    Script-UpdateTranslations
    COMMAND ${QT_LUPDATE} ${CMAKE_CURRENT_LIST_DIR} -ts ${CMAKE_SOURCE_DIR}/res/i18n/${PROJECT_NAME}_en_US.ts WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    COMMAND ${QT_LUPDATE} ${CMAKE_CURRENT_LIST_DIR} -ts ${CMAKE_SOURCE_DIR}/res/i18n/${PROJECT_NAME}_zh_CN.ts WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    COMMAND ${QT_LRELEASE} ${CMAKE_SOURCE_DIR}/res/i18n/${PROJECT_NAME}_en_US.ts WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    COMMAND ${QT_LRELEASE} ${CMAKE_SOURCE_DIR}/res/i18n/${PROJECT_NAME}_zh_CN.ts WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${APPLICATION_DIR_PATH}/i18n
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/res/i18n/${PROJECT_NAME}_en_US.qm ${CMAKE_SOURCE_DIR}/res/i18n/${PROJECT_NAME}_zh_CN.qm ${APPLICATION_DIR_PATH}/i18n
    SOURCES ${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${PROJECT_NAME} MANUAL_FINALIZATION ${PROJECT_SOURCES})
    # Define target properties for Android with Qt 6 as: set_property(TARGET
    # ${PROJECT_NAME} APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
    # ${CMAKE_CURRENT_SOURCE_DIR}/android) For more information, see
    # https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(
            ${PROJECT_NAME} SHARED
            ${PROJECT_SOURCES}
        )
        # Define properties for Android with Qt 5 after find_package() calls as:
        # set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(
            ${PROJECT_NAME}
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
        ${ELAWIDGETTOOLS_INCLUDE_DIRS}
)

target_link_directories(
    ${PROJECT_NAME}
    PRIVATE
        ${ELAWIDGETTOOLS_LIBRARY_DIRS}
)

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        ${OpenCV_LIBS}
        QtNodes::QtNodes
        ElaWidgetTools
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1. If
# you are developing for iOS or macOS you should consider setting an explicit,
# fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION
        MACOSX_BUNDLE_GUI_IDENTIFIER com.example.${PROJECT_NAME}
    )
endif()
set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
        ${BUNDLE_ID_OPTION} MACOSX_BUNDLE_BUNDLE_VERSION
        ${PROJECT_VERSION} MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE TRUE
        WIN32_EXECUTABLE FALSE
)

include(GNUInstallDirs)
install(
    TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME})
endif()
