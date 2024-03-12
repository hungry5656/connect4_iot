import argparse
from connect4 import connect4
from players import human2, stupidAI, randomAI, human, minimaxAI, alphaBetaAI, humanIoT
from montecarlo import monteCarloAI
import awsGateway
from message import messageUtil
import gameConstant

parser = argparse.ArgumentParser(description='Run programming assignment 1')
parser.add_argument('-w', default=6, type=int, help='Rows of game')
parser.add_argument('-l', default=7, type=int, help='Columns of game')
parser.add_argument('-p1', default='human', type=str,
					help='Player 1 agent. Use any of the following: [human, humanTxt, stupidAI, randomAI, monteCarloAI, minimaxAI, alphaBetaAI]')
parser.add_argument('-p2', default='human', type=str,
					help='Player 2 agent. Use any of the following: [human, humanTxt, stupidAI, randomAI, monteCarloAI, minimaxAI, alphaBetaAI]')
parser.add_argument('-seed', default=0, type=int,
					help='Seed for random algorithms')
parser.add_argument('-visualize', default='True', type=str, help='Use GUI')
parser.add_argument('-verbose', default='True', type=str,
					help='Print boards to shell')
parser.add_argument('-limit_players', default='1,2', type=str,
					help='Players to limit time for. List players as numbers eg [1,2]')
parser.add_argument('-time_limit', default='0.5,0.5', type=str,
					help='Time limits for each player. Must be list of 2 elements > 0. Not used if player is not listed')

# Bools and argparse are not friends
bool_dict = {'True': True, 'False': False}

args = parser.parse_args()

w = args.w
l = args.l

seed = args.seed
visualize = bool_dict[args.visualize]
verbose = bool_dict[args.verbose]
limit_players = args.limit_players.split(',')
for i, v in enumerate(limit_players):
	limit_players[i] = int(v)
time_limit = args.time_limit.split(',')
for i, v in enumerate(time_limit):
	time_limit[i] = float(v)


agents = {'human': human2, 'humanTxt': human, 'stupidAI': stupidAI, 'randomAI': randomAI,
		'monteCarloAI': monteCarloAI, 'minimaxAI': minimaxAI, 'alphaBetaAI': alphaBetaAI}

agents_iot = {
		0: randomAI,
		1: stupidAI,
		2: monteCarloAI,
		3: minimaxAI,
		4: alphaBetaAI
}

def initGameConnection(isMultiplayer, player1, player2):
	# seed = 0 # TODO: need to randomly generate seed
	
	# check if the message is initialize game
	response, errorCode = awsGateway.instance.receiveMsgFromSQS()
	if errorCode != 0:
		return -1
	message, parseCode = messageUtil.parseCMDMsg(message)
	if parseCode != 0:
		return -1
	elif message["messageType"] != "cmd":
		print("ERROR: This is a move msg, which appeared in the wrong place")
		return -1
	elif message["message"] == "START_GAME":
		isMultiplayer = True
		player1 = humanIoT(1, seed, message["shadowName"])
	elif message["message"] in agents_iot: # if the message is one of the five difficulty level
		player2 = agents_iot[message["message"]](2, seed, "AI")
	else:
		print("wrong command, please retry")
		return -1

	if isMultiplayer: # if this is multiplayer, wait for the next player to connect
		message2, errorCode2 = awsGateway.instance.receiveMsgFromSQS()
		if errorCode2 != 0:
			return -1
		elif message2["shadowName"] != message["shadowName"]:
			print("ERROR: Same connection twice")
			return -1
		elif message2["message"] == "START_GAME":
			player2 = humanIoT(2, seed, message["shadowName"])
		else:
			print("ERROR: not a start command")
			return -1
	
	# send the confirmation message
	newMsg1 = messageUtil.buildToShadowMsg("cmd", 0, gameConstant.SET_PLAYER0)
	awsGateway.instance.sendMsgToIoT(player1.playerName, newMsg1)
	if isMultiplayer:
		newMsg2 = messageUtil.buildToShadowMsg("cmd", 1, gameConstant.SET_PLAYER1)
		awsGateway.instance.sendMsgToIoT(player2.playerName, newMsg2)
	return 0

if __name__ == '__main__':
	while(True):
		isMultiplayer = False
		player1 = None
		player2 = None
		if initGameConnection(isMultiplayer, player1, player2) == -1:
			continue

		c4 = connect4(player1, player2, board_shape=(w, l), visualize=visualize,
					limit_players=limit_players, time_limit=time_limit, verbose=verbose)
		c4.play()
