/*   
 Atakan Aydoğan - Yiğit Gökbunar
 15/11/21 tarihinde başladığımız parmak izi sensörü ile geçiş sistemi yapmaya çalıştığımız kodlar 
 içerir.
 Amaç: 12 saatte 1 kere resetlenmek şartıyla bir parmak izi 1 kere okutulup 12 saat içinde bir daha okunursa dışarıya uyarı vermesi
 NOT: burada daima çalışan son kod vardır.
 10/12/21 tarihinde güncellenmiştir

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
int i = 1;
void setup(){
  int i=0;
  lcd.begin();
  Serial.begin(9600);
  lcd.print("SISTEM ACILDI");
  delay(300);
  lcd.clear();
  lcd.print("SISTEM ACILDI.");
  delay(300);
   lcd.clear();
  lcd.print("SISTEM ACILDI..");
  delay(300);
   lcd.clear();
  lcd.print("SISTEM ACILDI...");
  delay(300);
  lcd.clear();
   lcd.print("SISTEM ACILDI");
  delay(300);
  lcd.clear();
  lcd.print("SISTEM ACILDI.");
  delay(300);
   lcd.clear();
  lcd.print("SISTEM ACILDI..");
  delay(300);
   lcd.clear();
  lcd.print("SISTEM ACILDI...");
  delay(300);
  lcd.clear();
   lcd.print("SISTEM ACILDI");
  delay(300);
  lcd.clear();
  lcd.print("SISTEM ACILDI.");
  delay(300);
   lcd.clear();
  lcd.print("SISTEM ACILDI..");
  delay(300);
   lcd.clear();
  lcd.print("SISTEM ACILDI...");
  delay(300);
  lcd.clear();
  lcd.print("KOSBI  MTAL");
  delay(1000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("ATAKAN ve YIGIT");
  lcd.setCursor(0,1);
  lcd.print("  TESEKKURLER");
  delay(2000);
 lcd.clear();
  Serial.println("KOSBİ ZÜLFÜ MEVLÜT ÇELİK MTAL"); 
  Serial.println("Parmakizi okuma ve kaydetme projesi"); 
  pinMode(7,OUTPUT);
  digitalWrite(7,HIGH);
  finger.begin(57600);
  if (finger.verifyPassword()) {
    lcd.clear();
    lcd.print("SENSOR BAGLANDI :(");
     delay(2000);
    Serial.println("Parmakizi sensörü bağlantı başarılı");} 
    else{
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("SENSOR");
    lcd.setCursor(0,1);
    lcd.print("BAGLANAMADI :(");
    delay(1000);
    lcd.clear();
    Serial.println("Parmakizi sensörü bağlantı başarısız ");
    while (1) { delay(1); }}
  Serial.println(F("Sensör parametreleri okunuyor"));
  finger.getParameters();
  Serial.print(F("Durum: 0x")); Serial.println(finger.status_reg, HEX);
  Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
  Serial.print(F("Kapasite ")); Serial.println(finger.capacity);
  Serial.print(F("Güvenlik Seviyesi: ")); Serial.println(finger.security_level);
  Serial.print(F("Cihz Adresi: ")); Serial.println(finger.device_addr, HEX);
  Serial.print(F("Paket len: ")); Serial.println(finger.packet_len);
  Serial.print(F("Band Hızı: ")); Serial.println(finger.baud_rate);}
uint8_t readnumber(void) {
  uint8_t num = 0;
  while (num == 0) {
    while (! Serial.available());
    num = Serial.parseInt();}
  return num;}
void loop(){ 
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("PARMAKIZI KAYIT");
  lcd.setCursor(0,1);
  lcd.print("ICIN HAZIR");
  Serial.println("Parmakizi kaydedilmeye hazır");
  Serial.println("Kaydetmek için ID yaz");
  id = readnumber();
  if (id == 0) {// ID #0 not allowed, try again!
     return;}
  Serial.print("ID kaydediliyor#");
  Serial.print("ID:");
  Serial.println(id);
  while (!  getFingerprintEnroll() );}
uint8_t getFingerprintEnroll() {
  int p = -1;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("KAYIT ICIN");
  lcd.setCursor(0,1);
  lcd.print("PARMAKIZI OKUTUN");
  Serial.print("Kayıt için parmakizi bekleniyor");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("PARMAKIZI ALINDI");
    delay(1000);
      Serial.println("Parmakizi alındı");
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
      break;}}
  // OK success!
  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("PARMAKIZI");
    lcd.setCursor(0,1);
    lcd.print("CEVRILDI");
    delay(1000);
      Serial.println("Parmakizi çevrildi");
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
      return p;}
  Serial.println("Parmağı kaldırınız");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("PARMAGINIZI");
  lcd.setCursor(0,1);
  lcd.print("KALDIRINIZ");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();}
  Serial.print("ID "); Serial.println(id);
  p = -1;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("PARMAKIZINIZI");
  lcd.setCursor(0,1);
  lcd.print("TEKRAR OKUTUN");
  delay(500);
  Serial.println("Parmakizinizi tekrar okutun");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
    lcd.clear();
      Serial.println("Parmakizi alındı");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Bağlantı hatası");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Parmkaizi hatası");
      break;
    default:
      Serial.println("Bilinmeyen hata");
      break;}}
  // OK success!
  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Parmakizi çevrildi");
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
      return p;}
  // OK converted!
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("MODEL");
  lcd.setCursor(0,1);
  lcd.print("OLUSRUTULDU");
  delay(1000);
  Serial.print("Model oluşturuldu");  Serial.println(id);
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("PARMAKIZINIZ");
  lcd.setCursor(0,1);
  lcd.print("ESLESTI");
  delay(2000);
    Serial.println("Parmakizi eşleşti");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Eşleşme hatası");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("PARMAKZINIZ");
  lcd.setCursor(0,1);
  lcd.print("ESLESEMEDI :(");
  delay(2000);
    Serial.println("Parmakizi eşleşemedi");
    return p;
  } else {
    Serial.println("Bilinmeyen hata");
    return p;}
  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("PARMAKIZINIZ");
  lcd.setCursor(0,1);
  lcd.print("KAYDEDILDI :)");
  delay(2000);
    Serial.println("Kaydedildi");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("İletişim hatası");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Arduinoya kaydedilemedi");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println(" Belleğe yazma hatası");
    return p;
  } else {
    Serial.println("Bilinmeyen hata");
    return p;}
  return true;}
