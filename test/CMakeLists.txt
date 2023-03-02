project(fractalsTests LANGUAGES CXX)

add_executable(fractals_test source/fractals_test.cc)
target_link_libraries(fractals_test PRIVATE fractals_lib)
target_compile_features(fractals_test PRIVATE cxx_std_20)

add_test(NAME fractals_test COMMAND fractals_test)

