from flask import Flask, request, render_template
from ClientConnect import ClientConnect

class FlaskRequest:
    def __init__(self, app, client):
        self.app = app
        self.client = client
        self.app.add_url_rule('/submit_form', 'submit_form', self.submit_form, methods=['POST'])

    def submit_form(self):
        name = request.form.get('name') 

        print("收到的姓名：", name)

        self.client.type_in_send(name)
        return render_template('index.html')
