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
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -D_ADVANCED_DEBUG")
else(ENABLE_ADVANCED_DEBUG)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")
endif(ENABLE_ADVANCED_DEBUG)


set(SocketCAN_DIR ${openivi-html5_SOURCE_DIR})

find_package(Qt5Widgets REQUIRED)
find_package(Qt5WebKitWidgets REQUIRED)
find_package(Qt5DBus REQUIRED)
find_package(Qt5Bluetooth REQUIRED)
find_package(X11)

find_program(CTAGS NAMES ctags)


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

qt5_wrap_ui(UI_MAINWINDOW_L5 mainwindow_l5.ui)
qt5_wrap_ui(UI_MAINWINDOW_P5 mainwindow_p5.ui)
qt5_wrap_ui(UI_MAINWINDOW_L6 mainwindow_l6.ui)
qt5_wrap_ui(UI_MAINWINDOW_P6 mainwindow_p6.ui)

foreach(LAYOUT L5 P5 L6 P6)
  string(TOLOWER ${LAYOUT} LAYTAG)
  set(UI_MAIN ${UI_MAINWINDOW_${LAYOUT}})
  message("UI variable = ${UI_MAIN}")

  add_executable(openivi-${LAYTAG}
          main.cc mainwindow.cc webgraphicview.cc softwareloadingmanager.cc
          car.cc installdetail.cc packageid.cc
          virtualkeyboard.cc allowlocation_webpage.cc phone.cc
          ${DBUS_FILES} ${RCC_FILES}
          ${UI_MAIN})
  set_target_properties(openivi-${LAYTAG} PROPERTIES COMPILE_FLAGS "-DDASH_LAYOUT_${LAYOUT}=1")
  set_property(TARGET openivi-${LAYTAG} PROPERTY CXX_STANDARD 11)

  if (X11_FOUND)
    target_link_libraries(openivi-${LAYTAG} canreader ofono-dash bt_manager ${X11_LIBRARIES} Qt5::Widgets Qt5::DBus Qt5::WebKitWidgets Qt5::Bluetooth)
    include_directories(${X11_INCLUDE_DIR})
  else(X11_FOUND)
    target_link_libraries(openivi-${LAYTAG} canreader ofono-dash bt_manager Qt5::Widgets Qt5::DBus Qt5::WebKitWidgets Qt5::Bluetooth)
  endif(X11_FOUND)
endforeach(LAYOUT)

# qt5_use_modules(openivi-html5 Widgets DBus WebKitWidgets)

message("Build type is ${CMAKE_BUILD_TYPE}")

# Export compile_commands.json for clang-check
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#if (CTAGS)
    ## Generate tags if the 'ctags' tool is available
    #set_source_files_properties(tags PROPERTIES GENERATED true)
    #add_custom_target(tags
        #COMMAND ${CTAGS} -R --c++-kinds=+p --fields=+iaS --extra=+q .
        #WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
    #add_dependencies(openivi-html5 tags)
#endif() # CTAGS

# vim: set tabstop=4 shiftwidth=4 expandtab:
