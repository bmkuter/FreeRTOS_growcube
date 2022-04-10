from flask import Flask
from flask import render_template
from flask_restful import Resource, Api
from flask import request
import sqlite3 as sl
import json
# Thanks: https://docs.aws.amazon.com/elasticbeanstalk/latest/dg/create-deploy-python-flask.html

# EB looks for an 'application' callable by default.
application = Flask(__name__)

delay = int(1000)

#Creating or accessing database
db = sl.connect('Database/pod_settings_new.db', check_same_thread=False)
db.execute('CREATE TABLE IF NOT EXISTS settings (id INTEGER PRIMARY KEY, delay_on INTEGER, delay_off INTEGER, pulse_width INTEGER)')
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
        delay_on = request.form.get('delay_on')
        delay_off = request.form.get('delay_off')
        pulse_width = request.form.get('pulse_width')
        payload = (device_number, delay_on, delay_off, pulse_width)
        print(payload)
        cur.execute("INSERT OR REPLACE INTO settings VALUES(?, ?, ?, ?);", payload)
        db.commit()
        cur.execute("SELECT delay_on FROM settings WHERE id =:device_number", {"device_number": device_number})
        results = cur.fetchall()
        print(json.dumps(results))
    return render_template('form.html')


### Convert so this returns a JSON of all needed info.
@application.route('/query_device/<device_number>', methods=['POST', 'GET'])
def flask_query_delay(device_number):
    #How can we combine these queries into 1??
    cur.execute("SELECT id FROM settings WHERE id =:device_number", {"device_number": device_number})
    device_id = cur.fetchall()[0]
    cur.execute("SELECT delay_on FROM settings WHERE id =:device_number", {"device_number": device_number})
    device_delay_on = cur.fetchall()[0]
    cur.execute("SELECT delay_off FROM settings WHERE id =:device_number", {"device_number": device_number})
    device_delay_off = cur.fetchall()[0]
    cur.execute("SELECT pulse_width FROM settings WHERE id =:device_number", {"device_number": device_number})
    device_pulse_width = cur.fetchall()[0]
    output_json = {
        "id": device_id, 
        "delay_on": device_delay_on,
        "delay_off": device_delay_off,
        "pulse_width": device_pulse_width
        }
    return (json.dumps(output_json))

# run the app.
if __name__ == "__main__":
    # Setting debug to True enables debug output. This line should be
    # removed before deploying a production app.
    application.debug = True
    application.run(host="0.0.0.0")

