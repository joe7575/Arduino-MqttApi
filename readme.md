# MqttApi - MQTT for Arduino Devices

MqttApi is an Arduino library to implement a MQTT Client based on an ESP8266-01 module
acting as a MQTT/WIFI shield. The software on the ESP8266 is based on the 
PubSubClient library from Nicholas O'Leary.
(See https://github.com/knolleary/pubsubclient)


## History
- 2016-03-28  V0.1  Initial Version


## Documentation
The library on Arduino side is only a thin layer with a small memory footprint.
The real MQTT client is implemented on the ESP8266 module, connected to the Arduino
device via TX/RX pins (Serial/SoftwareSerial).
Hint: The ESP8266 accepts only 3.3V power supply. For the RX port I used a resistor divider.

```
  ESP8266:
    +-------------+----+
    |          RX-|1  8|-VCC
    |      GPIO 0-|2  7|-RST
    |      GPIO 2-|3  6|-CH_PD
    |         GND-|4  5|-TX 
    +-------------+----+

  Arduino            ESP8266
    GND --------------> 4
    3V3 --------------> 8 + 6
    10  <-------------- 5 (TX)
    11  --[2k2]-+-----> 1 (RX)
                | 
    GND --[3k8]-+    
```
  
In the example folder are the sketches for the ESP8266 module (module) and the Arduino device (client).

To compile and download the sketch to the ESP8266 module, you have to in addition:
 - install "ESP8266 core for Arduino" from https://github.com/esp8266/Arduino
 - install the "PubSubClient" from https://github.com/Imroy/pubsubclient


## Installation
Download and extract the ZIP file, rename the folder "MqttApi-master" into 'MqttApi' and copy the 'MqttApi' folder 
into the Arduino 'libraries' folder.


## Hints
- This API is not finished. Changes on the API are not planned but could occur.
- The documentation is far away from complete. You have to have experiences with MQTT
- I used a Raspery Pi as MQTT broker and additional Python scripts on the PC to simulate other devices


## Questions/Feedback
Questions, hint, found issues with this project should be posted to joe.stolberg(at)gmx(dot)de

## License
MqttApi is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

MqttApi is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with MqttApi.  If not, see <http://www.gnu.org/licenses/>.






