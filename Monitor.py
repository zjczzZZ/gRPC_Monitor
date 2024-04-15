import psutil
import os
import time
import psutil


class ProcData:
    def __init__(self,pid,proc_name) -> None:
        self.pid = pid
        self.name = proc_name
        self.cpu_percent = 0
        self.net_recv_rate = 0
        self.net_send_rate = 0
        self.mem_use = 0
        self.mem_rate = 0
        self.is_Running = True
        if self.pid != 0:
            process = psutil.Process(self.pid)
            io_counters = process.io_counters()
            self.net_send_pre = io_counters.write_bytes
            self.net_recv_pre = io_counters.read_bytes

        self.net_send_now = 0
        self.net_recv_now = 0
    def tick(self):
        process = psutil.Process(self.pid)
        self.cpu_percent = process.cpu_percent(None)
        
        memory_info = process.memory_info()
        memory_percent = process.memory_percent()
        self.mem_use = memory_info.rss / (1024 * 1024) #MB
        self.mem_rate = memory_percent

        io_counters = process.io_counters()

        self.net_send_now = io_counters.write_bytes  #b
        self.net_recv_now = io_counters.read_bytes  #b
        self.net_recv_rate = (self.net_recv_now - self.net_recv_pre)/1024/1024/5 #MB/s
        self.net_send_rate = (self.net_send_now - self.net_send_pre)/1024/1024/5 
        self.net_recv_pre = self.net_recv_now
        self.net_send_pre = self.net_send_now

        self.print()

    def print(self):
        print(self.name," cpu: ",self.cpu_percent," mem: ",self.mem_use," ",self.mem_rate," net: ",self.net_recv_rate," ",self.net_send_rate)

class Monitor:
    def __init__(self) -> None:
        self.linux_mem_rate = 0
        self.linux_mem_use = 0
        self.linux_cpu_rate = 0
        self.linux_dk_rate = 0
        self.linux_dk_use = 0
        self.linux_net_send_rate = 0
        self.linux_net_recv_rate = 0
        self.proc_dict = {}
        self.proc_cpu_rate=0
        self.proc_net_=0
        self.proc_net=0
        self.proc_data_mem_use=0
        self.proc_data_mem_rate=0

        self.linux_net_send_now = 0
        self.linux_net_recv_now = 0
        self.linux_net_send_pre = psutil.net_io_counters(pernic=True)['eth0'].bytes_sent
        self.linux_net_recv_pre = psutil.net_io_counters(pernic=True)['eth0'].bytes_recv

    def Tick(self):
        mem = psutil.virtual_memory()
        memtotal = float(mem.total) / 1024 / 1024 / 1024
        memused = float(mem.used) / 1024 / 1024 / 1024
        memrate = memused/memtotal
        self.linux_mem_rate = memrate
        self.linux_mem_use = memused

        self.linux_cpu_rate = psutil.cpu_percent()
        
        dk = psutil.disk_usage('/')
        used = dk.used / 1024 / 1024 / 1024
        self.linux_dk_rate = dk.percent
        self.linux_dk_use = used

        self.linux_net_recv_now = psutil.net_io_counters(pernic=True)['eth0'].bytes_recv
        self.linux_net_send_now = psutil.net_io_counters(pernic=True)['eth0'].bytes_sent
        #'Mb/s'
        self.linux_net_send_rate = (self.linux_net_send_now-self.linux_net_send_pre) /1024/1024/5
        self.linux_net_recv_rate = (self.linux_net_recv_now-self.linux_net_recv_pre) /1024/1024/5

        self.linux_net_recv_pre = self.linux_net_recv_now
        self.linux_net_send_pre = self.linux_net_send_now

        for k,v in self.proc_dict.items():
            v.tick()

    def GetPidByName(self,proc_name):
        pid = 0
        for proc in psutil.process_iter(attrs=['pid', 'name']):
            if proc.info['name'] == proc_name:
                pid = proc.info['pid']
                break
        return pid

    def AddProc(self,proc_name):
        if proc_name in self.proc_dict:
            return
        pid = self.GetPidByName(proc_name)
        if proc_name not in self.proc_dict:
            self.proc_dict[proc_name] = ProcData(pid,proc_name)
         
    def DelProc(self,proc_name):
        if proc_name in self.proc_dict:
            del self.proc_dict[proc_name]
    
# m = Monitor()
# m.AddProc('mysqld')
# for i in range(0,10):
#     m.AddProc('nginx')
    
#     m.Tick()
#     m.AddProc('etcd')
#     time.sleep(5)
    

