/*
 * Copyright (c) 2016 Joachim Stolberg. All rights reserved.
 *
 * This demo is part of MqttApi for Arduino.
 *
 * MqttApi is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MqttApi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MqttApi.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
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
    11  --2k2-+-------> 1 (RX)
              | 
    GND --3k8-+    
*/

#include <SoftwareSerial.h>
#include <MOS.h>
#include <MqttApi.h>

/*
 * Adapt the following lines according to your needs
 */
const char ssid[]        = "XXXXXXXXXXX";       // SSID for your WIFI access point
const char password[]    = "XXXXXXXXXXX";       // password for your WIFI access point
const char mqttServer[]  = "192.168.178.245";   // IP address of the MQTT server/broker    
const char port[]        = "1883";              // port number of the MQTT server/broker     
const char clientId[]    = "myArduino";
const char txTopic[]     = "sensor1/temperature";
const char rxTopic[]     = "sensor2/humidity";


/*
 * Data for the MqttTask
 */
uint16_t temperature;         // Dummy data
bool connected = false;       // MOS flag to signal "we are connected"

/*
 * Used to communicate with the ESP8266 module
 */
SoftwareSerial mySerial(10, 11); // RX, TX
MqttApi mqtt(mySerial);


/*
 * Called when client receives a subsrcibed message from the server.
 */
void on_message(char topic[], char payload[])
{
  Serial.print("RX: ");
  Serial.print(topic);
  Serial.print(" ");
  Serial.println(payload);
}

/*
 * Called when the client receives a CONNACK response from the server.
 */
void on_connect()
{
  Serial.println(F("connected"));
  Serial.print(F("Local IP: "));
  Serial.println(mqtt.readLocalIp());

  // Subscribing in on_connect() means that if we lose the connection and
  // reconnect then subscriptions will be renewed.
  mqtt.subscribe(rxTopic);

  // Start the PublishTask
  MOS_Signal(connected);
}

/*
 * Cyclically called while the client is the state "not connected".
 */
void on_state(int16_t state)
{
  Serial.print(F("Client in state "));
  Serial.println(state);
}

/*
 * Task to publish cyclically.
 */
void PublishTask(PTCB tcb)
{
  static char buffer[20];     // buffer must be static. Otherwise it gets corrupted
                              // before the publish process is finished! 
  
  MOS_Continue(tcb);        // continue at previous interrupted position

  MOS_WaitFor(tcb, connected);

  temperature = 200;         // simulate a range between 20 and 25 Celsius degree
  while(1)
  {
    if(!mqtt.connected())         
    {
      MOS_ResetTask(tcb);
    }
    while(mqtt.busy())      // wait while the comm. channel to the ESP8288 module is busy
    {
      MOS_Delay(tcb, 100);
      Serial.print('.');
    }
    sprintf(buffer, " %u.%1u degree", temperature / 10, temperature % 10);
    temperature = ((temperature + 1) % 50) + 200;

    Serial.print("TX: ");
    Serial.print(txTopic);
    Serial.println(buffer);

    mqtt.publish(txTopic, buffer, false);

    MOS_Delay(tcb, 1000);
  }
}

/*
 * Blink the onboard LED
 */
void LedTask(PTCB tcb)
{
  MOS_Continue(tcb);        // continue at previous interrupted position

  while(1)
  {
    digitalWrite(13, HIGH);
    MOS_Delay(tcb, 200);    // wait some time
    digitalWrite(13, LOW);
    MOS_Delay(tcb, 200);    // wait some time
  }
}


void setup()
{
  pinMode(13, OUTPUT);
  Serial.begin(115600);
  mySerial.begin(38400);

  Serial.println(F("MQTT Client Demo"));
  
  mqtt.setupWifi(ssid, password);    
  mqtt.connectServer(mqttServer, port, clientId);

  mqtt.on_message = on_message;
  mqtt.on_connect = on_connect;
  mqtt.on_state   = on_state;
}


void loop()
{
  mqtt.loop();

  MOS_Call(PublishTask);
  MOS_Call(LedTask);
}

