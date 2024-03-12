
from enum import Enum
from copy import deepcopy
import gameConstant

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
        tempMsg = message["state"]["reported"]
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
        tempMsg = message["state"]["reported"]
        if tempMsg["sender"] == 1:
            return -1, None
        else:
            if tempMsg["messageType"] != "cmd":
                return 0, None
            else:
                return 0, tempMsg["message"]