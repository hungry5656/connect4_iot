# import boto3
import csv
import pandas as pd
import warnings
import boto3
warnings.filterwarnings('ignore')

def storeGameResult(shadowName, result):
    df = pd.read_csv("accountSys.csv", index_col=0)
    df.loc[shadowName][result] += 1
    df.to_csv("accountSys.csv", index=True)
                

def showGameResult(shadowName):
    df = pd.read_csv("accountSys.csv", index_col=0)
    df.loc[shadowName]
    df.to_csv("accountSys.csv", index=True)
    msg = f"Hi Player {shadowName},\r\n\r\n"
    msg += "Here is a summary of your stats in connect4 game\r\n\r\n"
    msg += f"You won {df.loc[shadowName][0]} times\r\n"
    msg += f"You lost {df.loc[shadowName][1]} times\r\n"
    msg += f"You tied {df.loc[shadowName][2]} times\r\n"
    return msg

def sendToSNS(shadowName, msg):
    sns_client = boto3.client('sns', region_name='us-east-1')
    topic_arn = 'arn:aws:sns:us-east-1:851725326245:' + shadowName + '_Message'
    print(topic_arn)

    # Publish the message to the specified SNS topic
    response = sns_client.publish(
        TopicArn=topic_arn,
        Message=msg
    )

    # Check the response for any errors
    if 'MessageId' in response:
        print("Message published successfully with message ID:", response['MessageId'])
    else:
        print("Failed to publish message:", response)

# storeGameResult("Joker", 1)
# sendToSNS("Joker", showGameResult("Joker"))

