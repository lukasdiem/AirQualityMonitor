#include "BME680.h"
#include <ArduinoJson.h>
#include <CircularBuffer.h>
#include <NTPClient.h>

#ifndef BUFFER_SIZE
    #define BUFFER_SIZE 3
#endif

namespace LD
{
    typedef CircularBuffer<float, BUFFER_SIZE> FloatBuffer;
    typedef CircularBuffer<unsigned long, BUFFER_SIZE> TimeBuffer;
    typedef CircularBuffer<uint8_t, BUFFER_SIZE> Uint8Buffer;

    class BME680History
    {
        private:
            NTPClient* timeClient;

            TimeBuffer timestamp;

            FloatBuffer iaq; 
            FloatBuffer rawTemperature;
            FloatBuffer pressure;
            FloatBuffer rawHumidity;
            FloatBuffer gasResistance;
            FloatBuffer temperature;
            FloatBuffer humidity;
            FloatBuffer staticIaq;
            FloatBuffer co2Equivalent;
            FloatBuffer breathVocEquivalent;
            FloatBuffer compGasValue;
            FloatBuffer gasPercentage;
            
            Uint8Buffer iaqAccuracy;
            Uint8Buffer staticIaqAccuracy; 
            Uint8Buffer co2Accuracy;
            Uint8Buffer breathVocAccuracy;
            Uint8Buffer compGasAccuracy;
            Uint8Buffer gasPercentageAcccuracy;

            void copyArray(FloatBuffer &buffer, JsonArray dst);
            void copyArray(Uint8Buffer &buffer, JsonArray dst);
            void copyArray(TimeBuffer &buffer, JsonArray dst);

        public:
            BME680History(NTPClient& timeClient);

            void update(BME680& sensor);

            DynamicJsonDocument toJson();

            void fromJson(DynamicJsonDocument &data);
    };
}