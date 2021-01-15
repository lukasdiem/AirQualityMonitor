/**
 * Copied from: https://draeger-it.blog/arduino-lektion-113-umweltsensor-bme680-fuer-rel-luftfeuchtigkeit-luftdruck-temperatur-und-luftqualitaet/?cn-reloaded=1
 * */
#include <heltec.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"

#define SDA 4
#define SCL 5

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME680 bme; // I2C

#define OLED_RESET 4


float hum_score, gas_score;
float gas_reference = 250000;
float hum_reference = 40;
int   getgasreference_count = 0;

void setup() {
  // set the SDA, SCL ports
  Wire.begin(SDA, SCL);
  

  Serial.begin(9600);
  while (!Serial);
  Serial.println(F("BME680 test"));

  Heltec.begin(true /*DisplayEnable Enable*/, false /*Serial Enable*/);
  Heltec.display->clear();

  delay(2000);
  
  if (!bme.begin(0x77)) {
    Serial.println(F("Der Sensor BME680 konnte nicht gefunden werden!"));
    //while (1);
  }

  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms
}

void loop() {
  if (!bme.performReading()) {
    Serial.println(F("Failed to perform reading :("));
    delay(3000);
    return;
  }

  delay(1000);
  
  String temp = "T: " + String(bme.temperature, 2) + "C";
  temp += " | H: " + String(bme.humidity, 2) + "%";
  Heltec.display->drawString(0, 0, temp);

  /*String gas = "Gas: ";
  gas += String(bme.gas_resistance / 1000.0, 2);
  gas += " KOhms";
  Heltec.display->drawString(0, 16, gas);*/

  float iaqScore = CalculateAirQualityScore();
  String airQuality = CalculateIAQ(iaqScore);

  String scoreString = "Score: " + String(iaqScore, 2);
  Heltec.display->drawString(0, 10, scoreString);

  Heltec.display->drawString(0, 20, airQuality);

  Heltec.display->display();
  Heltec.display->clear();
}

float CalculateAirQualityScore() {
  float current_humidity = bme.readHumidity();
  if (current_humidity >= 38 && current_humidity <= 42)
    hum_score = 0.25*100; // Humidity +/-5% around optimum 
  else
  { //sub-optimal
    if (current_humidity < 38) 
      hum_score = 0.25/hum_reference*current_humidity*100;
    else
    {
      hum_score = ((-0.25/(100-hum_reference)*current_humidity)+0.416666)*100;
    }
  }
  
  //Calculate gas contribution to IAQ index
  float gas_lower_limit = 5000;   // Bad air quality limit
  float gas_upper_limit = 50000;  // Good air quality limit 
  if (gas_reference > gas_upper_limit) gas_reference = gas_upper_limit; 
  if (gas_reference < gas_lower_limit) gas_reference = gas_lower_limit;
  gas_score = (0.75/(gas_upper_limit-gas_lower_limit)*gas_reference -(gas_lower_limit*(0.75/(gas_upper_limit-gas_lower_limit))))*100;
  float air_quality_score = hum_score + gas_score;
  if ((getgasreference_count++)%10==0) GetGasReference(); 
  
  return air_quality_score;
}

void GetGasReference(){
  int readings = 10;
  for (int i = 1; i <= readings; i++){
    gas_reference += bme.readGas();
  }
  gas_reference = gas_reference / readings;
}

String CalculateIAQ(float score) {
  String IAQ_text = "Air quality is ";
  score = (100 - score) * 5;
  if      (score >= 301)                  IAQ_text += "Hazardous";
  else if (score >= 201 && score <= 300 ) IAQ_text += "Very Unhealthy";
  else if (score >= 176 && score <= 200 ) IAQ_text += "Unhealthy";
  else if (score >= 151 && score <= 175 ) IAQ_text += "Unhealthy for Sensitive Groups";
  else if (score >=  51 && score <= 150 ) IAQ_text += "Moderate";
  else if (score >=  00 && score <=  50 ) IAQ_text += "Good";
  else IAQ_text += "Unknown (" + String(score, 2) + ")";
  return IAQ_text;
}