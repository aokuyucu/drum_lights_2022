/*
 * Wemos board, Piezo trigger, Pushbutton interrupt color change, NeoPixel LED library.
 * Pushbutton cycles through 5 different LED colors.
 * Header file for drum names and timing definitions.
 * Second LED "strip" with a single pixel as an indicator light.
 * Rewrote code to get and set colors.
 * Added colorInit() function to assign colors for all drums in an array
 */

#include <Adafruit_NeoPixel.h>
#include "drum_config.h"

/* ATTN: These next three consts (drumID, N_PIXELS_MAIN, threshold)
 * must be adjusted to the correct value for each drum. 
 * 
 * First is drumID, which should be set to the appropriate value from drum_config.h
 */
const drumID myDrum = white_knight;   // What drum am I?

/* Number of LEDs attached to the Arduino.
 * For 2022, this is 125 for pawns (64 for grid + 31 for first strip + 30 for second strip)
 * For knights: 108 (75 + 33)
 * For kings: 121 (88 + 33)
 * Other values could be: 150 for full LED strip, 64 for small grid, 256 for large grid.
 */
const int N_PIXELS_MAIN = 108;

/* Threshold value to decide when the detected sound is a knock or not
 * Other useful values: 20, 50, 100, 200
 */
uint8_t threshold = 10;   //65;

// Brightness settings. Valid values are 0-255.
uint8_t brightness_main = 255;      // Brightness of the main light strip or grid.
                                    // Full = 255. Use 150 for bass 2 (yellow).
uint8_t brightness_indicator = 25;  // Brightness of the single pixel inidicator.

const int N_PIXELS_INDICATOR = 1;   // Single pixel indicator light.

// Which pin on the Arduino is connected to the NeoPixels?
const int ANALOG_PIN = A0;        // Piezo is connected to Analog A0 on Wemos or Gemma D2 (A1)
const int LED_PIN_MAIN = D5;      // NeoPixel LED strand is connected to D5
const int LED_PIN_INDICATOR = D7; // Single, indicator LED is connected to D7
const int BUTTON_PIN = D4;        // Push button is connected to D4 on Wemos without resistor

const int DELAYVAL = 25;    // Time (in milliseconds) to pause between pixels
const int LOOP_DELAY = 200; // Time (in milliseconds) to pause between loops

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
// Adafruit_NeoPixel pixels(NUMPIXELS, LED_PIN_MAIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels(N_PIXELS_MAIN, LED_PIN_MAIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel indicator(N_PIXELS_INDICATOR, LED_PIN_INDICATOR, NEO_GRB + NEO_KHZ800);

const uint32_t black = pixels.Color(0, 0, 0);  // i.e. off
const uint32_t red = pixels.Color(255, 0, 0);
const uint32_t blue = pixels.Color(0, 0, 255);
volatile uint32_t myColor0 = blue;
volatile uint32_t myColor1 = red;
volatile uint32_t myColor2 = black;
uint8_t brightness_main0 = 125;
uint8_t brightness_main1 = 255;
byte selectedEffect = 0;

volatile unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
volatile uint32_t interruptDebounce = 250;    // or 150; 250 seems to work best
//volatile unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers
//volatile uint32_t interruptMillis = millis();

void setup() {
  Serial.begin(9600);  // use the serial port

  pixels.begin();     // INITIALIZE NeoPixel strip object (REQUIRED)
  indicator.begin();  // Initialize the Indicator strip
  
  // Startup sequence(s)
  //chase(255, 0, 0);
  //startup(myColor0);
  
  // Set brightness of main LEDs
  pixels.setBrightness(brightness_main0);

   // Indicator pixel should always be ON and show the strip color.
  setIndicator(myColor0);
  
  // initialize the pushbutton pin as an input
  // INPUT_PULLUP and FALLING seem to work best
  pinMode(BUTTON_PIN, INPUT_PULLUP);  // or INPUT
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), handleInterrupt, FALLING); // FALLING or RISING or CHANGE
}

void loop() {
  setEffect();

  //delay(LOOP_DELAY);  // delay to avoid overloading the serial port buffer
}

// *************************
// ** LEDEffect Functions **
// *************************
void setEffect() {
  if (selectedEffect > 1) { 
    selectedEffect=0; 
  }

  switch (selectedEffect) {
    case 0  : {
      // All on blue
      setIndicator(myColor0); // Indicator pixel should always be ON and should show the strip color.
      pixels.setBrightness(brightness_main0);
      pixels.fill(myColor0, 0, N_PIXELS_MAIN);
      showStrip();
      //delay(3);
      break;
    }
    case 1  : {
      // Glow red
      setIndicator(myColor1); // Indicator pixel should always be ON and should show the strip color.
      FadeInOut(0xff, 0x00, 0x00); // red
      break;
    }
    case 2  : {
      // Off
      setIndicator(myColor2);
      pixels.clear();
      showStrip();
      break;
    }
  }
}

