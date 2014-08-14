
#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include <RTClib.h>
/////////////////////////////
//VARS
//the time we give the sensor to calibrate (10-60 secs according to the datasheet)
int calibrationTime = 30;        

//the time when the sensor outputs a low impulse
long unsigned int lowIn;         

//the amount of milliseconds the sensor has to be low 
//before we assume all motion has stopped
long unsigned int pause = 20;  

boolean lockLow = true;
boolean takeLowTime;  
//boolean isAM = true;

//Pin declarations

//int ledPin = 13;
int Vin_pir = 4;
int Gnd_pir = 5;
int Vin_RTC = 17;
int Gnd_RTC = 16;

int pirPin = 3;    //the digital pin connected to the PIR sensor's output
int HourRing = 11; //Data out to hour ring
int MinRing = 12; //Data out to minute/second ring


int DefaultBrightness = 30;  //default global brightness

//NEOPIXEL DECLARATION  - h- hour ring, m- minute ring
#define PINh      HourRing
#define PINm      MinRing 

#define N_LEDS_h 24
#define N_LEDS_m 60
//Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_LEDS, PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip_h = Adafruit_NeoPixel(N_LEDS_h, PINh);
Adafruit_NeoPixel strip_m = Adafruit_NeoPixel(N_LEDS_m, PINm);

//Specific to RTC
RTC_DS1307 rtc;

/////////////////////////////
//SETUP
void setup(){

  //Configure and set Pins for Vin and gnd
  pinMode(Vin_pir, OUTPUT);
  pinMode(Gnd_pir, OUTPUT);
  pinMode(Vin_RTC, OUTPUT);
  pinMode(Gnd_RTC, OUTPUT);

  digitalWrite(Vin_pir, HIGH);
  digitalWrite(Gnd_pir, LOW);
  digitalWrite(Vin_RTC, HIGH);
  digitalWrite(Gnd_RTC, LOW);

  //Configure IO except the data inputs to neopixel rings 
  pinMode(pirPin, INPUT);
  // Use the pull-up
  digitalWrite(pirPin, LOW);


  //give the sensor some time to calibrate on first boot up
  Serial.begin(9600);
  Serial.print("calibrating sensor ");
  for(int i = 0; i < calibrationTime; i++){
    Serial.print(".");
    delay(1000);
  }
  Serial.println(" done");
  Serial.println("SENSOR ACTIVE");
  delay(50);


  //Start the strip
  strip_h.begin();
  strip_h.show();
  strip_h.setBrightness(DefaultBrightness);
  strip_h.show();
  strip_m.begin();
  strip_m.show();
  strip_m.setBrightness(DefaultBrightness);
  strip_m.show();


  //Start the RTC
  Wire.begin();
  rtc.begin();
  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
  }
}
////////////////////////////
//LOOP
void loop(){


  //Get time 
  DateTime now = rtc.now();
  int hour = now.hour();
  int min = now.minute();
  boolean isAM = true;
  //condition hour format
  if (hour>=12){
    hour=hour-12;
    isAM =!isAM;
  }

  if(digitalRead(pirPin) == HIGH){

    if(lockLow){  
      //makes sure we wait for a transition to LOW before any further output is made:
      lockLow = false;            

//      Serial.println("---");
//      Serial.print("motion detected at ");
//      Serial.print(millis()/1000);
//      Serial.println(" sec");
      //First run 


        //restore brightness everytime
      strip_h.setBrightness(DefaultBrightness);
      strip_m.setBrightness(DefaultBrightness);
      strip_h.show();
      strip_m.show();

      //display time

      //Set hours

      HourInitialization(strip_h.Color(0, 255, 0), hour, DefaultBrightness);
      //strip_h.setPixelColor(2*hour, 255,0,0 );
      //trip_h.setPixelColor(2*hour+1, 255,0,0 );

      //set minutes

      MinInitialization(strip_h.Color(0, 255, 0), min, DefaultBrightness);      

      //start seconda animation  - runs for ~2 mins
      //Run Draw cycle
      SecondCycle_Draw(strip_m.Color(0, 0, 255),min, hour, DefaultBrightness, isAM);
      //Redraw last min (m) to seconds color
      strip_m.setPixelColor(min, 0,0,255 );
      strip_m.show();
      //Draw a new minute (m+1)
      //puase a bit, helps with transition
      delay(800);
      strip_m.setPixelColor(min+1, 0,255,0 );
      strip_m.show();
      //puase a bit, helps with transition
      delay(800);
      //Run erase cycle
      SecondCycle_Erase(min+1, DefaultBrightness);
      //Erase last min (m+1) and draw a new min (m+2)
      //puase a bit, helps with transition
      delay(800);
      strip_m.setPixelColor(min+1, 0,0,0 );
      strip_m.setPixelColor(min+2, 0,255,0 );
      strip_m.show();
      delay(2000); //don't fade right away
    }         
    takeLowTime = true;
  }

  if(digitalRead(pirPin) == LOW){       
    //digitalWrite(ledPin, LOW);  //the led visualizes the sensors output pin state

    if(takeLowTime){
      lowIn = millis();          //save the time of the transition from high to LOW
      takeLowTime = false;       //make sure this is only done at the start of a LOW phase
    }
    //if the sensor is low for more than the given pause, 
    //we assume that no more motion is going to happen
    if(!lockLow && millis() - lowIn > pause){  
      //makes sure this block of code is only executed again after 
      //a new motion sequence has been detected
      lockLow = true;                        

      //slowly reduce brightness:  at this point only the two hour and a minute pixel be active
      //fadeout(2*DefaultBrightness);  //flicker before die
      //Turn off all the pixels
      for(uint16_t i=0; i<strip_h.numPixels(); i++) {
        strip_h.setPixelColor(i  , 0); // erase  pixel
        strip_h.show();
      }
      //force erase all pixels
      for(uint16_t i=0; i<strip_m.numPixels(); i++) {
        strip_m.setPixelColor(i  , 0); // erase pixel
        strip_m.show();
      }

      Serial.print("motion ended at ");      //output
      Serial.print((millis() - pause)/1000);
      Serial.println(" sec");
      delay(50);
    }
  }
}

