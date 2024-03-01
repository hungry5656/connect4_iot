import argparse
from connect4 import connect4
from players import human2, stupidAI, randomAI, human, minimaxAI, alphaBetaAI, humanIoT
from montecarlo import monteCarloAI
from awsGateway import awsGateway
from utilTool import util

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
		'EASY': randomAI,
		'MEDIUM': stupidAI,
		'HARD': monteCarloAI,
		'EXTREME': minimaxAI,
		'NIGHTMARE': alphaBetaAI
}

if __name__ == '__main__':
	aws_gateway = awsGateway()
	while(True):
		seed = None # TODO: need to randomly generate seed
		isMultiplayer = False
		# check if the message is initialize game
		message, errorCode = aws_gateway.receiveMsgFromSQS()
		if errorCode != 0:
			continue
		elif message["messageType"] != "cmd":
			print("ERROR: This is a move msg, which appeared in the wrong place")
			continue
		elif message["message"] == "START_GAME":
			isMultiplayer = True
			player1 = humanIoT(1, seed, message["shadowName"])
			aws_gateway.linkPlayer(message["shadowName"], 0) # assign shadow as the first player
		elif message["message"] in agents_iot: # if the message is one of the five difficulty level
			player2 = agents_iot[message["message"]](1, seed, "AI")
		else:
			print("wrong command, please retry")
			continue

		if isMultiplayer: # if this is multiplayer, wait for the next player to connect
			message2, errorCode2 = aws_gateway.receiveMsgFromSQS()
			if errorCode2 != 0:
				continue
			elif message2["shadowName"] != message["shadowName"]:
				print("ERROR: Same connection twice")
				continue
			elif message2["message"] == "START_GAME":
				player2 = humanIoT(2, seed, message["shadowName"])
				aws_gateway.linkPlayer(message2["shadowName"], 1) # assign shadow as the second player
			else:
				print("ERROR: not a start command")
				continue

		c4 = connect4(player1, player2, board_shape=(w, l), visualize=visualize,
					limit_players=limit_players, time_limit=time_limit, verbose=verbose)
		c4.play()
