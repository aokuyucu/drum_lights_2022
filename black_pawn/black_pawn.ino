/*
 * Wemos board, Piezo trigger, Pushbutton interrupt color change, NeoPixel LED library.
 * Pushbutton cycles through 5 different LED colors.
 * Header file for drum names and timing definitions.
 * Second LED "strip" with a single pixel as an indicator light.
 * Rewrote code to get and set colors.
 * Added colorInit() function to assign colors for all drums in an array
 */

#include <Adafruit_NeoPixel.h>

// Black Pawn

// ATTN: These next three consts must be adjusted to the correct value for each drum
/* Number of LEDs attached to the Arduino.
 * For 2022, this is 125 (64 for grid + 31 for first strip + 30 for second strip)
 * Other values could be: 150 for full LED strip, 64 for small grid, 256 for large grid.
 */
const int N_PIXELS_MAIN = 125;
const uint8_t numColors = 2;  // Number of colors

/* Threshold value to decide when the detected sound is a knock or not
 * Other useful values: 20, 50, 100, 200
 */
uint8_t threshold = 7;   //65;

// Brightness settings. Valid values are 0-255.
uint8_t brightness_main = 255;      // Brightness of the main light strip or grid.
                                    // Use 150 for bass 2 (yellow).
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
volatile uint32_t myColor = red;

void setup() {
  Serial.begin(9600);  // use the serial port

  pixels.begin();     // INITIALIZE NeoPixel strip object (REQUIRED)
  indicator.begin();  // Initialize the Indicator strip

  // Startup sequence(s)
  //chase(255, 0, 0);
  
  // Set to full brightness for the duration of the sketch
  pixels.setBrightness(brightness_main);
  indicator.setBrightness(brightness_indicator);

  setIndicator(myColor); // Indicator pixel should always be ON and should show the strip color.
}

void loop() {
  int sensorReading;

  pixels.clear(); // Set all pixel colors to 'off'

  sensorReading = analogRead(ANALOG_PIN); // Raw reading from sensor
  if (sensorReading >= threshold) {
    // The first NeoPixel in a strand is #0, second is 1, all the way up
    // to the count of pixels minus one.
    // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
  
    // Limit the number of readings printed
    //Serial.print("Raw sensor reading: ");
    //Serial.println(sensorReading);

    // Color all pixels myColor, starting at position 0 
    pixels.fill(myColor, 0, N_PIXELS_MAIN);
  }
  else {
    // Turn off pixels
    pixels.clear();
  }

  pixels.show(); // Send the updated pixel colors to the hardware.

  //delay(LOOP_DELAY);  // delay to avoid overloading the serial port buffer
}

/* 
 * Set the first pixel on D7 to be always ON.
 */
void setIndicator(uint32_t color) {
  indicator.clear();
  indicator.fill(color, 0, 1); // Indicator pixel should always be ON.
  indicator.show();
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
void gradualAscent(uint32_t color) {
    uint8_t  interimDelay = 150;  // delay between each increasing step of brightness in the gradual ascent
    
    pixels.fill(myColor, 0, N_PIXELS_MAIN);
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
