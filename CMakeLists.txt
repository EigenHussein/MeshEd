cmake_minimum_required(VERSION 3.14)

# if(NOT CMAKE_BUILD_TYPE)
#   set(CMAKE_BUILD_TYPE Debug)
# endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 14)

project(MeshEd)

find_package(Armadillo REQUIRED)
find_package(OpenCV REQUIRED)

set(SRC_DIR /home/hussein/Dev/Sources/CoreV2)
set(PROJ ModelTransition/ModelTrans)

file(GLOB imgui_cpps
     "./dependencies/imgui/*.cpp"
)


add_executable(${PROJECT_NAME} 
 #   ./fun2learn/smart_pointers.cpp
    ./source/${PROJ}.cpp
    ./src/Cycloid.cpp
    ./src/CycloidGraphs.cpp
    ./src/SVGParser.cpp
    ${SRC_DIR}/Recorder.cpp
    ${SRC_DIR}/Core2.cpp
    ${SRC_DIR}/Program.cpp
    ${SRC_DIR}/Model.cpp
    ${SRC_DIR}/VirtualScreen.cpp

    #imgui
    ${imgui_cpps}
#    ./dependencies/imgui/**.cpp
#    ./dependencies/imgui/imgui.cpp
#    ./dependencies/imgui/imgui_demo.cpp
#    ./dependencies/imgui/imgui_draw.cpp
#    ./dependencies/imgui/imgui_tables.cpp
#    ./dependencies/imgui/imgui_widgets.cpp
    ./dependencies/imgui/backends/imgui_impl_glfw.cpp
    ./dependencies/imgui/backends/imgui_impl_opengl3.cpp
)

include_directories(${SRC_DIR})
#target_include_directories(./include)
include_directories(include)
include_directories(dependencies/imgui)
include_directories(dependencies/rapidxml-1.13)

#target_include_directories()
#target_include_directories(${PROJECT_NAME} PRIVATE /home/hussein/Dev/Sources/CoreV2)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-D__STDC_CONSTANT_MACROS")

find_path(AVCODEC_INCLUDE_DIR libavcodec/avcodec.h)
find_library(AVCODEC_LIBRARY avcodec)

message(${AVCODEC_LIBRARY})

find_path(AVFORMAT_INCLUDE_DIR libavformat/avformat.h)
find_library(AVFORMAT_LIBRARY avformat)

find_path(AVUTIL_INCLUDE_DIR libavutil/avutil.h)
find_library(AVUTIL_LIBRARY avutil)

find_path(AVDEVICE_INCLUDE_DIR libavdevice/avdevice.h)
find_library(AVDEVICE_LIBRARY avdevice)

#add_executable(decode_encode main.cpp)
#target_include_directories(${PROJECT_NAME} PRIVATE ${AVCODEC_INCLUDE_DIR} ${AVFORMAT_INCLUDE_DIR} ${AVUTIL_INCLUDE_DIR} ${AVDEVICE_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE ${AVCODEC_LIBRARY} ${AVFORMAT_LIBRARY} ${AVUTIL_LIBRARY} ${AVDEVICE_LIBRARY})


target_link_directories(${PROJECT_NAME} PUBLIC /usr/lib/x86_64-linux-gnu)

target_link_libraries(${PROJECT_NAME} PUBLIC ${OpenCV_LIBS} ${CMAKE_DL_LIBS} -lfftw3 -lfftw3f -larmadillo -lswscale -lGLEW -lGLU -lGL -lglfw -lassimp)

