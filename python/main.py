from flask import Flask, request, render_template
from flask_socketio import SocketIO
from ImageStream import ImageStream
from Config import Config
from FlaskRequest import FlaskRequest
from ClientConnect import ClientConnect

class FlaskApp:
    def __init__(self):
        self.app = Flask(__name__)
        self.app.config.from_object(Config)
        self.client_connect = ClientConnect()
        self.socketio = SocketIO(self.app)
        self.flask_request = FlaskRequest(self.app, self.client_connect)
        self.image_streaming = ImageStream(self.socketio)
        
        self._register_routes()
    
    def _register_routes(self):
        """注册 HTTP 路由"""
        self.app.add_url_rule('/', 'index', self.index)
        # self.app.add_url_rule('/upload', 'upload_image', self.upload_image, methods=['POST'])
    
    def index(self):
        return render_template('index.html')
    
    def run(self):
        self.socketio.run(self.app, host='0.0.0.0', port=5000, debug=False)

if __name__ == '__main__':
    app = FlaskApp()
    app.run()
