add_library(ofono-dash STATIC OfonoManager.cpp OfonoVoiceCallManager.cpp OfonoVoiceCall.cpp OfonoNetworkRegistration.cpp OfonoHandsfree.cpp OfonoCallVolume.cpp)

target_link_libraries(ofono-dash Qt5::DBus)
