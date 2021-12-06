/*   
 Atakan Aydoğan - Yiğit Gökbunar
 15/11/21 tarihinde başladığımız parmak izi sensörü ile geçiş sistemi yapmaya çalıştığımız kodlar 
 içerir.
 Amaç: 12 saatte 1 kere resetlenmek şartıyla bir parmak izi 1 kere okutulup 12 saat içinde bir daha okunursa dışarıya uyarı vermesi
 NOT: burada daima çalışan son kod vardır.
 17/11/21 tarihinde güncellenmiştir
 NOT: 06/12/21 tarihinde tekrar güncellendi. Şuanda kayıt işlemini yapabiliyor fakat bazı sorunlar devam ediyor hâlâ.

FINGERPRINT_OK : İmaj alındı
FINGERPRINT_NOFINGER : Parmakizi bulunamadı
FINGERPRINT_PACKETRECIEVEERR : Bağlantı hatası
FINGERPRINT_IMAGEFAIL : İmaj hatası
 default: kalan hatalar için
FINGERPRINT_IMAGEMESS : Parmakizi sensörünün parmak okutulan kısmında kirlilik olması
FINGERPRINT_FEATUREFAIL : Parmakizi özellikleri bulunamadı
FINGERPRINT_INVALIDIMAGE : Parmakizi özellikleri bulunamadı
p = finger.fingerSearch(); : Parmakizi arama
FINGERPRINT_NOTFOUND : Eşleşme bulunamadı
finger.fingerID : Eşlemşme bulunduğu zaman ID yazar
*/

#include <Adafruit_Fingerprint.h> // Parmak izi için başlatılan kütüphane
#include <LiquidCrystal_I2C.h>    // LCD ekran ve I2C kullanımı için başlatıan kütüphane
LiquidCrystal_I2C lcd(0x27, 16, 2);  // LCD ekranın ölçüleri ve bağlantısı için gereken kod satırı
// For UNO and others without hardware serial, we must use software serial...
// 2. pine parmak izinden çıkan yeşil kablo
// 3. pine parmak izinden çıkan beyaz kablo
SoftwareSerial mySerial(2, 3); // Serial haberleşmenin  bağlı olduğu pin numaraları 
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
uint8_t id;

void setup()
{
  Serial.begin(9600);
  finger.begin(57600);
  if (finger.verifyPassword()) {
    Serial.println("Parmakizi sensörü bulundu");
  } else {
    Serial.println("Parmakizi sensörü bulunamadı :(");
    while (1) { delay(1); }
  }

  Serial.println(F("Sensör parametreleri okunuyor"));
  finger.getParameters();
  Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
  Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
  Serial.print(F("Capacity: ")); Serial.println(finger.capacity);
  Serial.print(F("Security level: ")); Serial.println(finger.security_level);
  Serial.print(F("Device address: ")); Serial.println(finger.device_addr, HEX);
  Serial.print(F("Packet len: ")); Serial.println(finger.packet_len);
  Serial.print(F("Baud rate: ")); Serial.println(finger.baud_rate);
}

uint8_t readnumber(void) {
  uint8_t num = 0;

  while (num == 0) {
    while (! Serial.available());
    num = Serial.parseInt();
  }
  return num;
}

void loop()                     // run over and over again
{
  Serial.println("Parmakizi kaydedilmeye hazır");
  Serial.println("Kaydetmek istedigin ID yaz");
  id = readnumber();
  if (id == 0) {// ID #0 not allowed, try again!
     return;
  }
  Serial.print("ID kaydediliyor#");
  Serial.println(id);

  while (!  getFingerprintEnroll() );
}

uint8_t getFingerprintEnroll() {

  int p = -1;
  Serial.print("Kayıt için parmakizi bekleniyor"); Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Imaj alındı");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("İletişim hatası");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaj hatası");
      break;
    default:
      Serial.println("Parmak izi bununamadı");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Imaj çevrildi");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Parmakizi sensörü kirli");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("İletişim hatasi");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Parmakizi özellikleri bulunamadı");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Parmakizi özellikleri bulunamadı");
      return p;
    default:
      Serial.println("Bilinmeyen hata");
      return p;
  }

  Serial.println("Parmağı kaldırınız");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("Parmakizinizi okutun");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Imaj alındı");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Bağlantı hatası");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaj hatası");
      break;
    default:
      Serial.println("bilinmeyen hata");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Imaj çevrildi");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Parmakizi sensörünün parmak okutulan kısmında kirlilik olması");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Bağlantı hatası");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Parmakizi özellikleri bulunamadı");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Parmakizi özellikleri bulunamadı");
      return p;
    default:
      Serial.println("Bilinmeyen hata");
      return p;
  }

  // OK converted!
  Serial.print("Model oluşturuldu");  Serial.println(id);

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Parmakizi eşleşti");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Eşleşme hatası");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Parmakizi eşleşemedi");
    return p;
  } else {
    Serial.println("Bilinmeyen hata");
    return p;
  }

  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("İletişim hatası");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Arduinoya kaydedilemedi");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Belleğe yazma hatası");
    return p;
  } else {
    Serial.println("Bilinmeyen hata");
    return p;
  }

  return true;
}
  
