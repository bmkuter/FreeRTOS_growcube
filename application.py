from flask import Flask, jsonify
from flask import render_template
from flask_restful import Resource, Api
from flask import request
import sqlite3 as sl
import json
from flask_cors import CORS

# Thanks: https://docs.aws.amazon.com/elasticbeanstalk/latest/dg/create-deploy-python-flask.html

# EB looks for an 'application' callable by default.
application = Flask(__name__)
#application.config['SERVER_NAME'] = 'localhost:5000'
CORS(application)

delay = int(1000)

# Creating or accessing database
db = sl.connect('Database/pod_settings_new.db', check_same_thread=False)
db.execute('CREATE TABLE IF NOT EXISTS settings (id INTEGER PRIMARY KEY, delay_on INTEGER, delay_off INTEGER, pulse_width INTEGER, source_delay_on INTEGER, source_delay_off INTEGER, source_pulse_width INTEGER, drain_delay_on INTEGER, drain_delay_off INTEGER, drain_pulse_width INTEGER, food_delay_on INTEGER, food_delay_off INTEGER, food_pulse_width INTEGER, air_delay_on INTEGER, air_delay_off INTEGER, air_pulse_width INTEGER, LED_delay_on INTEGER, LED_delay_off INTEGER, LED_pulse_width INTEGER)')
cur = db.cursor()
# db.close()

# print a nice greeting.

# The type of endpoint (get, set, etc.) is usually just distinguished by the type of request (GET, POST, PUT, etc.)
@application.route('/set_device/<device_number>/<delay_on>/<delay_off>/<pulse_width>', methods=['POST', 'GET'])
def flask_set_delay(device_number=0, delay_on=0, delay_off=0, pulse_width=0):
    cur = db.cursor()
    payload = (device_number, delay_on, delay_off, pulse_width)
    cur.execute("INSERT OR REPLACE INTO settings VALUES(?, ?, ?, ?);", payload)
    db.commit()
    cur.execute("SELECT delay_on FROM settings where id = 1")
    results = cur.fetchall()
    print(results[0])
    return str(results[0])


# Set should not accept GET
# @application.route('/device', methods=['POST'])
# def flask_set_delay_GUI():
#     data = json.loads(request.data)
#     # if request.method == 'POST':
#         # Making payload
#     payload = (data["id"],data["delay_on"],data["delay_off"],data["pulse_width"],data["source"]["delay_on"],data["source"]["delay_off"],data["source"]["pulse_width"],data["drain"]["delay_on"],data["drain"]["delay_off"],
#                 data["drain"]["pulse_width"],data["food"]["delay_on"],data["food"]["delay_off"],data["food"]["pulse_width"],data["air"]["delay_on"],data["air"]["delay_off"],data["air"]["pulse_width"],data["LED"]["delay_on"],data["LED"]["delay_off"],data["LED"]["pulse_width"])
#     cur.execute(
#         "INSERT OR REPLACE INTO settings VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);", payload)
#     db.commit()
#     cur.execute("SELECT * FROM settings WHERE id =:device_number", {
#                 "device_number": data["id"]})
#     results = cur.fetchall()
#     print(json.dumps(results))
#     return json.dumps(results)
#     # Don't interlock an API endpoint with a UI endpoint. This should only return data


# Convert so this returns a JSON of all needed info.

@application.route('/device/<device_number>', methods=['GET','POST']) # Should not be a POST for a Get command. Should only be GET
def flask_query_delay(device_number):
    if request.method == 'GET':
        # This is just because maintaining multiple SQL queries like that is a PITA
        row = []
        for record in db.execute("SELECT id, delay_on, delay_off, pulse_width, source_delay_on, source_delay_off, source_pulse_width, drain_delay_on, drain_delay_off, drain_pulse_width, food_delay_on, food_delay_off, food_pulse_width, air_delay_on, air_delay_off, air_pulse_width, LED_delay_on, LED_delay_off, LED_pulse_width FROM settings WHERE id =:device_number", {"device_number": device_number}):
            row = record
        # Formatting JSON
        # I don't think its the best idea to keep this as just indices
        output_json = {
            "id": row[0],
            "delay_on": row[1],
            "delay_off": row[2],
            "pulse_width": row[3],
            "source": {
                "delay_on": row[4],
                "delay_off": row[5],
                "pulse_width": row[6]
            },
            "drain": {
                "delay_on": row[7],
                "delay_off": row[8],
                "pulse_width": row[9]
            },
            "food": {
                "delay_on": row[10],
                "delay_off": row[11],
                "pulse_width": row[12]
            },
            "air": {
                "delay_on": row[13],
                "delay_off": row[14],
                "pulse_width": row[15]
            },
            "LED": {
                "delay_on": row[16],
                "delay_off": row[17],
                "pulse_width": row[18]
            }
        }
        return output_json

    elif request.method == 'POST':
        data = json.loads(request.data)
        # Making payload
        payload = (device_number,data["delay_on"],data["delay_off"],data["pulse_width"],data["source"]["delay_on"],data["source"]["delay_off"],data["source"]["pulse_width"],data["drain"]["delay_on"],data["drain"]["delay_off"],
                    data["drain"]["pulse_width"],data["food"]["delay_on"],data["food"]["delay_off"],data["food"]["pulse_width"],data["air"]["delay_on"],data["air"]["delay_off"],data["air"]["pulse_width"],data["LED"]["delay_on"],data["LED"]["delay_off"],data["LED"]["pulse_width"])
        cur.execute(
            "INSERT OR REPLACE INTO settings VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);", payload)
        db.commit()
        return {}


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
