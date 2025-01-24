cmake_minimum_required(VERSION 3.16)

project(TrainAppSimulation VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
    main.cpp
    mainwindow/mainwindow.cpp
    mainwindow/mainwindow.h
    mainwindow/mainwindow.ui
    styles/colors.h
    styles/textStyle.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(TrainAppSimulation
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        pages/trainParameterPage.cpp
        pages/trainParameterPage.h
        pages/runningParameterPage.cpp
        pages/runningParameterPage.h
        pages/trackParameterPage.cpp
        widgets/inputField.cpp
        widgets/inputField.h
        styles/colors.h
        styles/textStyle.h
        widgets/inputWidget.h
        widgets/inputWidget.cpp
        mainwindow/mainwindowDummy.txt
        widgets/inputDropdown.h
        widgets/inputDropdown.cpp
        resources/inputType.h
        widgets/inputInvalid.h
        widgets/inputInvalid.cpp
        widgets/inputUpload.h
        widgets/inputUpload.cpp
        widgets/buttonAction.h
        widgets/buttonAction.cpp
        resources/icons.qrc
        widgets/buttonSidebarActive.cpp
        widgets/buttonSidebarActive.h
        resources/iconPaths.h
        resources/icons.qrc
        widgets/leftPanel.cpp
        widgets/leftPanel.h
        widgets/buttonSidebarWidget.cpp
        widgets/buttonSidebarWidget.h
        widgets/toggleButton.h
        widgets/toggleButton.cpp
    )

# Define target properties for Android with Qt 6 as:
# set_property(TARGET TrainAppSimulation APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
# ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(TrainAppSimulation SHARED
            ${PROJECT_SOURCES}
        )

    # Define properties for Android with Qt 5 after find_package() calls as:
    # set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(TrainAppSimulation
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(TrainAppSimulation PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.TrainAppSimulation)
endif()

set_target_properties(TrainAppSimulation PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS TrainAppSimulation
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(TrainAppSimulation)
endif()
