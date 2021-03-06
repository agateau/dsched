project(ui)

# Sources
set(dsched_SRCS
    loghighlighter.cpp
    main.cpp
    mainwindow.cpp
    taskproxymodel.cpp
    traycontroller.cpp
)

qt5_add_resources(UI_RESOURCES icons/icons.qrc)

# Building
include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${core_SOURCE_DIR}
)

add_executable(dsched ${dsched_SRCS} ${UI_RESOURCES})

target_link_libraries(dsched dschedcore Qt5::Core Qt5::Widgets)

# Install
install(TARGETS dsched
    DESTINATION bin
)

install(FILES icons/dsched.desktop
    DESTINATION share/applications
    RENAME com.agateau.dsched.desktop
)

install(FILES icons/appicon.svg
    DESTINATION share/icons/hicolor/scalable/apps
    RENAME dsched.svg
)
