from flask import Flask, request, render_template
from flask_socketio import SocketIO
from ImageStream import ImageStream
from Config import Config

class FlaskApp:
    def __init__(self):
        self.app = Flask(__name__)
        self.app.config.from_object(Config)
        self.socketio = SocketIO(self.app)
        self.image_streaming = ImageStream(self.socketio)
        self._register_routes()
    
    def _register_routes(self):
        """注册 HTTP 路由"""
        self.app.add_url_rule('/', 'index', self.index)
        self.app.add_url_rule('/upload', 'upload_image', self.upload_image, methods=['POST'])
    
    def index(self):
        return render_template('index.html')

    def upload_image(self):
        if 'file' not in request.files:
            return "No file part", 400

        file = request.files['file']
        
        if file.filename == '':
            return "No selected file", 400
        
        return self.image_streaming.upload_image(file)
    
    def run(self):
        self.socketio.run(self.app, host='0.0.0.0', port=5000)

if __name__ == '__main__':
    app = FlaskApp()
    app.run()
