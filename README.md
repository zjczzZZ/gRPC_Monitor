一、python做grpc服务端，流式返回采集到的linux和进程数据，c++做客户端，可异步采集多服务端的数据

二、客户端为异步grpc，使用grpc::CompletionQueue，避免为每一个服务端流开一个线程

三、使用tcpdump抓包，wireshark分析可见流式grpc请求与返回过程

命令行如下：
proto生成python版本
```
    python3 -m grpc_tools.protoc -I. --python_out=. --grpc_python_out=. PMonitor.proto
```

proto生成c++版本
```
    protoc --cpp_out=. --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` PMonitor.proto
```

编译命令 或 用Makefile或cmake
```
g++ protobuf/PMonitor.grpc.pb.cc protobuf/PMonitor.pb.cc Rpc_Sync_Client.cpp -std=c++17  -g -L/usr/local/lib `pkg-config --libs protobuf grpc++` -Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed -ldl -o SyncClient
```

抓包生成.pcap文件
```
tcpdump -X -nn -i lo port 50052 -w /root/Grpc_Monitor/grpc.pcap
```

![grpc](https://github.com/zjczzZZ/gRPC_Monitor/assets/167063511/9506ba32-1d1d-495a-ab11-054e0c2993c2)
