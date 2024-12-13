import secrets, base64, hashlib
import pyotp
from flask import Flask, json, jsonify, request
from flask_jwt_extended import JWTManager, create_access_token, jwt_required

app = Flask(__name__)

app.config["JWT_SECRET_KEY"] = secrets.token_urlsafe(64)
jwt = JWTManager(app)

def ticket_token_hash(token: str) -> str:
    return (
        base64.urlsafe_b64encode(hashlib.sha256(token.encode("ascii")).digest())
        .rstrip(b"=")
        .decode("ascii")
    )



tickets = {}

@app.get("/hello_world")
def hello_world():
    name = request.args["name"]
    return jsonify(msg=f"hello {name}!")

@app.post("/test")
def test():
    parameter = request.form["test"]
    return jsonify(msg=f"Your parameter was {parameter}")

@app.post("/login")
def login():
    username = request.form["username"]
    password = request.form["password"]
    if username == "defaultUser" and password == "password1234":
        access_token = create_access_token(identity="defaultUser")
        return jsonify(client_token=access_token)
    return jsonify(msg="Login invalid")



@app.post("/generate_token")
@jwt_required()
def generate_token():
    token = secrets.token_urlsafe()
    token_hash = ticket_token_hash(token)
    totp_secret = pyotp.random_base32()
    tickets[token_hash] = totp_secret
    return jsonify(ticket_token = token, totp_secret = totp_secret)

@app.get("/validate_ticket")
def validate_ticket():
    base64Object = request.args["ticket"]
    jsonObjectAsString = base64.b64decode(base64Object).decode('utf-8')
    jsonObject = json.loads(jsonObjectAsString)
    return jsonify(msg=f"object is {jsonObject['token']}")
