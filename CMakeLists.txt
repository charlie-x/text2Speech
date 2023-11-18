# set the minimum version of CMake required
cmake_minimum_required(VERSION 3.17)

project(text2Speech VERSION 1.0 DESCRIPTION "text2Speech" LANGUAGES CXX)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


# Set the default build type if not specified
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type (default Debug)" FORCE)
endif()

#	
# no
# -fpic -shared

# match VC
set(CMAKE_C_FLAGS "-funsigned-char ")
set(CMAKE_C_FLAGS_DEBUG "-O0 -ggdb -fasynchronous-unwind-tables")
set(CMAKE_C_FLAGS_RELEASE "-mtune=generic -O2 -ftree-loop-vectorize -Wall -pedantic -mstackrealign  -flto -fomit-frame-pointer -DNDEBUG -fcf-protection-Wno-unknown-pragmas -fexceptions -fstack-clash-protection -fpie -Wl,-pie -Wl,-z,relro	-Wl,-z,defs	-Wl,-z,now	")

set(CMAKE_CXX_FLAGS "-funsigned-char")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb -fasynchronous-unwind-tables")
set(CMAKE_CXX_FLAGS_RELEASE "-mtune=generic -O2 -ftree-loop-vectorize -Wall -pedantic -mstackrealign  -flto -fomit-frame-pointer -DNDEBUG -fcf-protection -Wno-unknown-pragmas -fexceptions -fstack-clash-protection -fpie -Wl,-pie -Wl,-z,relro	-Wl,-z,defs	-Wl,-z,now")

find_package(argparse CONFIG REQUIRED)

# find the spdlog library package using vcpkg
find_package(spdlog CONFIG REQUIRED)

# find the nlohmann-json package using vcpkg
find_package(nlohmann_json CONFIG REQUIRED)

# find the cpr package using vcpkg
find_package(cpr CONFIG REQUIRED)

add_executable(${PROJECT_NAME} text2Speech.cpp)


# Set output file name for different build types
set_target_properties(${PROJECT_NAME} PROPERTIES
  DEBUG_POSTFIX _d
  RELEASE_POSTFIX ""
  MINSIZEREL_POSTFIX _msr
  RELWITHDEBINFO_POSTFIX _rd
)

# Set output file name for Debug configuration with _d postfix
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX _d)

target_link_libraries(${PROJECT_NAME}  PRIVATE argparse::argparse)

target_link_libraries(${PROJECT_NAME}  PRIVATE cpr::cpr)

target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog_header_only)



# link the libraries to your project, conditionally include hwloc
target_link_libraries(${PROJECT_NAME} PRIVATE 
  nlohmann_json 
  nlohmann_json::nlohmann_json
  )

# Define Debug and Release specific compile options
target_compile_options(${PROJECT_NAME} PRIVATE
"$<$<CONFIG:Debug>:-DDEBUG -D_DEBUG -O0 -g>"
"$<$<CONFIG:Release>:>"
)


# since cpp-httplib is header-only, we don't need to link it as a library,
# but we do need to ensure we add the include directory if it's not automatically added.

# set up installation rules
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
