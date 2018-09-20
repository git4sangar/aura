#sgn
cmake_minimum_required(VERSION 2.8.4)
project(WatchDogServer)

# compiling options
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(AURA_DEPS $ENV{HOME}/sgn/dev)
set(AURA_DEPS_LIB ${AURA_DEPS}/lib)
set(AURA_DEPS_INCLUDE ${AURA_DEPS}/include)
message ("AURA_DEPS: ${AURA_DEPS}")

add_definitions(-DAURA_BIN_PATH=${AURA_DEPS}/bin)
add_definitions(-DAURA_BIN_FILE=SoapSeller)
set(CMAKE_INSTALL_PREFIX ${AURA_DEPS})
message ("CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")

INCLUDE_DIRECTORIES(${AURA_DEPS_INCLUDE})
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)
message("project source dir: ${PROJECT_SOURCE_DIR}")

find_library(BOOST_SYSTEM NAMES boost_system HINTS "${AURA_DEPS_LIB}/boost")
find_library(BOOST_CHRONO NAMES boost_chrono HINTS "${AURA_DEPS_LIB}/boost")
find_library(BOOST_THREAD NAMES boost_thread HINTS "${AURA_DEPS_LIB}/boost")
LINK_DIRECTORIES(${AURA_DEPS_LIB})

add_executable(WatchDogServer WatchDog.cpp)

target_link_libraries(WatchDogServer ${CMAKE_THREAD_LIBS_INIT} ${BOOST_THREAD} ${BOOST_SYSTEM} pthread dl)
install(TARGETS ${PROJECT_NAME} DESTINATION bin)