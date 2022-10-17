cmake_minimum_required(VERSION 3.5)
cmake_policy(SET CMP0079 NEW)
cmake_policy(SET CMP0060 NEW)

string(ASCII 27 ESC)

project(task_test )

set(CMAKE_BUILD_TYPE ) 

set(C_HEADER_SUFFIX "h")
set(C_SOURCE_SUFFIX "c")
set(CXX_HEADER_SUFFIX "hpp")
set(CXX_SOURCE_SUFFIX "cpp")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# debug options 
message("${ESC}[31m${ESC}")
message("\t CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")
message("${ESC}[0m${ESC}")

IF(CMAKE_BUILD_TYPE MATCHES Debug)
  message("${ESC}[31m${ESC}")
  message(" \t——————————► DEBUG ◄—————————— ")
  message("${ESC}[0m${ESC} ")

  #add_compile_options(-Werror)
  add_compile_options(-pedantic-errors)
  add_compile_options(-Wall)
  add_compile_options(-Wextra)
  add_compile_options(-pg -ggdb -O0 -g3 -pg)
  add_compile_options(-Wfloat-equal) 
  add_compile_options(-Wpointer-arith)
  add_compile_options(-Wstrict-overflow=5) 
  add_compile_options(-Wwrite-strings) 
  add_compile_options(-Wswitch-default -Wswitch-enum)
  add_compile_options(-Wconversion) 
  add_compile_options(-Wunreachable-code)
  add_compile_options(-fstack-clash-protection -fstack-protector	-fcf-protection)
  add_compile_options(-fasynchronous-unwind-tables)
  
  add_compile_definitions(DEBUG)
  add_compile_definitions(DEBUG_PRINT)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
  enable_testing()

ELSEIF(CMAKE_BUILD_TYPE MATCHES Release)
message("${ESC}[32m${ESC}")
message("\t————————► RELEASE ◄————————")
message("${ESC}[0m${ESC}")
  add_compile_options( -O2  -std=c++17 )
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

  add_compile_definitions(RELEASE)

ELSE()
   message("${ESC}[31m${ESC}")
  message( FATAL_ERROR "ALARM Build type abort")
  message("${ESC}[0m${ESC} ")
ENDIF()

message(" src load ")
link_libraries(pthread)
add_subdirectory(src)
add_executable (server src/main.cpp)
