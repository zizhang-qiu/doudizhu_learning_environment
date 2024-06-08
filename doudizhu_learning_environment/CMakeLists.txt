add_subdirectory(third_party/pybind11)

pybind11_add_module(pydoudizhu SHARED pydoudizhu.cc)
target_link_libraries(pydoudizhu PUBLIC doudizhu)