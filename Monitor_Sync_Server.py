import Monitor
import grpc
import protobuf.PMonitor_pb2 as PMonitor_pb2 
import protobuf.PMonitor_pb2_grpc as PMonitor_pb2_grpc 
from google.protobuf import empty_pb2
from concurrent import futures
import time
import threading


class RpcServer(PMonitor_pb2_grpc.GrpcServiceServicer):
    def __init__(self) -> None:
        super().__init__()
        self.Monitor = Monitor.Monitor()
        threading.Thread(target=self.Tick,args=(5,)).start()

    
    def Tick(self,interval):
        while True:
            self.Monitor.Tick()
            time.sleep(interval)

    def MonitorTick(self, request, context):
        while True:
            resp = PMonitor_pb2.MonitorMessage()
            resp.cpum.load = self.Monitor.linux_cpu_rate
            resp.memm.mem_use = self.Monitor.linux_mem_use
            resp.memm.mem_rate = self.Monitor.linux_mem_rate
            resp.netm.net_recv_Rate = self.Monitor.linux_net_recv_rate
            resp.netm.net_send_Rate = self.Monitor.linux_net_send_rate

            for k,v in self.Monitor.proc_dict.items():
                item = PMonitor_pb2.ProcessMessage()
                item.proc_Name = k
                item.isRunning = v.is_Running
                item.cpu_load = v.cpu_percent
                item.mem_use = v.mem_use
                item.mem_load = v.mem_rate
                item.net_recv_Rate = v.net_recv_rate
                item.net_send_Rate = v.net_send_rate
                resp.procm.append(item)
            time.sleep(5)
            yield resp

    def ProcessMonitorCtrl(self, request, context):
        if request.add_del == 0:
            for str in request.name:
                self.Monitor.AddProc(str)
        if request.add_del == 1:
            for str in request.name:
                self.Monitor.DelProc(str)
        return empty_pb2.Empty()
    

def server():
    server = grpc.server(futures.ThreadPoolExecutor(10))
    PMonitor_pb2_grpc.add_GrpcServiceServicer_to_server(RpcServer(),server)
    server.add_insecure_port('[::]:50052')
    print('server start')
    server.start()
    server.wait_for_termination()

server()
    