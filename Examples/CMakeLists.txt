add_executable(example example/main.cpp)
target_link_libraries(example advobfuscator)

add_executable(demo demo/demo.cpp)
set_target_properties(demo PROPERTIES CXX_VISIBILITY_PRESET hidden)
target_link_libraries(demo advobfuscator)

add_executable(guessme demo/guessme.cpp)
set_target_properties(guessme PROPERTIES CXX_VISIBILITY_PRESET hidden)
target_link_libraries(guessme advobfuscator)

add_executable(guessme_aes demo/guessme_aes.cpp)
set_target_properties(guessme_aes PROPERTIES CXX_VISIBILITY_PRESET hidden)
target_link_libraries(guessme_aes advobfuscator)
