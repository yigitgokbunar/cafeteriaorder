#include <Adafruit_Fingerprint.h> // Parmak izi için başlatılan kütüphane
#include <LiquidCrystal_I2C.h>    // LCD ekran ve I2C kullanımı için başlatıan kütüphane
LiquidCrystal_I2C lcd(0x27, 16, 2);  // LCD ekranın ölçüleri ve bağlantısı için gereken kod satırı

#if (defined(_AVR) || defined(ESP8266)) && !defined(AVR_ATmega2560_) // Uno dışında kullanılacaksa gereken kod 
// For UNO and others without hardware serial, we must use software serial...
// 4. pine parmak izinden çıkan yeşil kablo
// 5. pine parmak izinden çıkan beyaz kablo
SoftwareSerial mySerial(4, 5); // Serial haberleşmenin  bağlı olduğu pin numaraları
 
#else
// Leonardo'da hardware serial'leri kullan!
//#0 ise yeşil , #1 ise beyaz 
#define mySerial Serial //  Serial haberlşemeye verdiğimiz ad

#endif


Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

void setup(){ 
  lcd.begin(16,2); // LCD ekranın başlatılması
  Serial.begin(9600); // Serial ünitenin bant hızını ayarladığımız yer 
  while (!Serial);  // bazı arduıno modelleri için geçerlidir Yun/Leo/Micro/Zero/...
  delay(100);
  Serial.println("parmak izi test"); 
  // parmak izi sensrü için bant hızı
  finger.begin(57600); // parmak 
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
    Serial.print("Sensor herhangi bir parmakizi bilgisi icermiyor. Lutfen ornegine bakın.");
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
}
