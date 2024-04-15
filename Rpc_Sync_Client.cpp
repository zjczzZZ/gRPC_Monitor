#include <grpcpp/grpcpp.h>
#include <grpcpp/security/credentials.h>
#include <grpcpp/support/async_unary_call.h>
#include <unistd.h>

#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "protobuf/PMonitor.grpc.pb.h"

class RpcClient {
public:
    RpcClient() {}
    RpcClient(std::shared_ptr<grpc::Channel> channel)
        : _stub(GrpcService::NewStub(channel)) {}

    ~RpcClient() {}

    void ProcessMonitorCtrl(std::vector<std::string> addprocs, std::vector<std::string> delprocs) {
        google::protobuf::Empty req;
        if (addprocs.size() > 0) {
            ProcMonitorRequest request;
            request.set_add_del(0);
            for (auto value : addprocs) {
                request.add_name(value);
            }

            grpc::ClientContext context;
            grpc::Status status = _stub->ProcessMonitorCtrl(&context, request, &req);

            if (status.ok()) {
                std::cout << "add proc success" << std::endl;
            } else {
                std::cout << "add proc failed" << std::endl;
            }
        }

        if (delprocs.size() > 0) {
            ProcMonitorRequest request;
            request.set_add_del(1);
            for (auto value : delprocs) {
                request.add_name(value);
            }

            grpc::ClientContext context;
            grpc::Status status = _stub->ProcessMonitorCtrl(&context, request, &req);

            if (status.ok()) {
                std::cout << "del proc success" << std::endl;
            } else {
                std::cout << "del proc failed" << std::endl;
            }
        }
    }

    void MonitorTick() {
        google::protobuf::Empty req;
        grpc::ClientContext context;
        MonitorMessage replyinfo;
        std::unique_ptr<grpc::ClientReader<MonitorMessage>> reader(_stub->MonitorTick(&context, req));
        while (reader->Read(&replyinfo)) {
            print(replyinfo);
        }

        grpc::Status status = reader->Finish();
        if (!status.ok()) {
            std::cout << "服务端流 failed" << std::endl;
        }
    }

    void print(MonitorMessage &reply) {
        std::cout << "platform, cpu: " << reply.cpum().load() << "\%  memory use: " << reply.memm().mem_use() << " memory load: " << reply.memm().mem_rate() << "\% net recv: " << reply.netm().net_recv_rate() << "kb/s net send: " << reply.netm().net_send_rate() << "kb/s\n";
        for (int i = 0; i < reply.procm_size(); i++) {
            auto item = reply.procm(i);
            std::cout << "        " << item.proc_name() << " ,状态: " << item.isrunning() << " cpu: " << item.cpu_load() << "% memory use:" << item.mem_use() << " memory load: " << item.mem_load() << "\% net recv: " << item.net_recv_rate() << "kb/s net send: " << item.net_send_rate() << "kb/s\n";
        }
    }

private:
    std::unique_ptr<GrpcService::Stub> _stub;
};

using grpc::Channel;
using grpc::ClientAsyncResponseReader;
using grpc::ClientContext;
using grpc::CompletionQueue;
using grpc::Status;

int main() {
    std::string address("0.0.0.0:50052");
    std::shared_ptr<grpc_impl::Channel> xxx = grpc::CreateChannel(address, grpc::InsecureChannelCredentials());

    RpcClient client(grpc::CreateChannel(address, grpc::InsecureChannelCredentials()));

    std::vector<std::string> addproc, delproc;
    addproc.push_back("mysqld");
    client.ProcessMonitorCtrl(addproc, delproc);

    ClientContext context;

    std::thread t(&RpcClient::MonitorTick, std::ref(client));
    t.join();
}