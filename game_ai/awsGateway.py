import boto3
import time
import sensitiveConfig
import json

# Parameters
QUEUE_MSG_WAIT_TIME = 20 # how many seconds to wait before the next message

class awsGateway():

    def __init__(self):
        # connect to a sqs queue
        self.sqs = boto3.client('sqs')
        self.queueUrl = sensitiveConfig.QUEUE_URL
        self.playerClient = boto3.client('iot-data')


    def receiveMsgFromSQS(self):
        while(True):
            try:
                response = self.sqs.receive_message(QueueUrl=self.queueUrl, WaitTimeSeconds=QUEUE_MSG_WAIT_TIME)
                break
            except:
                exit(1)
                

        # If a message is received, print it
        try:
            if response['Messages']:
                message = response['Messages'][0]
                messageBody = message['Body']
                receiptHandle = message['ReceiptHandle']
                self.sqs.delete_message(
                    QueueUrl=self.queueUrl,
                    ReceiptHandle=receiptHandle
                )
                return messageBody, 0
        except:
            print('No messages received')
            return "ERROR", -1

    def sendMsgToIoT(self, name, message):
        print("DEBUG: sending msg to {name} with message: {message}")
        response = self.playerClient.update_thing_shadow(
            thingName=name,
            payload=json.dumps(message)
        )
        # receiveMsg to purge the SQS queue
        msgBody, idx = self.receiveMsgFromSQS()

        return idx


instance = awsGateway()

