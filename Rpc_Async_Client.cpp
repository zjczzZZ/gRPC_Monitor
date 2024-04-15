#include <grpc/support/log.h>
#include <grpcpp/grpcpp.h>

#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include "protobuf/PMonitor.grpc.pb.h"
#include "protobuf/PMonitor.pb.h"

using grpc::Channel;
using grpc::ClientAsyncResponseReader;
using grpc::ClientContext;
using grpc::CompletionQueue;
using grpc::Status;

struct AsyncClientCall {
    MonitorMessage reply;
    ClientContext context;
    Status status;
    std::unique_ptr<grpc::ClientAsyncReader<MonitorMessage>> response_reader;

    void process() {
        if (status.ok()) {
            print();
        } else {
            std::cout << "rpc stream end" << std::endl;
            delete this;
            return;
        }
        response_reader->Read(&reply, (void*)this);
    }

    void print() {
        std::cout << "platform, cpu: " << reply.cpum().load() << "\%  memory use: " << reply.memm().mem_use() << " memory load: " << reply.memm().mem_rate() << "\% net recv: " << reply.netm().net_recv_rate() << "kb/s net send: " << reply.netm().net_send_rate() << "kb/s\n";
        for (int i = 0; i < reply.procm_size(); i++) {
            auto item = reply.procm(i);
            std::cout << "        " << item.proc_name() << " ,状态: " << item.isrunning() << " cpu: " << item.cpu_load() << "% memory use:" << item.mem_use() << " memory load: " << item.mem_load() << "\% net recv: " << item.net_recv_rate() << "kb/s net send: " << item.net_send_rate() << "kb/s\n";
        }
    }
};

class AsyncClient {
public:
    explicit AsyncClient(std::shared_ptr<Channel> channel)
        : stub_(GrpcService::NewStub(channel)) {}

    void MonitorTick() {
        google::protobuf::Empty request;
        AsyncClientCall* call = new AsyncClientCall;
        call->response_reader = stub_->PrepareAsyncMonitorTick(&call->context, request, &cq_);
        call->response_reader->StartCall((void*)call);
    }

    void ProcessMonitorCtrl(std::vector<std::string> addprocs, std::vector<std::string> delprocs) {
        google::protobuf::Empty req;
        if (addprocs.size() > 0) {
            ProcMonitorRequest request;
            request.set_add_del(0);
            for (auto value : addprocs) {
                request.add_name(value);
            }

            grpc::ClientContext context;

            grpc::Status status = stub_->ProcessMonitorCtrl(&context, request, &req);

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
            grpc::Status status = stub_->ProcessMonitorCtrl(&context, request, &req);

            if (status.ok()) {
                std::cout << "del proc success" << std::endl;
            } else {
                std::cout << "del proc failed" << std::endl;
            }
        }
    }

    void AsyncCompleteRpc() {
        void* got_tag;
        bool ok = false;

        while (cq_.Next(&got_tag, &ok)) {
            AsyncClientCall* call = static_cast<AsyncClientCall*>(got_tag);
            if (call != nullptr) {
                if (!ok) {
                    std::cout << "异常" << std::endl;
                    call->status = Status::CANCELLED;
                }
            }
            call->process();
        }
    }

private:
    std::unique_ptr<GrpcService::Stub> stub_;
    CompletionQueue cq_;
};

int main(int argc, char** argv) {
    AsyncClient client(grpc::CreateChannel(
        "localhost:50052", grpc::InsecureChannelCredentials()));

    std::thread thread_ = std::thread(&AsyncClient::AsyncCompleteRpc, &client);
    std::vector<std::string> add, del;
    add.push_back("mysqld");
    add.push_back("nginx");
    client.ProcessMonitorCtrl(add, del);
    for (int i = 0; i < 1; i++) {
        client.MonitorTick();
    }

    std::this_thread::sleep_for(std::chrono::seconds(10));
    add.clear();
    del.push_back("nginx");
    client.ProcessMonitorCtrl(add, del);

    thread_.join();

    return 0;
}