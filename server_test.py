# warning: this is not the "real" api implement
# it just a test program which simulate server side

#!/usr/bin/env python

import os, time
from geventwebsocket import WebSocketServer, WebSocketError, WebSocketApplication, Resource
from flask import Flask, request, render_template, abort

app = Flask(__name__)

#esp32 last reponse time
lastresponse = time.time()

@app.route('/')
def index():
    return "Hello nothing there"

@app.route('/api/status')
def isonline():
    global lastresponse
    if ((time.time() - lastresponse) < 5):
        return "online"
    else:
        return "offline"

@app.route('/api/ESP32/saveimg', methods=['POST'])
def saveimg():
    if request.files['img']:
        file = request.files['img']
        file.save(file.filename)
        print("saveimg: {}".format(file.filename))
        return "OK"
    else:
        return "FAIL"

class ChatApplication(WebSocketApplication):
    def on_open(self):
        global lastresponse
        print("client connected!")
        lastresponse = time.time()
    def on_message(self, message):
        global lastresponse
        print("message received!")
        lastresponse = time.time()
    def on_close(self, reason):
        print("Connection closed!")


if __name__ == '__main__':
    server = WebSocketServer(("0.0.0.0", 8080), Resource([
        ('^/websocket', ChatApplication),
        ('^/.*', app)
    ]))
    server.serve_forever()
