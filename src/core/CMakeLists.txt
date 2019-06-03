project(core)

set(core_SRCS
    iniparser.cpp
    logger.cpp
    maincontroller.cpp
    task.cpp
    tasklogfile.cpp
    taskmodel.cpp
    tasktools.cpp
)

add_library(dschedcore STATIC ${core_SRCS})

target_link_libraries(dschedcore Qt5::Core)
