#include <SPI.h>
#include <SD.h>

File root;
File myFile;

// Type into serial monitor to use
// Commands - add, delete, read
// Text files are Angelica.txt, Robert.txt

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.print("Initializing SD card...");

  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");
  Serial.println("Directory");
  root = SD.open("/");

  printDirectory(root, 0);

  Serial.println("done!");
  Serial.println();
}

void loop() {
  // Wait for input from user
  while (Serial.available() == 0) {
  }
  String input = Serial.readString();
  input.trim();

  // read files
  if (input.compareTo("read") == 0){
    Serial.println("Which user to read");
    while (Serial.available() == 0) {
    }
    input = Serial.readString();
    input.trim();

    myFile = SD.open(input);

    if (myFile) {
      Serial.println(input);
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

  // delete files
  if (input.compareTo("delete") == 0){
    Serial.println("Which file to delete");
    while (Serial.available() == 0) {
    }
    input = Serial.readString();
    input.trim();

    Serial.println(input);
    SD.remove(input);

    // Reprint directory
    Serial.println("Directory");
    root = SD.open("/");
    printDirectory(root, 0);
    Serial.println("done!");
  }

  // add files
  if (input.compareTo("add") == 0){
    Serial.println("Which user to add");
    while (Serial.available() == 0) {
    }
    input = Serial.readString();
    input.trim();

    // open the file. note that only one file can be open at a time,
    // so you have to close this one before opening another.
    if (input.compareTo("Robert") == 0){
      myFile = SD.open("Robert.txt", FILE_WRITE);
      // if the file opened okay, write to it:
      if (myFile) {
        Serial.print("Writing to Robert.txt...");
        myFile.println("Amazon;ashley87@gmail.com;grbel;");
        myFile.println("Ebay;nivlongsd@storyday.biz;jDyLrEFT54,R(cPr5DAzYmEGzMno:`V7;");
        myFile.println("ErinMills;keshawn45@gmail.com;superunsafe32;");
        myFile.println("Facebook;sawoud@sawoud.com;Password123;");
        myFile.println("G-mail;cornell33@hotmail.com;supersafe;");
        myFile.println("Insta;nivlongsd@pickuplanet.com;disispas;");
        myFile.println("Sorting;of;tsble;");
        myFile.println("Spotify;fausto17@gmail.com;mushymush32;");
        myFile.println("Work Email;mittie48@hotmail.com;testing#;");

        // close the file:
        myFile.close();
        Serial.println("done.");
      } else {
        // if the file didn't open, print an error:
        Serial.println("error opening test.txt");
      }
    }
    else if (input.compareTo("PIN") == 0){
      myFile = SD.open("Angelica.txt", FILE_WRITE);
      // if the file opened okay, write to it:
      if (myFile) {
        Serial.print("Writing to Angelica.txt...");
        myFile.println("G-mail;cornell33@hotmail.com;supersafe;");
        myFile.println("Insta;forrest78@gmail.com;hi;");
        myFile.println("Sorting;of;tsble;");
        myFile.println("Facebook;chaya.durgan@hotmail.com;KRYPTOGSNG;");
        myFile.println("Amazon;barklake12@gmail.com;bubbleteadanatic142#*;");
        // close the file:
        myFile.close();
        Serial.println("done.");
      } else {
        // if the file didn't open, print an error:
        Serial.println("error opening test.txt");
      }
    }

    // Reprint directory
    Serial.println("Directory");
    root = SD.open("/");
    printDirectory(root, 0);
    Serial.println("done!");
  }

}

void printDirectory(File dir, int numTabs) {
  while (true) {

    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}



