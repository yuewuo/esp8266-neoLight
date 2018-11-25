from flask import Flask, render_template, jsonify, request, session, redirect, url_for
from flask_socketio import SocketIO, emit, join_room, leave_room
from optparse import OptionParser
import logging, json, socket, sys, os, threading, time
app = Flask(__name__, static_folder=os.path.dirname(os.path.abspath(__file__)))
socketio = SocketIO()
socketio.init_app(app=app)

@app.route("/")
def index():
    return app.send_static_file('index.html')
@app.route("/<path:path_name>")
def getstatic(path_name):
    print(path_name)
    return app.send_static_file(path_name)

if __name__=='__main__':
    parser = OptionParser()
    parser.add_option("--host", type="string", dest="host", help="Server Host IP", default="127.0.0.1")
    parser.add_option("--port", type="int", dest="port", help="Server Host Port", default=8080)
    options, args = parser.parse_args()
    print("\n\n##############################################")
    print("server will run on port %d of '%s'" % (options.port, options.host))
    print("##############################################\n\n")
    socketio.run(app, host=options.host, port=options.port)
