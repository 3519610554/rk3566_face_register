import os
import json
import socket
from flask_wtf import FlaskForm
from flask_wtf.file import FileField
from flask_sqlalchemy import SQLAlchemy
from werkzeug.utils import secure_filename
from sqlalchemy import String,Column,Integer
from wtforms.validators import DataRequired,Length
from flask import request,redirect,url_for,Flask,render_template,flash
from wtforms import StringField,TelField,DateField,SubmitField,PasswordField

#定义读取的最大文件数
max_len=1024


app=Flask(__name__)
app.secret_key='files'

class Config:
    SQLALCHEMY_DATABASE_URI='mysql+pymysql://root:root@127.0.0.1:3306/main'
    SQLALCHEMY_TRACK_MODIFICATIONS=False

app.config.from_object(Config)
mysql=SQLAlchemy(app)

class FormFiles(FlaskForm):
    filename=FileField(label='传输文件',validators=[DataRequired()])
    submit=SubmitField(label='提交文件')

# class FormText(FlaskForm):
#     endtext=StringField(label='结束符')
#     end = SubmitField(label='结束传输')

# http://127.0.0.1:5000/sendFiles
@app.route('/sendFiles',methods=['POST','GET'])
def sendFiles():
    formfiles=FormFiles()
    if request.method=='POST':
        if formfiles.validate_on_submit():
            # 定义端口和IP地址
            remote_PORT = 8080
            remote_IP = '10.34.45.164'
            remote_addr = (remote_IP, remote_PORT)
            # 绑定端口号和IP地址
            Socket_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            Socket_socket.connect(remote_addr)

            if formfiles.filename.data!=None:
                filename=secure_filename(formfiles.filename.data.filename)
                name=filename
                formfiles.filename.data.save('files/'+filename)
                #切分文件名和点后缀名
                filename = 'files/' + filename
                file,type_file=os.path.splitext(name)
                #获取文件大小
                file_Size=os.stat(filename).st_size
                #定义发送消息头
                msg_header={'filename':file,'msg_type':type_file,'msg_len':file_Size}
                msg_header_bytes=bytes(json.dumps(msg_header),encoding='utf-8')
                #当消息头的长度不满1024时，使用空格填充
                msg_header_bytes+=b''*(max_len-len(msg_header_bytes))
                #发送消息头
                Socket_socket.send(msg_header_bytes)
                file_len=0
                recv_count=0
                #发送的文件消息头大小
                print('msg_header_bytes: {}'.format(len(msg_header_bytes)))
                # 发送的文件大小
                print('file_size: {}'.format(file_Size))
                #发送文件的类型
                print('type_file: {}'.format(type_file))

                with open(filename,'rb') as fp:
                    while file_len!=file_Size:
                        message=fp.read(max_len)
                        Socket_socket.send(message)
                        file_len+=len(message)
                        recv_count+=1
                flash(message='文件传输成功！')
                print('发送次数: {}'.format(recv_count))
                print('发送完毕！')
            else:
                # 发送结束消息
                print('结束文件传输')
                flash(message='文件传输结束！')
                endstr='Gameover'.encode('utf-8')
                Socket_socket.send(endstr)
                Socket_socket.close()

    return render_template('index.html',formfiles=formfiles)


@app.route('/index',methods=['POST','GET'])
def index():
    formfiles=FormFiles()
    return render_template('index.html',formfiles=formfiles)


if __name__ == '__main__':
    print('Pycharm')
    app.run(debug=True)

# import os
# from torchstat import stat
# import torchvision.models as models
#
# model=models.shufflenet_v2_x1_0()
# stat(model,(3,224,224))

