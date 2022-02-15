#pragma once

#include <bsec.h>

namespace LD
{
    class BME680
    {
    private:
        Bsec sensor;

        uint8_t sensorState[BSEC_MAX_STATE_BLOB_SIZE] = {0};
        uint16_t stateUpdateCounter = 0;

        void loadState();
        void updateSavedState();
        uint8_t* getSensorConfig();

        void printSensorStatus(void);

        typedef void (*CallbackFunction)(BME680 &);
        
        CallbackFunction update_callback = NULL;

    public:
        #pragma region "Parameters"
        bsec_virtual_sensor_t sensorSubscriptionList[10] = {
            BSEC_OUTPUT_RAW_TEMPERATURE,
            BSEC_OUTPUT_RAW_PRESSURE,
            BSEC_OUTPUT_RAW_HUMIDITY,
            BSEC_OUTPUT_RAW_GAS,
            BSEC_OUTPUT_IAQ,
            BSEC_OUTPUT_STATIC_IAQ,
            BSEC_OUTPUT_CO2_EQUIVALENT,
            BSEC_OUTPUT_BREATH_VOC_EQUIVALENT,
            BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE,
            BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY,
        };

        float       sampleRate = BSEC_SAMPLE_RATE_LP;
        uint32_t    stateSaveIntervalMs = 360 * 60 * 1000; // 360 minutes - 4 times a day
        #pragma endregion "Parameters"

        #pragma region "Sensor Values"
        float iaq = 0; 
        float rawTemperature = 0;
        float pressure = 0;
        float rawHumidity = 0;
        float gasResistance = 0;
        //float stabStatus = 0;
        //float runInStatus = 0;
        float temperature = 0;
        float humidity = 0;
        float staticIaq = 0;
        float co2Equivalent = 0;
        float breathVocEquivalent = 0;
        float compGasValue = 0;
        float gasPercentage = 0;
        
        uint8_t iaqAccuracy = 0;
        uint8_t staticIaqAccuracy = 0; 
        uint8_t co2Accuracy = 0;
        uint8_t breathVocAccuracy = 0;
        uint8_t compGasAccuracy = 0;
        uint8_t gasPercentageAcccuracy = 0;

        #pragma endregion "Sensor Values"

        #pragma region "Methods"
        /**
         * @brief Initialize the sensor
         */
        void begin(uint8_t i2cAddr, TwoWire &i2c);

        /**
         * @brief Update the sensor values. Returns true if new sensor values are available.
         */
        bool update();

        /**
         * @brief Callback that is called if new sensor values are read
         */
        void setUpdateCallback(CallbackFunction f);
        #pragma endregion "Methods"
    };    
} // namespace LD
