LDFLAGS = -g -L/usr/local/lib `pkg-config --libs protobuf grpc++`\
        -Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed\
        -ldl

CXX = g++
CPPFLAGS += `pkg-config --cflags protobuf grpc`
CXXFLAGS += -std=c++17

GRPC_CPP_PLUGIN = grpc_cpp_plugin
GRPC_CPP_PLUGIN_PATH ?= `which $(GRPC_CPP_PLUGIN)`

all: SyncClient 

SyncClient: protobuf/PMonitor.grpc.pb.cc protobuf/PMonitor.pb.cc Rpc_Sync_Client.cpp
	$(CXX) $^ $(CXXFLAGS)  $(LDFLAGS) -o $@

%.grpc.pb.cc: %.proto
	protoc --grpc_out=. --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $<

%.pb.cc: %.proto
	protoc --cpp_out=. $<

clean:
	rm -f *.o *.pb.cc *.pb.h SyncClient