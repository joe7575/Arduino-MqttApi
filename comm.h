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

#ifndef Comm_h
#define Comm_h

#include "Arduino.h"
#include <inttypes.h>

#define k_MAX_PACKET_SIZE     100
#define k_TIMOUT              200

//#define k_DELIMITER           '\n'
#define k_DELIMITER           ((uint8_t)0)


// callback function types
extern "C" {
  typedef void (*commClbkFunc)();
}

class Communicate 
{
  private:
    char      buffer[k_MAX_PACKET_SIZE];  
    uint8_t   buff_idx;
    bool      rxIndication;
    Stream    *_serial;
    commClbkFunc registeredCallback;   
    char      *next();
     
  public:
    Communicate(Stream &serial);

    char      getCmd();
    char*     getStr();
    uint8_t   getU8();
    uint16_t  getU16();
    uint32_t  getU32();
    int8_t    getI8();
    int16_t   getI16();
    int32_t   getI32();

    void      newTxMsg(char);
    void      add(const char*);
    void      add(uint8_t);
    void      add(uint16_t);
    void      add(uint32_t);
    void      add(int8_t);
    void      add(int16_t);
    void      add(int32_t);
    void      send();

    void      attach(commClbkFunc clbk);
    bool      available();
    void      reset();
    void      flush();
    void      loop();
    void      debug();
    char*     strcpy(char*, const char*, uint16_t);
};

#endif //Comm_h
