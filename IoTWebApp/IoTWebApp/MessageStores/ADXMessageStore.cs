using Kusto.Data.Common;
using Microsoft.Extensions.Configuration;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace IoTWebApp.MessageStores
{
    public class ADXMessageStore<DataType> : IMessageStore<DataType> where DataType : IList<string>
    {
        private readonly ICslQueryProvider _queryProvider;
        private readonly IConfiguration _configuration;

        // Default query if the query parameter for method GetItemsAsync is not provided
        private string _distinctDeviceQueryString = "TemperatureData | where TelemetryType==\"temperature\" and EnqueuedTime > ago(24h) | summarize arg_max(EnqueuedTime, *) by DeviceId | project DeviceId, EnqueuedTime, Temperature";


        public ADXMessageStore(ICslQueryProvider queryProvider, IConfiguration configuration)
        {
            _queryProvider = queryProvider;
            _configuration = configuration;
        }


        /// <summary>
        /// Gets one or more messages from a message store.
        /// </summary>
        /// <param name="query">Query to filter what messages are returned. If no query is provided, the default query will be executed ->
        /// TemperatureData | where TelemetryType==\"temperature\" and EnqueuedTime > ago(32d) | summarize arg_max(EnqueuedTime, *) by DeviceId | project DeviceId, EnqueuedTime, Temperature</param>
        /// <returns>A generic list of messages or data</returns>
        public async Task<DataType> GetItemsAsync(string query = null)
        {
            // The below code and logic has been refactored from -> https://github.com/Azure/azure-kusto-samples-dotnet/blob/master/client/HelloKusto/Program.cs

            // It is strongly recommended that each request has its own unique
            // request identifier. This is mandatory for some scenarios (such as cancelling queries)
            // and will make troubleshooting easier in others.
            var clientRequestProperties = new ClientRequestProperties() { ClientRequestId = Guid.NewGuid().ToString() };

            IList<string> allRecords = new List<string>();

            if (query == null)
            {
                query = _distinctDeviceQueryString;
            }

            try
            {
                using (var reader = await _queryProvider.ExecuteQueryAsync(_configuration["ADXDatabseName"], query, clientRequestProperties))
                {
                    while (reader.Read())
                    {
                        // Important note: For demonstration purposes we show how to read the data
                        // using the "bare bones" IDataReader interface. In a production environment
                        // one would normally use some ORM library to automatically map the data from
                        // IDataReader into a strongly-typed record type (e.g. Dapper.Net, AutoMapper, etc.)
                        string deviceId = reader.GetString(0);
                        string enqueuedTime = reader.GetDateTime(1).ToLocalTime().ToString();
                        string temperature = reader.GetDouble(2).ToString();

                        string finalMessage = "{\"deviceId\": \""+ deviceId + "\", \"enqueuedTime\": \""+ enqueuedTime + "\", \"temperature\": \""+ temperature + "\"}";

                        allRecords.Add(finalMessage);
                    }
                }
            }
            catch (Exception exc)
            {

            }

            return (DataType)allRecords;

        }
    }
}
