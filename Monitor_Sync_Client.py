from google.protobuf import empty_pb2
import logging
import time 
import grpc
import protobuf.PMonitor_pb2 as PMonitor_pb2
import protobuf.PMonitor_pb2_grpc as PMonitor_pb2_grpc


def run():
    with grpc.insecure_channel('localhost:50052') as channel:
        stub = PMonitor_pb2_grpc.GrpcServiceStub(channel)
        robj = PMonitor_pb2.ProcMonitorRequest()
        robj.add_del = 0
        robj.name.append('mysqld')
        stub.ProcessMonitorCtrl(robj)

        response = stub.MonitorTick(empty_pb2.Empty())
        try:
            for msg in response:
                print(msg.cpum.load,msg.memm.mem_use,msg.memm.mem_rate)
                print(msg.netm.net_recv_Rate,msg.netm.net_send_Rate)
                for proc in msg.procm:
                    print(proc.proc_Name,proc.isRunning,proc.cpu_load,proc.mem_use,proc.mem_load,proc.net_recv_Rate,proc.net_send_Rate)
        except:
            pass

run()



