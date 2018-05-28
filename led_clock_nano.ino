// Examples by Josh Kaplan

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>//TODO: check if needed for Uno R3
#endif

#include <Wire.h>
#include "RTClib.h"

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN            6

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      24

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

const int delayval = 7; // delay for half a second
const int second =   1000;
const int minute =   second * 60;

// Define colors as uint32 because Color seems to be a convenience method for making those
// values rather than an initializable type
// pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
const uint32_t red =    pixels.Color(1, 0, 0);
const uint32_t green =  pixels.Color(0, 1, 0);
const uint32_t blue =   pixels.Color(0, 0, 1);
const uint32_t cyan = pixels.Color(0, 1, 1); // Traditionally mixing red and green makes 
const uint32_t white =  pixels.Color(1, 1, 1);

uint32_t allColors[] = {red, green, blue, cyan, white}; // Array of all colors

const int kBottom = 0;
const int kTop =    5; // set to 256 at max

const uint32_t off =   pixels.Color(0, 0, 0);

// RTC
// RTC_DS1307 rtc;
RTC_DS3231 rtc; // Comment this out depending on time circuit used
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

void setup() {
  pixels.begin(); // Initializes the NeoPixel library.

  Serial.begin(57600);
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void loop() {
  DateTime now = rtc.now(); // Get time
  debugPrintTime(now.hour(), now.minute());

  int minutePosition = getMinutePosition(now.minute());
  int hourPosition = getHourPosition(now.hour());

  clearOtherPixels(minutePosition, hourPosition);
  showCoordinates();
  
  pixels.setPixelColor(minutePosition, green);
  pixels.setPixelColor(hourPosition, blue);

  // Set color to cyan if second and minute overlap
  if (minutePosition == hourPosition) {
    pixels.setPixelColor(minutePosition, cyan);
  }
  
  pixels.show();

  delay(second * 30); // check every 30 secs
}

// Displays 0, 3, 6, and 9 positions
void showCoordinates() {
  int zeroPosition = 0;
  int threePosition = pixels.numPixels() / 4; 
  int sixPosition = pixels.numPixels() / 2;
  int ninePosition = (threePosition) * 3;

  pixels.setPixelColor(zeroPosition, white);
  pixels.setPixelColor(threePosition, white);
  pixels.setPixelColor(sixPosition, white);
  pixels.setPixelColor(ninePosition, white);
}

int getHourPosition(int hourVal) {
  const int numberOfHours = 12.0;
  // Use - or + 1? somewhere
  int newHourVal = hourVal % numberOfHours;
  // TODO: rename vars here
  int returnValue = (int) round((float) newHourVal / (float) numberOfHours * (float) pixels.numPixels());
  
  return adjustReturnVal(returnValue);
}

int getMinutePosition(int minuteValue) {
  float newMinuteValue = (float) minuteValue / 60.0;
  int returnValue = (int) round(newMinuteValue * (float) pixels.numPixels()); 

  return adjustReturnVal(returnValue);
}

int adjustReturnVal(int value) {  
  if (value == pixels.numPixels()) {
    return 0;
  }
  
  return value;
}

void clearOtherPixels(int pixelOne, int pixelTwo) {
  for(int i = 0; i < pixels.numPixels(); i++) {
    if (i != pixelOne && i != pixelTwo) {
      pixels.setPixelColor(i, off);
    }
  }
}

// Turns previous index off, watching out for edge case when
// the previous index was the last pixel
// Note, does not include pixels.show()
void clearPreviousIndex(int i) {
  int previousIndex = i - 1;
  
  if (previousIndex == -1) { //TODO: can this by written as ternary operation in C?
    previousIndex = pixels.numPixels() - 1;
  }
  pixels.setPixelColor(previousIndex, off);
}

void debugPrintTime(int hourValue, int minuteValue) {
  Serial.print(hourValue, DEC);
  Serial.print(':');
  if (minuteValue < 10) {
    Serial.print("0");
  } 
  Serial.print(minuteValue, DEC);
  Serial.print("\n");
}
