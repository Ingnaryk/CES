cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE Release)

project(example LANGUAGES CXX)
add_executable(${PROJECT_NAME})
aux_source_directory(main sources)
aux_source_directory(src sources)
aux_source_directory(test sources)
target_sources(${PROJECT_NAME} PUBLIC ${sources})

target_include_directories(${PROJECT_NAME} PUBLIC include)