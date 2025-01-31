cmake_minimum_required(VERSION 3.0.4)
project(openivi-html5)


# Tell CMake to run moc when necessary:
set(CMAKE_AUTOMOC ON)

# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")
message("Compiling with flags ${CMAKE_CXX_FLAGS}")


find_package(Qt5Widgets REQUIRED)
find_package(Qt5WebKitWidgets REQUIRED)
find_package(Qt5DBus REQUIRED)
find_package(Qt5Bluetooth REQUIRED)
find_package(X11)


set_source_files_properties(${CMAKE_SOURCE_DIR}/src/softwareloadingmanager.xml
    PROPERTIES INCLUDE ${CMAKE_SOURCE_DIR}/src/softwareloadingmanagerdbus.h)

# Sources and headers
file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")
include_directories("${CMAKE_SOURCE_DIR}/src/")

# UI Files
qt5_wrap_ui(UI_MAINWINDOW ${CMAKE_SOURCE_DIR}/src/mainwindow.ui)
qt5_wrap_ui(UI_BLUETOOTH ${CMAKE_SOURCE_DIR}/src/bluetooth/bluetoothmanager.ui)

# Generate config.h from config.h.in
configure_file(${CMAKE_SOURCE_DIR}/src/config.h.in ${CMAKE_SOURCE_DIR}/src/config.h)

# Resources
qt5_add_resources(RCC_FILES ${CMAKE_SOURCE_DIR}/resources/resources.qrc)

# DBUS Interfaces
qt5_add_dbus_interfaces(DBUS_FILES ${CMAKE_SOURCE_DIR}/src/softwareloadingmanager.xml ${CMAKE_SOURCE_DIR}/src/bluetooth/bluez_device.xml)


add_executable(openivi
        ${SOURCES}
        ${DBUS_FILES} ${RCC_FILES}
        ${UI_MAINWINDOW} ${UI_BLUETOOTH})
set_property(TARGET openivi PROPERTY CXX_STANDARD 11)

if (X11_FOUND)
  target_link_libraries(openivi ${X11_LIBRARIES} Qt5::Widgets Qt5::DBus Qt5::WebKitWidgets Qt5::Bluetooth)
  include_directories(${X11_INCLUDE_DIR})
else(X11_FOUND)
  target_link_libraries(openivi Qt5::Widgets Qt5::DBus Qt5::WebKitWidgets Qt5::Bluetooth)
endif(X11_FOUND)


message("Build type is ${CMAKE_BUILD_TYPE}")

# Export compile_commands.json for clang-check
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

