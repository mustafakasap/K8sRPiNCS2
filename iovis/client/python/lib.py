import os
from concurrent import futures

import grpc
import time

import inference_service_pb2, inference_service_pb2_grpc

CHUNK_SIZE = 1024 * 1024  # 1MB

def get_file_chunks(filename):
    with open(filename, 'rb') as f:
        while True:
            piece = f.read(CHUNK_SIZE);
            if len(piece) == 0:
                return
            yield inference_service_pb2.Chunk(buffer=piece)

def get_result_chunks(filename):
    with open(filename, 'rb') as f:
        while True:
            piece = f.read(CHUNK_SIZE);
            if len(piece) == 0:
                return
            yield inference_service_pb2.Chunk(buffer=piece)

class InferenceClient:
    def __init__(self, address):
        channel = grpc.insecure_channel(address)
        self.stub = inference_service_pb2_grpc.InfererStub(channel)

    def infer(self, in_file_name):
        chunks_generator = get_file_chunks(in_file_name)
        response = self.stub.infer(chunks_generator)
        return response.length
