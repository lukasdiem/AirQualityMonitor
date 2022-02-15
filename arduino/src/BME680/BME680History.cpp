#include "BME680History.h"

namespace LD
{
    BME680History::BME680History(NTPClient &timeClient)
    {
        this->timeClient = &timeClient;
    }

    void BME680History::update(BME680 &sensor)
    {
        // add the current timestamp
        unsigned long now = 0;
        if (timeClient != nullptr)
        {
            now = timeClient->getEpochTime();
        }
        timestamp.push(now);

        /*if (iaq.size() > 0)
            iaq.push(iaq.last() + 0.1);
        else
            iaq.push(10.3);*/

        // set all values
        iaq.push(sensor.iaq);
        rawTemperature.push(sensor.rawTemperature);
        pressure.push(sensor.pressure);
        rawHumidity.push(sensor.rawHumidity);
        gasResistance.push(sensor.gasResistance);
        temperature.push(sensor.temperature);
        humidity.push(sensor.humidity);
        staticIaq.push(sensor.staticIaq);
        co2Equivalent.push(sensor.co2Equivalent);
        breathVocEquivalent.push(sensor.breathVocEquivalent);
        compGasValue.push(sensor.compGasValue);
        gasPercentage.push(sensor.gasPercentage);

        // accuracy states
        iaqAccuracy.push(sensor.iaqAccuracy);
        staticIaqAccuracy.push(sensor.staticIaqAccuracy);
        co2Accuracy.push(sensor.co2Accuracy);
        breathVocAccuracy.push(sensor.breathVocAccuracy);
        compGasAccuracy.push(sensor.compGasAccuracy);
        gasPercentageAcccuracy.push(sensor.gasPercentageAcccuracy);
    }

    DynamicJsonDocument  BME680History::toJson()
    {
        DynamicJsonDocument doc(2048);
        copyArray(timestamp, doc.createNestedArray("timestamp"));
        copyArray(iaq, doc.createNestedArray("iaq"));
        copyArray(pressure, doc.createNestedArray("pressure"));
        copyArray(temperature, doc.createNestedArray("temperature"));
        /*copyArray(rawTemperature, doc.createNestedArray("rawTemperature"));
        copyArray(humidity, doc.createNestedArray("humidity"));
        copyArray(rawHumidity, doc.createNestedArray("rawHumidity"));
        copyArray(staticIaq, doc.createNestedArray("staticIaq"));
        copyArray(co2Equivalent, doc.createNestedArray("co2Equivalent"));
        copyArray(breathVocEquivalent, doc.createNestedArray("breathVocEquivalent"));
        copyArray(compGasValue, doc.createNestedArray("compGasValue"));
        copyArray(gasResistance, doc.createNestedArray("gasResistance"));
        copyArray(gasPercentage, doc.createNestedArray("gasPercentage"));

        // accuracy states
        copyArray(iaqAccuracy, doc.createNestedArray("iaqAccuracy"));
        copyArray(staticIaqAccuracy, doc.createNestedArray("staticIaqAccuracy"));
        copyArray(co2Accuracy, doc.createNestedArray("co2Accuracy"));
        copyArray(breathVocAccuracy, doc.createNestedArray("breathVocAccuracy"));
        copyArray(compGasAccuracy, doc.createNestedArray("compGasAccuracy"));
        copyArray(gasPercentageAcccuracy, doc.createNestedArray("gasPercentageAcccuracy"));*/

        return doc;
    }

    void BME680History::fromJson(DynamicJsonDocument &data)
    {
        // TODO: Read the values from memory
    }

    void BME680History::saveData()
    {
        // TODO: Read the values from memory
    }

    void BME680History::loadData()
    {
        // TODO: Read the values from memory
    }

    void BME680History::copyArray(Uint8Buffer &buffer, JsonArray dst)
    {
        for (int i = 0; i < buffer.size(); i++)
        {
            dst.add(buffer[i]);
        }
    }

    void BME680History::copyArray(FloatBuffer &buffer, JsonArray dst)
    {
        for (int i = 0; i < buffer.size(); i++)
        {
            dst.add(buffer[i]);
        }
    }

    void BME680History::copyArray(TimeBuffer &buffer, JsonArray dst)
    {
        for (int i = 0; i < buffer.size(); i++)
        {
            dst.add(buffer[i]);
        }
    }
}