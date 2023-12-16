


//#define DHTTYPE DHT11 // DHT 11

#include <RTClib.h>
#include <Wire.h>
RTC_DS1307 rtc;
char t[32]; 
#include <LiquidCrystal.h>
#include <Stepper.h>

volatile unsigned char* port_f = (unsigned char*) 0x31; 
volatile unsigned char* ddr_f  = (unsigned char*) 0x30;
volatile unsigned char* pin_f  = (unsigned char*) 0x2F;

volatile unsigned char* ddr_k  = (unsigned char*) 0x107;
volatile unsigned char* pin_k  = (unsigned char*) 0x106; 
volatile unsigned char* port_k  = (unsigned char*) 0x108; 

volatile unsigned char* port_b = (unsigned char*)0x25;
volatile unsigned char* ddr_b  = (unsigned char*) 0x24;
volatile unsigned char* pin_b  = (unsigned char*) 0x23; 

volatile unsigned char* ddr_c  = (unsigned char*) 0x27;
volatile unsigned char* pin_c  = (unsigned char*) 0x26; 
volatile unsigned char* port_c  = (unsigned char*) 0x28; 

volatile unsigned char* ddr_h  = (unsigned char*) 0x101;
volatile unsigned char* pin_h  = (unsigned char*) 0x100; 
volatile unsigned char* port_h  = (unsigned char*) 0x102; 

volatile unsigned char* ddr_l  = (unsigned char*) 0x10A;
volatile unsigned char* pin_l  = (unsigned char*) 0x109; 
volatile unsigned char* port_l  = (unsigned char*) 0x10B; 

volatile unsigned char* ddr_g  = (unsigned char*) 0x33;
volatile unsigned char* pin_g  = (unsigned char*) 0x32; 
volatile unsigned char* port_g  = (unsigned char*) 0x34; 

volatile unsigned char* ddr_e  = (unsigned char*) 0x2D;
volatile unsigned char* pin_e  = (unsigned char*) 0x2C; 
volatile unsigned char* port_e  = (unsigned char*) 0x2E; 

volatile unsigned char *myTCCR1A  = 0x80;
volatile unsigned char *myTCCR1B  = 0x81;
volatile unsigned char *myTCCR1C  = 0x82;
volatile unsigned char *myTIMSK1  = 0x6f;
volatile unsigned char *myTIFR1   = 0x50;
volatile unsigned int  *myTCNT1   = 0x84;

const int SPR = 2048; 
const int RPM = 17; 
int currentTicks = 0;

const int RS = 11, EN = 12, D4 = 7, D5 = 8, D6 = 9, D7 = 10;
LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);
Stepper stepMotor(SPR, 2, 4, 3, 5);
//DHT11 test(36);
//DHT dht(36, DHTTYPE);
String state = "";
char important = LOW;
int temperature = 40;
int humidity = 15;


void setup() {
  lcd.begin(16, 2); // set up number of columns and rows
  //dht.begin();
  stepMotor.setSpeed(RPM);
  Serial.begin(9600);
  Serial.begin(9600);
  Wire.begin();
  rtc.begin();
  rtc.adjust(DateTime(F(__DATE__),F(__TIME__)));
  
  *ddr_b |= 0b00001111;
  *ddr_c |= 0b10101000;
  *ddr_h &= 0b11110111;
  *ddr_g &= 0b11111101;
  *ddr_l &= 0b01011111;
  pinMode(A0, INPUT); // water level sensor
  //*ddr_f &= 0b11111110;
  *port_h |= 0b00001000;
  *port_l |= 0b10100000;
  *port_g |= 0b00000010;
} 

