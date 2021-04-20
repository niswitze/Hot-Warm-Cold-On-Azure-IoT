/* 

Licensed under the MIT license. See LICENSE file in the project root for full license information.

THIS CODE IS FOR DEMONSTRATION PURPOSES ONLY.

This sample has been refactored from the following sources:
  * https://github.com/Azure/azure-iot-arduino/tree/master/examples/esp8266/iothub_ll_telemetry_sample
  * https://github.com/Azure/azure-iot-sdk-c/tree/master/iothub_client/samples/iothub_convenience_sample
  * https://github.com/Azure-Samples/iot-hub-feather-huzzah-client-app

Please use the below software and corresponding versions for this demo:

  1. Arduino IDE - version 1.8.13 or greater -> https://www.arduino.cc/en/software
  2. ESP8266 platform package - version 2.7.4 -> https://github.com/esp8266/Arduino/tree/master
  3. AzureIoTHub Library - version 1.5.0
  4. AzureIoTProtocol_MQTT Library - version 1.5.0
  5. AzureIoTUtility Library - version 1.5.0
  6. ArduinoJson Library - version 6.17.3

For all users who plane to either use this sample or the AzureIoTHub library with Azure IoT Hub directly, you will need to modify the build.extra_flags value 
in the esp8266's platform.txt file to be -DESP8266 -DDONT_USE_UPLOADTOBLOB -DUSE_BALTIMORE_CERT

The end results should look like the following -> build.extra_flags=-DESP8266 -DDONT_USE_UPLOADTOBLOB -DUSE_BALTIMORE_CERT

For all users who plan to either use this sample or the AzureIoTHub library with a field gateway using Azure IoT Edge, the root CA certificate must be included in 
the AzureIoTHub library's cert file stored in AzureIoTHub\src\certs\certs.c. In this file there will be an array called certificates that you will need to add your root CA certificate to. 
The exact entry you will need to add to the end of this array will look like the following:

#if defined(USE_IOT_EDGE_CERT)
"-----BEGIN CERTIFICATE-----\r\n"
"MIIFjTCCA3WgAwIBAgIUSvSX/SnFAEbZgleuWw3sQwz8DIwwDQYJKoZIhvcNAQEL\r\n"
"BQAwKjEoMCYGA1UEAwwfQXp1cmVfSW9UX0h1Yl9DQV9DZXJ0X1Rlc3RfT25seTAe\r\n"
"Fw0yMDExMDUyMDE2NTJaFw0yMDEyMDUyMDE2NTJaMCoxKDAmBgNVBAMMH0F6dXJl\r\n"
...
"-----END CERTIFICATE-----\r\n"
#endif

Then you will need to modify the build.extra_flags value in the esp8266's platform.txt file to be -DESP8266 -DDONT_USE_UPLOADTOBLOB -DUSE_IOT_EDGE_CERT

The end results should look like the following -> build.extra_flags=-DESP8266 -DDONT_USE_UPLOADTOBLOB -DUSE_IOT_EDGE_CERT


NOTE!) If you receive the below error while compiling, please ensure the line #define round(x) in Arduino.h, under esp8266\2.7.4\cores\esp8266, has been commented out.

    ERROR: expected unqualified-id before '(' token
    
    #define round(x)     ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))

                    ^
This is a known issue and this workaround is documented here -> https://github.com/esp8266/Arduino/issues/5787
*/

#include "config.h"

#include <AzureIoTHub.h>
#include <AzureIoTProtocol_MQTT.h>
#include <iothubtransportmqtt.h>
#include <ArduinoJson.h>


IOTHUB_MESSAGE_HANDLE messageHandle;
IOTHUB_DEVICE_CLIENT_LL_HANDLE deviceHandle;

char *ssid;
char *pass;
char *connectionString;
static char temperatureMessage[80];


/* -- initIoTHub --
 * Method to initialize our instance of the Azure IoT Hub SDK.
*/
static void initIoTHub()
{
  //SDK init
  IoTHub_Init();
  IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol = MQTT_Protocol;
  deviceHandle = IoTHubDeviceClient_LL_CreateFromConnectionString(connectionString, protocol);
  
  if (deviceHandle == NULL)
  {
      Serial.println("Error AZ002: Failure creating IoT Hub device. Hint: Check you connection string.");
  }
  else
  {       

      /* Turning off diagnostic sampling */
      int diagOff = 0;
      IoTHubDeviceClient_LL_SetOption(deviceHandle, OPTION_DIAGNOSTIC_SAMPLING_PERCENTAGE, &diagOff);

      /* Setting device twin call back, so we can receive updates from the device's digital twin. */
      IoTHubDeviceClient_LL_SetDeviceTwinCallback(deviceHandle, deviceTwinCallback, NULL);

      bool urlEncodeOn = true;
      IoTHubDeviceClient_LL_SetOption(deviceHandle, OPTION_AUTO_URL_ENCODE_DECODE, &urlEncodeOn);

     /* Setting Message call back, so we can receive C2D messages. */
      if (IoTHubClient_LL_SetMessageCallback(deviceHandle, receiveMsgCallback, NULL) != IOTHUB_CLIENT_OK)
      {
          Serial.println("ERROR: IoTHubClient_LL_SetMessageCallback..........FAILED!");
      }
      else
      {
          Serial.println("IoTHubClient_LL_SetMessageCallback..........PASSED!");
      }
      
      /* Setting connection status callback, so we can receive determine if we've connected to Azure IoT Hub. */
      IoTHubDeviceClient_LL_SetConnectionStatusCallback(deviceHandle, connectionStatusCallback, NULL);
   
  }

}

