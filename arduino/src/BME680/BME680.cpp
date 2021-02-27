#include "BME680.h"
#include <EEPROM.h>

namespace LD
{
    void BME680::begin(uint8_t i2cAddr, TwoWire &i2c)
    {
        sensor.begin(0x77, Wire);
        //Serial.println(output);

        String output = "\nBSEC library version " + String(sensor.version.major) + "." + String(sensor.version.minor) + "." + String(sensor.version.major_bugfix) + "." + String(sensor.version.minor_bugfix);
        Serial.println(output);
        printSensorStatus();

        // Set the configuration from memory
        sensor.setConfig(getSensorConfig());
        printSensorStatus();

        // Load a saved state from the eeprom
        loadState();

        // Set the subscriptions for the sensor values of interest
        sensor.updateSubscription(sensorSubscriptionList, sizeof(sensorSubscriptionList), sampleRate);
        printSensorStatus();
    }

    uint8_t* BME680::getSensorConfig()
    {
        // get the sample rate
        String stringSampleRate = "config/generic_33v";
        if (sampleRate == BSEC_SAMPLE_RATE_LP)
            stringSampleRate += "3s";
        else if (sampleRate == BSEC_SAMPLE_RATE_ULP)
            stringSampleRate += "300s";
    
        stringSampleRate += "4d/bsec_iaq.txt";

        // load the config array
        uint8_t bsecConfig[] = {
            #include "config/generic_33v_3s_4d/bsec_iaq.txt"
        };

        return bsecConfig;
    }

    bool BME680::update()
    {
        if (sensor.status != BSEC_OK || sensor.bme680Status != BME680_OK)
        {
            printSensorStatus();
            return false;
        }

        bool newData = sensor.run();
        if (newData) 
        { 
            // set all values
            iaq = sensor.iaq;
            rawTemperature = sensor.rawTemperature;
            pressure = sensor.pressure;
            rawHumidity = sensor.rawHumidity;
            gasResistance = sensor.gasResistance;            
            temperature = sensor.temperature;
            humidity = sensor.humidity;
            staticIaq = sensor.staticIaq;
            co2Equivalent = sensor.co2Equivalent;
            breathVocEquivalent = sensor.breathVocEquivalent;
            compGasValue = sensor.compGasValue;
            gasPercentage = sensor.gasPercentage;

            // accuracy states
            iaqAccuracy = sensor.iaqAccuracy;
            staticIaqAccuracy  = sensor.staticIaqAccuracy;
            co2Accuracy = sensor.co2Accuracy;
            breathVocAccuracy = sensor.breathVocAccuracy;
            compGasAccuracy = sensor.compGasAccuracy;
            gasPercentageAcccuracy = sensor.gasPercentageAcccuracy;
        }

        return newData;
    }

    void BME680::printSensorStatus()
    {
        String output;
        if (sensor.status != BSEC_OK)
        {
            if (sensor.status < BSEC_OK)
            {
                output = "BSEC error code : " + String(sensor.status);
                Serial.println(output);
            }
            else
            {
                output = "BSEC warning code : " + String(sensor.status);
                Serial.println(output);
            }
        }

        if (sensor.bme680Status != BME680_OK)
        {
            if (sensor.bme680Status < BME680_OK)
            {
                output = "BME680 error code : " + String(sensor.bme680Status);
                Serial.println(output);
            }
            else
            {
                output = "BME680 warning code : " + String(sensor.bme680Status);
                Serial.println(output);
            }
        }
    }

    #pragma region "Save/Load state from the EEPROM"
    void BME680::loadState(void)
    {
        if (EEPROM.read(0) == BSEC_MAX_STATE_BLOB_SIZE)
        {
            // Existing state in EEPROM
            Serial.println("Reading state from EEPROM");

            for (uint8_t i = 0; i < BSEC_MAX_STATE_BLOB_SIZE; i++)
            {
                sensorState[i] = EEPROM.read(i + 1);
                Serial.println(sensorState[i], HEX);
            }

            sensor.setState(sensorState);
            //checksensorStatus();
        }
        else
        {
            // Erase the EEPROM with zeroes
            Serial.println("Erasing EEPROM");

            for (uint8_t i = 0; i < BSEC_MAX_STATE_BLOB_SIZE + 1; i++)
                EEPROM.write(i, 0);

            EEPROM.commit();
        }
    }

    void BME680::updateSavedState(void)
    {
        bool update = false;
        /* Set a trigger to save the state. Here, the state is saved every STATE_SAVE_PERIOD with the first state being saved once the algorithm achieves full calibration, i.e. iaqAccuracy = 3 */
        if (stateUpdateCounter == 0)
        {
            if (sensor.iaqAccuracy >= 3)
            {
                update = true;
                stateUpdateCounter++;
            }
        }
        else
        {
            /* Update every stateSaveIntervalMs milliseconds */
            if ((stateUpdateCounter * stateSaveIntervalMs) < millis())
            {
                update = true;
                stateUpdateCounter++;
            }
        }

        if (update)
        {
            sensor.getState(sensorState);
            //checksensorStatus();

            Serial.println("Writing state to EEPROM");

            for (uint8_t i = 0; i < BSEC_MAX_STATE_BLOB_SIZE; i++)
            {
                EEPROM.write(i + 1, sensorState[i]);
                Serial.println(sensorState[i], HEX);
            }

            EEPROM.write(0, BSEC_MAX_STATE_BLOB_SIZE);
            EEPROM.commit();
        }
    }
    #pragma endregion "Save/Load state from the EEPROM"
    
} // namespace LD