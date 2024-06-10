doudizhu_learning_environment is a research platform for a game named
doudizhu (aka fighting the landlord) [(wiki)](https://en.wikipedia.org/wiki/Dou_dizhu).

The library is written in C++. You can use Cmake to compile and run examples.

The moves in this library are encoded identically with [RLCard](https://github.com/datamllab/rlcard/tree/master).

The main usage of the library is very similar
with [hanabi_learning_environment](https://github.com/google-deepmind/hanabi-learning-environment/tree/master)
and [bridge_learning_environment](https://github.com/zizhang-qiu/bridge_learning/tree/main/bridge_lib).

# Installation

You need to have [CMake](https://cmake.org/) as well as python to install the library.

```shell
mkdir build && cd build
cmake ..
make -j10
```

If you are using python in a specific environment created by programs like Anaconda,
you should specify your python executable path

```shell
cmake .. -DPython_EXECUTABLE=/path/to/python_executable
```

The library can also be used by other programs using cmake `add_subdirectory()` syntax.

# Todo:

- [ ] Doudizhu observation encoders.
- [ ] Gym-like rl envs.
- [x] Doudizhu game examples.