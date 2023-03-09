#include <SPI.h>
#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiAvrI2c.h"
#include <Keyboard.h>
#include "SdFat.h"
#include <Adafruit_Fingerprint.h>
#include <EEPROM.h>

// Define hardware serial using RX 1, TX 0 pins
#define mySerial Serial1
#define RX_PIN    0     
#define TX_PIN    1        
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

SdFat SD;
#define SD_CS_PIN 10
File myFile;

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
SSD1306AsciiAvrI2c display;

// Leonardo setup with digital pins
uint8_t joystick_up = 6;  
uint8_t joystick_down = 4;  
uint8_t joystick_left = 9;  
uint8_t joystick_right = 12; 
uint8_t joystick_select = 8;
uint8_t mode_select = 50;

// Variables for debouncing joystick
bool last_up_state = HIGH;
bool last_down_state = HIGH;
bool last_left_state = HIGH;
bool last_right_state = HIGH;
bool last_select_state = HIGH;
bool last_mode_state = HIGH;

// 0-no action, 4-up, 6-down, 12-left, 9-right, 8-select
uint8_t joystick_action;

// Cursor
uint8_t horiz_pos;
uint8_t vert_pos;
uint8_t list_pos;

// Pin
int pin;
int check_pin;

// Fingerprint database ID
uint8_t FP_id;

// Controls menu screen
/*0/10 pincode
1/11 fingerprint
3/settings
4/username
5/password
6/change pin
7/change pin confirmed
8/enroll fingerprint
13/ connecting
14/ connected
15/ info sent
16/ transfer done*/
uint8_t menu_state;

// Text file name
char text_file[] = "Robert.txt";

// List of domains
char domains[15][15] = {};
uint8_t num_domains;

void setup() {
  Serial.begin(9600);
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // Initialize fingerprint sensor
  finger.begin(57600);
  if (finger.verifyPassword()) {
    Serial.println(F("Found fingerprint sensor!"));
  }

  // Disable jtag
  MCUCR = (1<<JTD);
  MCUCR = (1<<JTD);

  // Start display
  display.begin(&Adafruit128x64, SCREEN_ADDRESS);
  display.setFont(Adafruit5x7);
  display.displayRemap(true); //flip screen

  // Initialize SD card reader
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println(F("initialization failed!"));
    while (1);
  }

  // Initialize keyboard output
  Keyboard.begin();

  // initialize the pushbutton pin as an input:
  pinMode(joystick_up, INPUT);
  pinMode(joystick_down, INPUT);
  pinMode(joystick_left, INPUT);
  pinMode(joystick_right, INPUT);
  pinMode(joystick_select, INPUT);
  DDRE &= 0xFB; // pinMode PE2 INPUT

  // Set pin to EEPROM value
  pin = readIntFromEEPROM(0);
  FP_id = readIntFromEEPROM(2);

  // Reset variables
  check_pin = 1111;
  menu_state = 0;
  joystick_action = 1;
  horiz_pos = 0;
  vert_pos = 0;
  list_pos = 0;
  num_domains = 0;

  // Fill domain array with empty strings, and load the text file to the domains list
  for (int i = 0; i < 15; i++) {
    domains[i][0] = '\0';
  }
  sd_load_domains();

  // show title screen
  display_one_line("  HPM KEY");
  delay(2000);

  // Check the switch to see if we're starting in programming or keyboard mode
  if (!!(PINE & 0x04)){
    menu_state = 0;
    last_mode_state = HIGH;
  } else{
    menu_state = 10;
    last_mode_state = LOW;
  }
}

