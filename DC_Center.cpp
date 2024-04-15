#include <grpc/support/log.h>
#include <grpcpp/grpcpp.h>

#include <iostream>
#include <map>
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
    AsyncClientCall(std::string epoint) : endpoint(epoint) {
    }
    ~AsyncClientCall() {
        std::cout << "call destory" << std::endl;
    }
    std::string endpoint;
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
        std::cout << endpoint << " platform, cpu: " << reply.cpum().load() << "\%  memory use: " << reply.memm().mem_use() << " memory load: " << reply.memm().mem_rate() << "\% net recv: " << reply.netm().net_recv_rate() << "kb/s net send: " << reply.netm().net_send_rate() << "kb/s\n";
        for (int i = 0; i < reply.procm_size(); i++) {
            auto item = reply.procm(i);
            std::cout << "                " << item.proc_name() << " ,状态: " << item.isrunning() << " cpu: " << item.cpu_load() << "% memory use:" << item.mem_use() << " memory load: " << item.mem_load() << "\% net recv: " << item.net_recv_rate() << "kb/s net send: " << item.net_send_rate() << "kb/s\n";
        }
    }
};

class AsyncClient {
public:
    explicit AsyncClient(std::string endpoint, CompletionQueue* cq)
        : endpoint_(endpoint), stub_(GrpcService::NewStub(grpc::CreateChannel(endpoint, grpc::InsecureChannelCredentials()))), cq_(cq) {
    }

    ~AsyncClient() {
        std::cout << "Call ~AsyncClient()\n";
    }

    void MonitorTick() {
        if (call) {
            return;
        }
        google::protobuf::Empty request;
        call = new AsyncClientCall(endpoint_);
        call->response_reader = stub_->PrepareAsyncMonitorTick(&call->context, request, cq_);
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

    void Close() {
        if (call) {
            call->status = Status::CANCELLED;
        }
    }

private:
    std::unique_ptr<GrpcService::Stub> stub_;
    CompletionQueue* cq_;
    AsyncClientCall* call = nullptr;
    std::string endpoint_;
};

class DataCollectCenter {
public:
    DataCollectCenter() {
        cq_ = new CompletionQueue();
    }
    ~DataCollectCenter() {
        if (cq_) {
            for (auto& item : clients_) {
                item.second->Close();
            }
            delete cq_;
        }
        for (auto it = clients_.begin(); it != clients_.end();) {
            //
            clients_.erase(it++);
        }
    }

    void AddClient(std::string endpoint) {
        if (clients_.find(endpoint) == clients_.end()) {
            clients_[endpoint] = std::make_unique<AsyncClient>(endpoint, cq_);
        }
    }

    void MonitorTick(std::string endpoint) {
        if (clients_.find(endpoint) != clients_.end()) {
            clients_[endpoint]->MonitorTick();
        } else {
            std::cout << "请调用AddClient(),再发送监控请求\n";
        }
    }

    void AsyncCompleteRpc() {
        void* got_tag;
        bool ok = false;

        while (cq_->Next(&got_tag, &ok)) {
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
    std::map<std::string, std::unique_ptr<AsyncClient>> clients_;
    CompletionQueue* cq_;
};

int main(int argc, char** argv) {
    DataCollectCenter DCCenter;
    DCCenter.AddClient("127.0.0.1:50052");
    DCCenter.MonitorTick("127.0.0.1:50052");

    std::thread thread_ = std::thread(&DataCollectCenter::AsyncCompleteRpc, std::ref(DCCenter));
    std::vector<std::string> add, del;

    std::this_thread::sleep_for(std::chrono::seconds(10));

    DCCenter.AddClient("127.0.0.1:50051");
    DCCenter.MonitorTick("127.0.0.1:50051");

    thread_.join();

    return 0;
}