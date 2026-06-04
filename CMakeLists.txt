cmake_minimum_required(VERSION 3.20)
project(NeuralMesh VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(neuralmesh
  src/buffer.cpp
  src/event_log.cpp
  src/framed_rpc.cpp
  src/model.cpp
  src/thread_pool.cpp
)
target_include_directories(neuralmesh PUBLIC include)

if(MSVC)
  target_compile_options(neuralmesh PRIVATE /W4)
else()
  target_compile_options(neuralmesh PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(neuralmesh_server src/server_main.cpp)
target_link_libraries(neuralmesh_server PRIVATE neuralmesh)

add_executable(neuralmesh_tests
  tests/test_main.cpp
  tests/test_buffer.cpp
  tests/test_event_log.cpp
  tests/test_model.cpp
  tests/test_thread_pool.cpp
)
target_link_libraries(neuralmesh_tests PRIVATE neuralmesh)

enable_testing()
add_test(NAME neuralmesh_tests COMMAND neuralmesh_tests)
