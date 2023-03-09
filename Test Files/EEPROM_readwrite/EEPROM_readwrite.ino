#include <EEPROM.h>
void setup() {
  Serial.begin(9600);
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  // put your setup code here, to run once:
  writeIntIntoEEPROM(2, 0);
  Serial.println(readIntFromEEPROM(0));
  Serial.println(readIntFromEEPROM(2));

}

void loop() {
  // put your main code here, to run repeatedly:

}

// Writes a 2 byte int into EEPROM for permanent storage
void writeIntIntoEEPROM(int address, int number)
{ 
  byte byte1 = number >> 8;
  byte byte2 = number & 0xFF;
  EEPROM.update(address, byte1);
  EEPROM.update(address + 1, byte2);
}

// Reads an int value from EEPROM
int readIntFromEEPROM(int address)
{
  byte byte1 = EEPROM.read(address);
  byte byte2 = EEPROM.read(address + 1);
  return (byte1 << 8) + byte2;
}