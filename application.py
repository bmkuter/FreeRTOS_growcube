from plistlib import UID
from urllib import response
from bson import ObjectId,json_util
import json
import bson
from flask import Flask, Response, jsonify, make_response, render_template, request, current_app, g
from flask_restful import Resource, Api
from flask_cors import CORS
from pymongo import MongoClient
import sqlite3 as sl
import json

#These are the collections (tables) we are dealing with 
PLANTS_COLLECTION = "plants"
SETTINGS_COLLECTION = "settings"

def get_database():

    # Provide the mongodb atlas url to connect python to mongodb using pymongo
    CONNECTION_STRING = "mongodb://localhost:27017"
    DATABASE = "grow_cube"
    # Create a connection using MongoClient. You can import MongoClient or use pymongo.MongoClient
    client = MongoClient(CONNECTION_STRING)

    # Create the database for our example (we will use the same database throughout the tutorial
    return client[DATABASE]

# This is added so that many files can reuse the function get_database()
if __name__ == "__main__":
    # Get the database
    database = get_database()

# Thanks: https://docs.aws.amazon.com/elasticbeanstalk/latest/dg/create-deploy-python-flask.html

# EB looks for an 'application' callable by default.
application = Flask(__name__)
#application.config['SERVER_NAME'] = 'localhost:5000'
CORS(application)

delay = int(1000)

# Creating or accessing database
plants = database[PLANTS_COLLECTION]
settings = database[SETTINGS_COLLECTION]
# print a nice greeting.

# The type of endpoint (get, set, etc.) is usually just distinguished by the type of request (GET, POST, PUT, etc.)


@application.route('/device/<device_id>', methods=['POST'])
def flask_set_delay(device_id):
    body = request.json
    response = make_response()
    delay_on = body["delay_on"]
    delay_off = body["delay_off"]
    pulse_width = body["pulse_width"]

    try:
        settings.update_many({ "_id": ObjectId(device_id) }, {
            "$set": {
                "system.delay_on": delay_on,
                "system.pulse_width": pulse_width, 
                "system.delay_off": delay_off
            },
        },
        upsert=True)
        response.status_code = 200
    except:
        response = make_response("Invalid request")
        response.status_code = 400

    return response

# Should not be a POST for a Get command. Should only be GET
@application.route('/device/<device_number>', methods=['GET'])
def flask_query_delay(device_number):

    try:
        res = settings.find_one({ "_id": ObjectId(device_number) })
    except:
        response = make_response("Device id must be a valid 24 character hex string")
        response.status_code = 400

    if 'res' in locals():
        response_body = json.loads(json_util.dumps(res))
        print(response_body)
        response = make_response(response_body)
        response.content_type = "Application/JSON"
        response.status_code = 200
    
    return response


@application.route('/')
def welcome_screen():
    packet = "Welcome to Growpod's GUI:\nPlease visit .../set_device to configure settings. "
    return (packet)


# run the app.
if __name__ == "__main__":
    # Setting debug to True enables debug output. This line should be
    # removed before deploying a production app.
    application.debug = True
    application.run(host="0.0.0.0")
