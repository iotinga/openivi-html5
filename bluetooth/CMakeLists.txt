
set(CMAKE_INCLUDE_CURRENT_DIR ON)

#qt5_wrap_ui(UI_BLUETOOTH bt_manager.ui)
qt5_wrap_ui(UI_BLUETOOTH bluetoothmanager.ui)
qt5_add_dbus_interfaces(BLUEZ_DBUS_FILES bluez_device.xml)

message("Binary dir: ${CMAKE_CURRENT_BINARY_DIR}")

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(bt_manager STATIC bluetoothmanager.cpp ${UI_BLUETOOTH} ${BLUEZ_DBUS_FILES})

target_link_libraries(bt_manager Qt5::Widgets)
