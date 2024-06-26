# Generated by the gRPC Python protocol compiler plugin. DO NOT EDIT!
"""Client and server classes corresponding to protobuf-defined services."""
import grpc

import PMonitor_pb2 as PMonitor__pb2
from google.protobuf import empty_pb2 as google_dot_protobuf_dot_empty__pb2


class GrpcServiceStub(object):
    """定义服务接口
    """

    def __init__(self, channel):
        """Constructor.

        Args:
            channel: A grpc.Channel.
        """
        self.MonitorTick = channel.unary_stream(
                '/GrpcService/MonitorTick',
                request_serializer=google_dot_protobuf_dot_empty__pb2.Empty.SerializeToString,
                response_deserializer=PMonitor__pb2.MonitorMessage.FromString,
                )
        self.ProcessMonitorCtrl = channel.unary_unary(
                '/GrpcService/ProcessMonitorCtrl',
                request_serializer=PMonitor__pb2.ProcMonitorRequest.SerializeToString,
                response_deserializer=google_dot_protobuf_dot_empty__pb2.Empty.FromString,
                )
        self.ShellControl = channel.unary_unary(
                '/GrpcService/ShellControl',
                request_serializer=PMonitor__pb2.ShellExe.SerializeToString,
                response_deserializer=google_dot_protobuf_dot_empty__pb2.Empty.FromString,
                )


class GrpcServiceServicer(object):
    """定义服务接口
    """

    def MonitorTick(self, request, context):
        """Missing associated documentation comment in .proto file."""
        context.set_code(grpc.StatusCode.UNIMPLEMENTED)
        context.set_details('Method not implemented!')
        raise NotImplementedError('Method not implemented!')

    def ProcessMonitorCtrl(self, request, context):
        """Missing associated documentation comment in .proto file."""
        context.set_code(grpc.StatusCode.UNIMPLEMENTED)
        context.set_details('Method not implemented!')
        raise NotImplementedError('Method not implemented!')

    def ShellControl(self, request, context):
        """Missing associated documentation comment in .proto file."""
        context.set_code(grpc.StatusCode.UNIMPLEMENTED)
        context.set_details('Method not implemented!')
        raise NotImplementedError('Method not implemented!')


def add_GrpcServiceServicer_to_server(servicer, server):
    rpc_method_handlers = {
            'MonitorTick': grpc.unary_stream_rpc_method_handler(
                    servicer.MonitorTick,
                    request_deserializer=google_dot_protobuf_dot_empty__pb2.Empty.FromString,
                    response_serializer=PMonitor__pb2.MonitorMessage.SerializeToString,
            ),
            'ProcessMonitorCtrl': grpc.unary_unary_rpc_method_handler(
                    servicer.ProcessMonitorCtrl,
                    request_deserializer=PMonitor__pb2.ProcMonitorRequest.FromString,
                    response_serializer=google_dot_protobuf_dot_empty__pb2.Empty.SerializeToString,
            ),
            'ShellControl': grpc.unary_unary_rpc_method_handler(
                    servicer.ShellControl,
                    request_deserializer=PMonitor__pb2.ShellExe.FromString,
                    response_serializer=google_dot_protobuf_dot_empty__pb2.Empty.SerializeToString,
            ),
    }
    generic_handler = grpc.method_handlers_generic_handler(
            'GrpcService', rpc_method_handlers)
    server.add_generic_rpc_handlers((generic_handler,))


 # This class is part of an EXPERIMENTAL API.
class GrpcService(object):
    """定义服务接口
    """

    @staticmethod
    def MonitorTick(request,
            target,
            options=(),
            channel_credentials=None,
            call_credentials=None,
            insecure=False,
            compression=None,
            wait_for_ready=None,
            timeout=None,
            metadata=None):
        return grpc.experimental.unary_stream(request, target, '/GrpcService/MonitorTick',
            google_dot_protobuf_dot_empty__pb2.Empty.SerializeToString,
            PMonitor__pb2.MonitorMessage.FromString,
            options, channel_credentials,
            insecure, call_credentials, compression, wait_for_ready, timeout, metadata)

    @staticmethod
    def ProcessMonitorCtrl(request,
            target,
            options=(),
            channel_credentials=None,
            call_credentials=None,
            insecure=False,
            compression=None,
            wait_for_ready=None,
            timeout=None,
            metadata=None):
        return grpc.experimental.unary_unary(request, target, '/GrpcService/ProcessMonitorCtrl',
            PMonitor__pb2.ProcMonitorRequest.SerializeToString,
            google_dot_protobuf_dot_empty__pb2.Empty.FromString,
            options, channel_credentials,
            insecure, call_credentials, compression, wait_for_ready, timeout, metadata)

    @staticmethod
    def ShellControl(request,
            target,
            options=(),
            channel_credentials=None,
            call_credentials=None,
            insecure=False,
            compression=None,
            wait_for_ready=None,
            timeout=None,
            metadata=None):
        return grpc.experimental.unary_unary(request, target, '/GrpcService/ShellControl',
            PMonitor__pb2.ShellExe.SerializeToString,
            google_dot_protobuf_dot_empty__pb2.Empty.FromString,
            options, channel_credentials,
            insecure, call_credentials, compression, wait_for_ready, timeout, metadata)
