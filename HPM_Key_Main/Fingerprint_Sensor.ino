
uint8_t getFingerprintEnroll() {
  int p = -1;
  delay(1000);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      display_one_line("Image", "taken");
      break;
    case FINGERPRINT_NOFINGER:
      display_two_line("No finger", " detected");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      display_two_line("Communication", " error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      display_two_line("Imaging", " error");
      break;
    default:
      display_two_line("Unknown", " error");
      break;
    }
  }
  delay(1000);

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      display_two_line("Image", "converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      display_two_line("  Image", " too messy");
      return 0;
    case FINGERPRINT_PACKETRECIEVEERR:
      display_two_line("Communication", " error");
      return 0;
    case FINGERPRINT_FEATUREFAIL:
      display_three_line("Could not find", "fingerprint", " features");
      return 0;
    case FINGERPRINT_INVALIDIMAGE:
      display_three_line("Could not find", "fingerprint", " features");
      return 0;
    default:
      display_two_line("Unknown", " error");
      return 0;
  }
  delay(1000);

  display_two_line("Remove", " finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  p = -1;
  display_two_line(" Place same", "finger again");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      display_one_line("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      display_one_line(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      display_two_line("Communication", " error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      display_two_line("Imaging", " error");
      break;
    default:
      display_two_line("Unknown", " error");
      break;
    }
  }
  delay(1000);

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      display_one_line("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      display_two_line("  Image", " too messy");
      return 0;
    case FINGERPRINT_PACKETRECIEVEERR:
      display_two_line("Communication", " error");
      return 0;
    case FINGERPRINT_FEATUREFAIL:
      display_three_line("Could not find", "fingerprint", " features");
      return 0;
    case FINGERPRINT_INVALIDIMAGE:
      display_three_line("Could not find", "fingerprint", " features");
      return 0;
    default:
      display_two_line("Unknown", " error");
      return 0;
  }

  delay(1000);

  // OK converted!
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    display_one_line("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    display_two_line("Communication", " error");
    return 0;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    display_two_line("Fingerprints", "did not match");
    return 0;
  } else {
    display_two_line("Unknown", " error");
    return 0;
  }

  // Update ID position
  FP_id = readIntFromEEPROM(2);
  FP_id ++;
  writeIntIntoEEPROM(2, FP_id);

  p = finger.storeModel(FP_id);
  if (p == FINGERPRINT_OK) {
    display_one_line("Stored!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    display_two_line("Communication", " error");
    return 0;
  } else if (p == FINGERPRINT_BADLOCATION) {
    display_two_line("Could not store", "in that location");
    return 0;
  } else if (p == FINGERPRINT_FLASHERR) {
    display_two_line("Error writing"," to flash");
    return 0;
  } else {
    display_two_line("Unknown", " error");
    return 0;
  }
  delay(1000);
  return true;
}

// Get fingerprint match
uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  delay(1000);
  switch (p) {
    case FINGERPRINT_OK:
      display_one_line("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      display_two_line("No finger", " detected");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      display_two_line("Communication", " error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      display_two_line("Imaging", " error");
      break;
    default:
      display_two_line("Unknown", " error");
      break;
    }

  // OK success!
  delay(1000);
  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      display_one_line("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      display_two_line("  Image", " too messy");
      return 0;
    case FINGERPRINT_PACKETRECIEVEERR:
      display_two_line("Communication", " error");
      return 0;
    case FINGERPRINT_FEATUREFAIL:
      display_three_line("Could not find", "fingerprint", " features");
      return 0;
    case FINGERPRINT_INVALIDIMAGE:
      display_three_line("Could not find", "fingerprint", " features");
      return 0;
    default:
      display_two_line("Unknown", " error");
      return 0;
  }

  // OK converted!
  delay(1000);
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    display_two_line(" Found a", "  match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    display_two_line("Communication", " error");
    return 0;
  } else if (p == FINGERPRINT_NOTFOUND) {
    display_two_line("Did not", "find match");
    return 0;
  } else {
    display_two_line("Unknown", " error");
    return 0;
  }
  delay(1000);
  return 1;
}
