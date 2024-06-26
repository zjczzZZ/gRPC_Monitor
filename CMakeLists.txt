cmake_minimum_required(VERSION 3.0)
 
project(test)
 
set(CMAKE_CXX_STANDARD 17)
set(PROTO_PATH ${CMAKE_CURRENT_SOURCE_DIR}/protobuf)
 
file(GLOB PROTO_FILES ${PROTO_PATH}/*.cc)
 
find_package(Threads REQUIRED)
find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
 
message(STATUS "Using protobuf-${Protobuf_VERSION}")
message(STATUS "Using gRPC-${gRPC_VERSION}")
 
include_directories(${PROTO_PATH})
 
foreach(TARGET DC_Center Rpc_Async_Client Rpc_Sync_Client)
    add_executable(${TARGET} "${TARGET}.cpp"
        ${PROTO_FILES})
 
    target_link_libraries(${TARGET}
        gRPC::grpc++
        gRPC::grpc++_reflection
        protobuf::libprotobuf)
endforeach()