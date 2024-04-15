proto生成python版本
    python3 -m grpc_tools.protoc -I. --python_out=. --grpc_python_out=. PMonitor.proto


proto生成c++版本
    protoc --cpp_out=. --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` PMonitor.proto


编译命令 或 用Makefile或cmake
g++ protobuf/PMonitor.grpc.pb.cc protobuf/PMonitor.pb.cc Rpc_Sync_Client.cpp -std=c++17  -g -L/usr/local/lib `pkg-config --libs protobuf grpc++` -Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed -ldl -o SyncClient


抓包生成.pcap文件
tcpdump -X -nn -i lo port 50052 -w /root/Grpc_Monitor/grpc.pcap