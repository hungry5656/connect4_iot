
from enum import Enum
from copy import deepcopy
import gameConstant
import json

ToShadowTemp = {
    "sender": 1,
    "messageType": "move/cmd",
    "currTurnIdx": "0/1",
    "message": "text..."
}
ToServerTemp = {
    "sender": 0,
    "messageType": "move/cmd",
    "shadowName": "Batman/Joker",
    "message": "text..."
}

class messageUtil():
    def getToShadowTemplate():
        return deepcopy(ToShadowTemp)
    
    def getToServerTemplate():
        return deepcopy(ToServerTemp)
    
    def buildPollingMsg(msgType):
        tempMsg = {
            'state': {
                'reported': messageUtil.getToShadowTemplate()
            }
        }
        tempMsg["state"]["reported"]["sender"] = 0
        tempMsg["state"]["reported"]["messageType"] = msgType
        tempMsg["state"]["reported"]["shadowName"] = "AI"
        tempMsg["state"]["reported"]["message"] = ""
        return tempMsg

    def buildToShadowMsg(msgType, currTurnIdx, message):
        tempMsg = {
            'state': {
                'reported': messageUtil.getToShadowTemplate()
            }
        }
        tempMsg["state"]["reported"]["messageType"] = msgType
        tempMsg["state"]["reported"]["currTurnIdx"] = currTurnIdx
        tempMsg["state"]["reported"]["message"] = message
        return tempMsg
    
    def parseMoveMsg(message, expectedName):
        messageJson = json.loads(message)
        print(messageJson)
        tempMsg = messageJson["reported"]
        if tempMsg["sender"] == 1:
            return -1, None
        elif tempMsg["shadowName"] != expectedName:
            return -1, None
        else:
            if tempMsg["messageType"] == "cmd" and tempMsg["message"] != gameConstant.QUIT_GAME:
                return 0, None
            else:
                return 0, tempMsg["message"]
    
    def parseCMDMsg(message):
        messageJson = json.loads(message)
        print(messageJson)
        tempMsg = messageJson["reported"]
        print("DEBUG: parsing...................")
        print(tempMsg["sender"])
        if tempMsg["sender"] == 1:
            # print("fjdaskljdfsklajfsklajfkldsajfldas11111111111111111111111111111111")
            return -1, None
        else:
            if tempMsg["messageType"] != "cmd":
                # print("fjdaskljdfsklajfsklajfkldsajfldas2222222222222222222222222222222")
                return -1, None
            else:
                # print("fjdaskljdfsklajfsklajfkldsajfldas33333333333333333333333333333333")
                return 0, tempMsg
