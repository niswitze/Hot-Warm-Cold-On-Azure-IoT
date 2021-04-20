# This function an HTTP starter function for Durable Functions.
# Before running this sample, please:
# - create a Durable orchestration function
# - create a Durable activity function (default name is "Hello")
# - add azure-functions-durable to requirements.txt
# - run pip install -r requirements.txt

#refactored example of the async HTTP APIs pattern from -> https://docs.microsoft.com/en-us/azure/azure-functions/durable/durable-functions-overview?tabs=python#async-http
 
import logging

import azure.functions as func
import azure.durable_functions as df


async def main(req: func.HttpRequest, starter: str) -> func.HttpResponse:
    client = df.DurableOrchestrationClient(starter)

    #getting the existingInstanceId query parameter. This will be created in code when an alert is triggered
    existingInstanceId = req.params.get("existingInstanceId")

    if existingInstanceId:
        #raising event to let the AlertOrchestrator function know an alert has been accepted
        await client.raise_event(existingInstanceId, "AlertAccepted", True)

        #returning a response a user will see in their browser after they have accepted an alert
        return func.HttpResponse(body="Alert Accepted")
    else:

        #starting a new alert orchestration
        instance_id = await client.start_new("AlertOrchestrator", None, None)

        logging.info(f"Started orchestration with ID = '{instance_id}'.")

        return client.create_check_status_response(req, instance_id)