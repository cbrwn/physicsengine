cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(physicsengine)


set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})

add_executable(${PROJECT_NAME}
    main.cpp
    actor.cpp
    ball.cpp
    basestate.cpp
    box.cpp
    camera.cpp
    collider.cpp
    collideraabb.cpp
    collidercone.cpp
    collidercylinder.cpp
    collidersphere.cpp
    demostate.cpp
    game.cpp
    gamehud.cpp
    objectpool.cpp
    physicsactor.cpp
    physicsbody.cpp
    physicsmanager.cpp
    plane.cpp
    shapes.cpp
    util.cpp
    world.cpp
    )
target_link_libraries(${PROJECT_NAME} aieBootstrap mylib)
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME game)

target_include_directories(${PROJECT_NAME} PUBLIC
	${PROJECT_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/dependencies/glm
	)
