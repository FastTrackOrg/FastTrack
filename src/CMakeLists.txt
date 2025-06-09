cmake_minimum_required(VERSION 3.16)
project(FastTrackApp VERSION 6.3.4)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network Svg Sql LinguistTools)

set(TS_FILES
    ../translations/fasttrack_en.ts
)

add_executable(fasttrack
    updater.cpp
    main.cpp
    mainwindow.cpp
    tracking.cpp
    Hungarian.cpp
    replay.cpp
    batch.cpp
    interactive.cpp
    data.cpp
    annotation.cpp
    trackingmanager.cpp
    videoreader.cpp
    timeline.cpp
    autolevel.cpp
    imageviewer.cpp

    updater.h
    mainwindow.h
    tracking.h
    Hungarian.h
    replay.h
    batch.h
    interactive.h
    data.h
    annotation.h
    trackingmanager.h
    videoreader.h
    timeline.h
    autolevel.h
    imageviewer.h

    mainwindow.ui
    replay.ui
    trackingmanager.ui
    batch.ui
    interactive.ui
    timeline.ui

    resources.qrc
    assets/theme.qrc
    ${TS_FILES}
)

qt_add_lrelease(TS_FILES fasttrack_en.ts
    QM_OUTPUT_DIRECTORY translations
)
add_custom_target(translations ALL DEPENDS ${QM_FILES})

target_compile_definitions(fasttrack PRIVATE APP_VERSION="${PROJECT_VERSION}")

add_executable(fasttrack-cli
    fasttrack-cli.cpp
    tracking.cpp
    videoreader.cpp
    Hungarian.cpp

    tracking.h
    videoreader.h
    Hungarian.h
)

target_compile_definitions(fasttrack-cli PRIVATE APP_VERSION="${PROJECT_VERSION}")

function(configure_target target_name)
    if(UNIX AND NOT APPLE)
        target_compile_options(${target_name} PRIVATE -O3)
        target_include_directories(${target_name} PRIVATE /usr/include/opencv4 /usr/local/include/opencv4 ${CMAKE_CURRENT_SOURCE_DIR})
        target_link_libraries(${target_name} PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Sql Qt6::Network
            opencv_core opencv_imgproc opencv_highgui opencv_imgcodecs
            opencv_videoio opencv_video opencv_calib3d opencv_photo opencv_features2d)
    elseif(WIN32)
        set(app_icon_win "${CMAKE_SOURCE_DIR}/src/assets/icon.rc")
        target_sources(${target_name} PRIVATE ${app_icon_win})
        target_compile_options(${target_name} PRIVATE -O3)
        set(MACOSX_BUNDLE_ICON_FILE "${CMAKE_SOURCE_DIR}/src/assets/icon.icns")
        target_include_directories(${target_name} PRIVATE "${CMAKE_SOURCE_DIR}/OpenCV_MinGW_64/include" ${CMAKE_CURRENT_SOURCE_DIR})
        target_link_libraries(${target_name} PRIVATE
          Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Sql Qt6::Network
          "${CMAKE_SOURCE_DIR}/OpenCV_MinGW_64/lib/libopencv_world4100.dll.a"
        )
    elseif(APPLE)
        set_target_properties(${target_name} PROPERTIES
            MACOSX_BUNDLE TRUE
            MACOSX_BUNDLE_ICON_FILE "icon.icns"
        )

        set(app_icon_macos "${CMAKE_SOURCE_DIR}/src/assets/icon.icns")
        set_source_files_properties(${app_icon_macos} PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources"
        )
        target_sources(${target_name} PRIVATE ${app_icon_macos})

        target_compile_options(${target_name} PRIVATE -std=c++1z -stdlib=libc++ -O3)
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(OPENCV REQUIRED opencv4)
        find_package(OpenCV REQUIRED COMPONENTS
            core imgproc highgui imgcodecs
            videoio video calib3d photo features2d
        )
        target_include_directories(${target_name} PRIVATE ${OPENCV_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})
        target_link_libraries(${target_name} PRIVATE
            Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Sql Qt6::Network
            ${OPENCV_LIBRARIES}
        )
    endif()
endfunction()

configure_target(fasttrack)
configure_target(fasttrack-cli)

if(UNIX AND NOT APPLE)
    install(TARGETS fasttrack DESTINATION /usr/bin)
    install(FILES sh.fasttrack.fasttrack.desktop
            DESTINATION share/applications)
    install(FILES sh.fasttrack.fasttrack.metainfo.xml
            DESTINATION share/metainfo)
    install(FILES src/assets/fasttrack.png
            DESTINATION share/icons)
    install(FILES ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/fasttrack_en.qm
            DESTINATION share/locale)
    install(FILES ${CMAKE_SOURCE_DIR}/LICENSE
            DESTINATION share/licenses/fasttrack)
endif()

install(TARGETS fasttrack-cli DESTINATION /usr/bin)
install(FILES ${CMAKE_SOURCE_DIR}/LICENSE DESTINATION /usr/share/licenses/fasttrack)