/* -- connectionStatusCallback --
 * Callback method which executes on receipt of a connection status message from Azure IoT Hub.
*/
static void connectionStatusCallback(IOTHUB_CLIENT_CONNECTION_STATUS result, IOTHUB_CLIENT_CONNECTION_STATUS_REASON reason, void* userContext)
{
    (void)reason;
    (void)userContext;
    
    if (result == IOTHUB_CLIENT_CONNECTION_AUTHENTICATED)
    {
        #if defined(USE_IOT_EDGE_CERT)
           Serial.println("The device client is connected to Edge Gateway");
        #else
           Serial.println("The device client is connected to IoT Hub");
        #endif
        
    }
    else
    {
        #if defined(USE_IOT_EDGE_CERT)
           Serial.println("The device client is disconnected from Edge Gateway");
        #else
           Serial.println("The device client is disconnected from the IoT Hub");
        #endif

        delay(10000);
    }
}

/* -- receiveMsgCallback --
 * Callback method which executes on receipt of a C2D message from Azure IoT Hub.
*/
static IOTHUBMESSAGE_DISPOSITION_RESULT receiveMsgCallback(IOTHUB_MESSAGE_HANDLE  message, void* userContext)
{
    (void)userContext;
    const char* messageId;
    const char* correlationId;

    //Properties of the D2C Message
    if ((messageId = IoTHubMessage_GetMessageId(message)) == NULL)
    {
        messageId = "<unavailable>";
    }

    if ((correlationId = IoTHubMessage_GetCorrelationId(message)) == NULL)
    {
        correlationId = "<unavailable>";
    }

    IOTHUBMESSAGE_CONTENT_TYPE contentType = IoTHubMessage_GetContentType(message);
    if (contentType == IOTHUBMESSAGE_BYTEARRAY)
    {
        const unsigned char* buffMsg;
        size_t buffLen;

        if (IoTHubMessage_GetByteArray(message, &buffMsg, &buffLen) != IOTHUB_MESSAGE_OK)
        {
            Serial.println("Failure retrieving byte array message");
        }
        else
        {
            Serial.printf("Received Binary message\r\nMessage ID: %s\r\n Correlation ID: %s\r\n Data: <<<%.*s>>> & Size=%d\r\n", messageId, correlationId, (int)buffLen, buffMsg, (int)buffLen);
        }
    }
    else
    {
        const char* stringMsg = IoTHubMessage_GetString(message);
        if (stringMsg == NULL)
        {
            Serial.println("Failure retrieving byte array message");
        }
        else
        {
            Serial.printf("Received String Message\r\nMessage ID: %s\r\n Correlation ID: %s\r\n Data: <<<%s>>>\r\n", messageId, correlationId, stringMsg);
        }
    }

/*  


    //The following code is only required if you need to read specific properties off a C2D message
    
    const char* property_value = "interval";
    const char* property_key = IoTHubMessage_GetProperty(message, property_value);
    if (property_key != NULL)
    {
        Serial.printf("\r\nMessage Properties:\r\n");
        Serial.printf("\tKey: %s Value: %s\r\n", property_value, property_key);
    }
    
*/
    return IOTHUBMESSAGE_ACCEPTED;
}

