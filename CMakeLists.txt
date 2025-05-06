cmake_minimum_required(VERSION 3.16)

project(TrainSimulationApp VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Widgets Charts)
find_package(Qt6 REQUIRED COMPONENTS Concurrent)

set(PROJECT_SOURCES
    main.cpp
    mainwindow/mainwindow.cpp
    mainwindow/mainwindow.h
    mainwindow/mainwindow.ui
    core/appcontext.h
    core/appcontext.cpp
)

set(PAGE_SOURCES
    pages/train_parameter_page.cpp
    pages/train_parameter_page.h
    pages/running_parameter_page.cpp
    pages/running_parameter_page.h
    pages/track_parameter_page.cpp
    pages/track_parameter_page.h
    pages/constant_values_page.cpp
    pages/constant_values_page.h
    pages/electrical_parameter_page.cpp
    pages/electrical_parameter_page.h
    pages/train_speed_page.h
    pages/train_speed_page.cpp
    pages/traction_effort_page.h
    pages/traction_effort_page.cpp
    pages/train_power_page.h
    pages/train_power_page.cpp
    pages/traction_effort_page.h
    pages/traction_effort_page.cpp
    pages/train_track_page.h
    pages/train_track_page.cpp
    pages/train_consumption_page.h
    pages/train_consumption_page.cpp
)

set(WIDGET_SOURCES
    widgets/input_field.cpp
    widgets/input_field.h
    widgets/input_widget.cpp
    widgets/input_widget.h
    widgets/input_dropdown.cpp
    widgets/input_dropdown.h
    widgets/input_invalid.cpp
    widgets/input_invalid.h
    widgets/input_upload.cpp
    widgets/input_upload.h
    widgets/button_action.cpp
    widgets/button_action.h
    widgets/button_sidebar_active.cpp
    widgets/button_sidebar_active.h
    widgets/button_sidebar_widget.cpp
    widgets/button_sidebar_widget.h
    widgets/button_toggle.cpp
    widgets/button_toggle.h
    widgets/left_panel.cpp
    widgets/left_panel.h
    widgets/button_pagination.h
    widgets/button_pagination.cpp
    widgets/left_panel_category.h
    widgets/left_panel_category.cpp
    widgets/button_panel_category.h
    widgets/button_panel_category.cpp
    widgets/chart_widget.h
    widgets/chart_widget.cpp
    widgets/message_box_widget.h
    widgets/message_box_widget.cpp
)

set(RESOURCE_SOURCES
    styles/colors.h
    styles/text_style.h
    resources/input_type.h
    resources/icon_paths.h
    resources/icons.qrc
    core/auth/auth.qrc
    core/auth/auth_data.h
    core/auth/auth_manager.h
    core/auth/auth_manager.cpp
    core/auth/user_manager.cpp
    core/auth/user_manager.h
    widgets/login_dialog_widget.h
    widgets/login_dialog_widget.cpp
)

set(MODEL_SOURCES
    models/train_data.h
    models/simulation_data.h
    models/constant_data.h
    models/mass_data.h
    models/energy_data.h
    models/power_data.h
    models/resistance_data.h
    models/moving_data.h
    models/load_data.h
    models/efficiency_data.h
    models/station_data.h
    models/train_motor_data.h
)

set(CONTROLLER_SOURCES
    controllers/train_simulation/train_simulation.h
    controllers/train_simulation/train_simulation.cpp
    controllers/train_simulation/mass_handler.h
    controllers/train_simulation/mass_handler.cpp
    controllers/train_simulation/resistance_handler.h
    controllers/train_simulation/resistance_handler.cpp
    controllers/train_simulation/tractive_effort_handler.h
    controllers/train_simulation/tractive_effort_handler.cpp
    controllers/train_simulation/traction_motor_handler.h
    controllers/train_simulation/traction_motor_handler.cpp
    controllers/train_simulation/energy_handler.h
    controllers/train_simulation/energy_handler.cpp
    controllers/train_simulation/power_handler.h
    controllers/train_simulation/power_handler.cpp
    controllers/train_simulation/moving_handler.h
    controllers/train_simulation/moving_handler.cpp
    controllers/train_simulation/output_handler.h
    controllers/train_simulation/output_handler.cpp
    controllers/train_simulation/simulation_track_handler.h
    controllers/train_simulation/simulation_track_handler.cpp
    controllers/train_simulation/utility_handler.h
    controllers/train_simulation/utility_handler.cpp
    controllers/train_simulation/csv_handler.h
    controllers/train_simulation/csv_handler.cpp
    controllers/train_simulation/current_handler.h
    controllers/train_simulation/current_handler.cpp
    controllers/chart/save_button_handler.h
    controllers/chart/save_button_handler.cpp
    controllers/chart/static_simulation_chart_handler.h
    controllers/chart/static_simulation_chart_handler.cpp
    controllers/chart/dynamic_simulation_chart_handler.h
    controllers/chart/dynamic_simulation_chart_handler.cpp
    controllers/chart/output_table_handler.h
    controllers/chart/output_table_handler.cpp
    controllers/train_simulation/csv_variables_handler.h
    controllers/train_simulation/csv_variables_handler.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(TrainSimulationApp
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${PAGE_SOURCES}
        ${WIDGET_SOURCES}
        ${MODEL_SOURCES}
        ${RESOURCE_SOURCES}
        ${CONTROLLER_SOURCES}
        pages/testpage.h pages/testpage.cpp
    )

# Define target properties for Android with Qt 6 as:
# set_property(TARGET TrainSimulationApp APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
# ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(TrainSimulationApp SHARED
            ${PROJECT_SOURCES}
        )

    # Define properties for Android with Qt 5 after find_package() calls as:
    # set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(TrainSimulationApp
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(TrainSimulationApp PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(TrainSimulationApp PRIVATE Qt6::Concurrent)
target_link_libraries(TrainSimulationApp PRIVATE
    Qt6::Widgets
    Qt6::Charts
)
target_include_directories(TrainSimulationApp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})


# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.TrainSimulationApp)
endif()

set_target_properties(TrainSimulationApp PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS TrainSimulationApp
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(TrainSimulationApp)
endif()
