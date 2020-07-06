#include<LiquidCrystal_I2C.h>
#include<Wire.h>
LiquidCrystal_I2C lcd1(0x27,20,4);
LiquidCrystal_I2C lcd2(0x39,20,4);
#include <SoftwareSerial.h>
SoftwareSerial fingerPrint1(10, 11);
SoftwareSerial fingerPrint2(12, 13);
#include <Adafruit_Fingerprint.h>
#include<DS3231.h>
DS3231  rtc(SDA, SCL);// Init the DS3231 using the hardware interface
//SD card read/write libraries
#include <SPI.h>
#include <SD.h>
File myFile;



//////////////////////////////////
uint8_t id;
Adafruit_Fingerprint finger1 = Adafruit_Fingerprint(&fingerPrint1);
Adafruit_Fingerprint finger2 = Adafruit_Fingerprint(&fingerPrint2);
#define enroll 5
#define del 4
#define up 3
#define scan 19
#define down 2
int relay_pin=6;
int main_user_ID =0;   
String file_name_to_save = "blackBox.txt";
void setup()
{
  Serial.begin(57600);  
  rtc.begin();                //Start the real time clock (remember to set time before this code)
  SD.begin(53);               //Start SD card module with CS pin connected to D53 of the Arduino MEGA. The other pins are SPI pins
myFile = SD.open("blackBox.txt", FILE_WRITE);   //Create a new file on the SD card named before
myFile.print("Door lock system started at ");
myFile.print(rtc.getTimeStr()); myFile.print(" and day ");  myFile.print(rtc.getDateStr());
myFile.println(" ");myFile.println(" ");
myFile.close();
lcd1.init();
lcd2.init();
lcd1.backlight();
lcd2.backlight();
delay(1000);
digitalWrite(relay_pin,HIGH);
pinMode(enroll, INPUT_PULLUP);
pinMode(up, INPUT_PULLUP);
pinMode(down, INPUT_PULLUP);
pinMode(del, INPUT_PULLUP);
pinMode(relay_pin,OUTPUT);
pinMode(scan,INPUT_PULLUP);
lcd1.init();
lcd2.init();
lcd1.backlight();
lcd2.backlight();
lcd1.setCursor(0,0);
lcd1.print("   Biometric ");
lcd2.setCursor(0,0);
lcd2.print("   Biometric ");
lcd1.setCursor(0,1);
lcd1.print("Security System");
lcd2.setCursor(0,1);
lcd2.print("Security System");
delay(2000);
lcd1.clear();
lcd2.clear();
finger1.begin(57600);
finger2.begin(57600);
Serial.begin(9600);
lcd1.clear();
lcd2.clear();
lcd1.print(" Finding Module ");
lcd1.setCursor(0,1);
lcd2.print(" Finding Module ");
lcd2.setCursor(0,1);
delay(2000);
fingerPrint1.listen();
if (finger1.verifyPassword())
{
Serial.println("Found fingerprint sensor1!");
lcd1.clear();
lcd1.print("  Module Found ");
delay(5000);
}
fingerPrint2.listen();
if (finger2.verifyPassword())
{
Serial.println("Found fingerprint sensor2!");
lcd2.clear();
lcd2.print("  Module Found ");
delay(2000);
}
else
{
Serial.println("Did not find fingerprint sensor1 :(");
lcd1.clear();
lcd1.print("Module Not Found");
lcd1.setCursor(0,1);
lcd1.print("Check Connections");
while (1);
}
}

