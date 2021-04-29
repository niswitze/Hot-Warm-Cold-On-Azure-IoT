// Physical device information for board and sensor
#define DEVICE_ID "ESP8226-TS1"
#define DHTTYPE DHT11

// Pin layout configuration
#define LED_PIN 16
#define TEMP_PIN 14

// Interval time(ms) for sending messages
int WAIT_INTERVAL = 5000;

// Telemetry type this device will send
#define TELEMETRY_TYPE "temperature"

//Default data rate to use for Serial
#define DATA_RATE 115200

// EEPROM address configuration
#define EEPROM_SIZE 512

// SSID and SSID password's length should < 32 bytes
// http://serverfault.com/a/45509
#define SSID_LEN 32
#define PASS_LEN 32
#define CONNECTION_STRING_LEN 256

#define MESSAGE_MAX_LEN 256
