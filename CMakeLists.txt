cmake_minimum_required(VERSION 3.23.2)

set (CMAKE_CXX_STANDARD 17)

project(four-souls)

add_subdirectory(external/SDL)
add_subdirectory(external/SDL_ttf)
add_subdirectory(external/Lua)

# Source code for server		
set (server_src src/server.cpp)

# Source code for client		
set (client_src src/client.cpp)

add_executable(four-souls-server
    ${server_src}
    # ${srcFiles}
)

add_executable(four-souls-client
    ${client_src}
    # ${srcFiles}
)

add_library(asio INTERFACE)
target_include_directories(asio INTERFACE external/asio/asio/include)

add_library(game STATIC)
target_include_directories(game PUBLIC src/game)

# ! reconfigure when adding new cpp files
file(GLOB gameFiles 
    src/util.cpp
    src/game/*.cpp
)

target_sources(game PRIVATE
    ${gameFiles}
)

target_include_directories(game
    PUBLIC external/SDL/include
    PUBLIC external/SDL_ttf
)

target_link_directories(game
    PUBLIC external/SDL/src
    PUBLIC external/SDL_ttf
)

target_link_libraries(game
    SDL2
    SDL2_ttf
    lua_static
    asio
)

target_include_directories(four-souls-server 
    PUBLIC external/SDL/include
    PUBLIC external/SDL_ttf
)

target_link_directories(four-souls-server
    PUBLIC external/SDL/src
    PUBLIC external/SDL_ttf
)

target_link_libraries(four-souls-server
    SDL2
    SDL2_ttf
    lua_static
    asio
    game
)

target_include_directories(four-souls-client 
    PUBLIC external/SDL/include
    PUBLIC external/SDL_ttf
)

target_link_directories(four-souls-client
    PRIVATE external/SDL/src
    PRIVATE external/SDL_ttf
)

target_link_libraries(four-souls-client
    SDL2
    SDL2_ttf
    lua_static
    asio
    game
)