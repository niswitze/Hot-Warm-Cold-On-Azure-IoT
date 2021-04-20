using IoTWebApp.MessageStores;
using Kusto.Data;
using Kusto.Data.Common;
using Kusto.Data.Net.Client;
using Microsoft.AspNetCore.Builder;
using Microsoft.AspNetCore.Hosting;
using Microsoft.AspNetCore.HttpsPolicy;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using Polly;
using Polly.Extensions.Http;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Http;
using System.Threading.Tasks;

namespace IoTWebApp
{
    public class Startup
    {
        public Startup(IConfiguration configuration, IWebHostEnvironment env)
        {
            Configuration = configuration;
            _env = env;
        }

        public IConfiguration Configuration { get; }
        private IWebHostEnvironment _env;


        // This method gets called by the runtime. Use this method to add services to the container.
        public void ConfigureServices(IServiceCollection services)
        {
            services.AddControllersWithViews();

            KustoConnectionStringBuilder kcsb;
            string adxClusterURL = $"https://{Configuration["ADXClusterName"]}.{Configuration["ADXClusterRegion"]}.kusto.windows.net/{Configuration["ADXDatabseName"]}";

            // Before running this sample, ensure you have given either your system managed identity or app registration enough permissions to access the ADX cluster and database
            // Information on how to do this can be found here -> https://docs.microsoft.com/en-us/azure/data-explorer/provision-azure-ad-app
            if (_env.IsDevelopment())
            {
                // This creates an instance of a Kusto connection string builder using an app registration
                kcsb = new KustoConnectionStringBuilder(adxClusterURL).WithAadApplicationKeyAuthentication(Configuration["ApplicationClientId"], Configuration["ApplicationSecret"], Configuration["ApplicationAuthority"]);
            }
            else
            {
                // This creates an instance of a Kusto connection string builder using a system mananged identity
                kcsb = new KustoConnectionStringBuilder(adxClusterURL).WithAadManagedIdentity("system");
            }

            ICslQueryProvider queryProvider = KustoClientFactory.CreateCslQueryProvider(kcsb);

            services.AddSingleton(queryProvider);

            services.AddSingleton<IMessageStore<IList<string>>, ADXMessageStore<IList<string>>>();

            // This creates an injected http client with a retry policy 
            IAsyncPolicy <HttpResponseMessage> retryPolicy = GetRetryPolicy();

            TimeSpan httpTimeSpan = TimeSpan.FromMinutes(5);

            services.AddHttpClient("signalRConnectionInfo", c =>
            {
                c.BaseAddress = new Uri(Configuration["SignalRConnectionFunctionURI"]);
                c.DefaultRequestHeaders.Add("Accept", "application/json");
            })
            .SetHandlerLifetime(httpTimeSpan)
            .AddPolicyHandler(retryPolicy);

        }

        // This method gets called by the runtime. Use this method to configure the HTTP request pipeline.
        public void Configure(IApplicationBuilder app, IWebHostEnvironment env)
        {
            if (env.IsDevelopment())
            {
                app.UseDeveloperExceptionPage();
            }
            else
            {
                app.UseExceptionHandler("/Home/Error");
                // The default HSTS value is 30 days. You may want to change this for production scenarios, see https://aka.ms/aspnetcore-hsts.
                app.UseHsts();
            }
            app.UseHttpsRedirection();
            app.UseStaticFiles();

            app.UseRouting();

            app.UseAuthorization();

            app.UseEndpoints(endpoints =>
            {
                endpoints.MapControllerRoute(
                    name: "default",
                    pattern: "{controller=Temperature}/{action=Index}/{id?}");
            });
        }

        private static IAsyncPolicy<HttpResponseMessage> GetRetryPolicy()
        {
            return HttpPolicyExtensions
                .HandleTransientHttpError()
                .OrResult(msg => msg.StatusCode == System.Net.HttpStatusCode.NotFound)
                .OrResult(msg => msg.StatusCode == System.Net.HttpStatusCode.Unauthorized)
                .WaitAndRetryAsync(4, retryAttempt => TimeSpan.FromSeconds(Math.Pow(2, retryAttempt)));
        }
    }
}
