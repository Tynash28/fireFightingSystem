/* Batanai Rujeko Sigauke 
 * fire prevention system
 * scan for i2c devices (lcd)
 * check gsm lib rx and tx (10,11)
*/

//Project libraries 
#include <LiquidCrystal_I2C.h>
#include <Sim800l.h>
#include <SoftwareSerial.h>

// PIN CONFIGURATION FOR THE HARDWARE
#define mq2 A3
#define buzzer 6 
#define waterValve 8
#define foamValve 9
#define pump 7

//gsm module global parameters
#define Tx 11
#define Rx 10
char* text;
char* number = "+263776197932";
Sim800l Sim800l; 

// lcd object construction
LiquidCrystal_I2C lcd(0x27,20,4);

// other program global Parameters
int gasThres3 = 400;
int gasThres2 = 200;
int gasThres1 = 100;
int gasPv = 0;
long prevTime = 0;
long prevTimeBuzzer = 0;
int interval = 1000;
bool buzzerTone1 = false;
bool buzzerTone2 = false;
bool buzzerTone3 = false;
bool buzzerState = LOW;

void setup(){
  // hardware and libraries setup
  // routine runs once on arduino start-up
  lcd.init();     
  lcd.backlight();
  lcd.clear();
  lcd.print(">System Initializing");
  Sim800l.begin();
  delay(500);
  pinMode(buzzer, OUTPUT);
  pinMode(foamValve, OUTPUT);
  pinMode(pump, OUTPUT);
  lcd.clear();
  lcd.print(">Done Initializing");
  delay(2000);
}


void loop(){
  showPv(); // this function show the realtime gas levels on the lcd
  if(analogRead(mq2) > gasThres1 && analogRead(mq2) < gasThres2){
    digitalWrite(pump,LOW);
    digitalWrite(waterValve,LOW);
    buzzerTone1 = true;
    //send notifacation after every 10minutes
    if(millis() - prevTime > 10000UL){
      text="Gas level at " + analogRead(mq2);
    	Sim800l.sendSms(number,text);
      prevTime = millis();
    }
  }else if (analogRead(mq2) > gasThres2 && analogRead(mq2) < gasThres3){
    buzzerTone2 = true;
    //send notifacation after every 10minutes
    if(millis() - prevTime > 10000UL){
      text="Gas level at " + analogRead(mq2);
    	Sim800l.sendSms(number,text);
      prevTime = millis();
    }
    digitalWrite(pump,HIGH);
    digitalWrite(waterValve,HIGH);
  }else if (analogRead(mq2) > gasThres3){
    buzzerTone3 = true;
    //send notifacation after every 10minutes
    if(millis() - prevTime > 10000UL){
      text="Gas level at " + analogRead(mq2);
    	Sim800l.sendSms(number,text);
      prevTime = millis();
      digitalWrite(pump,HIGH);
      digitalWrite(waterValve,HIGH);
    }
  }else{}
  if(buzzerTone1){
    alarm(1000,450);
    
  }else if (buzzerTone2){
    alarm(500,300);
    
  }else if(buzzerTone3) {
    alarm(250,200);
  }
  resetBuzzer();  
}

void alarm(int buzzerDelay,int buzzerFreq){
  if(millis() - prevTimeBuzzer > buzzerDelay){
    if(buzzerState == LOW){
      buzzerState = HIGH;
      tone(buzzer,buzzerFreq);
    }else{
      buzzerState=LOW;
      noTone(buzzer);
    }
    prevTimeBuzzer = millis();
  }
}

void showPv(){
  lcd.clear();
  gasPv = analogRead(mq2); // get gas level
  lcd.print("Gas Value " + gasPv);
  gasPv = map(gasPv,0,1023,0,100);
  lcd.setCursor(2,0);
  lcd.print(gasPv + "%");
}

void resetBuzzer(){
  if(analogRead(mq2)<gasThres3){
    buzzerTone3 = false;
  }
  if (analogRead(mq2)<gasThres2){
    buzzerTone2 = false;
  }
  if (analogRead(mq2)<gasThres1){
    buzzerTone1 = false;
  }
}
