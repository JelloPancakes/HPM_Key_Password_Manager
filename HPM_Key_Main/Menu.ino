// Wrap pincode between 1-9
void pin_wrap(uint8_t& num){
  if (num == 10){
    num = 1;
  } else if (num == 0){
    num = 9;
  }
}

// Read joystick input only on changing states
void debounce(const uint8_t &pin_number, bool &lastButtonState){
  bool reading = LOW;
  if (pin_number == 50){
    reading = !!(PINE & 0x04);
  } else{
    reading = digitalRead(pin_number);
  }

  if (reading != lastButtonState) {
    if(reading == LOW){
      joystick_action = pin_number;
    } else if (pin_number == 50){
      joystick_action = pin_number;
    }
  }
  lastButtonState = reading;
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