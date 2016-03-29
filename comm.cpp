/**
 * COMM - A library for half-duplex, ASCII and block-oriented communication.
 *
 *
 * Copyright (c) 2016 Joachim Stolberg. All rights reserved.
 *
 * This file is part of MqttApi.
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

/*
 * Transport layer class for a serial, ASCII based communication
 * between two Arduino devices or one Arduino device and one host PC.
 */
Communicate::Communicate(Stream &serial) 
{
  _serial = &serial;
  buff_idx = 0;
  registeredCallback = NULL;
  rxIndication = false;
}


/*
 * Read pending characters from the serial line and 
 * call registered callback if command is complete.
 */
void Communicate::loop()
{
  if(!rxIndication)
  {
    while((_serial->available() > 0) && ((buff_idx + 1) < (int)sizeof(buffer))) 
    {
      buffer[buff_idx] = _serial->read();
      if(buffer[buff_idx] == k_DELIMITER)
      {
        buffer[buff_idx] = 0;
        buff_idx++;
        break;
      }
      buff_idx++;
    }
    if((buff_idx > 0) && (buffer[buff_idx-1] == 0))
    {
      buff_idx = 0;
      rxIndication = true;
      if(registeredCallback != NULL) 
      {
        registeredCallback();
      }
    }
  }
}


/*
 * Return true, if a complete command is received.
 */
bool Communicate::available() 
{
  return rxIndication;
}


/*
 * Get next part from the given command string.
 */
char *Communicate::next() 
{
  if(rxIndication)
  {
    byte idx = buff_idx;
    while((buffer[buff_idx] != 0) && (buffer[buff_idx] != '|'))
    {
      buff_idx++;
    }
    if(buffer[buff_idx] == '|') 
    {
      buffer[buff_idx] = 0;
      buff_idx++;
    }
    else
    {
      reset();
    }
    return &buffer[idx];
  }
  return NULL;
}

/*
 * Attach given callback function. 
 * The function is called after every received command.
 */
void Communicate::attach(commClbkFunc clbk) 
{
  registeredCallback = clbk;
}

/*
 * Flush the communication buffer
 */
void Communicate::flush() 
{
  while(_serial->available() > 0)
  {
    (void)_serial->read();
  }
  reset();
}

void Communicate::reset() 
{
  buff_idx = 0;
  rxIndication = false;
}

char Communicate::getCmd()
{
  return next()[0];
}

char *Communicate::getStr()
{
  return next();
}

uint8_t Communicate::getU8()
{
  return (uint8_t)atoi(next());
}

uint16_t Communicate::getU16()
{
  return (uint16_t)atol(next());
}

uint32_t Communicate::getU32()
{
  return (uint32_t)atol(next());
}

int8_t Communicate::getI8()
{
  return (int8_t)atoi(next());
}

int16_t Communicate::getI16()
{
  return (int16_t)atol(next());
}

int32_t Communicate::getI32()
{
  return (int32_t)atol(next());
}

void Communicate::newTxMsg(char cmnd)
{
  _serial->print(cmnd);
}

void Communicate::add(const char* str)
{
  _serial->print('|');
  _serial->print(str);
}

void Communicate::add(uint8_t val)
{
  _serial->print('|');
  _serial->print(val);
}

void Communicate::add(uint16_t val)
{
  _serial->print('|');
  _serial->print(val);
}

void Communicate::add(uint32_t val)
{
  _serial->print('|');
  _serial->print(val);
}

void Communicate::add(int8_t val)
{
  _serial->print('|');
  _serial->print(val);
}

void Communicate::add(int16_t val)
{
  _serial->print('|');
  _serial->print(val);
}

void Communicate::add(int32_t val)
{
  _serial->print('|');
  _serial->print(val);
}


void Communicate::send()
{
  _serial->write(k_DELIMITER);
  reset();
}

void Communicate::debug()
{
  Serial.print(rxIndication);
  Serial.print(" ");
  Serial.print(buff_idx);
  Serial.print(" ");
  Serial.println(registeredCallback != NULL);
}

/*
 * Copy string from 'src' to 'dst', considering the maximum
 * size of the buffer 'dst', and add the trailing zero.
 */
char* Communicate::strcpy(char* dst, const char* src, uint16_t size)
{
  while((*src != 0) && (--size > 0))
  {
    *dst++ = *src++;
  }
  *dst = 0; // trailing zero
  return dst;
}
