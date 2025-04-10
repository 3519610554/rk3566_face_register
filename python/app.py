from flask import Flask, request, render_template, Response, session
import cv2
import numpy as np
import os
import uuid
from collections import deque

app = Flask(__name__)
app.secret_key = '123456'

image = deque()

# 用于显示图像的路由
@app.route('/')
def index():
    return render_template('index.html')

@app.route('/upload', methods=['POST'])
def upload_image():
    if request.method == 'POST':
        # 获取上传的文件部分
        if 'file' not in request.files:
            return "No file part", 400

        file = request.files['file']
        
        if file.filename == '':
            return "No selected file", 400
        
        # 读取文件数据
        img_data = file.read()
        np_img = np.frombuffer(img_data, dtype=np.uint8)
        img = cv2.imdecode(np_img, cv2.IMREAD_COLOR)  # 解码图像

        # 将图像转换为 JPEG 格式的字节流
        _, img_encoded = cv2.imencode('.jpg', img)
        img_bytes = img_encoded.tobytes()

        # 将图像存储在 session 或文件中，稍后可以通过 /get_image 路由获取
        image.append(img_bytes)
        # session['img_bytes'] = img_bytes
        # cv2.imwrite('uploaded_image.jpg', img)

        return "Image uploaded and processed successfully", 200

@app.route('/get_image')
def get_image():
    if not image:  
        return "No image found", 404
    # 从 session 获取之前存储的图像字节
    img_bytes = image.popleft()
    
    return Response(img_bytes, mimetype='image/jpeg')


if __name__ == '__main__':
    app.run(host="0.0.0.0", port=5000)
