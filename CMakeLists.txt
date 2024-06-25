cmake_minimum_required(VERSION 3.28)
project(doudizhu_learning_environment)

set(CMAKE_CXX_STANDARD 17)
#set(Python_EXECUTABLE "D:\\Anaconda\\envs\\doudizhu\\python.exe")

if (not WIN32)
    set(CMAKE_CXX_FLAGS
            "${CMAKE_CXX_FLAGS} -O3 -Wall -Wextra -Wno-register -fPIC -march=native -Wfatal-errors")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DPYBIND11_COMPILER_TYPE=\\\"_gcc\\\" -DPYBIND11_STDLIB=\\\"_libstdcpp\\\" -DPYBIND11_BUILD_ABI=\\\"_cxxabi1011\\\"")
endif ()
# New Python support
if (DEFINED Python_EXECUTABLE)
    set(PYTHON_EXECUTABLE "${Python_EXECUTABLE}")
    set(PYTHON_VERSION "${Python_VERSION}")
endif ()

add_subdirectory(doudizhu_learning_environment/doudizhu_lib)
add_subdirectory(doudizhu_learning_environment)
