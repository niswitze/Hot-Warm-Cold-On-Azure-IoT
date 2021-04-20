using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace IoTWebApp.MessageStores
{
    public interface IMessageStore<DataType>
    {
        /// <summary>
        /// Gets one or more messages from a message store.
        /// </summary>
        /// <param name="query">Query to filter what messages are returned. If no query is provided, a default query should be implemented.</param>
        /// <returns>A generic list of messages or data</returns>
        Task<DataType> GetItemsAsync(string query = null);
    }
}
