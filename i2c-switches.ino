/*
MIT License

Copyright (c) 2020 IoT Crazy

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.


githib.com/iot-crazy
@iotcrazy

*/

#include <Arduino.h>
#include <Wire.h>
#include <Timer.h>

#define NUMBER_OF_SWITCHES  16  /* Number of swithces used */
#define INTERRUPT_PIN       14  /* The Adruino pin on which to receive the interrupt notification */
#define MCP_ADDRESS         0x20  /* The address of the MCp23018 device, refer to data sheet for how to set this */

/* Register addresses in the MCP23018 device */
#define IODIR               0x00
#define GPIOA               0x12
#define INTCONA             0x08
#define INTCONB             0x09
#define GPINTENA            0x04
#define GPINTENB            0x05
#define IOCONA              0x0A
#define IOCONB              0x0B
#define GPPUA               0x0C
#define GPPUB               0x0D

Timer t;
int intCount{0}; /* Interrupt count - used to help debug switch bounces */



/* Write a byte to the I2C connection */
void writeByte(byte address, byte data)
{
  Wire.beginTransmission(MCP_ADDRESS);
  Wire.write(address);
  Wire.write(data);
  Wire.endTransmission();
}

/* Write a word to the MCP device */
void writeWord(byte address, uint16_t data)
{
  Wire.beginTransmission(MCP_ADDRESS);
  Wire.write(address);
  Wire.write(data >> 8);
  Wire.write(data & 0xFF);
  Wire.endTransmission();
}

/* Set the mode of the GPIO pins on the MCP device */
void gpioPinMode(uint16_t mode)
{
  if (mode == INPUT)
  {
    writeWord(IODIR, 0xFFFF);
  }
  else if (mode == OUTPUT)
  {
    writeWord(IODIR, 0x0000);
  }
  else
  {
    writeWord(IODIR, mode);
  }
}

/* Convert bytes to a word */
uint16_t byte2word(byte high_byte, byte low_byte)
{
  return (word)high_byte << 8 | (word)low_byte;
}
/* Read data from a given reigster / address on the MCP device */
uint16_t readAddress(byte address)
{
  byte low_byte = 0;
  byte high_byte = 0;
  Wire.beginTransmission(MCP_ADDRESS);
  Wire.write(address);
  Wire.endTransmission();
  Wire.requestFrom((uint8_t)MCP_ADDRESS, (uint8_t)2);
  low_byte = Wire.read();
  high_byte = Wire.read();
  return byte2word(high_byte, low_byte);
}

/* Check if a bit is set in a byte */
bool isBitSet(int data, int bit)
{
  if (data & (1 << (bit - 1)))
    return true;
  else
    return false;
}

/* Print the content of a register read from the MCP device */
void printRegisterContent(int reg)
{
  int r = readAddress(reg);
  intCount++;
  Serial.printf("Interrupt hits: %d\n", intCount);

  for (int i = 1; i <= NUMBER_OF_SWITCHES; i++)
  {
    if (isBitSet(r, i))
    {
      Serial.printf("*");
    }
    else
    {
      Serial.printf(".");
    }

    if (i == 8)
    {
      Serial.printf("\n");
    }
  }

  Serial.printf("\n__________\n");
}

/* Get the state of the IO ports on the MCP device */
void getIOState()
{
  printRegisterContent(GPIOA);
}

/* Start the MCP device */
void beginMCP()
{
  Wire.begin();
  Wire.setClock(400000UL); // Set I2C frequency to 400kHz

  /* Configure to trigger interrupts when a switch changes from its last known state */
  writeByte(INTCONA, 0x00);
  writeByte(INTCONB, 0x00);

  /* Enable interrupt notification on all switches */
  writeByte(GPINTENA, 0xFF);
  writeByte(GPINTENB, 0xFF);

  /* set configuration data 
    Bit 7 = Mirror interrupt pins so changes on either bank send an interrupt on INTA
    Bit 6 = Disable sequential operation, so we read the IO data from the same place on each read
  */
  writeByte(IOCONA, 0b01100000);
  writeByte(IOCONB, 0b01100000);

  /* Enable the pull up resistors on both ports so we don't have to add resistors to the circuit */
  writeByte(GPPUA, 0xFF);
  writeByte(GPPUB, 0xFF);

  gpioPinMode(INPUT); /* Tell the MCP that all IO are for input */
}

/* Preocess the interrupt request */
ICACHE_RAM_ATTR void doInterupt()
{
  t.after(10, getIOState);
}

void setup()
{
  Serial.begin(115200);
  Serial.println("Starting...");

  beginMCP();


  printRegisterContent(GPIOA);

  pinMode(INTERRUPT_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), doInterupt, FALLING);
}


void loop()
{
  t.update(); /* update the timer so it can execute timed events */
}
