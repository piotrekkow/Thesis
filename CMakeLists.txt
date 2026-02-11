cmake_minimum_required(VERSION 3.16)

project(Thesis LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

add_executable(Thesis
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h

    src/debug_sink.h
    src/debug_sink.cpp

    src/core/network.cpp
    src/core/network.h
    src/core/topology/intersection.h
    src/core/topology/intersection.cpp
    src/core/topology/node.h
    src/core/topology/node.cpp
    src/core/topology/lane.h
    src/core/topology/lane_group.h
    src/core/id.h
    src/core/id_generator.h
    src/core/movement_it.h

    src/utils/vector2.h
    src/utils/vector2.cpp
    src/utils/position.h
    src/utils/position.cpp
    src/utils/polyline.h
    src/utils/polyline.cpp
    src/utils/line.h
    src/utils/line.cpp
    src/utils/polyline_utils.h
    src/utils/polyline_utils.cpp

    src/ui/network_view.h
    src/ui/network_view.cpp
    src/ui/network_scene.h
    src/ui/network_scene.cpp

    src/core/topology/edge.h
    src/core/topology/edge.cpp
    src/core/topology/lane.h
    src/core/topology/lane_group.h
    src/core/topology/lane_group.cpp
    src/core/topology/movement/movement.h
    src/core/topology/movement/movement_geometry_spec.h
    src/core/topology/movement/lane_range.h
    src/core/topology/movement/lane_range.cpp
    src/core/topology/movement/movement_structure.h
    src/core/topology/movement/movement_structure.cpp
    src/core/topology/crossing.h
    
    src/core/geometry/edge.h
    src/core/geometry/edge.cpp
    src/core/geometry/node.h
    src/core/geometry/geometry.h
    src/core/geometry/movement.h
    src/core/geometry/movement_map.h
    src/core/geometry/movement_map.cpp
    src/core/geometry/calculators/edge_calculator.h
    src/core/geometry/calculators/edge_calculator.cpp
    src/core/geometry/calculators/movement_calculator.h
    src/core/geometry/calculators/movement_calculator.cpp

    src/core/collisions/collision_point.h
    src/core/collisions/movement_collisions.h
    src/core/collisions/collision_map.h
    src/core/collisions/collision_map.cpp
)

target_link_libraries(Thesis PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)

target_include_directories(Thesis PRIVATE 
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/utils
    ${CMAKE_SOURCE_DIR}/src/ui
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/src/core/collisions
)

if(WIN32)
    # Find the windeployqt tool based on the Qt6 location
    get_target_property(_qmake_executable Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")

    # Run windeployqt after the build finishes
    add_custom_command(TARGET Thesis POST_BUILD
        COMMAND "${WINDEPLOYQT_EXECUTABLE}" 
                --no-compiler-runtime # We'll handle compiler runtime (MinGW) separately if needed, or let it copy standard
                --no-translations     # Optional: skips copying huge translation files to keep build fast
                "$<TARGET_FILE:Thesis>"
        COMMENT "Running windeployqt to copy DLLs..."
    )
endif()