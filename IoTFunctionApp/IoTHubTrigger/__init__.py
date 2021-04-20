from typing import List
import logging, json, os
from .helpers.requests_helper import RequestsHelper

import azure.functions as func
from azure.cosmosdb.table.tableservice import TableService
from azure.cosmosdb.table.models import Entity

async def main(events: List[func.EventHubEvent], outMessage: func.Out[str]):

    orchestration_id = "Null"
    signal_r_messages = []
    temperature_threshold = os.getenv("TemperatureThreshold")
    orchestration_id_entity = {'PartitionKey': 'orchestrationIdPK', 'RowKey': 'orchestrationIdRK','orchestrationId': 'Null', }


    for event in events:

        iot_message = event.get_body().decode('utf8').replace("'", '"')

        logging.info('Python IoT Hub trigger processed an event: %s',
                        iot_message)

        #broadcasting to SignalR clients the message sent from IoT Hub
        outMessage.set(json.dumps({
            'target': 'iotMessages',
            'arguments': [iot_message]
            }))



        #checking to see if the temperature reported from my ESP8266 is lower than my temperature threshold 
        if json.loads(iot_message)['temperature'] < int(temperature_threshold):
            
            table_service = TableService(connection_string=os.getenv("AzureWebJobsStorage"))
            # table_service = TableService(connection_string="AccountName=devstoreaccount1;AccountKey=Eby8vdM02xNOcqFlqUwJPLlmEtlCDXJ1OUzFT50uSRZ6IFsuFq2UVErCz4I6tq/K1SZFPTOtr/KBHBeksoGMGw==;DefaultEndpointsProtocol=http;BlobEndpoint=http://127.0.0.1:10000/devstoreaccount1;QueueEndpoint=http://127.0.0.1:10001/devstoreaccount1;TableEndpoint=http://127.0.0.1:10002/devstoreaccount1;")
            try:
                #checking to see if the table and orchestration id record exist
                orchestration_id = table_service.get_entity(
                table_name=os.getenv("TableStorageName"), 
                partition_key=os.getenv("TableStoragePartitionKey"), 
                row_key=os.getenv("TableStorageRowKey"),
                select="orchestrationId"
                ).orchestrationId
            except Exception as exception:
                #checking to see if the exception only occured due to the orchestration id record not already existing
                if exception.status_code == 404:
                    logging.info("Table entity to hold Alerting orchestration ID does not exist. Creating now.")
                    table_service.insert_entity(os.getenv("TableStorageName"), orchestration_id_entity)
                else:
                    logging.error(f"An error occured when performing the table lookup: {str(exception)}")
            
            if orchestration_id == "Null":
                #calling the HttpTriggerDurable function to start the alert orchestration
                orchestator_response = RequestsHelper.get_backend_api_session().get(os.environ.get("StartOrchestrationUri")).json()
                orchestration_id_entity["orchestrationId"] = orchestator_response["id"]
                
                #updating the orchestration id record with the latest orchestration id 
                table_service.merge_entity(os.getenv("TableStorageName"), orchestration_id_entity)
            else:
                #calling the durable function's API to review the progress of the current orchestration, i.e. text alert
                orchestator_status_response = RequestsHelper.get_backend_api_session().get(
                    os.environ.get("OrchestratioStatusnUri").format(instanceId=orchestration_id)
                    ).json()
                
                if orchestator_status_response["runtimeStatus"] != "Completed":
                    logging.info(f"Orchestration {orchestration_id} is still running and waiting for alert acceptance.")
                else:
                    logging.info(f"Orchestration {orchestration_id} has completed. Removing Orchestration {orchestration_id} value from table storage.")
                    orchestration_id_entity["orchestrationId"] = "Null"
                    table_service.merge_entity(os.getenv("TableStorageName"), orchestration_id_entity)
