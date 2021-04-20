//This function is used to collect data from the records displayed in the device and message table
const getTableData = () => {
    let table = $("#deviceTable");

    let data = [];
    table.find('tr').each(function (rowIndex, r) {

        if (rowIndex !== 0) {
            let cols = [];
            $(this).find('td').each(function (colIndex, c) {
                cols.push(c.textContent);
            });
            data.push(cols);
        }
    });

    let tableData = data.map((currentValue) => { return currentValue.toString().split(",") });

    return tableData;
}

// This function is used to either update an existing or add a new record in the device and message table
const updateIotDeviceTable = (messageData) => {

    let tableData = getTableData();
    let addRecord = true;
    let currentTime;
    const noDeviceMessage = "No device messages are coming through right now...";

    let tableRows = document.getElementById("deviceTable").rows;

    if (messageData["enqueuedTime"] === undefined)
    {
        let currentDate = new Date();
        currentTime = currentDate.toLocaleString();
    }
    else
    {
        currentTime = messageData["enqueuedTime"];
    }

    if (tableRows[1].innerText === noDeviceMessage)
    {
        document.getElementById("deviceTable").deleteRow(1);
    }

    tableData.forEach((rows, index) => {
        let deviceId = rows[0].toString().split(" ")[0];

        if (messageData["deviceId"] === deviceId)
        {

            let tableRowsCells = tableRows[index + 1];

            tableRowsCells.cells[0].innerHTML = messageData["deviceId"];
            tableRowsCells.cells[1].innerHTML = messageData["temperature"];
            tableRowsCells.cells[2].innerHTML = currentTime;

            addRecord = false;
        }
    });

    if (addRecord)
    {
        let table = document.getElementById("deviceTable");
        let row = table.insertRow(-1);

        let deviceId = row.insertCell(0);
        let temperature = row.insertCell(1);
        let lastUpdateTime = row.insertCell(2); 

        deviceId.innerHTML = messageData["deviceId"];
        deviceId.id = "deviceId";
        deviceId.setAttribute("style", "min-width: 80px;", 0);
        deviceId.setAttribute("scope", "col", 0);

        temperature.innerHTML = messageData["temperature"];
        temperature.id = "temperatureId"
        temperature.setAttribute("style", "min-width: 80px;", 0);
        temperature.setAttribute("scope", "col", 0);

        lastUpdateTime.innerHTML = currentTime;
        lastUpdateTime.id = "messageReceivedTime";
        lastUpdateTime.setAttribute("style", "min-width: 80px;", 0);
        lastUpdateTime.setAttribute("scope", "col", 0);

    }
}

const connectionStatusUpdate = (text, color) => {

    // This updates the text and color of the connection display
    document.getElementById("signalRStatus").innerHTML = text;
    document.getElementById("signalRStatus").style.color = color;
    document.getElementById("signalRStatus").style.fontWeight = "bold";
};

const signalRConnection = (data) => {
    let jsonData = JSON.parse(data);

    const accessToken = jsonData["accessToken"];
    const connectionUrl = jsonData["url"];

    // This creates client for connecting to an Azure SignalR instance
    const connection = new signalR.HubConnectionBuilder()
        .withUrl(connectionUrl, { accessTokenFactory: () => accessToken })
        .withAutomaticReconnect()
        .build()

    // This registers a handler for all incoming messages
    connection.on('iotMessages', (messageData) => {
        const messageJSONData = JSON.parse(messageData);
        updateIotDeviceTable(messageJSONData);

    });

    // This registers a handler for all incoming messages
    connection.onclose(() => {
        connectionStatusUpdate("Closed", "red");
    });

    // This registers a handler for reconnections
    connection.onreconnecting((err) => {
        connectionStatusUpdate("Reconnecting", "yellow");
    });

    // This registers a handler for when this client has connected
    connection.onreconnected((connectionId) => {
        connectionStatusUpdate("Connected", "green");
    });

    // This creats a WebhSocket connection and registers a handler for any errors
    connection
        .start()
        .catch(() => {
            connectionStatusUpdate("Closed", "red");

        });;
};

const getConnectionInformation = () => {

    // This executes a GET HTTP request for a URI and access token to connect to Azure SignalR
    $.get("/Temperature/GetSignalRConnectionInfo")
        .done(function (data) {
            signalRConnection(data);
            connectionStatusUpdate("Connected", "green");
        })
        .fail(function () {
            connectionStatusUpdate("Closed", "red");
            console.log("Error:");
        })
        .always(function () {
            console.log("Finished attempt to connect to SignalR Hub");
        });
};

const getMessagesFromStore = () => {

    // This executes a GET HTTP request to pull and display an initial set of messages
    $.get("/Temperature/GetMessagesFromStore")
        .done(function (data) {

            for (i = 0; i < data.length; i++) {

                let jsonData = JSON.parse(data[i]);

                updateIotDeviceTable(jsonData);

            }
        })
        .fail(function () {
            alert("ERROR: Could not retrieve database messages")
        })
        .always(function () {
            console.log("Finished attempt to get messages from database");
        });
};

$(document).ready(function () {

    // This will pull and display an initial set of messages when the document has finished loading
    getMessagesFromStore();

    // This will request the URI and access token for as well as connect to Azure SignalR
    getConnectionInformation();
});