void loop(){
lcd1.clear();
lcd2.clear();
lcd1.setCursor(0,0);
lcd2.setCursor(0,0);
lcd1.print("  Press Match ");
lcd2.print("  Press Match ");
lcd1.setCursor(0,1);
lcd2.setCursor(0,1);
lcd1.print("To Start System");
lcd2.print("To Start System");

if(digitalRead(up)==0 || digitalRead(down)==0)
{
for(int i=0;i<5;i++)
{
 fingerPrint1.listen();
  myFile = SD.open("blackBox.txt", FILE_WRITE);
  myFile.print(rtc.getDateStr()); myFile.print(" -- "); myFile.print(rtc.getTimeStr());
  myFile.println(" -- Attempt of openning in door");
  myFile.close(); 
lcd1.clear();
lcd1.print("Place Finger");
delay(2000);
int result1=getFingerprintIDez1();

if(result1>=0)
{
 Serial.println(result1); 
 myFile = SD.open("blackBox.txt", FILE_WRITE);
 myFile.print(rtc.getDateStr()); myFile.print(" -- "); myFile.print(rtc.getTimeStr());
 myFile.print(" -- IN User match for ID# "); myFile.print(finger1.fingerID);
 myFile.println(" - door open");
 myFile.close();  
lcd1.clear();
lcd1.print(" Allowed ");
lcd1.clear();
lcd1.print("Found ID #");
lcd1.print(finger1.fingerID);
lcd1.setCursor(0,1);
lcd1.print(" Gate Opened ");
digitalWrite(relay_pin,LOW);
delay(5000);
digitalWrite(relay_pin,HIGH);
lcd1.setCursor(0,1);
lcd1.print(" gate closed");

return finger1.fingerID;
}
}
}
 else if(digitalRead(scan)==0)
{
for(int i=0;i<5;i++)
{
 fingerPrint2.listen();
  myFile = SD.open("blackBox.txt", FILE_WRITE);
  myFile.print(rtc.getDateStr()); myFile.print(" -- "); myFile.print(rtc.getTimeStr());
  myFile.println(" -- Attempt of openning out door");
  myFile.close(); 
lcd2.clear();
lcd2.print("Place Finger");
delay(2000);
int result2=getFingerprintIDez2();
if(result2>=0)
{ 
 Serial.println(result2); 
 myFile = SD.open("blackBox.txt", FILE_WRITE);
 myFile.print(rtc.getDateStr()); myFile.print(" -- "); myFile.print(rtc.getTimeStr());
 myFile.print(" -- out User match for ID# "); myFile.print(finger2.fingerID);
 myFile.println(" - out door open");
 myFile.close();  
lcd2.clear();
lcd2.print(" Allowed ");
lcd2.clear();
lcd2.print("Found ID #");
lcd2.print(finger2.fingerID);
lcd2.setCursor(0,1);
lcd2.print(" Gate Opened ");
digitalWrite(relay_pin,LOW);
delay(5000);
digitalWrite(relay_pin,HIGH);
lcd2.setCursor(0,1);
lcd2.print(" gate closed");

return finger2.fingerID;
}
}
}

checkKeys();
delay(2000);
}


  
void checkKeys()
{
if(digitalRead(enroll) == 0)
{
lcd1.clear();
lcd1.print("Main User only");
delay(2000);
EnrollID();
}
else if(digitalRead(del) == 0)
{
lcd1.clear();
lcd1
.print("Main User only");
delay(2000);
deleteID();
  
}
}

void Enroll()
{
int count=1;
lcd1.clear();
lcd1.print("Enroll Finger ");
lcd1.setCursor(0,1);
lcd1.print("Location:");
while(1)
{
lcd1.setCursor(9,1);
lcd1.print(count);
if(digitalRead(up) == 0)
{
count++;
if(count>40)
count=1;
delay(500);
}

else if(digitalRead(down) == 0)
{
count--;
if(count<1)
count=40;
delay(500);
}
else if(digitalRead(del) == 0)
{
id=count;
getFingerprintEnroll();
return;
}

else if(digitalRead(enroll) == 0)
{
return;
}
}
}

void delet()
{
int count=1;
lcd1.clear();
lcd1.print("Delete Finger ");
lcd1.setCursor(0,1);
lcd1.print("Location:");
while(1)
{
lcd1.setCursor(9,1);
lcd1.print(count);
if(digitalRead(up) == 0)
{
count++;
if(count>40)
count=1;
delay(500);
}

else if(digitalRead(down) == 0)
{
count--;
if(count<1)
count=40;
delay(500);
}
else if(digitalRead(del) == 0)
{
id=count;
deleteFingerprint(id);
return;
}

else if(digitalRead(enroll) == 0)
{
return;
}
}
}

