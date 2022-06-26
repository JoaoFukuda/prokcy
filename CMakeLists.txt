cmake_minimum_required(VERSION 3.18)
project(prokcy)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(prokcy src/prokcy.c src/binio.c src/handlers.c)
