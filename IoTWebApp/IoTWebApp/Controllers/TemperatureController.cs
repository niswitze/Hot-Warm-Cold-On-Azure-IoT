using IoTWebApp.Models;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Logging;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Net.Http;
using System.Threading.Tasks;
using IoTWebApp.MessageStores;

namespace IoTWebApp.Controllers
{
    public class TemperatureController : Controller
    {
        private readonly ILogger<TemperatureController> _logger;
        private static HttpClient _connectorInfohttpClient;
        private readonly IMessageStore<IList<string>> _messageStore;
        
        public TemperatureController(ILogger<TemperatureController> logger, IHttpClientFactory clientFactory, IMessageStore<IList<string>> messageStore)
        {
            _logger = logger;
            _connectorInfohttpClient = clientFactory.CreateClient("signalRConnectionInfo");
            _messageStore = messageStore;
        }

        public IActionResult Index()
        {
            return View();
        }

        [ResponseCache(Duration = 0, Location = ResponseCacheLocation.None, NoStore = true)]
        public async Task<JsonResult> GetSignalRConnectionInfo()
        {
            // This sends an HTTP request to the Azure Function deployed with an Azure SignalR output binding
            // and will collect the URI and access token needed to connect to an Azure SignalR instance
            var connectorInfoResponse = await _connectorInfohttpClient.GetAsync(_connectorInfohttpClient.BaseAddress.AbsoluteUri);

            var responseContent = await connectorInfoResponse.Content.ReadAsStringAsync();

            return Json(responseContent);
        }

        [ResponseCache(Duration = 0, Location = ResponseCacheLocation.None, NoStore = true)]
        public async Task<JsonResult> GetMessagesFromStore()
        {
            // This retrieves all of the IoT messages stored in a message store
            // For this sample, the message store is Azure Data Explorer
            IList<string> messages = await _messageStore.GetItemsAsync();

            return Json(messages);
        }

        [ResponseCache(Duration = 0, Location = ResponseCacheLocation.None, NoStore = true)]
        public IActionResult Error()
        {
            return View(new ErrorViewModel { RequestId = Activity.Current?.Id ?? HttpContext.TraceIdentifier });
        }
    }
}
