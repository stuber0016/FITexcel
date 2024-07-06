cmake_minimum_required(VERSION 3.22)
project(excel)

set(CMAKE_CXX_STANDARD 20)


#add_compile_options(-std=c++20 -Wall -pedantic -O2)
add_compile_options(-std=c++20 -Wall -Wextra -Wpedantic -Werror -Wshadow -g -Wsuggest-attribute=const -Wmaybe-uninitialized)

add_executable(test test.cpp
        CPos.cpp
        CPos.hpp
        CSpreadsheet.hpp
        CSpreadsheet.cpp
        HashForPair.hpp
        CellValue.hpp
        PostfixExpressionBuilder.hpp
        PostfixExpressionBuilder.cpp
        stackoperators_custom.hpp
        stackoperators_custom.cpp
        CellValue.cpp
        constantexpressions.hpp
)
target_link_libraries(test ${CMAKE_SOURCE_DIR}/x86_64-linux-gnu/libexpression_parser.a)


