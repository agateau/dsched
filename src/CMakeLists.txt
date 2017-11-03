# Sources
set(dsched_SRCS
    iniparser.cpp
    logger.cpp
    main.cpp
    maincontroller.cpp
    task.cpp
    taskmodel.cpp
    tasktools.cpp
    mainwindow.cpp
)

qt5_add_resources(UI_RESOURCES ../icons/icons.qrc)

# Building
include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Woverloaded-virtual -std=c++11")
add_executable(dsched ${dsched_SRCS} ${UI_RESOURCES})

target_link_libraries(dsched Qt5::Core Qt5::Widgets)

# Install
install(TARGETS dsched
    DESTINATION bin
)
