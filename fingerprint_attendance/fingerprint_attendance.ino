/*12 Kasım 2021 Ben Yiğit Gökbunar. Bugün yemekhane için parmak izi ile yoklama sistemi projesine başladım, buraya gerekli notları ve gelişmeleri alacağım.
Öncelikle hibernate modu eklemeyi planladım fakat Arduino üzerinde bazı değişiklikler yapmak gerekliydi ve bu uğraşa raşmen istediğim şey değildi kesinlikle
bu yüzden eklemeyeceğim bu yüzden bu notu görüp hala hibernate sorusu sorulursa cevaplamayacağım. 324. Satır değişiklik yaptım FINGERPRINT_SEC ekledim*/
#include <Wire.h> //kütüphaneler eklendi
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_Fingerprint.h> 

#define switchReg 4    //Registration button ayrıca define ile pin tanımlamış olduk
#define switchAtten 5  //Attendance button  
#define ledBlue 6
#define ledRed 7 


// uncomment for uno 
//#include <SoftwareSerial.h>  
//SoftwareSerial mySerial(2, 3);  

// uncomment for Mega
#define mySerial Serial1

LiquidCrystal_I2C lcd(0x27,16,2);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial); 
 
uint8_t id;
File myFile;
int Status = 0;
const int chipSelect = 53;

void setup() {
  // İlk açılışta çalışacak kod buraya yazılmalı:
  
  pinMode(switchAtten, INPUT);
  pinMode(switchReg, INPUT);
  pinMode(ledBlue, OUTPUT); 
  pinMode(ledRed, OUTPUT); 
  pinMode(chipSelect,OUTPUT);
  

  lcd.init();
  lcd.backlight();
  lcd.clear();
  
  Serial.begin(9600);

  while (!Serial);  // For Yun/Leo/Micro/Zero/...  
  delay(100);  
  Serial.println("\n\nAdafruit finger detect test");  
  
  finger.begin(57600);  
    
    
  if (finger.verifyPassword()) {  
    Serial.println("Parmak izi sensörü bulundu!");  
  } else {  
    Serial.println("Parmak izi sensörü bulunamadı :(");  
    while (1) { delay(1); }  
  }   


  Serial.print("SD kart tanımlanıyor...");

  if (!SD.begin(chipSelect)) {
    Serial.println("Kart basarısız, veya yok");
    while (1);
  }
  Serial.println("Kart tanımlandı.");

}

void loop() {
  // Ana kod burada olmak zorunda:
  Status = 0;
  lcd.setCursor(1,0);
  lcd.print("Parmak izi sistemi");
  lcd.setCursor(3,1);
  lcd.print("KOSBI MTAL");
  
  
  int buttonStateP1 = 0; //Yoklama butonu
  int buttonStateP2 = 0; //Kayıt butonu
    
  digitalWrite(ledBlue,LOW);
  digitalWrite(ledRed,LOW);
  
  
  buttonStateP1 = digitalRead(switchReg); // Listen for the start button to be pressed
  buttonStateP2 = digitalRead(switchAtten);
  
  
  //Eğer kayıt tuşuna basıldıysa
  if (buttonStateP1 == HIGH ){
    
    digitalWrite(ledBlue,HIGH);
    digitalWrite(ledRed,HIGH);

    lcd.setCursor(3,0);
    lcd.print("Kayıt süreci");

    finger.getTemplateCount();  
    Serial.print("Sensor contains "); 
    Serial.print(finger.templateCount); 
    Serial.println(" templates");  
    id = finger.templateCount + 1;
    Serial.println(id); 
    getFingerprintEnroll();
    


    buttonStateP1 = 0;
    
  //if the attendance button has been pressed  
  }else if(buttonStateP2 == HIGH){

    uint8_t userId = -1;
    lcd.print("Yoklama işlemi");
    digitalWrite(ledBlue,LOW);
    digitalWrite(ledRed,LOW);

    Serial.println("Geçerli parmak için bekleniyor...");
    for(int i=0;i<1;i++)
    {
      Status = 0;
      lcd.clear();
      lcd.print("Place Finger");
      delay(2000);

      userId = getFingerprintIDez();
      if(userId >=1 && Status != 1)
      {
        digitalWrite(ledBlue,HIGH);
        lcd.clear();
        lcd.setCursor(3,0);
        lcd.print("Present");
        delay(2000);

        // Storing data in SD card

        String ID = String(userId);
 
        myFile = SD.open("test.txt", FILE_WRITE);

        // if the file opened okay, write to it:
        if (myFile) {
          
          myFile.print("User ID ");
          myFile.println(ID);
          // close the file:
          myFile.close();
          Serial.println("done.");
          } else {
          // Eğer dosya açılmadıysa, bu yazıyı yazdırır:
          Serial.println("test.txt'i açarken sorun");
          } 
      }
        
    }   
     
    
    buttonStateP2 = 0;
  }

}
  