uint8_t getFingerprintEnroll()
{
int p = -1;

lcd1.clear();
lcd1.print("Finger ID:");
lcd1.print(id);
lcd1.setCursor(0,1);
lcd1.print("Place Finger");
delay(2000);
while (p != FINGERPRINT_OK)
{
p = finger1.getImage();
switch (p)
{
case FINGERPRINT_OK:
Serial.println("Image taken");
lcd1.clear();
lcd1.print("Image taken");
break;
case FINGERPRINT_NOFINGER:
Serial.println("No Finger");
lcd1.clear();
lcd1.print("No Finger");
break;
case FINGERPRINT_PACKETRECIEVEERR:
Serial.println("Communication error");
lcd1.clear();
lcd1.print("Comm Error");
break;
case FINGERPRINT_IMAGEFAIL:
Serial.println("Imaging error");
lcd1.clear();
lcd1.print("Imaging Error");
break;
default:
Serial.println("Unknown error");
lcd1.clear();
lcd1.print("Unknown Error");
break;
}
}

// OK success!

p = finger1.image2Tz(1);
switch (p) {
case FINGERPRINT_OK:
Serial.println("Image converted");
lcd1.clear();
lcd1.print("Image converted");
break;
case FINGERPRINT_IMAGEMESS:
Serial.println("Image too messy");
lcd1.clear();
lcd1.print("Image too messy");
return p;
case FINGERPRINT_PACKETRECIEVEERR:
Serial.println("Communication error");
lcd1.clear();
lcd1.print("Comm Error");
return p;
case FINGERPRINT_FEATUREFAIL:
Serial.println("Could not find fingerprint features");
lcd1.clear();
lcd1.print("Feature Not Found");
return p;
case FINGERPRINT_INVALIDIMAGE:
Serial.println("Could not find fingerprint features");
lcd1.clear();
lcd1.print("Feature Not Found");
return p;
default:
Serial.println("Unknown error");
lcd1.clear();
lcd1.print("Unknown Error");
return p;
}

Serial.println("Remove finger");
lcd1.clear();
lcd1.print(" Remove Finger ");
delay(2000);
p = 0;
while (p != FINGERPRINT_NOFINGER) {
p = finger1.getImage();
}
Serial.print("ID "); Serial.println(id);
p = -1;
Serial.println("Place same finger again");
lcd1.clear();
lcd1.print(" Place Finger ");
lcd1.setCursor(0,1);
lcd1.print(" Again ");
while (p != FINGERPRINT_OK) {
p = finger1.getImage();
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
return;
}
}

// OK success!


p = finger1.image2Tz(2);
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
Serial.print("Creating model for #"); Serial.println(id);

p = finger1.createModel();


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
p = finger1.storeModel(id);


if (p == FINGERPRINT_OK) {
Serial.println("Stored!");
lcd1.clear();
lcd1.print("Stored!");
delay(2000);
} else if (p == FINGERPRINT_PACKETRECIEVEERR) {
Serial.println("Communication error");
return p;
} else if (p == FINGERPRINT_BADLOCATION) {
Serial.println("Could not store in that location");
return p;
} else if (p == FINGERPRINT_FLASHERR) {
Serial.println("Error writing to flash");
return p;
}
else {
Serial.println("Unknown error");
return p;
}
}

int getFingerprintIDez1()
{
uint8_t p = finger1.getImage();

if (p != FINGERPRINT_OK)
return -1;

p = finger1.image2Tz();
if (p != FINGERPRINT_OK)
return -1;

p = finger1.fingerFastSearch();

if (p != FINGERPRINT_OK)
{
lcd1.clear();
lcd1.print("Finger Not Found");
lcd1.setCursor(0,1);
lcd1.print("Try Later");
delay(2000);
myFile = SD.open("blackBox.txt", FILE_WRITE);   //Create a new file on the SD card named before
myFile.print("Invalid user attempt ");
myFile.print(rtc.getTimeStr()); myFile.print(" and day ");  myFile.print(rtc.getDateStr());
myFile.println(" ");myFile.println(" ");
myFile.close();
return -1; 
}
// found a match!
Serial.print("Found ID #");
lcd1.print("Found ID #");
lcd1.print(finger1.fingerID);
Serial.println(finger1.fingerID);
return finger1.fingerID;
return -1;
}
int getFingerprintIDez2()
{
uint8_t p = finger2.getImage();

if (p != FINGERPRINT_OK)
return -1;

p = finger2.image2Tz();
if (p != FINGERPRINT_OK)
return -1;

p = finger2.fingerFastSearch();

if (p != FINGERPRINT_OK)
{
lcd2.clear();
lcd2.print("Finger Not Found");
lcd2.setCursor(0,1);
lcd2.print("Try Later");
delay(2000);
myFile = SD.open("blackBox.txt", FILE_WRITE);   //Create a new file on the SD card named before
myFile.print("Invalid user attempt ");
myFile.print(rtc.getTimeStr()); myFile.print(" and day ");  myFile.print(rtc.getDateStr());
myFile.println(" ");myFile.println(" ");
myFile.close();
return -1; 
}
// found a match!
Serial.print("Found ID #");
lcd2.print("Found ID #");
lcd2.print(finger2.fingerID);
Serial.println(finger2.fingerID);
return finger2.fingerID;
return -1;
}

