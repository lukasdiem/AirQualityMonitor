/*#define ESP8266
#define WIFI_Kit_8
#define OLEDDISPLAYFONTS_h*/

#include <heltec.h>

#include <Wire.h>
#include <SPI.h>
//#include <Adafruit_GFX.h>
//#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"

//#include <bsec.h>


//Adafruit_BME680 bme; // I2C

#define BME_SCK 14
#define BME_MISO 12
#define BME_MOSI 13
#define BME_CS 15

#define SDA 4
#define SCL 5

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME680 bme; // I2C
//Adafruit_BME680 bme(BME_CS); // hardware SPI
//Adafruit_BME680 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK);

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

// Create an object of the class Bsec
//Bsec iaqSensor;
 
void setup() {
  Wire.begin(SDA, SCL);

  Serial.begin(9600);
  while (!Serial);
  Serial.println(F("BME680 test"));

  //Heltec.begin(true /*DisplayEnable Enable*/, false /*Serial Enable*/);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(1000);
  display.clearDisplay();

  delay(2000);

 
  if (!bme.begin(0x77))
  {
    Serial.println("Could not find a valid BME680 sensor, check wiring!");
    //while (1);
  }
 
  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms
}
 
void loop() 
{
  /*display.setTextSize(4);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);

  display.println("Hello World ...");*/

  Heltec.display->clear();
  Heltec.display->
			Heltec.display->drawString(0, 10, "Hello World");
			Heltec.display->display();

  if (! bme.performReading()) 
  {
    Serial.println("Failed to perform reading :(");
    delay(2000);
    return;
  }
  Serial.print("Temperature = ");
  Serial.print(bme.temperature);
  Serial.println(" *C");
 
  Serial.print("Pressure = ");
  Serial.print(bme.pressure / 100.0);
  Serial.println(" hPa");
 
  Serial.print("Humidity = ");
  Serial.print(bme.humidity);
  Serial.println(" %");
 
  Serial.print("Gas = ");
  Serial.print(bme.gas_resistance / 1000.0);
  Serial.println(" KOhms");

  Serial.print("Approx. Altitude = ");
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(" m");
 
  float iaqSimple = log(bme.gas_resistance) + 0.04 * bme.humidity;
  Serial.print("Approx. IAQ = ");
  Serial.print(iaqSimple);
  Serial.println(" ppm");

  Serial.println();
  delay(2000);
}