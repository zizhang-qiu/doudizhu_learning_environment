cmake_minimum_required(VERSION 3.28)
project(doudizhu_learning_environment_example)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(../doudizhu_learning_environment/doudizhu_lib doudizhu_lib)

add_executable(game_example game_example.cc)
target_link_libraries(game_example LINK_PUBLIC doudizhu)
