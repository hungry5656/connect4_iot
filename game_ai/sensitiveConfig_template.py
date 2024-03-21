# This file is used as a template file that store AWS sensitive info
# please rename this file to sensitiveConfig.py after setting up the params
QUEUE_URL = 'https://sqs.us-east-1.amazonaws.com/example-message-queue' # enter your AWS SQS Url here
SHADOW_NAME = ["Client1", "Client2"] # enter the names of the two shadow in AWS
AWS_SNS_ARN = 'arn:aws:sns:us-east-1:example-number:' # enter your AWS SNS ARN number for email notification