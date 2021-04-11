# warning: this is not the "real" api implement
# it just a test program which simulate server side

#!/usr/bin/env python

import os, time
import json
from geventwebsocket import WebSocketServer, WebSocketError, WebSocketApplication, Resource
from flask import Flask, request, render_template, abort

app = Flask(__name__)

#esp32 last reponse time
lastresponse = time.time()

client = None

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

@app.route('/api/setting')
def update():
    cmd = {"op":"update","config":{}}
    with open("config-example.txt", "r") as f:
        for line in f:
            if line[0] == '#' or len(line) < 1:
                continue
            temp = line.split('=', 1)
            temp[1] = temp[1].strip('\n')
            if temp[1].isdigit():
                cmd["config"][temp[0]] = int(temp[1])
            else:
                cmd["config"][temp[0]] = temp[1]
    client.ws.send(json.dumps(cmd))
    return "nuke code sended"

@app.route('/api/manual')
def all_manual():
    op = request.args.get('op')
    if (op == None):
        return "error"
    cmd = {"op": "manual", "device":{"sprinklers": int(op), "light": int(op)}}
    client.ws.send(json.dumps(cmd))
    return "nuke code sended"

@app.route('/api/manual/<string:device>')
def manual(device):
    op = request.args.get('op')
    cmd = {"op": "manual", "device":{}}
    if (op == None):
        return "error"
    if (device == "sprinklers"):
        cmd["device"] = {device: int(op)}
    elif (device == "light"):
        cmd["device"] = {device: int(op)}
        pass
    client.ws.send(json.dumps(cmd))
    return "nuke code sended"

@app.route('/api/manual/reboot')
def reboot():
    cmd = {"op": "reboot"}
    client.ws.send(json.dumps(cmd))
    return "nuke code sended"

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
        client = self
    def on_message(self, message):
        global lastresponse
        global client
        client = self
        if (message != "pong"):
            print("message received!")
            print(message)
        lastresponse = time.time()
    def on_close(self, reason):
        print("Connection closed!")

if __name__ == '__main__':
    server = WebSocketServer(("0.0.0.0", 8080), Resource([
        ('^/websocket', ChatApplication),
        ('^/.*', app)
    ]))
    server.serve_forever()
