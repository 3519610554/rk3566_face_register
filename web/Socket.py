import socket
import select
import struct
import json
import threading
import queue
import time
import logging
from Util import *

logging.basicConfig(
    level=logging.DEBUG,
    format='%(asctime)s - %(levelname)s - %(message)s',
    datefmt='%Y-%m-%d %H:%M:%S'
)

CHUNK_SIZE = 512

class Socket:
    def __init__(self, host='localhost', port=8000):
        self.__host = host
        self.__port = port 
        self.__m_send_queue = queue.Queue()
        self.__m_connect_state = False
        self.__m_mutex_connect = threading.Lock()
        self.__m_cv_connect = threading.Condition(self.__m_mutex_connect)
        self.__m_cmd_func = {}
        self.__m_recv_dict_buff = {}

        self.__m_connect_thread = threading.Thread(target=self._connect_thread)
        self.__m_send_thread = threading.Thread(target=self._send_thread)
        self.__m_recv_thread = threading.Thread(target=self._recv_thread)
    
    def __del__(self):
        self.__m_sock.close()

    #线程启动
    def start(self):
        self.__m_connect_thread.start()
        self.__m_send_thread.start()
        self.__m_recv_thread.start()

    #等待 __m_connect_state 为false
    def wait_for_connect_state_false(self):
        with self.__m_cv_connect:
            self.__m_cv_connect.wait_for(lambda: not self.__m_connect_state)
    
    #等待 __m_connect_state 为true
    def wait_for_connect_state_true(self):
        with self.__m_cv_connect:
            self.__m_cv_connect.wait_for(lambda: self.__m_connect_state)

    #设置连接状态和通知
    def set_connect_state_and_notify(self, state: bool):
        with self.__m_cv_connect:
            self.__m_connect_state = state
            self.__m_cv_connect.notify_all()

    #设置心跳包
    def set_keepalive(self, sock, keep_idle=10, keep_interval=5, keep_count=3):
        # 开启 SO_KEEPALIVE
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_KEEPALIVE, 1)

        sock.setsockopt(socket.IPPROTO_TCP, socket.TCP_KEEPIDLE, keep_idle)
        sock.setsockopt(socket.IPPROTO_TCP, socket.TCP_KEEPINTVL, keep_interval)
        sock.setsockopt(socket.IPPROTO_TCP, socket.TCP_KEEPCNT, keep_count)
    
    #接收固定字节
    def _recv_exact(self, sock, n):
        data = b''
        while len(data) < n:
            packet = sock.recv(n - len(data))
            if not packet:
                self.set_connect_state_and_notify(False)
                return None
            data += packet
        return data
    
    #接收 JSON 数据
    def _recv_json_message(self, sockfd):
        len_net = self._recv_exact(self.__m_sock, 4)
        if not len_net:
            return False, None
        msg_len = struct.unpack('!I', len_net)[0]
        data = self._recv_exact(self.__m_sock, msg_len)
        try:
            json_data = json.loads(data.decode('utf-8'))
        except:
            return False, None
        return True, json_data

    #socket发送数据
    def _socket_send(self, json_data):
        json_str = json.dumps(json_data)
        msg_len = struct.pack("<I", socket.htonl(len(json_str)))
        state = self.__m_sock.send(msg_len)
        state = self.__m_sock.send(json_str.encode('utf-8'))
        self.set_connect_state_and_notify(state > 0)

    #连接客户端线程
    def _connect_thread(self):
        while True:
            self.wait_for_connect_state_false()
            try:
                self.__m_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                self.__m_sock.connect((self.__host, self.__port))
                self.set_keepalive(self.__m_sock)
                self.set_connect_state_and_notify(True)
                logging.info("successfuly to C++ connect!")
            except Exception as e:
                self.__m_sock.close()
                time.sleep(10)
    
    #数据解包
    def _data_unpack(self, json_data):
        hash_str = json_data["Hash"]
        if hash_str not in self.__m_recv_dict_buff:
            logging.info(f"recv hash: {hash_str}")
            self.__m_recv_dict_buff[hash_str] = ""
        self.__m_recv_dict_buff[hash_str] += json_data["Payload"]
        if json_data["NumChunks"] == json_data["CurrentBlockNum"]:
            json_obj = json.loads(self.__m_recv_dict_buff[hash_str])
            del self.__m_recv_dict_buff[hash_str]
            # print(f"recv json: {json_obj}")
            if json_obj["Cmd"] in self.__m_cmd_func:
                self.__m_cmd_func[json_obj["Cmd"]](json_obj)
            

    def _send_thread(self):

        logging.info("socket send thread start successfuly!")

        while True:
            self.wait_for_connect_state_true()
            data = self.__m_send_queue.get()
            self._socket_send(data)
                
    def _recv_thread(self):

        logging.info("socket recv thread start successfuly!")

        while True:
            self.wait_for_connect_state_true()
            state, json_data = self._recv_json_message(self.__m_sock)
            if state:
                self._data_unpack(json_data)
            # print(f"recv json: {json_data}")

    #数据分包发送
    def data_subpackage(self, data:json):
        json_str = json.dumps(data)
        total_size = len(json_str)
        chunk_size = CHUNK_SIZE
        num_chunks = int((total_size + chunk_size - 1) / chunk_size)
        hash_base62 = generate_time_hash_string()

        logging.info(f"send hash: {hash_base62}")

        for i in range(num_chunks):
            send_json = {}
            offset = i * chunk_size
            current_chunk_size = min(chunk_size, total_size - offset)
            chunk_data = json_str[offset:current_chunk_size]
            send_json["Hash"] = hash_base62
            send_json["NumChunks"] = num_chunks
            send_json["CurrentBlockNum"] = i+1
            send_json["Payload"] = chunk_data
            self.send_data_add(send_json)

    def send_data_add(self, json_data):
        self.__m_send_queue.put(json_data)

    def recv_cmd_func_bind(self, cmd, func):
        self.__m_cmd_func[cmd] = func
