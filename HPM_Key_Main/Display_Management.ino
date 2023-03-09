// Display list of 3 items with a cursor to indicate selected item
void display_list(const char *title, const char *items[], uint8_t selected) {
  display.set2X(); 
  display.print(F(" "));
  display.println(title);
  uint8_t linecount = 0;
  while (*items[0]) {
    if (linecount == selected){
      display.print(F(">"));
    } else{
      display.print(F(" "));
    }
    display.println(*items++);
    linecount++;
  }
}

void display_pincode(const char *title, int& check_pin, uint8_t& horiz_pos){
  display.set2X();
  display.println(title);

  // Top Cursor
  display.print(F("   "));
  for (int i = 0; i < horiz_pos; i++) {
    display.print(F(" "));
  }
  display.print(F("^"));

  // Numbers
  display.println();
  display.print(F("   "));
  display.print(check_pin);

  // Bottom Cursor
  display.println();
  display.print(F("   "));
  for (int i = 0; i < horiz_pos; i++) {
    display.print(F(" "));
  }
  display.print(F("v"));
}

// Displays 1 line in middle of screen
void display_one_line(const char *line){
  display.set2X(); 
  display.println();
  display.println(line);
}

// Display 2 lines in middle of screen
void display_two_line(const char *line1, const char *line2){
  display.set2X(); 
  display.println();
  display.println(line1);
  display.println(line2);
}

// Display 3 lines in middle of screen
void display_three_line(const char *line1, const char *line2, const char *line3){
  display.set2X(); 
  display.println();
  display.println(line1);
  display.println(line2);
  display.println(line3);
}



