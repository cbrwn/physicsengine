cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(physicsengine)


set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})

add_executable(${PROJECT_NAME}
    box.cpp
    ball.cpp
    plane.cpp
    camera.cpp
    gamehud.cpp
	gamestate.cpp
    basestate.cpp
    objectpool.cpp
    collidercone.cpp
    physicsactor.cpp
    collidersphere.cpp
    collidercylinder.cpp
    physicsmanager.cpp
    collideraabb.cpp
    physicsbody.cpp
    demostate.cpp
    collider.cpp
    shapes.cpp
    world.cpp
    actor.cpp
    game.cpp
    main.cpp
    )

target_link_libraries(${PROJECT_NAME} aieBootstrap mylib)
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME game)

target_include_directories(${PROJECT_NAME} PUBLIC
	${PROJECT_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/dependencies/glm
	)
