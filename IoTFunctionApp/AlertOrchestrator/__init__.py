# This function is not intended to be invoked directly. Instead it will be
# triggered by an HTTP starter function.
# Before running this sample, please:
# - create a Durable activity function (default name is "Hello")
# - create a Durable HTTP starter function
# - add azure-functions-durable to requirements.txt
# - run pip install -r requirements.txt

#refactored example of the human interaction pattern from -> https://docs.microsoft.com/en-us/azure/azure-functions/durable/durable-functions-overview?tabs=python#human

import azure.durable_functions as df
import json, logging, os
from datetime import timedelta 

def orchestrator_function(context: df.DurableOrchestrationContext):

    phoneNumber = os.getenv("OnCallPhoneNumber")

    body = {"instanceId": context.instance_id, "phoneNumber": phoneNumber}

    yield context.call_activity("SendAlertActivity", body)

    due_time = context.current_utc_datetime + timedelta(minutes=5)
    durable_timeout_task = context.create_timer(due_time)
    alert_accepted_event_task = context.wait_for_external_event("AlertAccepted")

    winning_task = yield context.task_any([alert_accepted_event_task, durable_timeout_task])

    if alert_accepted_event_task == winning_task:
        logging.info('The alert has been accepted')
        durable_timeout_task.cancel()
    else:
        logging.info('The alert has timed out')

main = df.Orchestrator.create(orchestrator_function)