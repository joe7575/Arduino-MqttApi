#
#  Copyright (c) 2016 Joachim Stolberg. All rights reserved.
#  
#  This demo is part of MqttApi for Arduino.
#  
#  MqttApi is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  MqttApi is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#  
#  You should have received a copy of the GNU General Public License
#  along with MqttApi.  If not, see <http://www.gnu.org/licenses/>.


#
#    +-------------+                      +-----------+
#    |   sensor1   |                      |  sensor2  |
#    | temperature |--------------------> |           |
#    |             |<-------------------- | humidity  |
#    +-------------+                      +-----------+
#
#

import time
import paho.mqtt.client as mqtt

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe("sensor1/temperature")

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    print("RX: " + msg.topic + " " + str(msg.payload))

client = mqtt.Client(client_id="MySensor")
client.on_connect = on_connect
client.on_message = on_message

client.connect("192.168.178.245", 1883, 60)

run = True
cnt = 0
humidity = 50
while run:
    client.loop()
    if (cnt % 10) == 0:
        print("TX: sensor2/humidity %u percent" % humidity)
        client.publish("sensor2/humidity", "%u percent" % humidity)
        humidity = ((humidity + 1) % 50) + 50
    cnt += 1
    time.sleep(0.1)
