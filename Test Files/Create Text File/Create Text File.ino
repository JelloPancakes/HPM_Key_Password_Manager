#include <SD.h>


const int chipSelect = 10;


File myFile;
char UID[5] = "John.txt"; 

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  // wait for Serial Monitor to connect. Needed for native USB port boards only:
  while (!Serial);

  Serial.print("Initializing SD card...");

  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    while (1);
  }


  Serial.println("initialization done.");

  if (SD.exists(UID)) {
    Serial.println(UID);
    Serial.print(" exists.");

  } else {
    Serial.println(UID);
    Serial.print(" doesn't exist.");


  }

  // open a new file and immediately close it:
  Serial.print("Creating ");
  Serial.print(UID);

  myFile = SD.open(UID, FILE_WRITE);

  myFile.close();

  // Check to see if the file exists:
  if (SD.exists(UID)) {
    Serial.println(UID);
    Serial.print(" exists.");

  } else {
    Serial.println(UID);
    Serial.print(" doesn't exist.");
  }

  // delete the file:
  /*Serial.println("Removing example.txt...");
  SD.remove(UID);

  if (SD.exists(UID)) {
    Serial.println("example.txt exists.");

  } else {
    Serial.println("example.txt doesn't exist.");
  }*/

}


void loop() {
  // nothing happens after setup finishes.

}