void debounce(){
  read_input(joystick_up, last_up_state);
  read_input(joystick_down, last_down_state);
  read_input(joystick_left, last_left_state);
  read_input(joystick_right, last_right_state);
  read_input(joystick_select, last_select_state);
  read_input(mode_select, last_mode_state);

  if ( (millis() - lastDebounceTime) > debounceDelay && joystick_action != 0) {
    Serial.println(joystick_action);
    lastDebounceTime = millis();
  } else{
    Serial.println(millis() - lastDebounceTime);
    joystick_action = 0;
  }
}

// Wrap pincode between 1-9
void pin_wrap(uint8_t& num){
  if (num == 10){
    num = 1;
  } else if (num == 0){
    num = 9;
  }
}

// Read joystick input only on changing states
void read_input(const uint8_t &pin_number, bool &lastButtonState){
  bool reading = LOW;
  if (pin_number == 50){
    reading = !!(PINE & 0x04);
  } else{
    reading = digitalRead(pin_number);
  }

  if (reading != lastButtonState) {
    if(reading == LOW && joystick_action != joystick_select){
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