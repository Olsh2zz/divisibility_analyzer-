# Сборка: библиотека analyzer_core, программа divisibility_analyzer и тесты analyzer_tests.
cmake_minimum_required(VERSION 3.10)
project(divisibility_analyzer CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    # Исходники в UTF-8: без этого флага MSVC испортит русские строки.
    add_compile_options(/utf-8 /W4)
else()
    add_compile_options(-Wall -Wextra)
endif()

add_library(analyzer_core
    src/divisibility.cpp
    src/rules.cpp
    src/history.cpp
    src/console.cpp
)
target_include_directories(analyzer_core PUBLIC src)

add_executable(divisibility_analyzer src/main.cpp)
target_link_libraries(divisibility_analyzer PRIVATE analyzer_core)

enable_testing()
add_executable(analyzer_tests tests/tests.cpp)
target_link_libraries(analyzer_tests PRIVATE analyzer_core)
add_test(NAME analyzer_tests COMMAND analyzer_tests)