void loop() {
  // Joystick 
  debounce(joystick_up, last_up_state);
  debounce(joystick_down, last_down_state);
  debounce(joystick_left, last_left_state);
  debounce(joystick_right, last_right_state);
  debounce(joystick_select, last_select_state);
  debounce(mode_select, last_mode_state);
  
  // Check for serial data being received
  if (Serial.available() != 0){
    char input = Serial.read();

    if (menu_state == 13){
      if (input == 'A'){
        menu_state = 14;
        joystick_action = 1;
      }
    } else if (menu_state == 15){
      if (input == '*'){ // end character, close serial and move to next menu
        menu_state = 16;
        SD.remove(text_file); // delete old file and rename temp file
        if (!myFile.rename(text_file)){
          Serial.println(F("Rename failed"));
        }
        myFile.close();
        joystick_action = 1;
      } else{
        myFile.print(input);
      }
    }
  }

  // Update display when user input has occurred
  if (joystick_action != 0){
    // If mode select is switched
    if (joystick_action == 50){
      if (last_mode_state == HIGH){
        menu_state = 0;
      } else if (last_mode_state == LOW){
        menu_state = 10;
      }
      check_pin = 1111; // reset pincode
      horiz_pos = 0; // reset cursor on pincode
    }

    // Adjust cursor position based on joystick
    int joystick_vert = 0;
    if (joystick_action == 12){ // right
      horiz_pos = ++horiz_pos % 4;
    } else if (joystick_action == 9){ // left
      if(horiz_pos == 0){
        horiz_pos = 3;
      } else {
        horiz_pos--;
      }
      if(menu_state == 4 || menu_state == 5){
        menu_state = 3;
      }
    } else if (joystick_action == 6){ // up
        joystick_vert = 1;
    } else if (joystick_action == 4){ // down
      joystick_vert = -1;
    } else if (joystick_action == 8){ // select code
      if(menu_state == 0 || menu_state == 10){
        if (check_pin == pin){
          menu_state++;
        }
      } else if(menu_state == 3){
        if(vert_pos == 0){
          menu_state = 4;
        } else if(vert_pos == 1){
          menu_state = 6;
        } else{
          menu_state = 8;
        }
      } else if (menu_state == 1 || menu_state == 11){
        menu_state += 2;
      } else if(menu_state == 4){
        print_up_combo(1);
        menu_state++;
      } else if(menu_state == 5){
        print_up_combo(2);
        menu_state--;
      } else if (menu_state == 6){
        pin = check_pin;
        writeIntIntoEEPROM(0, check_pin);
        menu_state = 7;
        horiz_pos = 0; // reset cursor on pincode
      } else if (menu_state == 8){
        menu_state = 3;
      }
    }

    // Adjust check pin based on joystick actions
    if(joystick_vert != 0 && (menu_state == 0 || menu_state == 10 || menu_state == 6)){
      uint8_t u = check_pin%10;
      uint8_t t = (check_pin%100)/10;
      uint8_t h = (check_pin%1000)/100;
      uint8_t th = check_pin/1000;

      if (horiz_pos == 0){
        th += joystick_vert;
        pin_wrap(th);
      } else if (horiz_pos == 1){
        h += joystick_vert;
        pin_wrap(h);
      } else if (horiz_pos == 2){
        t += joystick_vert;
        pin_wrap(t);
      } else {
        u += joystick_vert;
        pin_wrap(u);
      }
      check_pin = th*1000+h*100+t*10+u;
    }

    // Scrolling with static list
    if (menu_state == 3){
      if (vert_pos - joystick_vert >= 0 && vert_pos - joystick_vert <= 2){
        vert_pos -= joystick_vert;
      }
    }

    // Adjust list position based on joystick
    if (menu_state == 4 || menu_state == 5){
      if(joystick_vert == 1){
        if(list_pos > 0){
          list_pos--;
        } else if (vert_pos > 0){
          vert_pos--;
        }
      } else if(joystick_vert == -1){
        if(list_pos + 3 < num_domains){
          list_pos++;
        } else if (vert_pos < 2){
          vert_pos++;
        }
      }
    }

    char *someItems[]={"\0","\0","\0", "\0"}; // list array
    
    // Display code
    if (menu_state == 0 || menu_state == 10){ // pincode
      display_pincode(" Enter Pin", check_pin, horiz_pos);
    } else if(menu_state == 1 || menu_state == 11){ //
      display_two_line("Put finger", "on scanner");
      while (getFingerprintID() == 0);
      display_two_line("Fingerprint", "Success!");
    } else if (menu_state == 3){
      someItems[0] = "UP Combos";
      someItems[1] = "Edit PIN";
      someItems[2] = "Add Print";
      display_list("Settings", someItems, vert_pos);
    } else if (menu_state == 4 || menu_state == 5){ // password display
      // Update list for UP combos
      for (int i = 0; i < 3; i++) {
        someItems[i] = domains[i+list_pos];
      }
      if (menu_state == 4){
        display_list("Output UN", someItems, vert_pos);
      } else {
        display_list("Output PW", someItems, vert_pos);
      }
    } else if (menu_state == 6){ // Set pincode
        display_pincode("  Set Pin", check_pin, horiz_pos);
    } else if (menu_state == 7){
        display_one_line("New Pin Set");
        menu_state = 3;
    } else if (menu_state == 8){
      display_two_line("Put finger", "on scanner");
      while (getFingerprintEnroll() == 0);
      display_two_line("Fingerprint", "Success!");
    }
      else if (menu_state == 13){
        display_one_line("Connecting");
        Serial.write(F("REQ\n"));
    } else if (menu_state == 14 || menu_state == 15){
        display_one_line("Connected");
        if (menu_state == 14){
          SD_to_PC();
          PC_to_SD();
          menu_state++;
        } 
    } else if (menu_state == 16){
      display_one_line("Transfer done");
    }
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
void debounce(uint8_t &pin_number, bool &lastButtonState){
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