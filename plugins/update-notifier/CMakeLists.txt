add_library (updatenotifier MODULE
    updatenotifier.h
    updatenotifier.cpp
)

target_compile_definitions (updatenotifier
    PRIVATE -DQT_PLUGIN -DQT_SHARED
)

target_link_libraries (updatenotifier morgoth)
