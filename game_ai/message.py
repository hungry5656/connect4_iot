
from enum import Enum
from copy import deepcopy

class msgTemplate():
    def toShadowTemplate():
        retTemp = {
            "messageType": "move/cmd",
            "currTurnIdx": "0/1",
            "message": "text..."
        }
        return deepcopy(retTemp)
    
    def toServerTemplate():
        retTemp = {
            "messageType": "move/cmd",
            "shadowName": "Batman/Joker",
            "message": "text..."
        }
        return deepcopy(retTemp)
