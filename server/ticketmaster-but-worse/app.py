import secrets, base64, hashlib
import pyotp
from flask import Flask, json, jsonify, request
from flask_jwt_extended import JWTManager, create_access_token, current_user, jwt_required

app = Flask(__name__)

app.config["JWT_SECRET_KEY"] = secrets.token_urlsafe(64)
jwt = JWTManager(app)

def ticket_token_hash(token: str) -> str:
    return (
        base64.urlsafe_b64encode(hashlib.sha256(token.encode("ascii")).digest())
        .rstrip(b"=")
        .decode("ascii")
    )


#these dicts simulate the servers database
tickets = {}
users = {
    "defaultUser": {
        "username": "defaultUser",
        "password": "password1234",
        "hasPayedForTickets": 1,
    }
}

@jwt.user_lookup_loader
def user_lookup_loader(_jwt_header, jwt_data):
    username = jwt_data["sub"]
    return users[username]

@app.post("/login")
def login():
    username = request.form["username"]
    password = request.form["password"]
    user = users.get(username)
    if user and password == user["password"]:
        access_token = create_access_token(identity=username)
        return jsonify(client_token=access_token)
    return jsonify(msg="Login invalid"), 403


@app.post("/generate_ticket")
@jwt_required()
def generate_ticket():
    user = current_user

    #check if this user has payed for this ticket
    if user["hasPayedForTickets"] > 0:
        token = secrets.token_urlsafe()
        token_hash = ticket_token_hash(token)
        totp_secret = pyotp.random_base32()

        #save only the hash of the token server-side
        tickets[token_hash] = totp_secret 

        #decrease ticket allowance of user
        users[user["username"]]["hasPayedForTickets"] -= 1

        return jsonify(ticket_token = token, totp_secret = totp_secret)
    else:
        return jsonify(msg="This user hasn't bought a ticket yet!"), 403

@app.get("/check_ticket_validity")
def validate_ticket():
    base32Object = request.args["ticket"]
    jsonObjectAsString = base64.b32decode(base32Object).decode('ascii')
    jsonObject = json.loads(jsonObjectAsString)

    #lookup ticket using the hash of its token
    token_hash = ticket_token_hash(jsonObject['token'])
    ticket_secret = tickets.get(token_hash)
    if not ticket_secret:
        return jsonify(msg=f"No such ticket exists"), 404

    totp = pyotp.TOTP(ticket_secret, digits=10, interval=15)
    if totp.verify(jsonObject['one_time_password']):
        return jsonify(msg=f"Success, this ticket is still valid")
    else:
        return jsonify(msg=f"This ticket is invalid!"), 403
