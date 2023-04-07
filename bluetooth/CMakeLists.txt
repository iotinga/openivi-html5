
set(CMAKE_INCLUDE_CURRENT_DIR ON)

#qt5_wrap_ui(UI_BLUETOOTH bt_manager.ui)
qt5_wrap_ui(UI_BLUETOOTH bluetoothmanager.ui)

add_library(bt_manager STATIC bluetoothmanager.cpp ${UI_BLUETOOTH})

target_link_libraries(bt_manager Qt5::Widgets)
