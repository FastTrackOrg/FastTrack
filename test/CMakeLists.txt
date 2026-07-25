cmake_minimum_required(VERSION 3.18)
project(FastTrackTest)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Sql Test)

add_executable(test-fasttrack
    trackingTest.cpp
    HungarianTest.cpp
    ../src/tracking.cpp
    ../src/videoreader.cpp
    ../src/Hungarian.cpp
    ../src/autolevel.cpp
    ../src/data.cpp

    ../src/tracking.h
    ../src/videoreader.h
    ../src/Hungarian.h
    ../src/autolevel.h
    ../src/data.h
)

include(GoogleTest)

add_compile_definitions(
    DATASET_DIR="${CMAKE_CURRENT_SOURCE_DIR}/dataSet"
)

target_compile_definitions(test-fasttrack PRIVATE QT_DEPRECATED_WARNINGS)
set_target_properties(test-fasttrack PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

if(OpenMP_CXX_FOUND)
    target_link_libraries(test-fasttrack PRIVATE OpenMP::OpenMP_CXX)
endif()

if(UNIX AND NOT APPLE)
    target_compile_options(test-fasttrack PRIVATE -O3 -std=c++17 -g)
    target_include_directories(test-fasttrack PRIVATE /usr/include/opencv4 /usr/local/include/opencv4 /usr/include/gtest)
    target_link_libraries(test-fasttrack PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Sql
        opencv_core opencv_imgproc opencv_highgui opencv_imgcodecs
        opencv_videoio opencv_video opencv_calib3d opencv_photo opencv_features2d
        gtest gtest_main)
elseif(WIN32)
    target_compile_options(test-fasttrack PRIVATE -O3 -g)
    target_include_directories(test-fasttrack PRIVATE "${CMAKE_SOURCE_DIR}/OpenCV_MinGW_64/include" "${CMAKE_SOURCE_DIR}/googletest/build/install/include")
    link_directories("${CMAKE_SOURCE_DIR}/OpenCV_MinGW_64/lib" "${CMAKE_SOURCE_DIR}/googletest/build/install/lib")
    target_link_libraries(test-fasttrack PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Sql "${CMAKE_SOURCE_DIR}/OpenCV_MinGW_64/lib/libopencv_world4100.dll.a" "${CMAKE_SOURCE_DIR}/googletest/build/install/lib/libgtest.a" "${CMAKE_SOURCE_DIR}/googletest/build/install/lib/libgtest_main.a")

    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${Qt6_DIR}/../../../bin" REQUIRED)
    add_custom_command(TARGET test-fasttrack POST_BUILD
        COMMAND "${WINDEPLOYQT_EXECUTABLE}"
                --no-translations
                --compiler-runtime
                "$<TARGET_FILE:test-fasttrack>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${CMAKE_SOURCE_DIR}/OpenCV_MinGW_64/bin/libopencv_world4100.dll"
                "$<TARGET_FILE_DIR:test-fasttrack>"
        VERBATIM
    )
elseif(APPLE)
    target_compile_options(test-fasttrack PRIVATE -std=c++1z -stdlib=libc++ -O3 -g)
    find_package(GTest REQUIRED)
    find_package(OpenCV REQUIRED COMPONENTS
        core imgproc highgui imgcodecs
        videoio video calib3d photo features2d
    )
    target_include_directories(test-fasttrack PRIVATE ${OpenCV_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS})
    target_link_libraries(test-fasttrack PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Sql ${OpenCV_LIBS} ${GTEST_LIBRARIES})
endif()

gtest_discover_tests(test-fasttrack DISCOVERY_MODE PRE_TEST)
