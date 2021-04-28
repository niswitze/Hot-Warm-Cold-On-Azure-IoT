#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

void initWifi()
{
    // Attempt to connect to Wifi network:

    Serial.printf("Attempting to connect to SSID: %s.\r\n", ssid);

    //disable this device as an access point
    WiFi.mode(WIFI_STA);
        
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED)
    {
        // Get Mac Address and show it.
        // WiFi.macAddress(mac) save the mac address into a six length array, but the endian may be different. The board should
        // start from mac[0] to mac[5], but some other kinds of board run in the oppsite direction.
        uint8_t mac[6];
        WiFi.macAddress(mac);
        Serial.printf("Your device with MAC address %02x:%02x:%02x:%02x:%02x:%02x connects to %s failed! Waiting 10 seconds to retry.\r\n",
                mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], ssid);
        delay(10000);
    }
    Serial.printf("Connected to wifi %s.\r\n", ssid);
}
