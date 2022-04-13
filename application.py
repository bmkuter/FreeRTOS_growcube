from flask import Flask
from flask import render_template
from flask_restful import Resource, Api
from flask import request
import sqlite3 as sl
import json
# Thanks: https://docs.aws.amazon.com/elasticbeanstalk/latest/dg/create-deploy-python-flask.html

# EB looks for an 'application' callable by default.
application = Flask(__name__)
#application.config['SERVER_NAME'] = 'localhost:5000'

delay = int(1000)

#Creating or accessing database
db = sl.connect('Database/pod_settings_new.db', check_same_thread=False)
db.execute('CREATE TABLE IF NOT EXISTS settings (id INTEGER PRIMARY KEY, delay_on INTEGER, delay_off INTEGER, pulse_width INTEGER, source_delay_on INTEGER, source_delay_off INTEGER, source_pulse_width INTEGER, drain_delay_on INTEGER, drain_delay_off INTEGER, drain_pulse_width INTEGER, food_delay_on INTEGER, food_delay_off INTEGER, food_pulse_width INTEGER, air_delay_on INTEGER, air_delay_off INTEGER, air_pulse_width INTEGER, LED_delay_on INTEGER, LED_delay_off INTEGER, LED_pulse_width INTEGER)')
db.row_factory = lambda cursor, row: row[0]
cur = db.cursor()
#db.close()

# print a nice greeting.
@application.route('/set_device/<device_number>/<delay_on>/<delay_off>/<pulse_width>', methods=['POST', 'GET'])
def flask_set_delay(device_number = 0, delay_on = 0, delay_off = 0, pulse_width = 0):
    cur = db.cursor()
    payload = (device_number, delay_on, delay_off, pulse_width)
    cur.execute("INSERT OR REPLACE INTO settings VALUES(?, ?, ?, ?);", payload)
    db.commit()
    cur.execute("SELECT delay_on FROM settings where id = 1")
    results = cur.fetchall()
    print(results[0])
    return str(results[0])

@application.route('/set_device', methods=['POST', 'GET'])
def flask_set_delay_GUI():
    if request.method == 'POST':
        device_number = request.form.get('device_number')
        #Example
        delay_on = request.form.get('delay_on')
        delay_off = request.form.get('delay_off')
        pulse_width = request.form.get('pulse_width')
        #Source
        source_delay_on = request.form.get('source_delay_on')
        source_delay_off = request.form.get('source_delay_off')
        source_pulse_width = request.form.get('source_pulse_width')
        #Drain
        drain_delay_on = request.form.get('drain_delay_on')
        drain_delay_off = request.form.get('drain_delay_off')
        drain_pulse_width = request.form.get('drain_pulse_width')
        #Food
        food_delay_on = request.form.get('food_delay_on')
        food_delay_off = request.form.get('food_delay_off')
        food_pulse_width = request.form.get('food_pulse_width')
        #Air
        air_delay_on = request.form.get('air_delay_on')
        air_delay_off = request.form.get('air_delay_off')
        air_pulse_width = request.form.get('air_pulse_width')
        #LED
        LED_delay_on = request.form.get('LED_delay_on')
        LED_delay_off = request.form.get('LED_delay_off')
        LED_pulse_width = request.form.get('LED_pulse_width')
        #Making payload
        payload = (device_number, delay_on, delay_off, pulse_width,source_delay_on,source_delay_off,source_pulse_width,drain_delay_on,drain_delay_off,drain_pulse_width,food_delay_on,food_delay_off,food_pulse_width,air_delay_on,air_delay_off,air_pulse_width,LED_delay_on,LED_delay_off,LED_pulse_width)
        print(payload)
        cur.execute("INSERT OR REPLACE INTO settings VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);", payload)
        db.commit()
        cur.execute("SELECT delay_on FROM settings WHERE id =:device_number", {"device_number": device_number})
        results = cur.fetchall()
        print(json.dumps(results))
    return render_template('form.html')


