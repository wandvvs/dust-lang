cmake_minimum_required(VERSION 3.27)

project(dust-lang)

find_package(GTest REQUIRED)
find_package(LLVM REQUIRED CONFIG)

option(BUILD_TESTS "Build tests" ON)

set(CMAKE_CXX_STANDARD 20)
include_directories(${LLVM_INCLUDE_DIRS}) 

add_executable(dust-lang
               main.cpp

               source/lexer/lexer.hpp
               source/lexer/lexer.cpp

               source/token/token_type.hpp
               source/token/token.cpp
               source/token/token.hpp

               source/token/token_buffer/token_buffer.cpp
               source/token/token_buffer/token_buffer.hpp

               source/llvm/llvm_compiler.hpp
               source/llvm/llvm_compiler.cpp

               source/llvm/llvm_executable_builder.hpp
               source/llvm/llvm_executable_builder.cpp
)

llvm_map_components_to_libnames(llvm_libs support core irreader)

target_link_libraries(dust-lang ${llvm_libs})

enable_testing()

if(BUILD_TESTS)
    add_executable(dust-lang-tests
                   test/tests.cpp
                   )
    
    target_link_libraries(dust-lang-tests ${GTEST_LIBRARIES} pthread)
    
    add_test(NAME dust-lang-tests COMMAND dust-lang-tests)
endif()
