cmake_minimum_required(VERSION 3.1)
project(sc)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(Freetype REQUIRED)

set(SOURCE_FILES src/Board.cpp
                 src/Cards.cpp
                 src/Draw.cpp
                 src/GLError.cpp
                 src/Letter.cpp
                 src/Rectangle.cpp
                 src/SpellCaster.cpp
                 src/UCTRave.cpp
                 src/UIState.cpp
                 src/geometry.cpp
                 src/sc.cpp
                 src/shaders.cpp
                 glad/glad.c
                 json11/json11.cpp)

add_executable(sc ${SOURCE_FILES})

set_property(TARGET sc PROPERTY CXX_STANDARD 14)
set_property(GLOBAL PROPERTY CXX_STANDARD_REQUIRED true)
target_compile_features(sc PRIVATE cxx_range_for)
target_compile_features(sc PRIVATE cxx_strong_enums)
target_compile_features(sc PRIVATE cxx_uniform_initialization)

include_directories(${PROJECT_BINARY_DIR})
include_directories(${SDL2_INCLUDE_DIR} ${SDL2_IMAGE_INCLUDE_DIR} ${FREETYPE_INCLUDE_DIRS})

target_link_libraries(sc ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARY} ${FREETYPE_LIBRARY} ${CMAKE_DL_LIBS})