static void chase(uint32_t c, int SkipPixel) {
  // for(uint16_t i=0; i<strip_a.numPixels()+4; i++) {
  //    strip_a.setPixelColor(i  , c); // Draw new pixel
  //   strip_a.setPixelColor(i-4, 0); // Erase pixel a few steps back
  //  strip_a.show();
  // delay(1000);
  // }
  for(uint16_t i=SkipPixel+1; i<=strip_m.numPixels()+4; i++) {

    strip_m.setPixelColor(i  , c); // Draw new pixel
    if (i-4!=SkipPixel){
      strip_m.setPixelColor(i-4, 0); // Erase pixel a few steps back
    }
    strip_m.show();
    delay(1000);
  }

  for(uint16_t i=0; i<SkipPixel; i++) {

    strip_m.setPixelColor(i  , c); // Draw new pixel
    //if (i-4!=SkipPixel){
    strip_m.setPixelColor(i-4, 0); // Erase pixel a few steps back
    //}
    strip_m.show();
    delay(1000);
  } 

}

static void fadeout (int CurrentBrightness) {
  for(uint16_t i=CurrentBrightness-1; i>=1; i--) {
    strip_h.setBrightness(i);
    strip_m.setBrightness(i);
    strip_h.show();
    strip_m.show();
    delay(32);
  }
}

