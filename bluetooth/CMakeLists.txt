
set(CMAKE_INCLUDE_CURRENT_DIR ON)

#qt5_wrap_ui(UI_BLUETOOTH bt_manager.ui)
qt5_wrap_ui(UI_BLUETOOTH_L5 bluetoothmanager_l5.ui)
qt5_wrap_ui(UI_BLUETOOTH_P5 bluetoothmanager_p5.ui)
qt5_wrap_ui(UI_BLUETOOTH_L6 bluetoothmanager_l6.ui)
qt5_wrap_ui(UI_BLUETOOTH_P6 bluetoothmanager_p6.ui)

qt5_add_dbus_interfaces(BLUEZ_DBUS_FILES bluez_device.xml)

message("Binary dir: ${CMAKE_CURRENT_BINARY_DIR}")

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(bt_manager_l5 STATIC bluetoothmanager.cpp ${UI_BLUETOOTH_L5} ${BLUEZ_DBUS_FILES})
add_library(bt_manager_p5 STATIC bluetoothmanager.cpp ${UI_BLUETOOTH_P5} ${BLUEZ_DBUS_FILES})
add_library(bt_manager_l6 STATIC bluetoothmanager.cpp ${UI_BLUETOOTH_L6} ${BLUEZ_DBUS_FILES})
add_library(bt_manager_p6 STATIC bluetoothmanager.cpp ${UI_BLUETOOTH_P6} ${BLUEZ_DBUS_FILES})

set_target_properties(bt_manager_l5 PROPERTIES COMPILE_FLAGS "-DDASH_LAYOUT_L5=1")
set_target_properties(bt_manager_p5 PROPERTIES COMPILE_FLAGS "-DDASH_LAYOUT_P5=1")
set_target_properties(bt_manager_l6 PROPERTIES COMPILE_FLAGS "-DDASH_LAYOUT_L6=1")
set_target_properties(bt_manager_p6 PROPERTIES COMPILE_FLAGS "-DDASH_LAYOUT_P6=1")

target_link_libraries(bt_manager_l5 Qt5::Widgets)
target_link_libraries(bt_manager_p5 Qt5::Widgets)
target_link_libraries(bt_manager_l6 Qt5::Widgets)
target_link_libraries(bt_manager_p6 Qt5::Widgets)
