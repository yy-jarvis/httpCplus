cmake_minimum_required(VERSION 3.22)
project(http)

set(CMAKE_CXX_STANDARD 17)

find_package(Poco REQUIRED COMPONENTS Foundation Net Util  XML JSON)

message(STATUS "poco version: ${Poco_VERSION}")

add_executable(http main.cpp
        client/HttpClient.cpp
        client/HttpClient.hpp
)

target_link_libraries(${PROJECT_NAME}
        Poco::Foundation
        Poco::Net
        Poco::Util
        Poco::XML
        Poco::JSON)
