cmake_minimum_required(VERSION 2.8.4)
project(SoapSeller)

# compiling options
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(MFCPP_HOME $ENV{HOME}/mfcpp)
set(MFCPP_DEPS ${MFCPP_HOME}/deps)
set(MFCPP_DEPS_LIBS ${MFCPP_DEPS}/lib)
set(DEPPREFIX ${MFCPP_DEPS})
message ("DEPPREFIX: ${DEPPREFIX}")

INCLUDE_DIRECTORIES(${DEPPREFIX})
INCLUDE_DIRECTORIES(${DEPPREFIX}/include)
INCLUDE_DIRECTORIES(include)

find_library(BOOST_SYSTEM NAMES boost_system HINTS "${MFCPP_DEPS_LIBS}/boost")
find_library(BOOST_CHRONO NAMES boost_chrono HINTS "${MFCPP_DEPS_LIBS}/boost")
find_library(BOOST_THREAD NAMES boost_thread-mt HINTS "${MFCPP_DEPS_LIBS}/boost")
find_library(OPEN_SSL NAMES ssl HINTS "${MFCPP_DEPS_LIBS}/openssl")
find_library(CRYPTO NAMES crypto HINTS "${MFCPP_DEPS_LIBS}/openssl")
find_library(SQLITECPP NAMES SQLiteCpp HINTS "${MFCPP_DEPS_LIBS}/sqlitecpp")

find_package(CURL)
include_directories(/usr/local/include ${OPENSSL_INCLUDE_DIR} ${Boost_INCLUDE_DIR})
if (CURL_FOUND)
    include_directories(${CURL_INCLUDE_DIRS})
    add_definitions(-DHAVE_CURL)
endif()

add_executable(SoapSeller src/Daemon.cpp src/SoapSeller.cpp)

target_link_libraries(SoapSeller /usr/local/lib/libTgBot.a ${CMAKE_THREAD_LIBS_INIT} ${BOOST_THREAD} ${BOOST_SYSTEM} ${BOOST_CHRONO} ${OPEN_SSL} ${CRYPTO} ${CURL_LIBRARIES} ${DEPPREFIX}/lib/sqlitecpp/libSQLiteCpp.a ${DEPPREFIX}/lib/sqlitecpp/libsqlite3.a)
