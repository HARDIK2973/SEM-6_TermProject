//#include <LiquidCrystal.h>
#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;

//const int rs = 6, en = 7, d4 = 8, d5 = 9, d6 = 12, d7 = 13;
//LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int irPin1=A2;
int irPin2=A1;
int count=0;
int buzz = A3;

//Defining pins for rotary encoder
const int RotaryCLK = 2; //CLK pin on the rotary encoder
const int RotaryDT = 3; //DT pin on the rotary encoder
const int RotarySW = 4; //SW pin on the rotary encoder (Button function)


int total_seat ;

int RotateCounter = 0; //counts the rotation clicks
bool rotated = true; //info of the rotation
bool ButtonPressed =true ; //info of the 

boolean state1 = true;
boolean state2 = true;
boolean insideState = false;
boolean outsideIr=false;
boolean isPeopleExiting=false;

int i=1;

//Statuses
int CLKNow;
int CLKPrevious;
int DTNow;
int DTPrevious;

// Timers
float TimeNow1;
float TimeNow2;//Statuses




void setup() {
 mySoftwareSerial.begin(9600);
  Serial.begin(115200);
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();

   if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    
    while(true){
      delay(0); // Code to compatible with ESP8266 watch dog.
    }
  }
 // myDFPlayer.volume(15);  //Set volume value. From 0 to 30
myDFPlayer.volume(30);  //Set volume value. From 0 to 30
  myDFPlayer.play(6);//Hi there, we're happy to have you on board.
  
//   lcd.begin(16, 2);
   lcd.clear();
    lcd.print("WELCOME!!");
   delay(4000);
   lcd.clear();
   
   lcd.print("Set Limit: ");
  // myDFPlayer.volume(15);  //Set volume value. From 0 to 30
   myDFPlayer.volume(30);
    myDFPlayer.play(4);//Hello and Please Set the limit of total Seats.

  

     //setting up pins  
   pinMode(2, INPUT_PULLUP);
   pinMode(3, INPUT_PULLUP);
   pinMode(4, INPUT_PULLUP);
pinMode(irPin1, INPUT);
pinMode(irPin2, INPUT);
pinMode(buzz,OUTPUT);

  //Store states
  CLKPrevious = digitalRead(RotaryCLK);
  DTPrevious = digitalRead(RotaryDT);
    
  // Atach a CHANGE interrupt to PinB and exectute the update function when this change occurs.
attachInterrupt(digitalPinToInterrupt(RotaryCLK), rotate, CHANGE);
  attachInterrupt(digitalPinToInterrupt(RotarySW), buttonPressed, FALLING); //either falling or rising but never "change".
  
  TimeNow1 = millis(); //Start timer 1  
}

void loop() {
 printLCD();
 ButtonPressed = digitalRead(RotarySW);

   // If the switch is pressed (LOW), print message
  if (ButtonPressed == LOW) {
      Serial.println("Switch pressed");
     total_seat = RotateCounter;
      lcd.clear();
      lcd.print("You have set ");
      lcd.setCursor(0,1);
      lcd.print("Limit: ");
      lcd.setCursor(7,1);
      lcd.print(total_seat);
      delay(2000);
      
      
  } 
  
  if (!digitalRead(irPin1) && i==1 && state1){
     outsideIr=true;
     delay(100);
     i++;
     state1 = false;
  }

   if (!digitalRead(irPin2) && i==2 &&   state2){
     outsideIr=true;
     delay(100);
     i = 1 ;
     count++;
      digitalWrite(buzz,HIGH);
     delay(100);
     if(count>total_seat){
      lcd.clear();
      lcd.print("Sorry!");
      lcd.setCursor(0,1);
      lcd.print("Seats are FULL");
    //  myDFPlayer.volume(15);  //Set volume value. From 0 to 30
        myDFPlayer.volume(30);  //Set volume value. From 0 to 30
 myDFPlayer.play(1);// * 1.>>> Sorry! Seats are Packed.
     }
     else{
      lcd.clear();
      lcd.print("Person in bus: ");
      lcd.setCursor(14,0);
     lcd.print(count);
     
     state2 = false;
    // myDFPlayer.volume(15);  //Set volume value. From 0 to 30
       myDFPlayer.volume(30);  //Set volume value. From 0 to 30
  myDFPlayer.play(5);  //Welcome, Please find a seat and enjoy the ride.
     }
    
     
  }
  else{
    digitalWrite(buzz,LOW);
  }

   if (!digitalRead(irPin2) && i==1 && state2 ){
     outsideIr=true;
     delay(100);
     i = 2 ;
     state2 = false;
  }

  if (!digitalRead(irPin1) && i==2 && state1 ){
     outsideIr=true;
     delay(100);
     count--;
     digitalWrite(buzz,HIGH);
     delay(100);
     if(count<0){
      count = 0;
     }
     
     lcd.clear();
     lcd.print("Person in bus: ");
       lcd.setCursor(14,0);
     lcd.print(count);
    // myDFPlayer.volume(15);  //Set volume value. From 0 to 30
      myDFPlayer.volume(30);  //Set volume value. From 0 to 30
  myDFPlayer.play(8);  //Play the first mp3 "Farewell and have a pleasant rest of your day!"
     i = 1;
     
     state1 = false;
  }  
   else{
    digitalWrite(buzz,LOW);
  }

    if (digitalRead(irPin1)){
     state1 = true;
    }

     if (digitalRead(irPin2)){
     state2 = true;
    }
}
void buttonPressed()
{  
  //This timer is a "software debounce". It is not the most effective solution, but it works
  TimeNow2 = millis();
  if(TimeNow2 - TimeNow1 > 500)
  {    
    ButtonPressed = false;    
    total_seat = RotateCounter;
    
  }
  TimeNow1 = millis();  //"reset" timer; the next 500 ms is counted from this moment
}

void rotate()
{
 
   

  CLKNow = digitalRead(RotaryCLK); //Read the state of the CLK pin

  // If last and current state of CLK are different, then a pulse occurred  
    if (CLKNow != CLKPrevious  && CLKNow == 1)
    {
    // If the DT state is different than the CLK state then
    // the encoder is rotating CCW so increase
      if (digitalRead(RotaryDT) != CLKNow) 
      {        
      RotateCounter++;
      }
      else
      {        
      RotateCounter--;
                   
      }       
    }   

  CLKPrevious = CLKNow;  // Store last CLK state
  rotated = true;
 
}


void printLCD()
{
    if(rotated == true) //refresh the CLK
    {
      lcd.setCursor(12,0); 
      lcd.print(RotateCounter);
      rotated = false;
    }
    
}



/*
 * 1.>>> Sorry! Seats are Packed.
2.>>> Hello Guys! Welcome to my youtube channel.
3.>>> Thanks dor coming.
4.>>> Welcome, Please find a seat and enjoy the ride.
5.>>> Hi there, we're happy to have you on board.
6.>>> "Farewell and have a pleasant rest of your day!"
7.>>> "Thank you and have a great day!"
8.>>> Hello and Please Set the limit of total Seats.
 * 
 * 
 */
/*1.
 * 2.Agency
 * 3.roz
 * 4.basti ka hast
 * 5.Astronut in the ocean
 * 6.deadwood
 * 7.devil eyes
 * 8.IDGAF
 * 9.JO bheji thi dua
 * 10.nam dang nam
 * 11.aurora
 * 12.trippistan
 * 13.mama
 */
