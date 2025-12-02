cmake_minimum_required(VERSION 3.16)

project(TrainSimulationApp VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set Qt paths
set(CMAKE_PREFIX_PATH
    /usr/lib64/cmake
    /usr/lib/cmake
    /usr/local/Qt6/lib/cmake
    ${CMAKE_PREFIX_PATH}
)

# Alternative approach - set Qt6_DIR explicitly if find_package fails
if(NOT DEFINED Qt6_DIR)
    # Try common Fedora/Nobara locations
    if(EXISTS "/usr/lib64/cmake/Qt6")
        set(Qt6_DIR "/usr/lib64/cmake/Qt6")
    elseif(EXISTS "/usr/lib/cmake/Qt6")
        set(Qt6_DIR "/usr/lib/cmake/Qt6")
    endif()
endif()

# Find Qt6 packages - IMPORTANT: Add WebEngineWidgets for embedding Next.js
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Network
    HttpServer
    Widgets
    PrintSupport
    WebEngineWidgets
    WebEngineWidgets
    WebChannel
    Concurrent
)
find_package(Qt6 REQUIRED COMPONENTS Core)

# Source files - adjust paths as needed
file(GLOB_RECURSE SOURCES
    "src/*.cpp"
    "src/*.h"
)

# If no sources found in src/, try current directory
if(NOT SOURCES)
    file(GLOB_RECURSE SOURCES
        "*.cpp"
        "*.h"
    )
endif()

set(HTTP_SERVER_SOURCES)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/http_server/http_server.cpp")
    list(APPEND HTTP_SERVER_SOURCES
        "http_server/http_server.h"
        "http_server/http_server.cpp"
    )
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/http_server/api_handler.cpp")
    list(APPEND HTTP_SERVER_SOURCES
        "http_server/api_handler.h"
        "http_server/api_handler.cpp"
    )
endif()

