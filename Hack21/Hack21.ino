//#include <SoftwareSerial.h>

//YWROBOT
//Compatible with the Arduino IDE 1.0
//Library version:1.1
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <HCSR04.h>
#include "ESP_MICRO.h" 
//#include <ESP8266WiFi.h>
//#include <WiFiClient.h>


LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display
#include <FastLED.h>

#define pedalPin    2
#define LED_PIN     14
#define NUM_LEDS    35
#define BRIGHTNESS  10
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define triggerTolerance 15 // distance in cm
#define triggerPin 13 // Pin for Ultrasonic trigger
#define echoPin 12 // Pin for Ultrasonic echo (read)
CRGB leds[NUM_LEDS];


// Wifi Variable ----------------------------------------
/*
// For AP mode connections:
const char *ssid = "Clicky, yeah?";  // You will connect your phone to this Access Point
const char *pw = "ope"; // and this is the password
IPAddress ip(192, 168, 0, 1); // From RoboRemo app, connect to this IP
IPAddress netmask(255, 255, 255, 0);
const int port = 6969; // and this port


WiFiServer server(port);
WiFiClient client;
*/
boolean stateChange = false;

// End Wifi Variables -----------------------------------------------



#define UPDATES_PER_SECOND 100



CRGBPalette32 currentPalette;
TBlendType    currentBlending;
CRGBPalette32 myProgressPalette;

unsigned long lastSecond = 0;
int8_t greenLed = 0;
bool pedalPressed = false;
unsigned long lightSecondHand = 0;
unsigned long secondHand = 0;
int failure = 0; // Failure = 1, Failure = 2 is success
int seconds = 0;
char *phrases[] = {"Let's Freaking Go", "You're saving lives!", "Go you!", "Soap those hands"};
int i = 0;

// Ultrasonic variables
boolean nearPerson = false; // Set to true if someone is found within triggerDistance cm
int cooldown = 5000; // Cooldown for a person to be undetected after a trigger
double last_trigger = 0; // Time of last trigger
double distances;
UltraSonicDistanceSensor distanceSensor(13, 12);  // Initialize sensor that uses digital pins 13 and 12.
const int numReadings = 7;

double readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average
int oldAverage = 0;
long lastRecording;

void setup()
{
  Serial.begin(115200);
  lcd.init();                      // initialize the lcd
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(1, 0);
  // lcd.print("Hello, world!");
  lcd.setCursor(2, 1);
  //  lcd.print("Clicky, Yeah?");
  
  /*
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(ip, ip, netmask); // configure ip address for softAP 
  WiFi.softAP("Clicky, yeah?"); // configure ssid and password for softAP
  server.begin();
  */

  pinMode(pedalPin, INPUT_PULLUP);

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(  BRIGHTNESS );

  //currentBlending = LINEARBLEND;
  currentBlending = NOBLEND;

  fill_gradient_RGB (myProgressPalette, 0, CRGB::Red, 16, CRGB::Green);
  currentPalette = myProgressPalette;

  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 25 ;

  WiFi.mode(WIFI_STA);  
  start("DEA Police Van","festivetree664");// Connect to wifi, enter your details here  
  
  }
}

