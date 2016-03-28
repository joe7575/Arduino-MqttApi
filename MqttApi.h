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

#ifndef MqttApi_h
#define MqttApi_h

#include "Arduino.h"
#include <inttypes.h>
#include "comm.h"

/*
 * Possible connevction state values
 */
#define MQTT_CONNECTION_TIMEOUT     -4
#define MQTT_CONNECTION_LOST        -3
#define MQTT_CONNECT_FAILED         -2
#define MQTT_DISCONNECTED           -1
#define MQTT_CONNECTED               0
#define MQTT_CONNECT_BAD_PROTOCOL    1
#define MQTT_CONNECT_BAD_CLIENT_ID   2
#define MQTT_CONNECT_UNAVAILABLE     3
#define MQTT_CONNECT_BAD_CREDENTIALS 4
#define MQTT_CONNECT_UNAUTHORIZED    5

#define MQTT_INVALID_CMND        6
#define MQTT_NO_LOCAL_IP         7
#define MQTT_PUB_FAILED          8
#define MQTT_SUB_FAILED          9


// callback function types
extern "C" {
  typedef void (*clbkOnMessage)(char topic[], char payload[]);
  typedef void (*clbkOnConnect)();
  typedef void (*clbkOnState)(int16_t state);
}

class MqttApi 
{
  private:
    Communicate   comm;
    uint16_t      timeout;
    bool          waitForResponse;    // from ESP8266 module
    bool          _connected;
    void          send();
    bool          wait();
     
  public:
    clbkOnMessage on_message;
    clbkOnConnect on_connect;
    clbkOnState   on_state;
    
    MqttApi(Stream &serial) :
      comm(serial)
    {
      waitForResponse = false;
      _connected = false;
      on_message = NULL;
      on_connect = NULL;
      on_state = NULL;
    }
    
    char*   readIdentity();
    bool    setupWifi(const char ssid[], const char password[]); 
    bool    connectServer(const char mqtt_server[], const char port[], const char client_id[]);
    bool    willMessage(const char willTopic[], const char willMessage[]);
    char*   readLocalIp();
    bool    publish(const char topic[], const char message[], bool retained);
    bool    subscribe(const char topic[]);
    bool    unsubscribe(const char topic[]);
    void    loop();
    bool    busy();
    bool    connected();
};

#endif //MqttApi_h
