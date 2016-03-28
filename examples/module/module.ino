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
 
/*
  MQTT application for ESP8266 devices acting as MQTT module,
  based on:
  https://github.com/knolleary/pubsubclient


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


  History:
  2016-03-25  V0.01  First try

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>   // MQTT client
#include <MOS.h>            // multitasking OS
#include <comm.h>           // serial communication

static WiFiClient   wifiClient;
static PubSubClient pubsubClient(wifiClient);
static Communicate  comm(Serial);

#define BUFF_SIZE     60      // string buffer

#define MIN(a, b)   ((a)<(b))?(a):(b)

#define INVALID_CMND        6
#define NO_LOCAL_IP         7
#define PUB_FAILED          8
#define SUB_FAILED          9


static char     ac_WillTopic[BUFF_SIZE] = {0};
static char     ac_WillMessage[BUFF_SIZE] = {0};
static char     ac_ClientId[BUFF_SIZE] = {0};
static char     ac_MqttServer[BUFF_SIZE] = {0};
static char     ac_Port[BUFF_SIZE] = {0};
static char     ac_RxTopic[BUFF_SIZE] = {0};
static char     ac_RxPayload[BUFF_SIZE] = {0};

static bool     mqttInitialized = false;
static bool     pendingRxMessage = false;
static bool     connected = false;

/*
 * Send simple response message to client
 */
static void response(const char *msg)
{
  comm.newTxMsg('r');   // response to a command message
  comm.add(msg);
  comm.send();
}

static void state(int16_t state)
{
  comm.newTxMsg('s');   // state message
  comm.add(state);
  comm.send();
}

/*
 * Setup WIFI connection
 */
static void setup_wifi(void) 
{
  char *ssid;
  char *password;

  ssid = comm.getStr();
  password = comm.getStr();
  
  if((ssid != NULL) && (password != NULL)) 
  {
    WiFi.begin(ssid, password);
    response("ok");
  }
  else 
  {
    state(INVALID_CMND);
  }
}

/*
 * Respond with local IP address
 */
static void get_local_ip(void)
{
  IPAddress ip;
  char buffer[20];

  if(WiFi.status() == WL_CONNECTED) 
  {
    ip = WiFi.localIP();
    sprintf(buffer, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
    response(buffer);
  }
  else
  {
    state(NO_LOCAL_IP);
  }
}

/*
 * Define will message
 */
static void will_message(void)
{
  char *topic;
  char *message;

  topic = comm.getStr();
  message = comm.getStr();

  // store for later use
  if(topic != NULL)
  {
    strcpy(ac_WillTopic, topic);
  }
  if(message != NULL)
  {
    strcpy(ac_WillMessage, message);
  }
}

/*
 * Setup MQTT connection
 */
static void setup_mqtt(void)
{
  char *mqtt_server;
  char *port;
  char *client_id;

  mqtt_server = comm.getStr();
  port = comm.getStr();
  client_id = comm.getStr();

  // store for later (re-)use
  if(mqtt_server != NULL)
  {
    strcpy(ac_MqttServer, mqtt_server);
  }
  if(port != NULL)
  {
    strcpy(ac_Port, port);
  }
  if(client_id != NULL)
  {
    strcpy(ac_ClientId, client_id);
  }
  if((mqtt_server != NULL) && (port != NULL) && (client_id != NULL))
  {
    pubsubClient.setServer((const char *)ac_MqttServer, (uint16_t)atoi(ac_Port));
    mqttInitialized = true;
    response("ok");
  }
  else
  {
    state(INVALID_CMND);
  }
}



/*
 * Publish a message
 */
static void publish(void)
{
  char *topic;
  char *message;
  char *retained;

  topic = comm.getStr();
  message = comm.getStr();
  retained = comm.getStr();

  if((topic != NULL) && (message != NULL) && (retained != NULL))
  {
    if(pubsubClient.publish(topic, message, atoi(retained)) == true)
    {
      response("ok");
    }
    else
    {
      state(PUB_FAILED);
    }
  }
  else
  {
    state(INVALID_CMND);
  }
}

/*
 * Subscribe to a topic to receive messages
 */
static void subscribe(void)
{
  char *topic;

  topic = comm.getStr();

  if(topic != NULL)
  {
    if(pubsubClient.subscribe(topic) == true)
    {
      response("ok");
    }
    else
    {
      state(SUB_FAILED);
    }
  }
  else
  {
    state(INVALID_CMND);
  }
}

/*
 * Unsubscribe from one topic.
 */
static void unsubscribe(void)
{
  char *topic;

  topic = comm.getStr();

  if(topic != NULL)
  {
    if(pubsubClient.unsubscribe(topic) == true)
    {
      response("ok");
    }
    else
    {
      state(SUB_FAILED);
    }
  }
  else
  {
    state(INVALID_CMND);
  }
}


/*
 * Return received MQTT message, connection state, or none.
 */
static void polling()
{
  if(!pubsubClient.connected())
  {
    state((int16_t)pubsubClient.state());
    connected = false;
  }
  else if(!connected)
  {
    connected = true;
    comm.newTxMsg('c');   // connected message
    comm.send();
  }
  else if(pendingRxMessage)
  {
    comm.newTxMsg('m');   // received message
    comm.add(ac_RxTopic);
    comm.add(ac_RxPayload);
    comm.send();
    pendingRxMessage = false;
  }
  else
  {
    comm.newTxMsg('n');   // none
    comm.send();
  }
}


/*
 * Process received command
 */
void process_command(char cmd)
{
  switch(cmd)  // command byte
  {
    case 'i': response("ESP8266 MQTT Module V0.01"); break;
    case 'w': setup_wifi(); break;
    case 'a': get_local_ip(); break;
    case 't': will_message(); break;
    case 'm': setup_mqtt(); break;
    case 'p': publish(); break;
    case 's': subscribe(); break;
    case 'u': unsubscribe(); break;
    case 'r': polling(); break;
    default:  state(INVALID_CMND); break;
  }
}

/*
 * Serial comunication callback function to handle
 * received Arduino commands.
 */
void comm_callback(void)
{
  process_command(comm.getCmd());
}

/*
 * MQTT callback function to handle received MQTT messages.
 */
static void mqtt_callback(char* topic, byte* payload, unsigned int length) 
{
  if(!pendingRxMessage)
  {
    // store locally until next token is received
    comm.strcpy(ac_RxTopic, topic, BUFF_SIZE);
    comm.strcpy(ac_RxPayload, (char*)payload, MIN(BUFF_SIZE, length+1));
    pendingRxMessage = true;
  }
}


/*
 * Task to keep the connection to the MQTT broker
 */
void ReconnectTask(PTCB ptcb)
{
  MOS_Continue(ptcb);          // continue on the previous interrupted position

  while(1)
  {
    // Loop until we're reconnected
    while(!pubsubClient.connected())
    {
      connected = false;
      if(pubsubClient.connect(ac_ClientId))
      {
        //pubsubClient.publish(ac_WillTopic, ac_WillMessage);
        break;
      } 
      else 
      {
        MOS_Delay(ptcb, 5000);  // retry in 5 sec.
      }
    }
    // check connection every 5 sec.
    while(pubsubClient.connected())
    {
      MOS_Delay(ptcb, 5000); 
    }
  }
}


void setup()
{
  Serial.begin(38400);
  comm.attach(comm_callback);
  pubsubClient.setCallback(mqtt_callback);
}



void loop()
{
  comm.loop();
  pubsubClient.loop();
 
  if(mqttInitialized)
  {
    MOS_Call(ReconnectTask);
  }
}


