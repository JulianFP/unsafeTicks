from flask import Flask, jsonify

app = Flask(__name__)

@app.get("/hello_world")
def hello_world():
    return jsonify(msg="hello world!")

