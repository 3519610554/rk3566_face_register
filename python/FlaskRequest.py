from flask import Flask, request, render_template

class FlaskRequest:
    def __init__(self, app):
        self.app = app
        self.app.add_url_rule('/submit_form', 'submit_form', self.submit_form, methods=['POST'])

    def submit_form(self):
        name = request.form.get('name') 

        print("收到的姓名：", name)

        return render_template('index.html')
