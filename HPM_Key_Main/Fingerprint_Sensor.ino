
uint8_t getFingerprintEnroll() {
  int p = -1;
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
  delay(500);

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      display_one_line("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      display_two_line("  Image", " too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      display_two_line("Communication", " error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      display_three_line("Could not find", "fingerprint", " features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      display_three_line("Could not find", "fingerprint", " features");
      return p;
    default:
      display_two_line("Unknown", " error");
      return p;
  }
  delay(500);

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
  delay(500);

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      display_one_line("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      display_two_line("  Image", " too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      display_two_line("Communication", " error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      display_three_line("Could not find", "fingerprint", " features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      display_three_line("Could not find", "fingerprint", " features");
      return p;
    default:
      display_two_line("Unknown", " error");
      return p;
  }

  // OK converted!
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    display_one_line("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    display_two_line("Communication", " error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    display_two_line("Fingerprints", "did not match");
    return p;
  } else {
    display_two_line("Unknown", " error");
    return p;
  }

  p = finger.storeModel(FP_id);
  if (p == FINGERPRINT_OK) {
    display_one_line("Stored!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    display_two_line("Communication", " error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    display_two_line("Could not store", "in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    display_two_line("Error writing"," to flash");
    return p;
  } else {
    display_two_line("Unknown", " error");
    return p;
  }

  return true;
}