// #################### Fingerprint Enroll Function ############################
uint8_t getFingerprintEnroll() {
 
  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #");
  Serial.println(id);
  lcd.clear();
  lcd.setCursor(4,0);
  lcd.print("Place the"); 
  lcd.setCursor(5,1);
  lcd.print("finger "); 
  
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  Serial.println("Remove finger");
  lcd.clear();
  lcd.print(" Remove finger "); 
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");
  lcd.clear();
  lcd.setCursor(3,0);
  lcd.print("Place same"); 
  lcd.setCursor(3,1);
  lcd.print("finger again"); 
  
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  // OK converted!
  Serial.print("Creating model for #");  Serial.println(id);
  
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
  
  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Stored!"); 
    lcd.setCursor(0,1);
    lcd.print("ID ");lcd.print(id); 
    delay(3000);
    
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Bağlantı hatası");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Bu konuma kaydedilemez");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Flash'a yazarken hata");
    return p;
  }
    else if(px == FINGERPRINT_SEC) {
      digitalWrite(ledRed, HIGH);
      Serial.println("2. Kez giremezsin");
      return px;
    }
    }
    else {
    Serial.println("Bilinmeyen hata");
    return p;
  }
  lcd.clear();   
}

// ##################  Fingerprint Search   ###########################

int getFingerprintIDez() {
    
    uint8_t p = finger.getImage();
    if (p != FINGERPRINT_OK){
      lcd.clear();
      lcd.setCursor(3,0);
      lcd.print("No Finger ");
      lcd.setCursor(3,1);
      lcd.print("found");
      delay(2000);
      Status = 1;
      return -1;  
    }
        

    p = finger.image2Tz();
    if (p != FINGERPRINT_OK){
      lcd.clear();
      lcd.setCursor(3,0);
      lcd.print("No Finger ");
      lcd.setCursor(3,1);
      lcd.print("found");
      delay(2000);      
      Status = 1;
      return -1;
    }

    p = finger.fingerFastSearch();
    if (p != FINGERPRINT_OK){
      Serial.println("Error Finger");
      digitalWrite(ledBlue,LOW);
      digitalWrite(ledRed,HIGH);
      lcd.clear();
      lcd.setCursor(3,0);
      lcd.print("No Finger ");
      lcd.setCursor(3,1);
      lcd.print("found");
      delay(2000);
      Status = 1;
      return -1;
      
    }
    px = finger.finger.SecondTry();
   if (px = FINGERPRINT_SEC){
    Serial.println("Sıradan çık!")
    digitalWrite(ledRed, HIGH);
    digitalWrite(ledBlue, LOW);
    lcd.clear();
    lcd.setCursor(3,0);
    lcd.print("Sıradan çık");
    lcd.setCursor(3,1);
    lcd.print(finger.fingerID);
    delay(2000);
    Status = 1;
    return -1
 
  Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID; 
}
