import socket
import struct
import json
import cv2
import threading
import base64
import queue
import numpy as np
import time
from Socket import Socket

CHUNK_SIZE=512

class ClientConnect:
    def __init__(self):
        self.server = Socket()
        self.server.recv_cmd_func_bind("Test", self.Test)
        self.data_subpackage("Test", "Hello C++")

    def type_in_send(self, name):
        send_json = {}
        send_json["Cmd"] = "TypeIn"
        send_json["Data"] = {}
        send_json["Data"]["Name"] = name
        self.server.send_data_add(send_json)

    def data_subpackage(self, cmd, data):
        total_size = len(data)
        chunk_size = CHUNK_SIZE
        num_chunks = int((total_size + chunk_size - 1) / chunk_size)

        for i in range(num_chunks):
            send_json = {}
            offset = i * chunk_size
            current_chunk_size = min(chunk_size, total_size - offset)
            chunk_data = data[offset:current_chunk_size]
            send_json["Cmd"] = cmd
            send_json["Data"] = {}
            send_json["Data"]["NumChunks"] = num_chunks
            send_json["Data"]["CurrentBlockNum"] = i
            send_json["Data"]["Payload"] = chunk_data
            self.server.send_data_add(send_json)
    
    def Test(self, json_data):
        print(f"Data: {json_data['Data']}")