void loop()
{
  // Begin smoothing for ultrasonic reads --------------------------------------

  if (millis()-lastRecording >= 250){
    lastRecording = millis();
    //distances = distanceSensor.measureDistanceCm();
    // subtract the last reading:
    total = total - readings[readIndex];
    // read from the sensor:
    readings[readIndex] = distanceSensor.measureDistanceCm();
    // add the reading to the total:
    total = total + readings[readIndex];
    // advance to the next position in the array:
    readIndex = readIndex + 1;
  
    // if we're at the end of the array...
    if (readIndex >= numReadings) {
      // ...wrap around to the beginning:
      readIndex = 0;
      oldAverage = average;
    }
  
    // calculate the average:
    average = total / numReadings;
    Serial.println(average);

  }
  

  // End smoothing for ultrasonic reads
  
  if (abs(oldAverage-average) >= triggerTolerance && (millis() - last_trigger) > cooldown)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Wash your hands!");

    ReminderFlash();

    
    last_trigger = millis();
    nearPerson = true;
    delay(500);
    lcd.clear();
  }
  else
  {
    nearPerson = false; 
  }
  

  
  // Wifi Section
  /*
  if(!client.connected()) { // if client not connected
    client = server.available(); // wait for it to connect
      pinMode(LED_BUILTIN,LOW);
  }
  else
  {
    pinMode(LED_BUILTIN,LOW);
  }
  */
  
  lcd.setCursor(0, 0);
  lcd.print("Press pedal to");
  lcd.setCursor(0, 1);
  lcd.print("begin washing.");

  if (digitalRead(pedalPin) == LOW)
  {
    pedalPressed = true;
    lastSecond = millis();
    Serial.print(lastSecond);
    i += 1;
  }
  else {
    pedalPressed = false;
  }
  while (pedalPressed) {
    //Serial.print();
    failure = 1;
    //stateChange = true;
    pedalPressed = !digitalRead(pedalPin);
    lightSecondHand = ((millis() - lastSecond) / 571) % 60;
    secondHand = ((millis() - lastSecond)  / 1000) % 60;

    //Serial.print(lastSecond);

    LightPerSecond(secondHand, lightSecondHand, 0);
    //lcd.setCursor(10,0);
    //lcd.print(secondHand);
    FastLED.show();
    FastLED.delay(1000 / UPDATES_PER_SECOND);
  }
  if (failure == 1) {
    for ( int i = 0; i < 36; ++i) {
      leds[i] = CRGB::Red;
      FastLED.show();
    }
    FastLED.show();
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("__________");
    lcd.setCursor(1, 1);
    lcd.print("_'          '_  ");

    delay(1000);
    lcd.clear();
    
  }

  
  // Send data to wifi
  if (stateChange)
  {
    waitUntilNewReq();    // Waits until a new request from python come

  returnThisInt(failure);        // Returns the data to python
    stateChange = false;
  }
  



  // End send data to wifi

  // Reset Variables
  secondHand = 0;
  lightSecondHand = 0;
  failure = 0;
  FastLED.clear();
  FastLED.show();
}

void LightPerSecond(int secondHand, int lightSecondHand, uint8_t colorIndex)
{
  if (seconds != secondHand) {
    seconds = secondHand;
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Keep Washing " + (String)(20 - secondHand));
    lcd.setCursor(2, 0);
    //lcd.print(phrases[index]);
  }

  uint8_t numLeds = (int)(lightSecondHand) % 36;
  if (secondHand <= 20)
  {
    //lastSecond = secondHand;
    uint8_t brightness = 255;

    for ( int i = 0; i < numLeds; ++i) {
      leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
      colorIndex += 3;
    }
  }
  else {
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("'_          _'  ");
    lcd.setCursor(3, 1);
    lcd.print("__________");
    for ( int i = 0; i < 36; ++i) {
      leds[i] = CRGB::Green;
      FastLED.show();
      delay(75);
    }

    failure = 2;
    delay(1000);
    secondHand = 0;
    lightSecondHand = 0;
    pedalPressed = false;
    Serial.print(1);
    FastLED.clear();
    FastLED.show();
  }
}

void ReminderFlash(){

  /*
  fill_solid(leds, NUM_LEDS, CRGB::White);
  FastLED.show();
  delay(300);
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
  delay(300);
  fill_solid(leds, NUM_LEDS, CRGB::White);
  FastLED.show();
  delay(300);
  FastLED.clear();
  FastLED.show();
  */


  FastLED.setBrightness(  BRIGHTNESS*3);
  for(int i = 0; i <= 17; i++)
  {
    leds[i] = CRGB::White;
    leds[34-i] = CRGB::White;
    FastLED.show();
    delay(30);
  }

  for(int i = 17; i >= 0; i--)
  {
    leds[i] = CRGB::Black;
    leds[34-i] = CRGB::Black;
    FastLED.show();
    delay(30);
  }

  FastLED.setBrightness(  BRIGHTNESS);
  FastLED.clear();
  FastLED.show();

  
}



