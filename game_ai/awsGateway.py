import boto3

# Parameters
QUEUE_MSG_WAIT_TIME = 20 # how many seconds to wait before the next message

class awsGateway():
    def __init__(self):
        # TODO: open aws_data_file, a sensitive private file that should be hide using .gitignore
        # e.g. QueueUrl

        # connect to a sqs queue
        self.sqs = boto3.client('sqs')
        self.queueUrl = self.sqs.create_queue(QueueName='my-queue')['QueueUrl']
        self.playerClient = [None] * 2


    def receiveMsgFromSQS(self):
        message = self.sqs.receive_message(QueueUrl=self.queueUrl, WaitTimeSeconds=QUEUE_MSG_WAIT_TIME)

        # If a message is received, print it
        if message['Messages']:
            print(message['Messages'][0]['Body'])
            return message, 0

        # Otherwise, wait for a message to arrive
        else:
            print('No messages received')
            return "", -1

    def sendMsgToIoT(self):
        # TODO: need to do research on how to send message from aws iot shadow to CC3200 using MQTT
        #       Possibly using AWS IoT SDK for embedded C
        print("sendMsgToIoT")
        return 0

    def linkPlayer(self, shadowName, position):
        if (self.playerClient[position] != None):
            print("ERROR: the player is already assigned")
            return -1
        self.playerClient[position] = boto3.client('iot-data')
        
        # TODO: map shadow_name to iot_data client

        return 0

