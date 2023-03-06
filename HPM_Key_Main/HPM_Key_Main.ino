#include <SPI.h>
#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiAvrI2c.h"
#include "SparkFun_AS108M_Arduino_Library.h"
#include <Keyboard.h>
#include "SdFat.h"
SdFat SD;
#define SD_CS_PIN 10
File myFile;

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
SSD1306AsciiAvrI2c display;

// Leonardo setup with digital pins
int joystick_up = 6;  
int joystick_down = 4;  
int joystick_left = 9;  
int joystick_right = 12; 
int joystick_select = 8;
int mode_select = 50;

// Variables for debouncing joystick
int last_up_state = HIGH;
int last_down_state = HIGH;
int last_left_state = HIGH;
int last_right_state = HIGH;
int last_select_state = HIGH;
int last_mode_state = HIGH;

// 0-no action, 4-up, 6-down, 12-left, 9-right, 8-select
int joystick_action;

// Cursor
int horiz_pos;
int vert_pos;
int list_pos;
int send_once;

// Pin
int pin = 1234;
int check_pin;

// Controls menu screen
int menu_state;

// Text file name
char text_file[] = "Robert.txt";

// List of domains
char domains[15][15] = {};
int num_domains;

void setup() {
  Serial.begin(9600);
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // Disable jtag
  MCUCR = (1<<JTD);
  MCUCR = (1<<JTD);

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

  // Reset variables
  check_pin = 1111;
  menu_state = 0;
  joystick_action = 1;
  horiz_pos = 0;
  vert_pos = 0;
  list_pos = 0;
  num_domains = 0;
  send_once = 0;

  // Fill domain array with empty strings
  for (int i = 0; i < 15; i++) {
    domains[i][0] = '\0';
  }
  sd_load_domains();

  // show title screen
  display_title_screen();

  int val = !!(PINE & 0x04);
  if (val == 1){
    menu_state = 0;
    last_mode_state = HIGH;
  } else{
    menu_state = 4;
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
  

  if (menu_state == 4 && Serial.available() != 0){
      char input = Serial.read();
      if (input == 'A'){
        menu_state = 5;
        joystick_action = 1;
      }
  }

  // Update display when user input has occurred
  if (joystick_action != 0){
    // If mode select is switched
    if (joystick_action == 50){
      if (last_mode_state == HIGH){
        menu_state = 0;
      } else if (last_mode_state == LOW){
        menu_state = 4;
        send_once = 0;
      }
    }

    // Adjust cursor position based on joystick
    int joystick_vert = 0;
    if (joystick_action == 12){ // right
      horiz_pos = ++horiz_pos % 4;
    } else if (joystick_action == 9){ // left
      horiz_pos--;
      if(horiz_pos < 0){
        horiz_pos = 3;
      }
    } else if (joystick_action == 6){ // up
        joystick_vert = 1;
    } else if (joystick_action == 4){ // down
      joystick_vert = -1;
    } else if (joystick_action == 8){ // select
      if(menu_state == 0){
        if (check_pin == pin){
          menu_state = 1;
        }
      } else if(menu_state == 1){
        print_up_combo(1);
        menu_state = 2;
      } else if(menu_state == 2){
        print_up_combo(2);
        menu_state = 1;
      }
    }

    // Adjust check pin based on joystick actions
    if(joystick_vert != 0 && menu_state == 0){
      int u = check_pin%10;
      int t = (check_pin%100)/10;
      int h = (check_pin%1000)/100;
      int th = check_pin/1000;

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

    // Adjust list position based on joystick
    if (menu_state == 1 || menu_state == 2){
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

    // Display code
    display.clear();
    if (menu_state == 0){ // pincode
      display_pincode(check_pin, horiz_pos);
    } else if (menu_state == 1 || menu_state == 2){ // password display
      char *someItems[]={"Item1","Item2","Item3", "\0"};
      // List scrolling
      for (int i = 0; i < 3; i++) {
        someItems[i] = domains[i+list_pos];
      }

      if (menu_state == 1){
        display_list("Output UN", someItems, vert_pos);
      } else {
        display_list("Output PW", someItems, vert_pos);
      }
    } else if (menu_state == 4){
        display_connect("Connecting");
        Serial.write("REQ\n");
    } else if (menu_state == 5){
      display_connect("Connected");
      if (send_once == 0){
        SD_to_PC();
        send_once = 1;
      }
      PC_to_SD();
    } else if (menu_state == 6){
      display_transfer_done();
    }
    joystick_action = 0;
  }
}

// Wrap pincode between 1-9
void pin_wrap(int& num){
  if (num == 10){
    num = 1;
  }
  if (num == 0){
    num = 9;
  }
}

// Read joystick input only on changing states
void debounce(int &pin_number, int &lastButtonState){
  int reading = LOW;
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
