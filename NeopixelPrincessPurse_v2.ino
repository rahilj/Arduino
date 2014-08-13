//Neopixel Princess Purse
//Parts of the code (especially animation routines) are forked from Adafruit's Awesome tutorials
//5 animations are defined. Button press loops through these animations infinitely

//Libraries
#include <Adafruit_NeoPixel.h>
 
//Constants
int DefaultBrightness = 80;  //default global brightness
 
//Neopixels - Two strip version
//Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_LEDS, PIN, NEO_GRB + NEO_KHZ800);
int NP_RING_PIN = 2;
int NP_LED_PIN = 1;

Adafruit_NeoPixel strip_l= Adafruit_NeoPixel(6, NP_LED_PIN,NEO_KHZ800 + NEO_GRB);  //DataOut on Pin 1, 6 LEDs
Adafruit_NeoPixel strip_r = Adafruit_NeoPixel(12, NP_RING_PIN);  //DataOut on Pin 2, 12 NP on ring 

//Adafruit_NeoPixel strip = Adafruit_NeoPixel(6, PIN, NEO_KHZ800 + NEO_GRB);
//Switch related
int switchPin = 3; // switch is connected to pin 3, active low
int val; // variable for reading the pin status
int val2;
int buttonState = HIGH; // variable to hold the button state
int lightMode = 0; // how many times the button has been pressed
 
void setup() {
pinMode(switchPin, INPUT_PULLUP); // Set the switch pin as input
//LED initialization
pinMode(NP_LED_PIN, OUTPUT);
strip_r.setBrightness(DefaultBrightness); //adjust brightness here
strip_r.begin();
strip_r.show(); // Initialize all pixels to 'off'
//NP Ring initialization
pinMode(NP_RING_PIN, OUTPUT);
strip_l.setBrightness(DefaultBrightness); //adjust brightness here
strip_l.begin();
strip_l.show(); // Initialize all pixels to 'off'
}

 
void loop(){

  val = digitalRead(switchPin); // read input value and store it in val
  delay (20);
  val2 = digitalRead(switchPin);
  if (val == val2 && val==LOW) {

       //Perform Animations
       //Initialize strip brightness
       strip_l.setBrightness(DefaultBrightness);
       strip_r.setBrightness(DefaultBrightness);
       //buttonState = val; // save the new state in our variable
      if (lightMode == 0) {
      //Colorwipe in three primary colors
      for (int j=0; j<8; j++) {
        colorWipe_l(strip_l.Color(255, 0, 0), 30); // Red
        colorWipe_r(strip_r.Color(255, 0, 0), 30); // Red
        colorWipe_l(strip_l.Color(0, 255, 0), 30); // Green
        colorWipe_r(strip_r.Color(0, 255, 0), 30); // Green
        colorWipe_l(strip_l.Color(0, 0, 255), 30); // Blue
        colorWipe_r(strip_r.Color(0, 0, 255), 30); // Blue
      }
        fadeout (DefaultBrightness);
      }
      
      if (lightMode == 1) {
        for (int j=0; j<3; j++) {
        theaterChase_l(strip_l.Color(127, 127, 127), 50); // White
        theaterChase_r(strip_r.Color(127, 127, 127), 50); // White
        theaterChase_l(strip_l.Color(127,   0,   0), 50); // Red
        theaterChase_r(strip_r.Color(127,   0,   0), 50); // Red
        theaterChase_l(strip_l.Color(  0,   0, 127), 50); // Blue
        theaterChase_r(strip_r.Color(  0,   0, 127), 50); // Blue
        }
        fadeout (DefaultBrightness);
      }
      
      if (lightMode == 2) {
      for (int j=0; j<3; j++) {
      rainbow_l(10);
      rainbow_r(10);}
      fadeout (DefaultBrightness);
      }
      
      if (lightMode == 3) {
      theaterChaseRainbow_l(50);
      theaterChaseRainbow_r(50);
      fadeout (DefaultBrightness);
        }
      if (lightMode == 4) {
      rainbowCycle_l(20);
      rainbowCycle_r(20);
      fadeout (DefaultBrightness);
      }  
      
      //Update lightMode
     if (lightMode < 4) {
       lightMode = lightMode+1;
     }
       else {
       lightMode=0;
     }      
     }  
}

 //COLORWIPE
