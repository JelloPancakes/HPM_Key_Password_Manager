// Display list of 3 items with a cursor to indicate selected item
void display_list(const char *title, const char *items[], int selected) {
  display.set2X(); 
  display.print(F(" "));
  display.println(title);
  int linecount = 0;
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

void display_pincode(int& check_pin, int& horiz_pos){
  display.set2X();
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

// Show title screen for 1 second
void display_title_screen(void){
  display.set2X();
  display.println();
  display.println("  HPM KEY");
  delay(2000);
}

void display_connect(const char *title){
  display.set2X(); 
  display.println();
  display.println(title);
  display.println("   to PC");
}

void display_transfer_done(){
  display.set2X(); 
  display.println();
  display.println("Transfer");
  display.println("Completed");
}