### Convert so this returns a JSON of all needed info.
@application.route('/query_device/<device_number>', methods=['POST', 'GET'])
def flask_query_delay(device_number):
    #Example
    cur.execute("SELECT id FROM settings WHERE id =:device_number", {"device_number": device_number})
    device_id = cur.fetchall()[0]
    cur.execute("SELECT delay_on FROM settings WHERE id =:device_number", {"device_number": device_number})
    device_delay_on = cur.fetchall()[0]
    cur.execute("SELECT delay_off FROM settings WHERE id =:device_number", {"device_number": device_number})
    device_delay_off = cur.fetchall()[0]
    cur.execute("SELECT pulse_width FROM settings WHERE id =:device_number", {"device_number": device_number})
    device_pulse_width = cur.fetchall()[0]
    #Source
    cur.execute("SELECT source_delay_on FROM settings WHERE id =:device_number", {"device_number": device_number})
    device_source_delay_on = cur.fetchall()[0]
    cur.execute("SELECT source_delay_off FROM settings WHERE id =:device_number", {"device_number": device_number})
    device_source_delay_off = cur.fetchall()[0]
    cur.execute("SELECT source_pulse_width FROM settings WHERE id =:device_number", {"device_number": device_number})
    device_source_pulse_width = cur.fetchall()[0]
    #Drain
    cur.execute("SELECT drain_delay_on FROM settings WHERE id =:device_number", {"device_number": device_number})
    device_drain_delay_on = cur.fetchall()[0]
    cur.execute("SELECT drain_delay_off FROM settings WHERE id =:device_number", {"device_number": device_number})
    device_drain_delay_off = cur.fetchall()[0]
    cur.execute("SELECT drain_pulse_width FROM settings WHERE id =:device_number", {"device_number": device_number})
    device_drain_pulse_width = cur.fetchall()[0]
    #Food
    cur.execute("SELECT food_delay_on FROM settings WHERE id =:device_number", {"device_number": device_number})
    device_food_delay_on = cur.fetchall()[0]
    cur.execute("SELECT food_delay_off FROM settings WHERE id =:device_number", {"device_number": device_number})
    device_food_delay_off = cur.fetchall()[0]
    cur.execute("SELECT food_pulse_width FROM settings WHERE id =:device_number", {"device_number": device_number})
    device_food_pulse_width = cur.fetchall()[0]
    #Air
    cur.execute("SELECT air_delay_on FROM settings WHERE id =:device_number", {"device_number": device_number})
    device_air_delay_on = cur.fetchall()[0]
    cur.execute("SELECT air_delay_off FROM settings WHERE id =:device_number", {"device_number": device_number})
    device_air_delay_off = cur.fetchall()[0]
    cur.execute("SELECT air_pulse_width FROM settings WHERE id =:device_number", {"device_number": device_number})
    device_air_pulse_width = cur.fetchall()[0]
    #LED
    cur.execute("SELECT LED_delay_on FROM settings WHERE id =:device_number", {"device_number": device_number})
    device_LED_delay_on = cur.fetchall()[0]
    cur.execute("SELECT LED_delay_off FROM settings WHERE id =:device_number", {"device_number": device_number})
    device_LED_delay_off = cur.fetchall()[0]
    cur.execute("SELECT LED_pulse_width FROM settings WHERE id =:device_number", {"device_number": device_number})
    device_LED_pulse_width = cur.fetchall()[0]
    #Formatting JSON
    output_json = {
        "id": device_id, 
        "delay_on": device_delay_on,
        "delay_off": device_delay_off,
        "pulse_width": device_pulse_width,
        "source": {
        "delay_on": device_source_delay_on,
        "delay_off": device_source_delay_off,
        "pulse_width": device_source_pulse_width
        },
        "drain": {
        "delay_on": device_drain_delay_on,
        "delay_off": device_drain_delay_off,
        "pulse_width": device_drain_pulse_width
        },
        "food": {
        "delay_on": device_food_delay_on,
        "delay_off": device_food_delay_off,
        "pulse_width": device_food_pulse_width
        },
        "air": {
        "delay_on": device_air_delay_on,
        "delay_off": device_air_delay_off,
        "pulse_width": device_air_pulse_width
        },
        "LED": {
        "delay_on": device_LED_delay_on,
        "delay_off": device_LED_delay_off,
        "pulse_width": device_LED_pulse_width
        }
    }
    return (json.dumps(output_json))

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