set(PROJECT_SOURCES
    main.cpp
    webengine/webengine_window.h
    webengine/webengine_window.cpp
    webengine/file_bridge.h
    webengine/file_bridge.cpp
    core/appcontext.h
    core/appcontext.cpp
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

set(UTILITY_SOURCES
    utils/fuzzy_engine.h
    utils/fuzzy_engine.cpp
)

set(CONTROLLER_SOURCES
    controllers/simulation/train_simulation_handler.h
    controllers/simulation/train_simulation_handler.cpp
    controllers/simulation/mass_handler.h
    controllers/simulation/mass_handler.cpp
    controllers/simulation/resistance_handler.h
    controllers/simulation/resistance_handler.cpp
    controllers/simulation/tractive_effort_handler.h
    controllers/simulation/tractive_effort_handler.cpp
    controllers/simulation/traction_motor_handler.h
    controllers/simulation/traction_motor_handler.cpp
    controllers/simulation/energy_handler.h
    controllers/simulation/energy_handler.cpp
    controllers/simulation/power_handler.h
    controllers/simulation/power_handler.cpp
    controllers/simulation/moving_handler.h
    controllers/simulation/moving_handler.cpp
    controllers/simulation/simulation_track_handler.h
    controllers/simulation/simulation_track_handler.cpp
    controllers/simulation/utility_handler.h
    controllers/simulation/utility_handler.cpp
    controllers/simulation/current_handler.h
    controllers/simulation/current_handler.cpp
    controllers/output/csv_output_handler.h
    controllers/output/csv_output_handler.cpp
    controllers/simulation/csv_variables_handler.h
    controllers/simulation/csv_variables_handler.cpp
    controllers/data/electrical_data_handler.h
    controllers/data/electrical_data_handler.cpp
    controllers/data/running_data_handler.h
    controllers/data/running_data_handler.cpp
    controllers/data/track_data_handler.h
    controllers/data/track_data_handler.cpp
    controllers/data/train_data_handler.h
    controllers/data/train_data_handler.cpp
    controllers/optimization/optimization_handler.h
    controllers/optimization/optimization_handler.cpp
)

set(HTTP_SERVER
    http_server/http_server.cpp
    http_server/api_handler.cpp
    http_server/http_server.h
    http_server/api_handler.h
)

set(HTTP_SERVER_PARAMS
    http_server/inputs/electrical_parameter_handler.cpp
    http_server/inputs/electrical_parameter_handler.h
    http_server/inputs/running_parameter_handler.cpp
    http_server/inputs/running_parameter_handler.h
    http_server/inputs/track_parameter_handler.cpp
    http_server/inputs/track_parameter_handler.h
    http_server/inputs/train_parameter_handler.cpp
    http_server/inputs/train_parameter_handler.h
)

# if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
qt_add_executable(TrainSimulationApp
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    ${MODEL_SOURCES}
    ${UTILITY_SOURCES}
    ${CONTROLLER_SOURCES}
    ${HTTP_SERVER}
    ${HTTP_SERVER_PARAMS}
    http_server/inputs/train_parameter_handler.h http_server/inputs/train_parameter_handler.cpp
    http_server/inputs/running_parameter_handler.h http_server/inputs/running_parameter_handler.cpp
    http_server/inputs/electrical_parameter_handler.h http_server/inputs/electrical_parameter_handler.cpp
    http_server/inputs/track_parameter_handler.h http_server/inputs/track_parameter_handler.cpp
    http_server/simulations/simulation_handler.h http_server/simulations/simulation_handler.cpp
)

# Define target properties for Android with Qt 6 as:
# set_property(TARGET TrainSimulationApp APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
# ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
# else()
# if(ANDROID)
# add_library(TrainSimulationApp SHARED
# ${PROJECT_SOURCES}
# )

# # Define properties for Android with Qt 5 after find_package() calls as:
# # set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
# else()
# add_executable(TrainSimulationApp
# ${PROJECT_SOURCES}
# )
# endif()
# endif()
if(TARGET Qt${QT_VERSION_MAJOR}::HttpServer)
    list(APPEND QT_LIBS Qt${QT_VERSION_MAJOR}::HttpServer)
    target_compile_definitions(TrainSimulationApp PRIVATE HAVE_QT_HTTPSERVER)
endif()

if(TARGET Qt${QT_VERSION_MAJOR}::Charts)
    list(APPEND QT_LIBS Qt${QT_VERSION_MAJOR}::Charts)
    target_compile_definitions(TrainSimulationApp PRIVATE HAVE_QT_CHARTS)
endif()

if(TARGET Qt${QT_VERSION_MAJOR}::Concurrent)
    list(APPEND QT_LIBS Qt${QT_VERSION_MAJOR}::Concurrent)
    target_compile_definitions(TrainSimulationApp PRIVATE HAVE_QT_CONCURRENT)
endif()

# target_link_libraries(TrainSimulationApp PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
# target_link_libraries(TrainSimulationApp PRIVATE Qt6::Concurrent)
target_link_libraries(TrainSimulationApp PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::HttpServer
    Qt6::Widgets
    Qt6::WebEngineWidgets
    Qt6::WebChannel
    Qt6::Concurrent

    # Qt6::Charts
)
target_link_libraries(TrainSimulationApp PRIVATE Qt6::Core)
target_link_libraries(TrainSimulationApp PRIVATE Qt6::Core)
target_link_libraries(TrainSimulationApp PRIVATE Qt6::Core)
target_link_libraries(TrainSimulationApp PRIVATE Qt6::Core)
target_link_libraries(TrainSimulationApp PRIVATE Qt6::Core)
target_include_directories(TrainSimulationApp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
# if(${QT_VERSION} VERSION_LESS 6.1.0)
# set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.TrainSimulationApp)
# endif()
# set_target_properties(TrainSimulationApp PROPERTIES
# ${BUNDLE_ID_OPTION}
# MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
# MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
# MACOSX_BUNDLE TRUE
# WIN32_EXECUTABLE TRUE
# )
set_target_properties(TrainSimulationApp PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Debug info
message(STATUS "Qt6_DIR: ${Qt6_DIR}")
message(STATUS "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")
message(STATUS "Sources found: ${SOURCES}")

include(GNUInstallDirs)
install(TARGETS TrainSimulationApp
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(TrainSimulationApp)
endif()