/*
void FillLEDsFromPaletteColors( uint8_t colorIndex, uint8_t numLeds)
{
  uint8_t brightness = 255;

  for ( int i = 0; i < numLeds; ++i) {
    leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
    colorIndex += 3;
  }
}


// There are several different palettes of colors demonstrated here.
//
// FastLED provides several 'preset' palettes: RainbowColors_p, RainbowStripeColors_p,
// OceanColors_p, CloudColors_p, LavaColors_p, ForestColors_p, and PartyColors_p.
//
// Additionally, you can manually define your own color palettes, or you can write
// code that creates color palettes on the fly.  All are shown here.

void ChangePalettePeriodically()
{
  uint8_t secondHand = (millis() / 1000) % 60;
  static uint8_t lastSecond = 99;

  if ( lastSecond != secondHand) {
    lastSecond = secondHand;
    if ( secondHand ==  0)  {
      currentPalette = RainbowColors_p;
      currentBlending = LINEARBLEND;
    }
    if ( secondHand == 10)  {
      currentPalette = RainbowStripeColors_p;
      currentBlending = NOBLEND;
    }
    if ( secondHand == 15)  {
      currentPalette = RainbowStripeColors_p;
      currentBlending = LINEARBLEND;
    }
    if ( secondHand == 20)  {
      SetupPurpleAndGreenPalette();
      currentBlending = LINEARBLEND;
    }
    if ( secondHand == 25)  {
      SetupTotallyRandomPalette();
      currentBlending = LINEARBLEND;
    }
    if ( secondHand == 30)  {
      SetupBlackAndWhiteStripedPalette();
      currentBlending = NOBLEND;
    }
    if ( secondHand == 35)  {
      SetupBlackAndWhiteStripedPalette();
      currentBlending = LINEARBLEND;
    }
    if ( secondHand == 40)  {
      currentPalette = CloudColors_p;
      currentBlending = LINEARBLEND;
    }
    if ( secondHand == 45)  {
      currentPalette = PartyColors_p;
      currentBlending = LINEARBLEND;
    }
    //        if( secondHand == 50)  { currentPalette = myRedWhiteBluePalette_p; currentBlending = NOBLEND;  }
    //        if( secondHand == 55)  { currentPalette = myRedWhiteBluePalette_p; currentBlending = LINEARBLEND; }
  }
}

// This function fills the palette with totally random colors.
void SetupTotallyRandomPalette()
{
  for ( int i = 0; i < 16; ++i) {
    currentPalette[i] = CHSV( random8(), 255, random8());
  }
}

// This function sets up a palette of black and white stripes,
// using code.  Since the palette is effectively an array of
// sixteen CRGB colors, the various fill_* functions can be used
// to set them up.
void SetupBlackAndWhiteStripedPalette()
{
  // 'black out' all 16 palette entries...
  fill_solid( currentPalette, 16, CRGB::Black);
  // and set every fourth one to white.
  currentPalette[0] = CRGB::White;
  currentPalette[4] = CRGB::White;
  currentPalette[8] = CRGB::White;
  currentPalette[12] = CRGB::White;

}

// This function sets up a palette of purple and green stripes.
void SetupPurpleAndGreenPalette()
{
  CRGB purple = CHSV( HUE_PURPLE, 255, 255);
  CRGB green  = CHSV( HUE_GREEN, 255, 255);
  CRGB black  = CRGB::Black;

  currentPalette = CRGBPalette16(
                     green,  green,  black,  black,
                     purple, purple, black,  black,
                     green,  green,  black,  black,
                     purple, purple, black,  black );
}

*/

// Additional notes on FastLED compact palettes:
//
// Normally, in computer graphics, the palette (or "color lookup table")
// has 256 entries, each containing a specific 24-bit RGB color.  You can then
// index into the color palette using a simple 8-bit (one byte) value.
// A 256-entry color palette takes up 768 bytes of RAM, which on Arduino
// is quite possibly "too many" bytes.
//
// FastLED does offer traditional 256-element palettes, for setups that
// can afford the 768-byte cost in RAM.
//
// However, FastLED also offers a compact alternative.  FastLED offers
// palettes that store 16 distinct entries, but can be accessed AS IF
// they actually have 256 entries; this is accomplished by interpolating
// between the 16 explicit entries to create fifteen intermediate palette
// entries between each pair.
//
// So for example, if you set the first two explicit entries of a compact
// palette to Green (0,255,0) and Blue (0,0,255), and then retrieved
// the first sixteen entries from the virtual palette (of 256), you'd get
// Green, followed by a smooth gradient from green-to-blue, and then Blue.
