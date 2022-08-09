cmake_minimum_required(VERSION 3.23.2)

set (CMAKE_CXX_STANDARD 17)

project(four-souls)

add_subdirectory(external/SDL)
add_subdirectory(external/SDL_ttf)
add_subdirectory(external/Lua)

# set(CMAKE_MACOSX_RPATH 1)
# set(CMAKE_INSTALL_RPATH ${CMAKE_BINARY_DIR}/../lib)

# file(GLOB RESOURCE_FILES
#     assets/config.json
# )

# Source code for server		
set (server_src 
    src/server.cpp
)

# Source code for client		
set (client_src 
    src/client.cpp 
    # ${RESOURCE_FILES}
)

add_executable(four-souls-server
    ${server_src}
)

add_executable(four-souls-client
    ${client_src}
    # ${RESOURCE_FILES}
)
# set_target_properties(four-souls-client PROPERTIES LINK_FLAGS "-Wl,-rpath,./")

# if (APPLE)
#     set_target_properties(four-souls-client PROPERTIES
#         MACOSX_BUNDLE TRUE
#         # RESOURCE "${RESOURCE_FILES}"
#     )
# endif()

add_library(asio INTERFACE)
target_include_directories(asio INTERFACE external/asio/asio/include)

add_library(game STATIC)
target_include_directories(game PUBLIC src/game)

file(GLOB gameFiles 
    src/util.cpp
    src/game/*.cpp
)

target_sources(game PRIVATE
    ${gameFiles}
)

# target_include_directories(game
    # PUBLIC external/SDL/include
    # PUBLIC ${SDL2_INCLUDE_DIR}
# )

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
    # PUBLIC ${SDL2_INCLUDE_DIR}
    # PUBLIC ${SDL2_TTF_INCLUDE_DIR}
)
    
target_link_directories(four-souls-server
    PUBLIC external/SDL/src
    PUBLIC external/SDL_ttf
)

target_link_libraries(four-souls-server
    SDL2
    SDL2_ttf
    # ${SDL2_LIBRARY}
    # ${SDL2_IMAGE_LIBRARIES}
    lua_static
    asio
    game
)
    
target_include_directories(four-souls-client 
    PUBLIC external/SDL/include
    PUBLIC external/SDL_ttf
    # PUBLIC ${SDL2_INCLUDE_DIR}
    # PUBLIC ${SDL2_TTF_INCLUDE_DIR}
)
    
target_link_directories(four-souls-client
    PRIVATE external/SDL/src
    PRIVATE external/SDL_ttf
)
    
target_link_libraries(four-souls-client
    SDL2
    SDL2_ttf
    # ${SDL2_LIBRARY}
    # ${SDL2_IMAGE_LIBRARIES}
    lua_static
    asio
    game
)

# set_target_properties(four-souls-client PROPERTIES LINK_FLAGS "-Wl,-rpath,$ORIGIN")

# install(TARGETS four-souls-client RUNTIME DESTINATION bin)

# include(InstallRequiredSystemLibraries)
# include(CPack)
# message(STATUS ${PROJECT_BINARY_DIR})