#include <SPI.h>
#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiAvrI2c.h"
#include <Keyboard.h>
#include "SdFat.h"
#include <Adafruit_Fingerprint.h>
#include <EEPROM.h>

// Define hardware serial (Serial1) using RX 1, TX 0 pins
#define mySerial Serial1
#define RX_PIN    0     
#define TX_PIN    1        
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

// Define SD card, SD card pin, text file
SdFat SD;
#define SD_CS_PIN 10
File myFile;

// Define OLED display variables
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
SSD1306AsciiAvrI2c display;

// Leonardo setup with digital pins
const uint8_t joystick_up = 6;  
const uint8_t joystick_down = 4;  
const uint8_t joystick_left = 9;  
const uint8_t joystick_right = 12; 
const uint8_t joystick_select = 8;
const uint8_t mode_select = 50;

// Variables for debouncing joystick
bool last_up_state = HIGH;
bool last_down_state = HIGH;
bool last_left_state = HIGH;
bool last_right_state = HIGH;
bool last_select_state = HIGH;
bool last_mode_state = HIGH;

// 0-no action, 4-up, 6-down, 12-left, 9-right, 8-select
uint8_t joystick_action;
uint8_t carryover_action; // for specific menu updates

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
const char text_file[] = "Robert.txt";

// List of domains
char domains[15][15] = {};
uint8_t num_domains;

void setup() {
  // Start open Serial port through USB
  Serial.begin(9600);  

  // Initialize fingerprint sensor
  finger.begin(57600);
  finger.verifyPassword();

  // Disable jtag
  MCUCR = (1<<JTD);
  MCUCR = (1<<JTD);

  // Start display
  display.begin(&Adafruit128x64, SCREEN_ADDRESS);
  display.setFont(Adafruit5x7);
  display.displayRemap(true); //flip screen

  // Initialize SD card reader
  SD.begin(SD_CS_PIN);

  // Initialize keyboard output
  Keyboard.begin();

  // initialize the pushbutton pin as an input:
  pinMode(joystick_up, INPUT);
  pinMode(joystick_down, INPUT);
  pinMode(joystick_left, INPUT);
  pinMode(joystick_right, INPUT);
  pinMode(joystick_select, INPUT);
  DDRE &= 0xFB; // pinMode PE2 INPUT for the switch

  // Read pin from EEPROM and current open fingerprint ID
  pin = readIntFromEEPROM(0);
  FP_id = readIntFromEEPROM(2);

  // Initialize variables
  check_pin = 1111;
  menu_state = 0;
  joystick_action = 1;
  carryover_action = 0;
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
  // Debouncing for joystick
  debounce(joystick_up, last_up_state);
  debounce(joystick_down, last_down_state);
  debounce(joystick_left, last_left_state);
  debounce(joystick_right, last_right_state);
  debounce(joystick_select, last_select_state);
  debounce(mode_select, last_mode_state);
  
  // Check for serial data being received
  if (Serial.available() != 0){
    char input = Serial.read();
    if (menu_state == 13){ // Connecting state
      if (input == '+'){ // Wait for input from PC to move to the next menu state
        Serial.print(F("+"));
        menu_state++;
        joystick_action = 1; // Update display
      }
    } else if (menu_state == 15){ // Waiting for PC to confirm data has been received
      if (input == '-'){ // If PC has sent ACK, move to next menu state
        Serial.print(F("-"));
        menu_state++;
        joystick_action = 1; // Update display
      }
    } else if(menu_state == 16){ // Reading data from PC
        if (input == '*'){ // end character, close serial and move to next menu
          SD.remove(text_file); // delete old file and rename temp file
          myFile.rename(text_file);
          myFile.close();
          menu_state++;
          joystick_action = 1; // Update display
        } else{
          myFile.print(input); // if not end character, print to file
        }
    }
  }

  // Update display when user input has occurred
  if (joystick_action != 0 || carryover_action != 0){
    carryover_action = 0; //carry over action reset
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
      if(menu_state == 4){
        vert_pos = 0;
        menu_state = 3;
      }
    } else if (joystick_action == 6){ // up
      joystick_vert = 1;
    } else if (joystick_action == 4){ // down
      joystick_vert = -1;
    } else if (joystick_action == 8){ // select code
      if(menu_state == 0 || menu_state == 10){ // Pin menu
        if (check_pin == pin){
          menu_state++;
        }
      } else if(menu_state == 3){ // Main menu
        if(vert_pos == 0){
          menu_state = 4;
        } else if(vert_pos == 1){
          horiz_pos = 0; // reset cursor on pincode
          menu_state = 6;
        } else{
          menu_state = 8;
        }
      } else if(menu_state == 4){ // Username menu
        print_up_combo(1);
        menu_state++;
      } else if(menu_state == 5){// Password menu
        print_up_combo(2);
        menu_state--;
      } else if (menu_state == 6){ // Edit pin menu
        pin = check_pin;
        writeIntIntoEEPROM(0, check_pin);
        menu_state = 7;
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
    if (menu_state == 4){
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
    if (menu_state == 0 || menu_state == 10){ // Pincode menu
      display_pincode(" Enter Pin", check_pin, horiz_pos);
    } else if(menu_state == 1 || menu_state == 11){ //
      display_two_line("Put finger", "on scanner");
        while (getFingerprintID() == 0);
        display_two_line("Fingerprint", "Success!");
        Serial.print(F("1")); // For PC to check if fingerprint success has happened
        delay(1000);
        menu_state += 2;
        carryover_action = 1; //Update screen
    } else if (menu_state == 3){ // Main menu
      someItems[0] = "UP Combos";
      someItems[1] = "Edit PIN";
      someItems[2] = "Add Print";
      display_list("Settings", someItems, vert_pos);
    } else if (menu_state == 4 || menu_state == 5){ // Username and Password list
      // Update list for UP combos
      for (int i = 0; i < 3; i++) {
        someItems[i] = domains[i+list_pos];
      }
      if (menu_state == 4){
        display_list("Output UN", someItems, vert_pos);
      } else {
        display_list("Output PW", someItems, vert_pos);
      }
    } else if (menu_state == 6){ // Set pincode menu
        display_pincode("  Set Pin", check_pin, horiz_pos);
    } else if (menu_state == 7){ // Pin set
        display_one_line("New Pin Set");
        delay(1000);
        menu_state = 3;
        carryover_action = 1; //Update screen
    } else if (menu_state == 8){ //Enrolling new fingerprint
      display_two_line("Put finger", "on scanner");
      while (getFingerprintEnroll() == 0);
      display_two_line("Fingerprint", "Success!");
        delay(1000);
        menu_state = 3;
        carryover_action = 1; //Update screen
    }
      else if (menu_state == 13){
        display_one_line("Connecting");
    } else if (menu_state == 14 || menu_state == 15){
        display_one_line("Connected");
        if (menu_state == 14){
          SD_to_PC();
          PC_to_SD();
          menu_state++;
        } 
    } else if (menu_state == 16){
      display_two_line("Receiving", "data");
        
    } else if (menu_state == 17){
      display_two_line("Transfer", "done");
    }
    joystick_action = 0; // clear joystick action
  }
}

