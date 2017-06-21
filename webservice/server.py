#-*- encoding:utf-8 -*-
from flask import Flask, request
import json

app = Flask(__name__)
app.debug = True


@app.route('/helloworld', methods=['GET'])
def helloWorld():
	return 'hello world man'


if __name__ == '__main__':
	app.run()