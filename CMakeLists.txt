cmake_minimum_required(VERSION 3.28)
project(doudizhu_learning_environment)

set(CMAKE_CXX_STANDARD 17)
#set(Python_EXECUTABLE "D:\\Anaconda\\envs\\doudizhu\\python.exe")

# New Python support
if(DEFINED Python_EXECUTABLE)
    set(PYTHON_EXECUTABLE "${Python_EXECUTABLE}")
    set(PYTHON_VERSION "${Python_VERSION}")
endif()

add_subdirectory(doudizhu_learning_environment/doudizhu_lib)
add_subdirectory(doudizhu_learning_environment)
