import json
import time
import queue
from flask import Flask, request, render_template, jsonify
from flask_socketio import SocketIO, emit

CHUNK_SIZE = 512

class ClientConnect:
    def __init__(self, app_, socketio_, socket_, sqlite_):
        self.m_upload_buff = ""
        self.m_image_deque = queue.Queue()
        self.m_app = app_
        self.m_socketio = socketio_
        self.m_socket = socket_
        self.m_sqlite = sqlite_

        self._register_routes()
        self._register_socketio()
        self._register_recv()

    #录入人脸命令
    def type_in_send(self, name):
        send_json = {}
        send_json["Cmd"] = "TypeIn"
        send_json["Data"] = {}
        send_json["Data"]["Name"] = name
        self.m_socket.send_data_add(send_json)

    #删除照片发送给服务端
    def delete_image_send(self, id):
        send_json = {}
        send_json["Cmd"] = "DeleteImage"
        send_json["Data"] = {}
        send_json["Data"]["Id"] = id
        self.m_socket.send_data_add(send_json)

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
            self.m_socket.send_data_add(send_json)
    
    def recv_upload(self, json_data):
        self.m_upload_buff += json_data["Data"]["Payload"]
        if json_data["Data"]["NumChunks"] == json_data["Data"]["CurrentBlockNum"]:
            json_obj = json.loads(self.m_upload_buff)
            self.m_upload_buff = ""
            print(f"Time: {json_obj['Time']}")
            # insert_id = self.m_sqlite.insert(json_obj['Time'], json_obj["ImgBase64"])
            image_id = json_obj['Id']
            image_time = json_obj['Time']
            image_base64 = json_obj['ImgBase64']
            self.m_sqlite.insert(image_id, image_time, image_base64)
            row = [image_id, image_time, image_base64]
            self.m_image_deque.put({'id': row[0], 'time': row[1], 'image': row[2]})
                
    #注册接收socket接收命令函数
    def _register_recv(self):
        self.m_socket.recv_cmd_func_bind("upload", self.recv_upload)
        self.m_socket.recv_cmd_func_bind("UploadId", self._recv_UploadId)

    def _recv_UploadId(self):
        pass

    def _register_routes(self):
        self.m_app.add_url_rule('/submit_form', 'submit_form', self._submit_form, methods=['POST'])
        self.m_app.add_url_rule('/delete_image', 'delete_image', self._delete_image, methods=['POST'])
    
    def _register_socketio(self):
        self.m_socketio.on_event('start_stream', self.start_stream)
        self.m_socketio.on_event('connect', self.handle_connect)
        self.m_socketio.on_event('disconnect', self.handle_disconnect)

    #录入人脸
    def _submit_form(self):
        name = request.get_json().get('name') 
        print("_submit_form recv name: ", name)
        self.type_in_send(name)
        return jsonify({"message": "录入成功", "status": "ok"}), 200


    #删除显示图片
    def _delete_image(self):
        image_id = request.get_json().get('id')
        print("_delete_image recv id: ", image_id)
        self.delete_image_send(image_id)
        self.m_sqlite.delete_by_id(image_id)
        return jsonify({"message": "删除成功", "status": "ok"}), 200

    #向客户端显示照片
    def _stream_images(self):
        while True:
            if not self.m_image_deque.empty():
                img_bytes = self.m_image_deque.get()
                self.m_socketio.emit('image', {'data': img_bytes})
            self.m_socketio.sleep(0.01)
    
    def start_stream(self):
        print('Start streaming')
        data = self.m_sqlite.get_all_data()
        while not self.m_image_deque.empty():
            self.m_image_deque.get()
        for row in data:
            self.m_image_deque.put({'id': row[0], 'time': row[1], 'image': row[2]})
        self.m_socketio.start_background_task(self._stream_images) 

    def handle_connect(self):
        print('Client connected')
    
    def handle_disconnect(self):
        print('Client disconnected')