/* -- deviceTwinCallback --
 * Callback method which executes when the device's digital twin is updated in Azure IoT Hub.
*/
static void deviceTwinCallback(DEVICE_TWIN_UPDATE_STATE updateState, const unsigned char* payLoad, size_t size, void* userContextCallback)
{
    (void)updateState;
    (void)size;

    char reportedPropertiesMessage[80];
    DynamicJsonDocument jsonDocument(MESSAGE_MAX_LEN);
    DeserializationError error = deserializeJson(jsonDocument, payLoad);
    if (error)
    {
        Serial.printf("Parsing Failed for Device Twin");
        return;
    }

    //updateState will contain the full device twin on startup
    if (updateState == DEVICE_TWIN_UPDATE_COMPLETE)
    {
        Serial.println("Received Complete Device Twin Update.");
        if (jsonDocument["desired"]["interval"].isNull() == false)
        {
          Serial.print("Interval property found in Device Twin. Value: ");
          Serial.println((int)jsonDocument["desired"]["interval"]);
          WAIT_INTERVAL = (int)jsonDocument["desired"]["interval"];
        }
        else
        {
            Serial.println("No interval property found in Device Twin.");
        }
    }
    else
    {
        Serial.println("Received Partial Device Twin Update.");
        if (jsonDocument["interval"].isNull() == false)
        {
          Serial.print("Interval property found in Device Twin. Value: ");
          Serial.println((int)jsonDocument["interval"]);
          WAIT_INTERVAL = (int)jsonDocument["interval"];
        }
        else
        {
            Serial.println("No interval property found in Device Twin.");
        }
    }

    Serial.print("WAIT_INTERVAL now set to: ");
    Serial.println(WAIT_INTERVAL);

    sprintf_s(reportedPropertiesMessage, sizeof(reportedPropertiesMessage), "{\"interval\":%d}", WAIT_INTERVAL);

    /* Sending reported properties back to the device's digital twin in Azure IoT Hub. */
    IoTHubDeviceClient_LL_SendReportedState(deviceHandle, (const unsigned char*)reportedPropertiesMessage, strlen(reportedPropertiesMessage), reportedStateCallback, NULL);

}

/* -- reportedStateCallback --
 * Callback method which executes when the device's digital twin's reported properties are updated in Azure IoT Hub.
*/
static void reportedStateCallback(int statusCode, void* userContextCallback)
{
    (void)userContextCallback;
    
    //Getting a 204 for status_code means the properties were updated
    Serial.printf("Device Twin reported properties update completed with result: %d\r\n", statusCode);
}

/* -- sendConfirmCallback --
 * Callback method which executes upon confirmation that a message originating from this device has been received by Azure IoT Hub.
 */
static void sendConfirmCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback)
{
    (void)userContextCallback;
    
    if (IOTHUB_CLIENT_CONFIRMATION_OK == result)
    {
        Serial.println("Message sent to Azure IoT Hub");
    }
    else
    {
        Serial.println("Failed to send message to Azure IoT Hub");
    }
    
}

/* -- initTime --
 * Method to sync time.
*/
static void initTime()
{
    time_t epochTime;
    configTime(0, 3600, "pool.ntp.org", "time.nist.gov");

    while (true)
    {
        epochTime = time(NULL);

        if (epochTime == 0)
        {
            Serial.println("Fetching NTP epoch time failed! Waiting 2 seconds to retry.");
            delay(2000);
        }
        else
        {
            Serial.printf("Fetched NTP epoch time is: %lu.\r\n", epochTime);
            break;
        }
    }
}

void setup(){
    pinMode(LED_PIN, OUTPUT);
    initSerial();
    readCredentials();
    initWifi();
    initTime();
    initIoTHub();
}

void loop(){

    //create telemetry message to send as a D2C message to Azure IoT Hub
    sprintf_s(temperatureMessage, sizeof(temperatureMessage), "{\"deviceId\":\"%s\",\"temperature\":%.2f,\"telemetryType\":\"%s\" }", DEVICE_ID, readTemperature(), TELEMETRY_TYPE);
    
    if ((messageHandle = IoTHubMessage_CreateFromString(temperatureMessage)) == NULL)
    {
        Serial.println("ERROR: messageHandle is NULL");
    }
    else
    { 
        //These sections are commented for demo purposes only. Message and Correlation ID values should be unique per message.
//        
//        IoTHubMessage_SetMessageId(messageHandle, "NISWITZE_MESSAGE_ID");
//        IoTHubMessage_SetCorrelationId(messageHandle, "NISWITZE_CORRELATION_ID");
        IoTHubMessage_SetContentTypeSystemProperty(messageHandle, "application%2fjson");
        IoTHubMessage_SetContentEncodingSystemProperty(messageHandle, "utf-8");
        IoTHubMessage_SetProperty(messageHandle, "telemetryType", TELEMETRY_TYPE);


        if (IoTHubDeviceClient_LL_SendEventAsync(deviceHandle, messageHandle, sendConfirmCallback, NULL) != IOTHUB_CLIENT_OK)
        {
            Serial.println("ERROR: IoTHubModuleClient_LL_SendEventAsync..........FAILED!");
        }
        else
        {
            Serial.println("IoTHubModuleClient_LL_SendEventAsync accepted message for transmission to IoT Hub.");
        }
    }
    
    IoTHubMessage_Destroy(messageHandle);
    
    IoTHubDeviceClient_LL_DoWork(deviceHandle);

    delay(WAIT_INTERVAL);

    /*
     * Ideally, if you're planning for your loop to exit at any time, make sure to call IoTHubDeviceClient_LL_Destroy(deviceHandle) and IoTHub_Deinit() before the loop fully exits.
     * This will ensure the AzureIoTHub SDK exits gracefully.
    */
    
}
