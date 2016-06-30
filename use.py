import subprocess
from subprocess import Popen, PIPE
import flask
from flask import Flask, request
from skimage import io
from array import *
import json

app = Flask(__name__)

@app.route("/")
def hello():
    url = request.args['url']
    img = io.imread(url)
    io.imsave("process.jpg", img)
    cmd = ["./faceDet", "process.jpg"]
    result = subprocess.Popen(cmd, stdin=subprocess.PIPE, stdout=subprocess.PIPE)
    a = ""
    temp_list = []
    temp_list.append("Driver: ")
    temp_list.append("Expressive: ")
    temp_list.append("Analytical: ")
    temp_list.append("Amiable: ")
    data = {}
    i = 0
    with open("result.txt") as f:
    	for line in f:
    		data[temp_list[i]] = line
    		i+= 1
   	json_data = json.dumps(data)

    return flask.jsonify(**json_data)


if __name__ == "__main__":
   app.run()