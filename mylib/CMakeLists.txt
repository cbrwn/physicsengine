cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(mylib)

add_library(${PROJECT_NAME}
	color.cpp
	gmath.cpp
	matrix2.cpp
	matrix3.cpp
	matrix4.cpp
	vector2.cpp
	vector3.cpp
	vector4.cpp
 )

target_include_directories(${PROJECT_NAME} PUBLIC
    ${PROJECT_SOURCE_DIR}
    )
