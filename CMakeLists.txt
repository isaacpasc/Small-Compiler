cmake_minimum_required(VERSION 3.23)
project(P3CSE340)

set(CMAKE_CXX_STANDARD 11)

add_executable(P3CSE340 demo.cc execute.cc execute.h inputbuf.cc inputbuf.h lexer.h lexer.cc parser.cpp parser.h)