//also fade-in and out rest of hour ring pixels based on if its AM or PM
static void SecondCycle_Draw (uint32_t c, int MinPixel, int HourPixel, int CurrentBrightness, boolean AM) {   
  for(uint16_t i=1; i<=59; i++) {
    if (MinPixel+i<=59)
    {
      strip_m.setPixelColor(MinPixel+i, c);
    }
    else
    {
      strip_m.setPixelColor(MinPixel+i-60, c);
    }
    strip_m.show();
    //Fade in and out every second the entire hour ring
    //First lit the rest of the pixels
    if (i==1){ 
      if (AM) 
      {  //set to white for morning
        for(uint16_t l=0; l<2*HourPixel; l++) {
          strip_h.setPixelColor(l, 255,255,255);
        }
        for(uint16_t l=23; l>2*HourPixel+1; l--) {
          strip_h.setPixelColor(l, 255,255,255);
        }
      }
      if(!AM)  //set to ORANGE if PM
      {  
        for(uint16_t l=0; l<2*HourPixel; l++) {
          strip_h.setPixelColor(l, 255,140,0);
        }
        for(uint16_t l=23; l>2*HourPixel+1; l--) {
          strip_h.setPixelColor(l, 255,140,0);
        }
      }
      strip_h.show(); 
    }
    
    // if i is odd dim, if i is even bring them back in steps of  //do opposite for erase
   int temp = i % 2;
    if ( temp == 0) { //even runs - lit up
      for(uint16_t r=1; r<=CurrentBrightness; r++) {
        strip_h.setBrightness(r);
        strip_h.show();
        delay(980/(CurrentBrightness-1));  //100 ms overhead - have to time better
      }
    }
    if ( temp == 1)  //odd runs - fade out
    {  
      for(uint16_t s=CurrentBrightness; s>=1; s--) {
        strip_h.setBrightness(s);
        strip_h.show();
        delay(980/(CurrentBrightness-1));  //20 ms overhead - have to time better
      }
    }
    //delay(1000); //time and put exact
  }
}


static void SecondCycle_Erase (int MinPixel,int CurrentBrightness) {
  for(uint16_t i=1; i<=59; i++) {
    if (MinPixel+i<=59)
    {
      strip_m.setPixelColor(MinPixel+i, 0);
    }
    else
    {
      strip_m.setPixelColor(MinPixel+i-60, 0);
    }
    strip_m.show();
    //The hour strip is all on and set right coloriwise
    //Cycle start should be with lit up
    // if i is odd lit, if i is even fade  
   int temp = i % 2;
    if ( temp == 1) { //odd runs - lit up
      for(uint16_t r=1; r<=CurrentBrightness; r++) {
        strip_h.setBrightness(r);
        strip_h.show();
        delay(980/(CurrentBrightness-1));  //100 ms overhead - have to time better
      }
    }
    if ( temp == 0) { //even runs - fade out 
      for(uint16_t s=CurrentBrightness; s>=1; s--) {
        strip_h.setBrightness(s);
        strip_h.show();
        delay(980/(CurrentBrightness-1));  //100 ms overhead - have to time better
      }
    }
  }
}




static void HourInitialization(uint32_t c,int HourPixel, int CurrentBrightness) {

  //lit all the hour pixels, followed by fading in
  strip_h.setBrightness(20);  //min brightness to start from
  for(uint16_t i=0; i<strip_h.numPixels(); i++) {
    strip_h.setPixelColor(i, c); // Erase pixel a few steps back
    strip_h.show();
  }
  //fade in
  for(uint16_t i=10; i<=CurrentBrightness; i++) {
    strip_h.setBrightness(i);
    strip_h.show();
    delay(32);
  }
  delay(1200); //wait a few
  //start turning them off from either sides of zero
  for(uint16_t i=0; i<=21; i++) {  //Maximum pixels to be turned off in one direction - when its 1/11 oclock 
    if (i<2*HourPixel)
    {
      strip_h.setPixelColor(i, 0);
    }       
    if (23-i>2*HourPixel+1)
    {
      strip_h.setPixelColor(23-i, 0);
    }
    strip_h.show();  
    delay(100);
  }
}


static void MinInitialization(uint32_t c,int MinPixel, int CurrentBrightness) {

  //lit all the hour pixels, followed by fading in
  strip_m.setBrightness(10);  
  for(uint16_t i=0; i<strip_m.numPixels(); i++) {
    strip_m.setPixelColor(i, c); // Erase pixel a few steps back
    strip_m.show();
  }
  //delay(1000);// for debug only
  //fade in
  for(uint16_t i=10; i<=CurrentBrightness; i++) {
    strip_m.setBrightness(i);
    strip_m.show();
    delay(32);
  }
  delay(1200); //wait a few
  //start turning them off from either sides of zero
  for(uint16_t i=0; i<=58; i++) {  //Maximum pixels to be turned off in one direction - when its 1/11 oclock 
    if (i<MinPixel)
    {
      strip_m.setPixelColor(i, 0);
    }       
    if (59-i>MinPixel)
    {
      strip_m.setPixelColor(59-i, 0);
    }
    strip_m.show();  
    delay(60);
  }
}

