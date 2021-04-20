import logging

import azure.functions as func


async def main(req: func.HttpRequest, connectionInfoJson: str) -> func.HttpResponse:
    logging.info('Python SignalR HTTP trigger function processed a request for connection info.')

    headers = {'Content-type': 'application/json'}
    
    return func.HttpResponse(connectionInfoJson, status_code=200, headers=headers)
