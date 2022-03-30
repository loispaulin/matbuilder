project(MatBuilder)
cmake_minimum_required (VERSION 3.13)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(${CMAKE_VERSION} VERSION_LESS 3.14)
    macro(FetchContent_MakeAvailable NAME)
        FetchContent_GetProperties(${NAME})
        if(NOT ${NAME}_POPULATED)
            FetchContent_Populate(${NAME})
            add_subdirectory(${${NAME}_SOURCE_DIR} ${${NAME}_BINARY_DIR})
        endif()
    endmacro()
endif()


set(CMAKE_OSX_ARCHITECTURES "x86_64")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

include(galois)
include_directories(${galois_SOURCE_DIR}/include)

if(APPLE)
  set(CPLEX_INC "/Applications/CPLEX_Studio_Community201/cplex/include")
  set(CPLEX_INC2 "/Applications/CPLEX_Studio_Community201/concert/include")
  set(CPLEX_LIB "/Applications/CPLEX_Studio_Community201/cplex/lib/x86-64_osx/static_pic")
  set(CPLEX_LIB2 "/Applications/CPLEX_Studio_Community201/concert/lib/x86-64_osx/static_pic")
else()
  set(CPLEX_INC "/opt/ibm/ILOG/CPLEX_Studio201/cplex/include")
  set(CPLEX_INC2 "/opt/ibm/ILOG/CPLEX_Studio201/concert/include")
  set(CPLEX_LIB "/opt/ibm/ILOG/CPLEX_Studio201/cplex/lib/x86-64_linux/static_pic")
  set(CPLEX_LIB2 "/opt/ibm/ILOG/CPLEX_Studio201/concert/lib/x86-64_linux/static_pic")
endif()

message(STATUS "CPLEX inc path: ${CPLEX_INC} and ${CPLEX_INC2}")
add_executable(MatBuilder MatBuilder.cpp cplexMatrices.cpp MatrixTools.cpp)
target_include_directories(MatBuilder PRIVATE ${CPLEX_INC} ${CPLEX_INC2})
target_link_directories(MatBuilder PRIVATE ${CPLEX_LIB} ${CPLEX_LIB2} )
target_link_libraries(MatBuilder PRIVATE galois++  concert ilocplex cplex m pthread dl)


add_executable(sampler sampler.cpp)
