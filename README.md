---
page_type: sample
languages:
  - Python
  - C#
  - C
products:
  - azure-active-directory
  - azure-iot-hub
  - azure-data-explorer
  - app-service
  - azure-signal-r
name: Create Hot, Warm, and Cold data paths using Azure IoT Hub, App Service, Functions, SignalR, and Data Explorer
urlFragment: Hot-Warm-Cold-On-Azure-IoT
---
# Create Hot, Warm, and Cold data paths using Azure IoT Hub, App Service, Functions, SignalR, and Data Explorer

 1. [Overview](#overview)
 1. [Scenario](#scenario)
 1. [Contents](#contents)
 1. [Prerequisites](#prerequisites)
 2. [Setup](#setup)
 3. [About the code](#about-the-code)
 4. [More information](#more-information)
 5. [Community Help and Support](#community-help-and-support)
 6. [Contributing](#contributing)

## Overview
This sample demonstrates how to create Hot, Warm, and Cold data paths using [Azure IoT Hub](https://docs.microsoft.com/en-us/azure/iot-hub/about-iot-hub) or [IoT Edge (version 1.1)](https://docs.microsoft.com/en-us/azure/iot-edge/about-iot-edge?view=iotedge-2018-06), [App Service](https://docs.microsoft.com/en-us/azure/app-service/), [Functions](https://docs.microsoft.com/en-us/azure/azure-functions/), [SignalR](https://docs.microsoft.com/en-us/azure/azure-signalr/signalr-overview), and [Data Explorer](https://docs.microsoft.com/en-us/azure/data-explorer/).

## Scenario

1. The ESP8226 connects either directly to an Azure IoT Hub or IoT Edge enabled gateway and submits temperature data produced by a DHT11 sensor.
2. This temperature data is then ingested from Azure IoT Hub to an Azure Function and Data explorer.
3. The Azure Data Explorer instance ingests all temperature data for long-term storage
4. The Azure Function broadcasts all temperature data to an Azure SignalR instance
   1. This Azure Function also sends out a text alert if the temperature data starts to fall below a certain threshold
5. The Azure SignalR instance broadcasts temperature data to all clients listening on a WebSocket based connection
6. The App Service hosts an ASP.NET MVC, .NET 5 based, web app that displays the last 24 hours worth of temperature data from Azure Data Explorer
   1. This web app also creates a WebSock connection to the Azure SignalR instance to receive temperature data in real time as well

![Overview](./ReadmeFiles/topology.png)

## Contents

| File/folder       | Description                                |
|-------------------|--------------------------------------------|
| `IotFunctionApp/`| The Azure Function App that submits all temperature data to SignalR & alerts if the temperature data is below a certain threshold.|
| `IoTHub-TempSensor/`| The ESP8226 device code for connecting and sending temperature data to either Azure IoT Hub or IoT Edge.|
| `IoTWebApp/`| The web app for the Azure App Service that displays the last 24 hours worth of temperature data store in Data Explorer and all temperature data sent through the SignalR instance.|
| `CODE_OF_CONDUCT.md` | Guidelines for contributing to the sample. |
| `CONTRIBUTING.md` | Guidelines for contributing to the sample. |
| `LICENSE`         | The license for the sample.                |

## Prerequisites

The below prerequisites are needed for local development:

- [Python 3.8](https://www.python.org/downloads/)
- [Arduino IDE 1.8.13](https://www.arduino.cc/en/software)
- ESP8266
- DHT11 Sensor
- [Visual Studio Code](https://code.visualstudio.com/download)
- [Visual Studio](https://visualstudio.microsoft.com/downloads/)
  - This prerequisite is only needed if you are not comfortable using Visual Studio Code for C# or ASP.NET MVC development
- [Azure Data Explorer](https://docs.microsoft.com/en-us/azure/data-explorer/create-cluster-database-portal)
- [Azure IoT Hub](https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-create-through-portal)
- [Azure SignalR](https://docs.microsoft.com/en-us/azure/azure-signalr/signalr-quickstart-azure-functions-csharp#create-an-azure-signalr-service-instance)
  - Ensure the ServiceMode setting for this resource is set to <strong>Serverless</strong>
- Azure AD
  - Specifically, you will need enough acces to create and modify an App Registration

The below prerequisites are only needed for production use in Azure:

- [Azure App Service for .NET 5](https://docs.microsoft.com/en-us/azure/app-service/quickstart-dotnetcore?tabs=net50&pivots=development-environment-vs#create-an-aspnet-web-app)
- [Python based Azure Function App](https://docs.microsoft.com/en-us/azure/azure-functions/functions-create-function-app-portal)
- [Azure Key Vault](https://docs.microsoft.com/en-us/azure/key-vault/general/quick-create-portal)

## Setup

### Step 1: Clone or download this repository

From your shell or command line:

```console
    git clone https://github.com/niswitze/Hot-Warm-Cold-On-Azure-IoT.git
```

or download and extract the repository .zip file.

> :warning: To avoid path length limitations on Windows, we recommend cloning into a directory near the root of your drive.

### Step 2: Configure Prerequisites

#### Visual Studio Code
#### Visual Studio
#### Azure IoT Hub
#### Azure Data Explorer 
#### Azure SignalR
#### Arduino IDE
#### ESP8226 

### Step 3: Configure Demo Applications

#### IoTFunctionApp
#### IoTWebApp
#### IoTHub-TempSensor

### Step 4 (Optional): Configure & Deploy Demo Applications to Azure

#### Azure Key Vault
#### Azure Managed Identities
#### IoTFunctionApp
#### IoTWebApp
#### Azure Function App
#### Azure App Service
#### Azure SignalR



> :information_source: Did the sample not work for you as expected? Then please reach out to us using the [GitHub Issues](../../../issues) page.

## About the code

## Community Help and Support

If you find a bug in the sample, raise the issue on [GitHub Issues](../../../issues).

To provide feedback on or suggest features for Azure Active Directory, visit [User Voice page](https://feedback.azure.com/forums/321918-azure-iot-hub-dps-sdks).

## Contributing

If you'd like to contribute to this sample, see [CONTRIBUTING.MD](/CONTRIBUTING.md).

This project has adopted the [Microsoft Open Source Code of Conduct](https://opensource.microsoft.com/codeofconduct/). For more information, see the [Code of Conduct FAQ](https://opensource.microsoft.com/codeofconduct/faq/) or contact [opencode@microsoft.com](mailto:opencode@microsoft.com) with any additional questions or comments.
