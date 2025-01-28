cmake_minimum_required(VERSION 3.0.4)
project(openivi-html5)

set(ENABLE_ADVANCED_DEBUG OFF CACHE BOOL "Enable Advanced debug")

# Tell CMake to run moc when necessary:
set(CMAKE_AUTOMOC ON)
# Tell CMake to run the UI compiler when necessary
# SET(CMAKE_AUTOUIC ON)
# Only available in cmake 3.0. yocto only has cmake 2.8.12

# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

if (ENABLE_ADVANCED_DEBUG)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -pg -D_ADVANCED_DEBUG")
else(ENABLE_ADVANCED_DEBUG)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")
endif(ENABLE_ADVANCED_DEBUG)

message("Compiling with flags ${CMAKE_CXX_FLAGS}")


set(SocketCAN_DIR ${openivi-html5_SOURCE_DIR})

find_package(Qt5Widgets REQUIRED)
find_package(Qt5WebKitWidgets REQUIRED)
find_package(Qt5DBus REQUIRED)
find_package(Qt5Bluetooth REQUIRED)
find_package(X11)


set_source_files_properties(softwareloadingmanager.xml
    PROPERTIES INCLUDE softwareloadingmanagerdbus.h)
qt5_add_dbus_interfaces(DBUS_FILES softwareloadingmanager.xml)
qt5_add_resources(RCC_FILES resources/resources.qrc)

# Generate config.h from config.h.in
configure_file(config.h.in config.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR} ${openivi-html5_SOURCE_DIR}/can_reader)

# Add subdirectory for CAN bus reading
add_subdirectory(can_reader)
# Add subdirectory for ofono interface classes
add_subdirectory(ofono)
# Add subdirectory for bluetooth management (scan, pairing, etc)
add_subdirectory(bluetooth)

qt5_wrap_ui(UI_MAINWINDOW mainwindow.ui)

set(UI_MAIN ${UI_MAINWINDOW})
message("UI variable = ${UI_MAIN}")

add_executable(openivi
        main.cc mainwindow.cc webgraphicview.cc softwareloadingmanager.cc
        car.cc installdetail.cc packageid.cc
        virtualkeyboard.cc allowlocation_webpage.cc phone.cc scriptrunner.cpp
        ${DBUS_FILES} ${RCC_FILES}
        ${UI_MAIN})
set_property(TARGET openivi PROPERTY CXX_STANDARD 11)

if (X11_FOUND)
  target_link_libraries(openivi canreader ofono-dash bt_manager ${X11_LIBRARIES} Qt5::Widgets Qt5::DBus Qt5::WebKitWidgets Qt5::Bluetooth)
  include_directories(${X11_INCLUDE_DIR})
else(X11_FOUND)
  target_link_libraries(openivi canreader ofono-dash bt_manager Qt5::Widgets Qt5::DBus Qt5::WebKitWidgets Qt5::Bluetooth)
endif(X11_FOUND)


message("Build type is ${CMAKE_BUILD_TYPE}")

# Export compile_commands.json for clang-check
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

