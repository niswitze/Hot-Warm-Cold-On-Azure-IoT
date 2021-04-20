#include "DHT.h"

bool dhtStarted = false;

float readTemperature()
{
    DHT dht(TEMP_PIN, DHTTYPE);
   
    if(dhtStarted == false)
    {
      dht.begin();
    }
    
    float fahrenheitTemp = dht.readTemperature(true);

    // Check if any reads failed and exit early (to try again).
    if (isnan(fahrenheitTemp)) {
      Serial.println("Failed to read from DHT sensor!");
      return 0.00;
    }
    
    Serial.print("Temperature = ");
    Serial.println(fahrenheitTemp);

    return fahrenheitTemp;
}
