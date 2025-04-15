import socket
import select
import struct
import json
import threading
import queue
import time

class Socket:
    def __init__(self, host='0.0.0.0', port=8080):
        self.__m_server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.__m_server.bind((host, port))
        self.__m_server.listen(1)
        self.__m_send_queue = queue.Queue()
        self.__m_recv_queue = queue.Queue()
        self.__m_connect_state = False
        self.__m_mutex_connect = threading.Lock()
        self.__m_cv_connect = threading.Condition(self.__m_mutex_connect)
        self.__m_cmd_func = {}

        self.__m_connect_thread = threading.Thread(target=self._connect_thread)
        self.__m_send_thread = threading.Thread(target=self._send_thread)
        self.__m_recv_thread = threading.Thread(target=self._recv_thread)

        self.__m_connect_thread.start()
        self.__m_send_thread.start()
        self.__m_recv_thread.start()
    
    def __del__(self):
        self.__m_server.close()

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
        len_net = self._recv_exact(self.client, 4)
        if not len_net:
            return False, None
        msg_len = struct.unpack('!I', len_net)[0]
        data = self._recv_exact(self.client, msg_len)
        try:
            json_data = json.loads(data.decode('utf-8'))
        except:
            return False, None
        return True, json_data

    #socket发送数据
    def _socket_send(self, json_data):
        json_str = json.dumps(json_data)
        msg_len = struct.pack("<I", socket.htonl(len(json_str)))
        state = self.client.send(msg_len)
        state = self.client.send(json_str.encode('utf-8'))
        self.set_connect_state_and_notify(state > 0)

    #连接客户端线程
    def _connect_thread(self):
        while True:
            self.wait_for_connect_state_false()
            self.client, self.client_addr = self.__m_server.accept()
            self.set_keepalive(self.client)
            self.set_connect_state_and_notify(True)
            print("successfuly to C++ connect!")

    def _send_thread(self):

        print("socket send thread start successfuly!")

        while True:
            self.wait_for_connect_state_true()
            data = self.__m_send_queue.get()
            self._socket_send(data)
                
    def _recv_thread(self):

        print("socket recv thread start successfuly!")

        while True:
            self.wait_for_connect_state_true()
            state, json_data = self._recv_json_message(self.client)
            if state and (json_data["Cmd"] in self.__m_cmd_func):
                self.__m_cmd_func[json_data["Cmd"]](json_data)
            print(f"recv json: {json_data}")

    def send_data_add(self, json_data):
        self.__m_send_queue.put(json_data)

    def recv_cmd_func_bind(self, cmd, func):
        self.__m_cmd_func[cmd] = func
