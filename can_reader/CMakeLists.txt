# Create a library to read CAN bus


set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Widgets COMPONENTS Core REQUIRED)

set(CAN_READER_SOURCES canreader.cpp)

add_library(canreader STATIC ${CAN_READER_SOURCES})

target_link_libraries(canreader Qt5::Core)
