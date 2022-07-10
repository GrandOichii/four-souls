cmake_minimum_required(VERSION 3.23.2)

set (CMAKE_CXX_STANDARD 17)

project(four-souls)

# ! reconfigure when adding new cpp files
file(GLOB srcFiles 
    src/*.cpp
    src/game/*.cpp
)

add_executable(${PROJECT_NAME}
    ${srcFiles}
)

add_subdirectory(external/SDL)
add_subdirectory(external/SDL_ttf)
add_subdirectory(external/Lua)

target_include_directories(${PROJECT_NAME} 
    PUBLIC external/SDL/include
    PUBLIC external/SDL_ttf
)
    
target_link_directories(${PROJECT_NAME}
    PRIVATE external/SDL/src
    PRIVATE external/SDL_ttf
)

target_link_libraries(${PROJECT_NAME}
    SDL2
    SDL2_ttf
    lua_static
)