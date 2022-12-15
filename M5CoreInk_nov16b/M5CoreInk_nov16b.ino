#include "arduino_secrets.h"
#include "M5CoreInk.h"
#include <Adafruit_NeoPixel.h>

#define N_LEDS 30

#include "thingProperties.h"

// pin constants:
const int motionPin           = 26;         // top connector pin G26
const int rgbOutputPin        = 23;         // bottom connector pin G32
const int mAnalogPin          = 33;
const int mDigitalPin         = 32;

Adafruit_NeoPixel strip       = Adafruit_NeoPixel(N_LEDS, rgbOutputPin, NEO_GRB + NEO_KHZ800);

int analogRead_value          = 0;
int digitalRead_value         = 0;

unsigned long timer           = 0;          // real-time clock
unsigned long m_timer         = 0;          // real-time clock

int ledVal                    = 0;          // variable to store the led brightness
int motionVal                 = LOW;        // variable to store the sensor status (value)
int state                     = LOW;        // by default, no motion detected

void setup() {
  M5.begin();                                     // initialize the NeoPixel library
  pinMode(motionPin, INPUT);                      // initialize sensor as an input
  pinMode(mAnalogPin, OUTPUT);                    // set analog pin
  pinMode(mDigitalPin, INPUT);                    // set digital pin
  dacWrite(25, 0);

  Serial.begin(9600);                             // open Serial port at 9600 bit per second

  M5.update();
  M5.M5Ink.clear();

  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(1500);

  // Defined in thingProperties.h
  initProperties();

  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);

  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();
}

void loop() {
  ArduinoCloud.update();

  motionVal = digitalRead(motionPin);

  if (motionVal == HIGH) {
    if (state == LOW) {
      Serial.println("Motion detected!");
      ledTurnOn();
      state = HIGH;       // update variable state to HIGH
    }
    timer = millis();
  }

  if (millis() > timer + 5000)
  {
    if (state == HIGH) {
      Serial.println("Motion stopped!");
      ledTurnOff();
      state = LOW;       // update variable state to LOW
    }
    timer = millis();
  }

  analogRead_value = analogRead(mAnalogPin);  // read analog value of EARTH
  digitalRead_value = digitalRead(mDigitalPin);// read digital value of EARTH

  if (millis() > m_timer + 1000) {
    Serial.println("Analog Value : ");
    Serial.println(analogRead_value);
    Serial.println("Digital Value : ");
    Serial.println(digitalRead_value);
    moistureLedOn();
    m_timer = millis();
  }
}

void onRghbledColorChange()  {

  for (int i = 17; i < N_LEDS; i++)
  {
    strip.setPixelColor(i,
                        strip.ColorHSV(rghbledColor.getValue().hue * 180,
                                       rghbledColor.getValue().sat * 2.5,
                                       rghbledColor.getValue().bri * 2.5));
    strip.show();
    delay(50);
  }
}

void onLightSensorValChange()  {

  for (int i = 17; i < N_LEDS; i++)
  {
    strip.setPixelColor(i,
                        strip.ColorHSV(rghbledColor.getValue().hue * 180,
                                       rghbledColor.getValue().sat * 2.5,
                                       lightSensorVal * 2.5));
    strip.show();
    delay(50);
  }
}

void ledTurnOn() {
  onLightSensorValChange();
}

void ledTurnOff() {
  for (int i = 17; i < N_LEDS; i++)
  {
    strip.setPixelColor(i, 0, 0, 0);
    strip.show();
    delay(50);
  }
}

void moistureLedOn() {
  if (analogRead_value < 3000)
  {
    for (int i = 0; i < 17; i++)
    {
      strip.setPixelColor(i, 0, 0, 255);
      strip.show();
      delay(25);
    }
  } else {
    for (int i = 0; i < 17; i++)
    {
      strip.setPixelColor(i, 255, 0, 0);
      strip.show();
      delay(25);
    }
  }
}
