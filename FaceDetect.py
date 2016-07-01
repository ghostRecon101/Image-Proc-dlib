import os
import subprocess
from subprocess import Popen, PIPE
import flask
from flask import Flask, request
from skimage import io
from array import *
import json
import time

app = Flask(__name__)

@app.route("/")
def hello():
	url = request.args['url']
	img = io.imread(url)
	io.imsave("process.jpg", img)
	cmd = ["./faceDet", "process.jpg"]
	result = subprocess.Popen(cmd, stdin=subprocess.PIPE, stdout=subprocess.PIPE)
	#for i in range(41): # repeat several times to show that it works
	#	print result.stdout.readline(), # read output
	temp_list = []
	while not os.path.exists('result.txt'):
		time.sleep(1)
	i = 0
	if os.path.getsize('result.txt') == 0:
		return "Face could not be detected in image"
	elif os.path.isfile('result.txt'):
		with open("result.txt") as f:
			for line in f:
				if line:
					line = line[:-1]
					if len(str(line)) > 2:
						temp_list.append(float(line))
					else:
						temp_list.append(int(line))
					i+= 1
		os.remove("result.txt")
		os.remove("process.jpg")
		return flask.jsonify(Driver=temp_list[0],
                  			 Expressive=temp_list[1],
                   			 Analytical=temp_list[2],
                   			 Amiable=temp_list[3])
	else:
		return "No results were obtained. Upload another image."

	return ""
if __name__ == "__main__":
   app.run()