// Fill the dots one after the other with a color
void colorWipe_l(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip_l.numPixels(); i++) {
    strip_l.setPixelColor(i, c);
    strip_l.show();
    delay(wait);
  }
}
//
// Fill the dots one after the other with a color
void colorWipe_r(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip_r.numPixels(); i++) {
    strip_r.setPixelColor(i, c);
    strip_r.show();
    delay(wait);
  }
}
//

//THEATRECHASE
//Theatre-style crawling lights.
void theaterChase_l(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip_l.numPixels(); i=i+3) {
        strip_l.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip_l.show();
      delay(wait);
      for (int i=0; i < strip_l.numPixels(); i=i+3) {
        strip_l.setPixelColor(i+q, 0);        //turn every third pixel off
      }
      strip_l.show();
    }
  }
}
void theaterChase_r(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip_r.numPixels(); i=i+3) {
        strip_r.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip_r.show();
      delay(wait);
      for (int i=0; i < strip_l.numPixels(); i=i+3) {
        strip_r.setPixelColor(i+q, 0);        //turn every third pixel off
      }
      strip_r.show();
    }
  }
}

//RAINBOW Program
void rainbow_l(uint8_t wait) {
  uint16_t i, j;
 
  for(j=0; j<256; j++) {
    for(i=0; i<strip_l.numPixels(); i++) {
strip_l.setPixelColor(i, Wheel((i+j) & 255));
}
strip_l.show();
    delay(wait);
  }
}
void rainbow_r(uint8_t wait) {
  uint16_t i, j;
 
  for(j=0; j<256; j++) {
    for(i=0; i<strip_r.numPixels(); i++) {
strip_r.setPixelColor(i, Wheel((i+j) & 255));
}
strip_r.show();
    delay(wait);
  }
}

//THEATRECHASERAINBOW
void theaterChaseRainbow_l(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
        for (int i=0; i < strip_l.numPixels(); i=i+3) {
          strip_l.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
        }
        strip_l.show();
       
        delay(wait);
       
        for (int i=0; i < strip_l.numPixels(); i=i+3) {
          strip_l.setPixelColor(i+q, 0);        //turn every third pixel off
        }
    }
  }
}
void theaterChaseRainbow_r(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
        for (int i=0; i < strip_r.numPixels(); i=i+3) {
          strip_r.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
        }
        strip_r.show();
       
        delay(wait);
       
        for (int i=0; i < strip_r.numPixels(); i=i+3) {
          strip_r.setPixelColor(i+q, 0);        //turn every third pixel off
        }
    }
  }
}

 //RAINBOWCYCLE
// Rainbow Cycle Program - Equally distributed
void rainbowCycle_l(uint8_t wait) {
uint16_t i, j;
 
for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
for(i=0; i< strip_l.numPixels(); i++) {
strip_l.setPixelColor(i, Wheel(((i * 256 / strip_l.numPixels()) + j) & 255));
}
strip_l.show();
delay(wait);
}
}
void rainbowCycle_r(uint8_t wait) {
uint16_t i, j;
 
for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
for(i=0; i< strip_r.numPixels(); i++) {
strip_r.setPixelColor(i, Wheel(((i * 256 / strip_r.numPixels()) + j) & 255));
}
strip_r.show();
delay(wait);
}
}


// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
if(WheelPos < 85) {
return strip_l.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
} else if(WheelPos < 170) {
WheelPos -= 85;
return strip_l.Color(255 - WheelPos * 3, 0, WheelPos * 3);
} else {
WheelPos -= 170;
return strip_l.Color(0, WheelPos * 3, 255 - WheelPos * 3);
}
}

static void fadeout (int CurrentBrightness) {
  for(uint16_t i=CurrentBrightness-1; i>=1; i--) {
    strip_l.setBrightness(i);
    strip_r.setBrightness(i);
    strip_l.show();
    strip_r.show();
    delay(32);
  }
  //Turn off all the pixels
    for(uint16_t i=0; i<strip_l.numPixels(); i++) {
    strip_l.setPixelColor(i  , 0); // erase  pixel
    strip_l.show();
    }
    for(uint16_t i=0; i<strip_r.numPixels(); i++) {
    strip_r.setPixelColor(i  , 0); // erase  pixel
    strip_r.show();
    }
}