void FadeInOut(byte red, byte green, byte blue) {
  float r, g, b;
      
  for (int k = 0; k < 256; k=k+1) { 
    r = (k/256.0)*red;
    g = (k/256.0)*green;
    b = (k/256.0)*blue;
    setAll(r,g,b);
    showStrip();
    delay(5);
  }
     
  for(int k = 255; k >= 0; k=k-2) {
    r = (k/256.0)*red;
    g = (k/256.0)*green;
    b = (k/256.0)*blue;
    setAll(r,g,b);
    showStrip();
    delay(10);
  }
}

// ***************************************
// ** NeoPixel Common Functions **
// ***************************************

// Apply LED color changes
void showStrip() {
   // NeoPixel
   pixels.show();
}

// Set all LEDs to a given color and apply it (visible)
void setAll(byte redValue, byte greenValue, byte blueValue) {
  for (int i = 0; i < N_PIXELS_MAIN; i++ ) {
    //setPixel(i, red, green, blue);
    pixels.setPixelColor(i, pixels.Color(redValue, greenValue, blueValue));
  }
  showStrip();
}

/* 
 * Handle a button press. 
 */
ICACHE_RAM_ATTR void handleInterrupt() {
  //Serial.println("handleInterrupt");
  //if (button1.isPressed())
  //  Serial.println("Oh, hi");
    
  if ((millis() - lastDebounceTime) >= interruptDebounce) {
    //Serial.print("button pressed --> selectedEffect: ");
    //Serial.println(selectedEffect);
    selectedEffect++;
    //Serial.print("button pressed --> selectedEffect++: ");
    //Serial.println(selectedEffect);

    // Set the indicaotr light right away because the main strip doesn't change
    // until the current effect finishes, which can take a few seconds.
    if (selectedEffect > 1)
      selectedEffect=0; 

    switch (selectedEffect) {
      case 0  : {
        // All on blue
        setIndicator(myColor0); // Indicator pixel should always be ON and should show the strip color.
        break;
      }
      case 1  : {
        // Glow red
        setIndicator(myColor1); // Indicator pixel should always be ON and should show the strip color.
        break;
      }
    }
    
    //setEffect();

    lastDebounceTime = millis();  // not sure if this should be current millis() or millis() from the if statement above.
  }
}

/* 
 * Set the first pixel on D7 to be always ON.
 */
void setIndicator(uint32_t color) {
  indicator.clear();
  indicator.fill(color, 0, 1); // Indicator pixel should always be ON.
  indicator.show();
}

/*
 * Startup function for the main LED strip
 * Flicker a few times at the start, then gradually ascend to full brightness.
 */
void startup(uint32_t color) {
  // Flicker a few times...
  flash(ShortFlash_OnMS, ShortFlash_OffMS, color);
  flash(ShortFlash_OnMS, ShortFlash_OffMS, color);
  flash(ShortFlash_OnMS, ShortFlash_OffMS, color);
  flash(MediumFlash_OnMS, MediumFlash_OffMS, color);
  flash(MediumFlash_OnMS, MediumFlash_OffMS, color);

  // ... then gradually ascend to full brightness
  gradualAscent(color);
}

void chase(uint8_t redValue, uint8_t greenValue, uint8_t blueValue) {
  uint8_t  i;
  for (i=0; i<N_PIXELS_MAIN; i++) {
    pixels.setPixelColor(i, pixels.Color(redValue, greenValue, blueValue));
    pixels.show();
    pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    delay(30);
  }
}

void flash(uint8_t onMS, uint8_t offMS, uint32_t color) {
  pixels.fill(color, 0, N_PIXELS_MAIN);
  pixels.show();
  delay(onMS);
  
  pixels.clear(); // Set all pixel colors to 'off'
  pixels.show();
  delay(offMS);
}

// Gradually ascend to full brightness for the given color
void gradualAscent(uint32_t ascentColor) {
    uint8_t  interimDelay = 150;  // delay between each increasing step of brightness in the gradual ascent
    
    pixels.fill(ascentColor, 0, N_PIXELS_MAIN);
    pixels.setBrightness(32);
    pixels.show();
    delay(interimDelay);
    pixels.setBrightness(64);
    pixels.show();
    delay(interimDelay);
    pixels.setBrightness(128);
    pixels.show();
    delay(interimDelay);
    pixels.setBrightness(160);
    pixels.show();
    delay(interimDelay);
    pixels.setBrightness(192);
    pixels.show();
    delay(interimDelay);
    pixels.setBrightness(215);
    pixels.show();
    delay(interimDelay);
    pixels.setBrightness(230);
    pixels.show();
    delay(interimDelay);
    pixels.setBrightness(240);
    pixels.show();
    delay(interimDelay);
    pixels.setBrightness(255);
    pixels.show();
    delay(500);
}

/*
 * Input a value 0 to 255 to get a color value.
 * The colors are a transition r - g - b - back to r.
 */
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
