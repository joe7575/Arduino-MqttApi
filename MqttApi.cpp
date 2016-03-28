/**
 * Copyright (c) 2016 Joachim Stolberg. All rights reserved.
 *
 * This file is part of MqttApi for Arduino.
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

#include "comm.h"
#include "MqttApi.h"

#define INFO(val)             Serial.print(val)
#define TIME_OVER(t)          ((uint16_t)(millis() - (t)) < 0x8000UL)
#define TIMEOUT               200

/*
 * Finalize the send process to the ESP8266 module
 */
void MqttApi::send()
{
  comm.send();
  waitForResponse = true;
}

/*
 * Private function, Waits for a resonse from the ESP8266 module.
 * Only used to setup the connection.
 * Returns true, if response is available
 */
bool MqttApi::wait()
{
  uint16_t t = millis() + TIMEOUT;
  while(!TIME_OVER(t))
  {
    comm.loop();
    if(comm.available())
    {
      waitForResponse = false;
      char c = comm.getCmd();
      return (c == 'r');
    }
  }
  return false;
}


/*
 * Read ESP8266 module identification string
 */
char* MqttApi::readIdentity() 
{
  comm.newTxMsg('i');
  send();

  if(wait())
  {
    return comm.getStr();
  }
  return NULL;
}

/*
 * setup a WIFI connection by means of the given ssid and password
 * of the WIFI access point
 */
bool MqttApi::setupWifi(const char ssid[], const char password[]) 
{
  comm.newTxMsg('w');
  comm.add(ssid);
  comm.add(password);
  comm.send();
  waitForResponse = true;

  return wait();
}

/*
 * request the ESP8266 IP address
 */
char* MqttApi::readLocalIp()
{
  comm.newTxMsg('a');
  comm.send();
  waitForResponse = true;

  if(wait())
  {
    return comm.getStr();
  }
  return NULL;
}

/*
 * Connect to a MQTT broker
 */
bool MqttApi::connectServer(const char mqtt_server[], const char port[], const char client_id[])
{
  comm.newTxMsg('m');
  comm.add(mqtt_server);
  comm.add(port);
  comm.add(client_id);
  comm.send();
  waitForResponse = true;

  return wait();
}

/*
 * Specify the will message
 */
bool MqttApi::willMessage(const char willTopic[], const char willMessage[])
{
  comm.newTxMsg('t');
  comm.add(willTopic);
  comm.add(willMessage);
  comm.send();
  waitForResponse = true;

  return wait();
}

/*
 * Subscribe to a topic to receive messages
 */
bool MqttApi::subscribe(const char* topic)
{
  comm.newTxMsg('s');
  comm.add(topic);
  comm.send();
  waitForResponse = true;

  return wait();
}

/*
 * Unsubscribe from one topic
 */
bool MqttApi::unsubscribe(const char* topic)
{
  if(!waitForResponse)
  {
    comm.newTxMsg('u');
    comm.add(topic);
    comm.send();
    waitForResponse = true;
    return true;
  }
  return false;
}

/*
 * Publish a message
 */
bool MqttApi::publish(const char topic[], const char message[], bool retained) 
{
  if(!waitForResponse)
  {
    comm.newTxMsg('p');
    comm.add(topic);
    comm.add(message);
    comm.add((uint8_t)retained);
    comm.send();
    waitForResponse = true;
    return true;
  }
  return false;
}

/*
 * If function returns true, no publish is possible
 */
bool MqttApi::busy()
{
  loop();
  return waitForResponse;
}

/*
 * Return the MQTT connection state
 */
bool MqttApi::connected()
{
  return _connected; 
}

/*
 * Keep the connection to the ESP8266 module
 */
void MqttApi::loop()
{
  comm.loop();
  if(comm.available())
  {
    waitForResponse = false;
    char c = comm.getCmd();
    if((c == 'm') && (on_message))
    {
      char *topic = comm.getStr();
      char *payload = comm.getStr();
      on_message(topic, payload);
    }
    else if((c == 'c') && (on_connect))
    {
      on_connect();
      _connected = true;
    }
    else if((c == 's') && (on_state))
    {
      on_state(comm.getI16());
      _connected = false;
    }
    comm.reset();
  }
  else if(!waitForResponse && TIME_OVER(timeout))
  {
    if(_connected)
    {
      timeout = millis() + TIMEOUT;
    }
    else
    {
      timeout = millis() + (TIMEOUT * 5);
    }
    comm.newTxMsg('r');
    comm.send();
    waitForResponse = true;
  }
}

