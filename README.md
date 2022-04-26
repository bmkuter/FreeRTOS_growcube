# FreeRTOS_growcube
 
# Goal
The aim here is to update my previous EC601 project with FreeRTOS, and improve the front end database handlnig. The previous generation's firmware worked, but was more of a prototype to ensure the physical concept worked. Now I hope to improve scalability through more thorough structure and code design, including a RTOS to alleviate many of the previous timing and blocking issues. 
Additionally, the front end database has been rewritten to include an actual database, ensuring data preservation between program power cycles. Currently the growpod has to be connected to the program to initially load values from a power cycle, but the data is persistent on the server's side. 

# Server
The server is currently implemented as a Rasperry Pi deployed as an access point. A reference to implement such will be found below. The growpods connect to the Pi AP via internal wireless modues (ESP32). Flask is deployed on the server to run a database querying program. Embedded devices connect to the Flask endpoint to access each device's configurations. Once the configuration is loaded, the growpods no longer need to be connected to the network, and the program/server can sleep. Once they come back online, the growpods will reconnect automatically. Current growpod power cycles require the program/server to be running, but that will be alleviated with flash memory soon. The database is SQLite, and can be accessed from the Pi AP through SSH. 

Pi AP: https://learn.pi-supply.com/make/how-to-setup-a-wireless-access-point-on-the-raspberry-pi/

# Operating System
The new iteration of the firmware is utilising FreeRTOS. Threads alleviate many of the annoying blocking and race conditions from the previous firmware. Through FreeRTOS' `vTaskDelay()` and `vTaskDelayUntil()`, schedules can be effortlessly set and maintained. They can also finally be readjusted mid cycle, allowing for instantaneous changes from the server. 

# The Microcontroller
The microcontroller has been changed from the Teensy 4.1 to the ESP32. The Teensy, while powerful and with its own threading API, lacked the smaller formfactor desired for the next enclosure revision, and more importantly lacked internal WiFi/BLE. The latter is critical for future scalability. Fortunately, the ESP32 has its own flavour of FreeRTOS baked in, with ample documentation. 

# The Python in a Flask
The databasing itself is handled through a python-flask deployment. The server can be accessed by connected devices through port `5000` to interact with the database. A growpod connects to `http://192.168.4.1:5000/query_device/<device_number>` and queries the database for its settings. To improve efficiency, the growpod only reinitilaises itself if the database has changed through a quick comparison-hash to the previous database state. A user interacts with the database through `http://192.168.4.1:5000/set_device`. Dynamics and more features to be added. 

# To Load
Download \*.h, \*.cpp, \*.ino files into an arduino-flavoured editor. Ensure ESP32 files are downloaded and installed. I suggest: https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/. 
Server is designed for a Pi. Slight changes may be needed in `application.py` file if the access point is set up differently from default IP address of `192.168.4.1`. Once server is set as an AP, SSH into the Pi and start the flask deployment. 

More features to come. 