uint8_t deleteFingerprint(uint8_t id)
{
uint8_t p = -1;
lcd1.clear();
lcd1.print("Please wait");
p = finger1.deleteModel(id);
if (p == FINGERPRINT_OK)
{
Serial.println("Deleted!");
lcd1.clear();
lcd1.print("Finger Deleted");
lcd1.setCursor(0,1);
lcd1.print("Successfully");
delay(2000);
}

else
{
Serial.print("Something Wrong");
lcd1.clear();
lcd1.print("Something Wrong");
lcd1.setCursor(0,1);
lcd1.print("Try Again Later");
delay(2000);
return p;
}
}
////delete function/////////////
uint8_t deleteID()
{
  uint8_t p = finger1.getImage();
  switch (p)
  {
    case FINGERPRINT_OK:
      break;
    case FINGERPRINT_NOFINGER: return p;
    case FINGERPRINT_PACKETRECIEVEERR: return p;
    case FINGERPRINT_IMAGEFAIL: return p;
    default: return p;
  }
  // OK success!

  p = finger1.image2Tz();
  switch (p)
  {
    case FINGERPRINT_OK: break;
    case FINGERPRINT_IMAGEMESS: return p;
    case FINGERPRINT_PACKETRECIEVEERR: return p;
    case FINGERPRINT_FEATUREFAIL: return p;
    case FINGERPRINT_INVALIDIMAGE: return p;
    default: return p;
  }
  // OK converted!

  p = finger1.fingerFastSearch();
  if (finger1.fingerID == main_user_ID)
  {
Serial.println(finger1.fingerID);  
lcd1.clear();
lcd1.print(" Allowed ");
lcd1.clear();
lcd1.print("Please Wait");
delay(2000);
delet();
myFile = SD.open("blackBox.txt", FILE_WRITE);
myFile.print(rtc.getDateStr()); myFile.print(" -- "); myFile.print(rtc.getTimeStr());
myFile.println(" -- user attempt to delete. Require identification!");
myFile.close();
return;
  }

else if (finger1.fingerID!=main_user_ID)
{ 
  lcd1.setCursor(0,0);
 
  lcd1.print("   invalid user    ");
  
  delay(1500);
  
}
}
////////////////End function///////////////////



///////////enroll function/////////////////////
uint8_t EnrollID()
{
  uint8_t p = finger1.getImage();
  switch (p)
  {
    case FINGERPRINT_OK:
      break;
    case FINGERPRINT_NOFINGER: return p;
    case FINGERPRINT_PACKETRECIEVEERR: return p;
    case FINGERPRINT_IMAGEFAIL: return p;
    default: return p;
  }
  // OK success!

  p = finger1.image2Tz();
  switch (p)
  {
    case FINGERPRINT_OK: break;
    case FINGERPRINT_IMAGEMESS: return p;
    case FINGERPRINT_PACKETRECIEVEERR: return p;
    case FINGERPRINT_FEATUREFAIL: return p;
    case FINGERPRINT_INVALIDIMAGE: return p;
    default: return p;
  }
  // OK converted!

p = finger1.fingerFastSearch();
  if (finger1.fingerID == main_user_ID)
{  
lcd1.clear();
lcd1.print(" Allowed ");
delay(2000);
lcd1.clear();
lcd1.print("Please Wait");
delay(2000);
while(digitalRead(enroll) == 0);
Enroll();
myFile = SD.open("blackBox.txt", FILE_WRITE);
myFile.print(rtc.getDateStr()); myFile.print(" -- "); myFile.print(rtc.getTimeStr());
myFile.println(" -- Add new user attempt. Require identification!");
myFile.close(); 
return;
}


else if (finger1.fingerID!=main_user_ID)
{ 
  lcd1.setCursor(0,0);
 
  lcd1.print("   invalid user    ");
  
  delay(1500);
  
}
}
//////////////////////End code////////////////////////////////
