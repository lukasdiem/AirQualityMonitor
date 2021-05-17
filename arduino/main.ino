#include <Button2.h>

#include <Button2.h>

/**
 * Adapted from: https://draeger-it.blog/arduino-lektion-113-umweltsensor-bme680-fuer-rel-luftfeuchtigkeit-luftdruck-temperatur-und-luftqualitaet/?cn-reloaded=1
 * */
//#include <h>

#include <Button2.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <NTPClient.h>

#include "src/BME680/BME680.h"
#include "src/BME680/BME680History.h"

// Pin definitions
#define LED_PIN 13 // GPIO13 (D7)
#define NUM_LEDS 5

#define OLED_RESET 16

#define SDA 4
#define SCL 5

#define BUTTON_LEFT 12
#define BUTTON_RIGHT 14

#define SEALEVELPRESSURE_HPA (1013.25)

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

WiFiManager wifiManager;

// Create an object of the class Bsec
LD::BME680 iaqSensor;
LD::BME680History iaqSensorHistory(timeClient);

//Adafruit_BME680 bme; // I2C
Adafruit_SSD1306 display(128, 32, &Wire, OLED_RESET);

// Initialize the Buttons
Button2 btnLeft = Button2(BUTTON_LEFT);
Button2 btnRight = Button2(BUTTON_RIGHT);

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel pixels(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

uint8_t ledBrightness = 30;
uint8_t ledBrightnessStep = 5;

void setup()
{  
  // Disable wifi to save power
  /*WiFi.mode(WIFI_OFF);
  WiFi.forceSleepBegin();
  delay(1);*/

  // Initialize the button callbacks
  btnLeft.setReleasedHandler(buttonLeftCallback);
  btnRight.setReleasedHandler(buttonRightCallback);

  // set the SDA, SCL ports (I2C)
  Wire.begin(SDA, SCL);

  Serial.begin(19200);
  while (!Serial);
  Serial.println(F("BME680 test"));

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C, true, true);
  display.clearDisplay();
  display.display();
  delay(1000);

  iaqSensor.begin(0x77, Wire);

  // Print the header
  String output = "Timestamp [ms], raw temperature [*C], pressure [hPa], raw relative humidity [%], gas [Ohm], IAQ, IAQ accuracy, temperature [°C], relative humidity [%], Static IAQ, CO2 equivalent, breath VOC equivalent";
  Serial.println(output);

  // Initialize NeoPixel
  pixels.begin();

  // start the wifi manager
  String apID = String("Dufte-") + ESP.getChipId();
  Serial.println("Access point id: " +  apID);

  //wifiManager.resetSettings();
  //wifiManager.setWiFiAPHidden(false);
  wifiManager.autoConnect(apID.c_str());

  // Wifi is ready here
  timeClient.begin();
  timeClient.setTimeOffset(3600); // GMT+1
  
  iaqSensor.setUpdateCallback(sensorUpdate);
}

void loop()
{
  timeClient.update();

  // DEBUG
  Serial.println("DateTime: " + timeClient.getFormattedTime());

  //wifiManager.process();

  static unsigned long last_time = 0;

  btnLeft.loop();
  btnRight.loop();

  /*if (btnLeft.pressed())
  {
    ledBrigthness = min(255, ledBrightness + ledBrightnessStep);
    Serial.println("Left Pressed");
  }

  if (btnRight.pressed())
  {
    ledBrigthness = max(0, ledBrightness - ledBrightnessStep);
    Serial.println("Right Pressed");
  }*/

  if (iaqSensor.update())
  {
    String output = "Data: ";
    output += ", " + String(iaqSensor.rawTemperature);
    output += ", " + String(iaqSensor.pressure);
    output += ", " + String(iaqSensor.rawHumidity);
    output += ", " + String(iaqSensor.gasResistance);
    output += ", " + String(iaqSensor.iaq);
    output += ", " + String(iaqSensor.iaqAccuracy);
    output += ", " + String(iaqSensor.temperature);
    output += ", " + String(iaqSensor.humidity);
    output += ", " + String(iaqSensor.staticIaq);
    output += ", " + String(iaqSensor.co2Equivalent);
    output += ", " + String(iaqSensor.breathVocEquivalent);
    Serial.println(output);

    display.setTextSize(1);
    display.setTextColor(WHITE);

    String temp = "T: " + String(iaqSensor.temperature, 1) + "C";
    temp += " | H: " + String(iaqSensor.humidity, 1) + "%";
    display.setCursor(0, 0);
    display.println(temp);

    String scoreString = "IAQ: " + String(iaqSensor.iaq, 0);
    display.setCursor(0, 10);
    display.println(scoreString);

    display.setCursor(0, 20);
    display.println("Static IAQ: " + String(iaqSensor.staticIaq, 0));

    display.display();

    // Set the led bar
    setLedBar(iaqSensor.iaq);
  }

  display.clearDisplay();

  // DEBUG
  sensorUpdate(iaqSensor);

  delay(100);
}

void sensorUpdate(LD::BME680& sensor)
{
  Serial.println("a");
  iaqSensorHistory.update(sensor);
  Serial.println("b");
  serializeJsonPretty(iaqSensorHistory.toJson(), Serial);
}

void buttonLeftCallback(Button2& btn)
{
  ledBrightness = max(0, ledBrightness - ledBrightnessStep);
  Serial.println("Brightness Down: " + ledBrightness);
}

void buttonRightCallback(Button2& btn)
{
  ledBrightness = min(255, ledBrightness + ledBrightnessStep);
  Serial.println("Brightness Up: " + ledBrightness);
}

void setLedBar(int iaq)
{
  int colorLedExcellent = pixels.Color(0, 255, 0);
  int colorLedGood = pixels.Color(150, 200, 0);
  int colorLedLightlyPolluted = pixels.Color(255, 255, 0);
  int colorLedModeratelyPolluted = pixels.Color(255, 130, 0);
  int colorLedHeavilyPolluted = pixels.Color(255, 0, 0);
  int colorLedSeverelyPolluted = pixels.Color(150, 0, 80);

  pixels.clear();
  pixels.setPixelColor(0, colorLedExcellent);
  if (iaq > 50)
    pixels.setPixelColor(1, colorLedGood);
  if (iaq > 100)
    pixels.setPixelColor(2, colorLedLightlyPolluted);
  if (iaq > 150)
    pixels.setPixelColor(3, colorLedModeratelyPolluted);
  if (iaq > 200)
    pixels.setPixelColor(4, colorLedHeavilyPolluted);

  pixels.setBrightness(ledBrightness);
  pixels.show();
}
