project(core)

set(core_SRCS
    iniparser.cpp
)

add_library(dschedcore STATIC ${core_SRCS})

target_link_libraries(dschedcore Qt5::Core)
