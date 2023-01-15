cmake_minimum_required(VERSION 3.14)
project(Cpp_Game LANGUAGES CXX)
find_package(Threads REQUIRED)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_executable(Cpp_Game src/main.cpp src/SHA256.cpp)
target_link_libraries(Cpp_Game PRIVATE sfml-graphics sfml-window sfml-system sfml-network sfml-audio Threads::Threads)
target_compile_features(Cpp_Game PRIVATE cxx_std_17)
if (WIN32 AND BUILD_SHARED_LIBS)
    add_custom_command(TARGET Cpp_Game POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:Cpp_Game> $<TARGET_FILE_DIR:Cpp_Game> COMMAND_EXPAND_LISTS)
endif()
add_custom_target(copy_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}/bin/assets
)
add_dependencies(Cpp_Game copy_assets)
add_custom_target(copy_maps
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}//maps ${CMAKE_CURRENT_BINARY_DIR}/bin/maps
)
add_dependencies(Cpp_Game copy_maps)
target_include_directories(Cpp_Game PRIVATE
                          "${PROJECT_SOURCE_DIR}/include"
                          )

install(TARGETS Cpp_Game)