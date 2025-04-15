from flask import Flask, request, render_template
from flask_socketio import SocketIO
from Config import Config
from Socket import Socket
from ClientConnect import ClientConnect

class FlaskApp:
    def __init__(self):
        self.app = Flask(__name__)
        self.app.config.from_object(Config)  
        self.socketio = SocketIO(self.app)
        self.server_socket = Socket()
        self.client_connect = ClientConnect(self.app, self.socketio, self.server_socket)
        self._register_routes()
    
    def _register_routes(self):
        """注册 HTTP 路由"""
        self.app.add_url_rule('/', 'index', self.index)
    
    def index(self):
        return render_template('index.html')
    
    def run(self):
        self.socketio.run(self.app, host='0.0.0.0', port=5000, debug=False)


if __name__ == '__main__':
    app = FlaskApp()
    app.run()
