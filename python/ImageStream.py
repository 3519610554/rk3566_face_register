from flask_socketio import SocketIO, emit
import cv2
import numpy as np
import base64
from collections import deque

class ImageStream:
    def __init__(self, socketio):
        self.socketio = socketio
        self.image_deque = deque()

        #注册事件
        self.socketio.on_event('start_stream', self.start_stream)
        self.socketio.on_event('connect', self.handle_connect)
        self.socketio.on_event('disconnect', self.handle_disconnect)

    #向客户端显示照片
    def _stream_images(self):
        while True:
            if self.image_deque:
                img_bytes = self.image_deque.popleft()
                b64_img = base64.b64encode(img_bytes).decode('utf-8')
                self.socketio.emit('image', {'data': b64_img})
            self.socketio.sleep(0.01)

    def start_stream(self):
        print('Start streaming')
        self.socketio.start_background_task(self._stream_images) 

    def handle_connect(self):
        print('Client connected')
    
    def handle_disconnect(self):
        print('Client disconnected')

    def upload_image(self, file):
        img_data = file.read()
        np_img = np.frombuffer(img_data, dtype=np.uint8)
        img = cv2.imdecode(np_img, cv2.IMREAD_COLOR)

        _, img_encoded = cv2.imencode('.jpg', img)
        img_bytes = img_encoded.tobytes()

        self.image_deque.append(img_bytes)

        return "", 200
