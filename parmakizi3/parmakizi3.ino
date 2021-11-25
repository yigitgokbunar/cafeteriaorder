/*   
 Atakan Aydoğan - Yiğit Gökbunar
 15/11/21 tarihinde başladığımız parmak izi sensörü ile geçiş sistemi yapmaya çalıştığımız kodlar 
 içerir.
 Amaç: 12 saatte 1 kere resetlenmek şartıyla bir parmak izi 1 kere okutulup 12 saat içinde bir daha okunursa dışarıya uyarı vermesi
 NOT: burada daima çalışan son kod vardır.
 17/11/21 tarihinde güncellenmiştir
*/
#include <Adafruit_Fingerprint.h> // Parmak izi için başlatılan kütüphane
#include <LiquidCrystal_I2C.h>    // LCD ekran ve I2C kullanımı için başlatıan kütüphane
LiquidCrystal_I2C lcd(0x27, 16, 2);  // LCD ekranın ölçüleri ve bağlantısı için gereken kod satırı
// For UNO and others without hardware serial, we must use software serial...
// 4. pine parmak izinden çıkan yeşil kablo
// 5. pine parmak izinden çıkan beyaz kablo
SoftwareSerial mySerial(4, 5); // Serial haberleşmenin  bağlı olduğu pin numaraları 
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
void setup(){ 
  lcd.begin(); // LCD ekranın başlatılması
  pinMode(7,OUTPUT);
  digitalWrite(7,HIGH);
  lcd.print("helloo");
  Serial.begin(9600); // Serial ünitenin bant hızını ayarladığımız yer 
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
