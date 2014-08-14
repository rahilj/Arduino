//Trinket version of Clock
//Libraries
#include <TinyWireM.h>
#include <TinyRTClib.h>
#include <Adafruit_NeoPixel.h>
//Constants, Pins and Parameters

int pirPin = 3;    //the digital pin connected to the PIR sensor's output
int calibrationTime = 30;        
//the time when the sensor outputs a low impulse
long unsigned int lowIn;         
//the amount of milliseconds the sensor has to be low 
//before we assume all motion has stopped
long unsigned int pause = 20;  

boolean lockLow = true;
boolean takeLowTime;  
//boolean isAM = true;

int DefaultBrightness = 80;  //default global brightness

//Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_LEDS, PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip_h = Adafruit_NeoPixel(24, 1);  //HouRing on Pin 1
Adafruit_NeoPixel strip_m = Adafruit_NeoPixel(60, 4);  //MinRing on Pin 4

//SendOnlySoftwareSerial Serial(3);  // Serial transmission on Trinket Pin 3
RTC_DS1307 rtc;                      // Set up real time clock

void setup () {
  
    //Configure IO except the data inputs to neopixel rings 
  pinMode(pirPin, INPUT);
  // Use the pull-up
  digitalWrite(pirPin, LOW);
  
  //PIR calibration
  delay(calibrationTime*1000);
    //Start the strip
  strip_h.begin();
  strip_h.setBrightness(DefaultBrightness);
  strip_h.show();
  
  strip_m.begin();
  strip_m.setBrightness(DefaultBrightness);
  strip_m.show();
  
  pinMode(1, OUTPUT);    // define PWM meter pins as outputs
  pinMode(4, OUTPUT);

  TinyWireM.begin();            // Begin I2C 
  rtc.begin();                  // Begin DS1307 real time clock
  
}

void loop(){


  //Get time 
  DateTime CurrentTime = rtc.now();
  int NowHour = CurrentTime.hour();
  int NowMin = CurrentTime.minute();
  int today = CurrentTime.day();
  boolean isAM = true;
  //condition hour format
  if (NowHour>=12){
    NowHour=NowHour-12;
    isAM =!isAM;
  }

  if(digitalRead(pirPin) == HIGH){

    if(lockLow){  
      //makes sure we wait for a transition to LOW before any further output is made:
      lockLow = false;            


      //Initialize the rings - For Trinket, lets just bring the three Leds (min+2hour) followed by fadein
      strip_m.setBrightness(5);
      strip_h.setBrightness(5);
      strip_m.setPixelColor(NowMin, 255, 0, 0 );
      strip_h.setPixelColor(2*NowHour, 255, 0, 0 );
      strip_h.setPixelColor(2*NowHour+1, 255, 0, 0 );
      strip_m.show();
      strip_h.show();
      delay(100);
      
      //Map today into the a smaller range, a week for starters
      //Can't use dayofweek due to memory issues
      int TodayColor =  25*(today % 7+1);  //range (25-175)
      
      
       //bring up rest of the pixels in hour ring
       if (isAM) 
      {  //set to white for morning
        for(uint16_t l=0; l<2*NowHour; l++) {
          strip_h.setPixelColor(l, 255,255,165);
        }
        for(uint16_t l=23; l>2*NowHour+1; l--) {
          strip_h.setPixelColor(l, 255,255,165);
        }
      }
      else  //set to ORANGE if PM
      {  
        for(uint16_t l=0; l<2*NowHour; l++) {
          strip_h.setPixelColor(l, 20,180,20);
        }
        for(uint16_t l=23; l>2*NowHour+1; l--) {
          strip_h.setPixelColor(l, 20,180,20);
        }
      }
      //Slowly fade-in - much better experience
      strip_h.show(); 
      //fade in
      fadein(DefaultBrightness);
      
     
      
      
      //start seconds animation  - runs for ~2 mins
      //Run Draw cycle
     //delay(100);
      SecondCycle_Draw(strip_m.Color(0, TodayColor, 255),NowMin, NowHour, DefaultBrightness, isAM);
      //Redraw last min (m) to seconds color
      strip_m.setPixelColor(NowMin, 0,TodayColor,255 );
      strip_m.show();
      //Draw a new minute (m+1)
      //puase a bit, helps with transition
      delay(800);
      strip_m.setPixelColor(NowMin+1, 255,0,0 );
      strip_m.show();
      //puase a bit, helps with transition
      delay(800);
      
      //Run erase cycle
      SecondCycle_Erase(NowMin+1, DefaultBrightness);
      //Erase last min (m+1) and draw a new min (m+2)
      //puase a bit, helps with transition
      delay(800);
      strip_m.setPixelColor(NowMin+1, 0,0,0 );
      strip_m.setPixelColor(NowMin+2, 255,0,0 );
      strip_m.show();
      delay(1000); //don't fade right away
      fadeout (DefaultBrightness);
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
      for(uint16_t i=0; i<24; i++) {
        strip_h.setPixelColor(i  , 0); // erase  pixel
        strip_h.show();
      }
      //force erase all pixels
      for(uint16_t i=0; i<60; i++) {
        strip_m.setPixelColor(i  , 0); // erase pixel
        strip_m.show();
      }

    }
  } 
  //Loop ends
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
static void fadein (int CurrentBrightness) {
  for(uint16_t i=5; i<=CurrentBrightness; i++) {
    strip_h.setBrightness(i);
    strip_m.setBrightness(i);
    strip_h.show();
    strip_m.show();
    delay(50);
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
    
    // if i is odd dim, if i is even bring them back in steps of  //do opposite for erase
   int temp = i % 2;
    if ( temp == 0) { //even runs - lit up
      for(uint16_t r=1; r<=CurrentBrightness; r++) {
        strip_h.setBrightness(r);
        strip_h.show();
        delay(980/(CurrentBrightness-1));  //100 ms overhead - have to time better
      }
    }
    else  //odd runs - fade out
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



