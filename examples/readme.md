This directory consists of several examples including

- `game_example.cc` : An example of random playing written in C++.
- `console_play.cc` : An example which allows human to play with random agent through console.
- `smoke_test.py` : A unittest file in python for pydoudizhu.

# Installation

To run these programs, you need to have cmake and run

```shell
mkdir build && cd build
cmake .. (-DPython_EXECUTABLE=/path/to/python_executable)
make -j10
```

# Todo:
There may be several bugs when play against random agent in console because of `GetMoveFromString()` function,
this will be fixed later.