cmake_minimum_required(VERSION 3.10)


project(PixelFrame)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_DIR})

set(  PIXEL_FRAME_SRC
	main.cpp
	pixelFrame.cpp

	)
add_executable(${PROJECT_NAME} ${PIXEL_FRAME_SRC})


target_link_libraries(${PROJECT_NAME} pthread)

