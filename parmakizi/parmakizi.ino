#include <Adafruit_Fingerprint.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
// For UNO and others without hardware serial, we must use software serial...
// pin #2 is IN from sensor (GREEN wire)
// pin #3 is OUT from arduino  (WHITE wire)
// Set up the serial port to use softwareserial..
SoftwareSerial mySerial(2, 3);

#else
// On Leonardo/M0/etc, others with hardware serial, use hardware serial!
// #0 is green wire, #1 is white
#define mySerial Serial1

#endif


Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

void setup()
{
  lcd.begin(16,2);
  Serial.begin(9600);
  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(100);
  Serial.println("\n\nAdafruit finger detect test");

  // set the data rate for the sensor serial port
  finger.begin(57600);
  delay(5);
  if (finger.verifyPassword()) {
    Serial.println("Parmakizi sensoru bulundu!");
  } else {
    Serial.println("Parmakizi sensoru bulunamadı :(");
    while (1) { delay(1); }
  }

  Serial.println(F("Sensor parametreleri okunuyor"));
  finger.getParameters();
  Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
  Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
  Serial.print(F("Capacity: ")); Serial.println(finger.capacity);
  Serial.print(F("Security level: ")); Serial.println(finger.security_level);
  Serial.print(F("Device address: ")); Serial.println(finger.device_addr, HEX);
  Serial.print(F("Packet len: ")); Serial.println(finger.packet_len);
  Serial.print(F("Baud rate: ")); Serial.println(finger.baud_rate);

  finger.getTemplateCount();

  if (finger.templateCount == 0) {
    Serial.print("Sensor herhangi bir parmakizi bilgisi icermiyor. Lutfen "enroll" ornegine bakın.");
  }
  else {
    Serial.println("Gecerli parmak icin bekleniyor...");
      Serial.print("Sensor iceriyor "); Serial.print(finger.templateCount); Serial.println(" templates");
  }
}

void loop()                     // run over and over again
{
  getFingerprintID();
  delay(50);            //don't ned to run this at full speed.
}

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Imaj alindi");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("Parmak bulunamadi");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Baglantı hatası");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaj hatasi");
      return p;
    default:
      Serial.println("Bilinmeyen hata");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Imaj cevrildi");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Imaj cok puslu");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Iletisim hatasi");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Parmakizi ozellikleri bulunamadı");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Parmakizi ozellikleri bulunamadı");
      return p;
    default:
      Serial.println("Bilinmeyen hata");
      return p;
  }

  // OK converted!
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Parmak eslesmesi bulundu!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Iletisim hatasi");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Eslesme bulunamadı");
    return p;
  } else {
    Serial.println("Bilinmeyen hata");
    return p;
  }

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);

  return finger.fingerID;
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  // found a match!
  Serial.print("ID bulundu #"); Serial.print(finger.fingerID);
  Serial.print(" Guveniyle "); Serial.println(finger.confidence);
  return finger.fingerID;
}
