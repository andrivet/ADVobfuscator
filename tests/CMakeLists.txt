add_executable(tests main.cpp)
target_link_libraries(tests advobfuscator)
add_test(NAME tests COMMAND tests)
