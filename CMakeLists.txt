cmake_minimum_required(VERSION 2.8.4)
project(SoapSeller)

# compiling options
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(AURA_DEPS $ENV{HOME}/sgn/dev)
set(AURA_DEPS_LIB ${AURA_DEPS}/lib)
set(AURA_DEPS_INCLUDE ${AURA_DEPS}/include)
message ("AURA_DEPS: ${AURA_DEPS}")

#	Set the macro DECRYPT_KEY with actual key from command line
add_definitions(-DDECRYPT_KEY=${AURA_KEY})
if(NOT AURA_KEY)
	message( FATAL_ERROR "***** Pls provide value for AURA_KEY *****")
endif(NOT AURA_KEY)

INCLUDE_DIRECTORIES(${AURA_DEPS_INCLUDE})
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)
LINK_DIRECTORIES(${AURA_DEPS_LIB})

add_executable(SoapSeller
	src/Daemon.cpp
	src/StartButton.cpp
	src/SoapButton.cpp
#	src/CartButton.cpp
	src/OrdersButton.cpp
	src/FlavoursButton.cpp
	src/DBInterface.cpp
	src/BuyButton.cpp
	src/QuantityButton.cpp
	src/ViewCart.cpp
	src/ShippingAddress.cpp
	)

target_link_libraries(SoapSeller ${AURA_DEPS_LIB}/libTgBot.a ${CMAKE_THREAD_LIBS_INIT} boost_system boost_thread boost_chrono ssl crypto curl ${AURA_DEPS_LIB}/libSQLiteCpp.a ${AURA_DEPS_LIB}/libsqlite3.a pthread dl)
