# warning: this is not the "real" api implement
# it just a test program which simulate server side

#!/usr/bin/env python

import os
from geventwebsocket import WebSocketServer, WebSocketError
from flask import Flask, request, render_template, abort

app = Flask(__name__)

@app.route('/')
def index():
    return "Hello nothing there"

@app.route('/api/ESP32/saveimg', methods=['POST'])
def saveimg():
    if request.files['img']:
        file = request.files['img']
        file.save(file.filename)
        print("saveimg: {}".format(file.filename))
        return "OK"
    else:
        return "FAIL"

@app.route('/websocket')
def websocket():
    ws = request.environ.get('wsgi.websocket')

    if not ws:
        abort(400, "Expected WebSocket request")

    while True:
        try:
            message = ws.receive()
            print("recived message: {}".format(message))
            ws.send("Your message was: {}".format(message))
        except WebSocketError:
            # Possibility to execute code when connection is closed
            break
    return 'bye'

if __name__ == '__main__':
    server = WebSocketServer(("0.0.0.0", 8080), app)
    server.serve_forever()