void loop() {
  DateTime now = rtc.now();
  sprintf(t, "%02d:%02d:%02d %02d/%02d/%02d", now.hour(), now.minute(), now.second(), now.day(), now.month(), now.year());  
  //int waterLevel = analogRead(A0);
  /*float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  delay(10000);*/
  int waterLevel = analogRead(A0);
  //int waterLevel = *pin_f;
  if(*pin_h & 0b00001000){
    if(important == LOW){
      important = HIGH;
  }
    else{
      important = LOW;
    }
  }
  else{
    important = LOW;
  }
  if(important == HIGH){
  Serial.print(F("Date/Time: "));
  Serial.println(t);
  lcd.clear();
  state = "ON";
  *port_b = 0b00000001;
  delay(5000);
  Serial.print(F("Date/Time: "));
  Serial.println(t);
  while(important==HIGH & temperature < 32){
  lcd.setCursor(0,0);
  lcd.print("Temperature: "); 
  lcd.print(temperature);
  lcd.print("F");
  lcd.setCursor(0,1);
  lcd.print("Humidiity: ");
  lcd.print(humidity);
  lcd.print("%");
  state = "IDLE";
  delay(60000);
  lcd.clear();
  }
  if(temperature >= 32 & waterLevel>=10){
    while(temperature >= 32 & waterLevel>=10){
    lcd.clear();
    waterLevel = analogRead(A0);
    state = "RUNNING";
    lcd.print("Temperature: "); 
    lcd.print(temperature);
    lcd.print("F");
    lcd.setCursor(0,1);
    lcd.print("Humidiity: ");
    lcd.print(humidity);
    lcd.print("%");
    *port_c = 0b00101000;
    *port_b = 0b00000100;

    if(waterLevel <= 10){
      state = "ERROR";
      *port_c = 0b00000000;
      *port_b = 0b00000010;
      lcd.clear();
      lcd.print("ERROR");
      lcd.setCursor(0,1); 
      lcd.print("Water too low");
      Serial.print(F("Date/Time: "));
      Serial.println(t);
    }
    delay(10000);
  }
  }
    else if(waterLevel <= 10){
      state = "ERROR";
      *port_c = 0b00000000;
      *port_b = 0b00000010;
      lcd.clear();
      lcd.print("ERROR");
      lcd.setCursor(0,1); 
      lcd.print("Water too low");
      Serial.println("Water level is too low");
      Serial.print(F("Date/Time: "));
      Serial.println(t);
    }
    else{
    state = "IDLE";
    *port_c = 0b00000000;
    *port_b = 0b00000001;
    while(temperature < 32 & waterLevel >= 10){
    lcd.setCursor(0,0);
    lcd.print("Temperature: "); 
    lcd.print(temperature);
    lcd.print("F");
    lcd.setCursor(0,1);
    lcd.print("Humidiity: ");
    lcd.print(humidity);
    lcd.print("%");
    state = "IDLE";
    delay(60000);
    lcd.clear();
  }
  }
  while(state=="ERROR"){
    waterLevel = analogRead(A0);
    if(waterLevel >= 10){
    if(*pin_l & 0b00100000){
      state = "IDLE";
    *port_c = 0b00000000;
    *port_b = 0b00000001;
    while(temperature <= 32){
    lcd.setCursor(0,0);
    lcd.print("Temperature: "); 
    lcd.print(temperature);
    lcd.print("F");
    lcd.setCursor(0,1);
    lcd.print("Humidiity: ");
    lcd.print(humidity);
    lcd.print("%");
    state = "IDLE";
    delay(60000);
    lcd.clear();
    *port_b = 0b00000001;
    }
    }
  }
  }
  Serial.print(F("Date/Time: "));
      Serial.println(t);
  
  }
  else{
    state = "OFF";
    *port_c = 0b00000000;
    lcd.clear();
    lcd.print(state);
    *port_b = 0b00001000;
  }
  if(state != "ERROR"){
  if(*pin_g & 0b00000010){
    Serial.print(F("Date/Time: "));
      Serial.println(t);
    stepMotor.step(SPR/4);
    delay(500);
    Serial.println("counterclockwise");
  }
  if(*pin_l & 0b10000000){
    Serial.print(F("Date/Time: "));
      Serial.println(t);
    stepMotor.step(-(SPR/4));
    delay(500);
    Serial.println("clockwise");
  }
  }
}
/*ISR(TIMER1_OVF_vect)
{
  // Stop the Timer
  *myTCCR1B &=0xF8;
  // Load the Count
  *myTCNT1 =  (unsigned int) (65535 -  (unsigned long) (currentTicks));
  // Start the Timer
  *myTCCR1B |=   0x05;
  // if it's not the STOP amount
  if(currentTicks != 65535)
  {
    // XOR to toggle PB6
    *portB ^= 0x40;
  }
}*/
