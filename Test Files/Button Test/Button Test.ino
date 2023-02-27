// constants won't change. They're used here to set pin numbers:
// Leonardo setup with digital pins
const int joystick_up = 6;  // physical pin (27)
const int joystick_down = 4;  // physical pin (25)
const int joystick_left = 12;  // physical pin (26)
const int joystick_right = 9;  // physical pin (29)
const int joystick_select = 8;  // physical pin (28)

// Setup with physical pins
/*
const int joystick_up = 27; 
const int joystick_down = 25; 
const int joystick_left = 26;  
const int joystick_right = 29;  
const int joystick_select = 28;
*/

int joystick_up_state = 0;  // variable for reading the pushbutton status
int joystick_down_state = 0;
int joystick_left_state = 0;
int joystick_right_state = 0;
int joystick_select_state = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("Testing begins");

  // initialize the pushbutton pin as an input:
  pinMode(joystick_up, INPUT);
  pinMode(joystick_down, INPUT);
  pinMode(joystick_left, INPUT);
  pinMode(joystick_right, INPUT);
  pinMode(joystick_select, INPUT);
}

void loop() {
  // read the state of the pushbutton value:
  joystick_up_state = digitalRead(joystick_up);
  joystick_down_state = digitalRead(joystick_down);
  joystick_left_state = digitalRead(joystick_left);
  joystick_right_state = digitalRead(joystick_right);
  joystick_select_state = digitalRead(joystick_select);

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (joystick_up_state == HIGH) {
    Serial.println("up");
  } 
  if (joystick_down_state == HIGH) {
    Serial.println("down");
  }
  if (joystick_left_state == HIGH) {
    Serial.println("left");
  }
  if (joystick_right_state == HIGH) {
    Serial.println("right");
  }
  if (joystick_select_state == HIGH) {
    Serial.println("select");
  }
}
