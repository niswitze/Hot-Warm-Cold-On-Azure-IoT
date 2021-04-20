# This function is not intended to be invoked directly. Instead it will be
# triggered by an orchestrator function.
# Before running this sample, please:
# - create a Durable orchestration function
# - create a Durable HTTP starter function
# - add azure-functions-durable to requirements.txt
# - run pip install -r requirements.txt

import logging, json, os
import azure.functions as func

def main(body, twilioMessage: func.Out[str])  -> str:

    #constructing URI that will trigger the AlertAccepted event which will stop the alert orchestration
    send_event_uri = os.getenv("SendEventUri").format(instanceId = body["instanceId"])

    payload = {
        "body": f"Alert has been triggered. Click the following link to acknowledge this alert {send_event_uri}",
        "to": body["phoneNumber"]
    }

    #using the twilio output binding to send a text message alert to
    twilioMessage.set(json.dumps(payload))

    return "Message Sent"
