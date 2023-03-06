void sd_load_domains(){
  myFile = SD.open(text_file, FILE_READ);

  if (myFile) {
    // read from the file until there's nothing else in it:
    int col = 0;
    int row = 0;
    int semicol_count = 0;
    while (myFile.available()) {
      // Read domain names into array character by character
      char letter = myFile.read();
      if (semicol_count == 0){ // stop reading after first semicolon
        if (!(letter == ';')){
          domains[row][col] = letter;
          col++;
        } else { // at semicolon, end string and go to next row
          domains[row][col] = '\0';
          col = 0;
          row ++;
          semicol_count ++;
          num_domains ++;
        }
      } else if(letter == '\n' && semicol_count != 0){ // start reading again after new line char
          semicol_count = 0;
      }
    }
  }
  myFile.close();
}

void print_up_combo(int up_type){
  int line_pos = list_pos + vert_pos;

  myFile = SD.open(text_file, FILE_READ);

  if (myFile) {
    // read from the file until there's nothing else in it:
    int newline_count = 0;
    int semicol_count = 0;
    int col = 0;

    while (myFile.available()) {
      char letter = myFile.read();
      if(letter == '\n'){ // Count the number of new lines until the up combo selected
          newline_count ++;
      } else if (newline_count >= line_pos){ // we've reached the line we want
        if (letter == ';'){
          semicol_count++;
          if (semicol_count > up_type){ // If we've passed the user/pass, break out of loop
            break;
          }
        } else if (semicol_count == up_type){ // 1 semicolon is username, 2 is password
          Keyboard.write(letter);
          delay(20);
        }
      }
    }
  }
  myFile.close();
}

// Read from SD and print serially
void SD_to_PC(){
  myFile = SD.open(text_file, FILE_READ);
  if (myFile) {
    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
}

// Read characters serially and write to SD
void PC_to_SD(){
  myFile = SD.open("temp.txt", FILE_WRITE);
  Serial.println("test");
  if (Serial.available() != 0) {
    char input = Serial.read();
    Serial.println(input);
    if (input == '|'){ // end character, close serial and move to next menu
      menu_state = 6;
      SD.remove(text_file); // delete old file and rename temp file
      if (!myFile.rename(text_file)){
        Serial.println(F("Rename failed"));
      }
      myFile.close();
    } else{
      Serial.write(input);
    }
  }
}

