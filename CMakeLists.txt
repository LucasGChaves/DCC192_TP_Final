cmake_minimum_required(VERSION 3.26)
# Configurando o padrão c++
set(CMAKE_CXX_STANDARD 17)

# Configurando o nome e executável do projeto
set(PROJECT_NAME "tp-final")
project(${PROJECT_NAME} LANGUAGES CXX)

find_package(SDL2 2.32.2 EXACT REQUIRED)
find_package(SDL2_image REQUIRED)

add_executable(${PROJECT_NAME}
        source/Main.cpp
        source/Math.cpp
        source/Random.cpp
        Source/actors/Actor.cpp
        Source/components/Component.cpp
        source/Game.cpp
        Source/components/DrawComponents/DrawComponent.cpp
        Source/components/DrawComponents/DrawComponent.h
        Source/components/RigidBodyComponent.cpp
        Source/components/RigidBodyComponent.h
        Source/components/ColliderComponents/CircleColliderComponent.cpp
        Source/components/ColliderComponents/CircleColliderComponent.h
        source/Json.h
        source/CSV.h
        Source/components/DrawComponents/DrawSpriteComponent.cpp
        Source/components/DrawComponents/DrawSpriteComponent.h
        Source/components/DrawComponents/DrawPolygonComponent.cpp
        Source/components/DrawComponents/DrawPolygonComponent.h
        Source/components/ColliderComponents/AABBColliderComponent.cpp
        Source/components/ColliderComponents/AABBColliderComponent.h
        Source/components/DrawComponents/DrawAnimatedComponent.cpp
        Source/components/DrawComponents/DrawAnimatedComponent.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2 SDL2_image::SDL2_image)