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
    styles/text_style.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(TrainAppSimulation
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        pages/train_parameter_page.cpp
        pages/train_parameter_page.h
        pages/running_parameter_page.cpp
        pages/running_parameter_page.h
        pages/track_parameter_page.cpp
        widgets/input_field.cpp
        widgets/input_field.h
        styles/colors.h
        styles/text_style.h
        widgets/input_widget.h
        widgets/input_widget.cpp
        mainwindow/mainwindowDummy.txt
        widgets/input_dropdown.h
        widgets/input_dropdown.cpp
        resources/input_type.h
        widgets/input_invalid.h
        widgets/input_invalid.cpp
        widgets/input_upload.h
        widgets/input_upload.cpp
        widgets/button_action.h
        widgets/button_action.cpp
        resources/icons.qrc
        widgets/button_sidebar_active.cpp
        widgets/button_sidebar_active.h
        resources/icon_paths.h
        resources/icons.qrc
        widgets/left_panel.cpp
        widgets/left_panel.h
        widgets/button_sidebar_widget.cpp
        widgets/button_sidebar_widget.h
        widgets/toggle_button.h
        widgets/toggle_button.cpp
        pages/constant_values_page.h
        pages/constant_values_page.cpp
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
