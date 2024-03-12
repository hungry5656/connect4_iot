from itertools import count
import random
import time
from awsGateway import awsGateway
import pygame
import math
import connect4
import numpy as np
import sys
from copy import deepcopy
import gameConstant
from message import messageUtil


class connect4Player(object):
	def __init__(self, position, seed=0, playerName="BLANK"):
		self.position = position
		self.playerName = playerName
		self.opponent = None
		self.seed = seed
		random.seed(seed)

	def play(self, env, move):
		move = [-1]


class human(connect4Player):

	def play(self, env, move):
		move[:] = [int(input('Select next move: '))]
		while True:
			if int(move[0]) >= 0 and int(move[0]) <= 6 and env.topPosition[int(move[0])] >= 0:
				break
			move[:] = [int(input('Index invalid. Select next move: '))]


class human2(connect4Player):

	def play(self, env, move):
		done = False
		while (not done):
			for event in pygame.event.get():
				if event.type == pygame.QUIT:
					sys.exit()

				if event.type == pygame.MOUSEMOTION:
					pygame.draw.rect(screen, BLACK, (0, 0, width, SQUARESIZE))
					posx = event.pos[0]
					if self.position == 1:
						pygame.draw.circle(screen, RED, (posx, int(SQUARESIZE/2)), RADIUS)
					else:
						pygame.draw.circle(screen, YELLOW, (posx, int(SQUARESIZE/2)), RADIUS)
				pygame.display.update()

				if event.type == pygame.MOUSEBUTTONDOWN:
					posx = event.pos[0]
					col = int(math.floor(posx/SQUARESIZE))
					move[:] = [col]
					done = True

class humanIoT(connect4Player):

	def play(self, env, move):
		move = []
		while True:
			message, errorCode = awsGateway.receiveMsgFromSQS()
			if errorCode == 0:
				parseCode, parseIdx = messageUtil.parseMoveMsg(message, self.playerName)
				if parseCode == -1:
					continue
				move[:] = [int(parseIdx)]
			else:
				continue
			if int(move[0]) == gameConstant.QUIT_GAME or int(move[0]) >= 0 and int(move[0]) <= 6 and env.topPosition[int(move[0])] >= 0:
				# send valid move to opponent
				break
				# TODO: if the input is invalid, send the message back to this shadow, probably not gonna happen


class randomAI(connect4Player):

	def play(self, env, move):
		possible = env.topPosition >= 0
		indices = []
		for i, p in enumerate(possible):
			if p:
				indices.append(i)
		move[:] = [random.choice(indices)]


class stupidAI(connect4Player):

	def play(self, env, move):
		possible = env.topPosition >= 0
		indices = []
		for i, p in enumerate(possible):
			if p:
				indices.append(i)
		if 3 in indices:
			move[:] = [3]
		elif 2 in indices:
			move[:] = [2]
		elif 1 in indices:
			move[:] = [1]
		elif 5 in indices:
			move[:] = [5]
		elif 6 in indices:
			move[:] = [6]
		else:
			move[:] = [0]


class minimaxAI(connect4Player):
	# construct a matrix that contains the weight of the board	
	weightBoard = [[0, 1, 2, 3, 2, 1, 0],
				   [0, 1, 2, 3, 2, 1, 0],
				   [0, 2, 3, 4, 3, 2, 0],
				   [0, 2, 3, 4, 3, 2, 0],
				   [0, 1, 2, 3, 2, 1, 0],
				   [0, 1, 2, 3, 2, 1, 0]]

	def play(self, env, move):
		env = env.getEnv()
		limit = 3
		move[:] = self.Minimax(env, limit)
		pass

	def game0ver(self, j, player, env):
		# Find extrema to consider
		i = env.topPosition[j] + 1
		minRowIndex = max(j - 3, 0)
		maxRowIndex = min(j + 3, env.shape[1]-1)
		maxColumnIndex = max(i - 3, 0)
		minColumnIndex = min(i + 3, env.shape[0]-1)
		minLeftDiag = [max(j - 3, j), min(i + 3, env.shape[0]-1)]
		maxLeftDiag = [min(j + 3, env.shape[1]-1), max(i - 3, 0)]
		minRightDiag = [min(j + 3, j), min(i + 3, env.shape[0]-1)]
		maxRightDiag = [max(j - 3, 0), max(i - 3, 0)]
		# Iterate over extrema to find patterns
		# Horizontal solutions
		count = 0
		for s in range(minRowIndex, maxRowIndex+1):
			if env.board[i, s] == player:
				count += 1
			else:
				count = 0
			if count == 4:
				return True
		# Verticle solutions
		count = 0
		for s in range(maxColumnIndex, minColumnIndex+1):
			if env.board[s, j] == player:
				count += 1
			else:
				count = 0
			if count == 4:
				return True
		# Left diagonal
		row = i
		col = j
		count = 0
		up = 0
		while row > -1 and col > -1 and env.board[row][col] == player:
			count += 1
			row -= 1
			col -= 1
		down_count = count
		row = i + 1
		col = j + 1
		while row < env.shape[0] and col < env.shape[1] and env.board[row][col] == player:
			count += 1
			row += 1
			col += 1
		if count >= 4:
			
					# top, bottom
					
			return True
		# Right diagonal
		row = i
		col = j
		count = 0
		while row < env.shape[0] and col > -1 and env.board[row][col] == player:
			count += 1
			row += 1
			col -= 1
		down_count = count
		row = i - 1
		col = j + 1
		while row > -1 and col < env.shape[1] and env.board[row][col] == player:
			count += 1
			row -= 1
			col += 1
		if count >= 4:
			return True

	def countConse(self, arr, currentPlayer):
		if len(arr) <= 1:
			return(0,0,0)
		n2, n3, n4, temp = 0, 0, 0, 0
		for i in range(0, len(arr)+1):
			if (i < len(arr)) and (arr[i] == currentPlayer):
				temp+=1
				#print(temp)
				continue
			else:
				if temp == 2:
					n2 += 1
				elif temp == 3:
					n3 +=1
				elif temp >= 4:
					n4 += 1
				else: 
					temp = 0
				#reset back to zero
				temp = 0
		return n2, n3, n4

	def countOnes(self, arr, currentPlayer):
		row = [-1, 0, 1]
		col = [-1, 0, 1]
		n1 = 0
		for i in range(0,6):
			for j in range(0,7):
				# print(i,j)
				if(arr[i][j]!=currentPlayer):
					continue
				check = True
				# print("get here")
				for r in row:
					for c in col:
						x = i + r
						y = j + c
						if(x < 0 or x > 5 or y < 0 or y > 6 or (x == i and y == j)):
							continue
						else:
							check = (check and (arr[x][y] != currentPlayer))
							# print(check)
				if(check):
					# print(i,j)
					arr[i][j] = 0
					n1 += 1
		return n1

	def CountNs(self, bd, p):
		n1 = self.countOnes(bd, p)
		#print(bd)
		n2 = 0
		n3 = 0
		n4 = 0
		width = len(bd)
		height = len(bd[0])
		for row in range(0, width):
			num = self.countConse(bd[row, :], p)
			numd = self.countConse(bd.diagonal(row), p)
			numdf = self.countConse(np.fliplr(bd.copy()).diagonal(row), p)
			n2 += (num[0] + numd[0]+numdf[0])
			n3 += (num[1] + numd[1]+numdf[1])
			n4 += (num[2] + numd[2]+numdf[2])
		#print("================================")

		for col in range(0, height):
			num = self.countConse(bd[:, col], p)
			numd = self.countConse(bd.diagonal(0-col), p)
			numdf = self.countConse(np.fliplr(bd.copy()).diagonal(0-col),p)
			if(col == 0):
				numd = (0,0,0)
				numdf = (0,0,0)
			n2 += (num[0] + numd[0]+numdf[0])
			n3 += (num[1] + numd[1]+numdf[1])
			n4 += (num[2] + numd[2]+numdf[2])
		return n1, n2, n3, n4

	def MAX(self, env, depth, move2):
		# print(depth)
		# move
		if self.game0ver(move2, self.position, env) or depth == 0:
			# print("222")
			return self.eval(env)
		possible = env.topPosition >= 0
		max_v = -math.inf
		for move, p in enumerate(possible):
			if p:
				newEnv = env.getEnv()
				child = self.simulateMove(newEnv, move, self.position)
				max_v = max(max_v, self.MIN(child, depth - 1, move))
		return max_v

	def MIN(self, env, depth, move2):
		# print(depth)
		if self.game0ver(move2, self.position, env) or depth == 0:
			# print("333")
			return self.eval(env)
		possible = env.topPosition >= 0
		min_v = math.inf
		for move, p in enumerate(possible):
			if p:
				newEnv = env.getEnv()
				child = self.simulateMove(newEnv, move, self.opponent.position)
				min_v = min(min_v, self.MAX(child, depth - 1, move))
		return min_v

	def eval(self, env):
		Alpha = 2
		Beta = 5
		Gamma = 8
		currWeight = 0
		currBoard = env.getBoard()
		currPlayer = env.turnPlayer.position
		oppoPlayer = env.turnPlayer.opponent.position
		# print(currBoard)
		numOne1, numTwo1, numThree1, numFour1 = self.CountNs(currBoard, currPlayer)
		numOne2, numTwo2, numThree2, numFour2 = self.CountNs(currBoard, oppoPlayer)
		if numFour2 >= 1:
			return -math.inf
		if numFour1 >= 1:
			return math.inf
		for i in range(0, 7):
			j = 5
			while j > env.topPosition[i]:
				if env.board[j][i] == currPlayer:
					currWeight += self.weightBoard[j][i]
				elif env.board[j][i] == oppoPlayer:
					currWeight -= self.weightBoard[j][i]
				j -= 1
		part2 = Alpha * (numOne1 - numOne2) + Beta * (numTwo1 - numTwo2) + Gamma * (numThree1 - numThree2)
		currWeight += part2
		
		# print(currWeight)
		return currWeight

	def Minimax(self, env, depth):
		coCunt = 0
		# print(depth)
		possible = env.topPosition >= 0
		# print(possible)
		max_v = -math.inf
		moveFinal = []
		# print(self.opponent.position)
		for move, p in enumerate(possible):
			if p:
				child = self.simulateMove(env.getEnv(), move, self.position)
				v = self.MIN(child, depth - 1, move)
				if v >= max_v:
					max_v = v
					moveFinal[:] = [move]
		return moveFinal

	def simulateMove(self, env, move, player):
		env.board[env.topPosition[move]][move] = player
		env.topPosition[move] -= 1
		env.history[0].append(move)
		return env	

class alphaBetaAI(minimaxAI):

	def play(self, env, move):
		env = env.getEnv()
		move[:] = self.iterativeDeepening(env)
		pass

	def MAX(self, env, depth, move2, alpha, beta):
		# print(depth)
		# move
		if self.game0ver(move2, self.position, env) or depth == 0:
			# print("222")
			return self.eval(env)
		possible = env.topPosition >= 0
		max_v = -math.inf
		for move, p in enumerate(possible):
			if p:
				newEnv = env.getEnv()
				child = self.simulateMove(newEnv, move, self.position)
				max_v = max(max_v, self.MIN(child, depth - 1, move, alpha, beta))
				alpha = max(alpha, max_v)
				if max_v >= beta:
					break
		return max_v

	def MIN(self, env, depth, move2, alpha, beta):
		if self.game0ver(move2, self.position, env) or depth == 0:
			return self.eval(env)
		possible = env.topPosition >= 0
		min_v = math.inf
		for move, p in enumerate(possible):
			if p:
				newEnv = env.getEnv()
				child = self.simulateMove(newEnv, move, self.opponent.position)
				min_v = min(min_v, self.MAX(child, depth - 1, move, alpha, beta))
				beta = min(beta, min_v)
				if min_v <= alpha:
					break
		return min_v


	def Minimax(self, env, depth):
		coCunt = 0
		# print(depth)
		possible = env.topPosition >= 0
		# print(possible)
		max_v = -math.inf
		alpha = -math.inf
		beta = math.inf
		moveFinal = []
		# print(self.opponent.position)
		for move, p in enumerate(possible):
			if p:
				child = self.simulateMove(env.getEnv(), move, self.position)
				v = self.MIN(child, depth - 1, move, alpha, beta)

				if v >= max_v:
					max_v = v
					moveFinal[:] = [move]
		return moveFinal

	def iterativeDeepening(self, env):
		MAXLIMIT = 4
		# limit means max_depth
		limit = 4
		move = []
		while (limit <= MAXLIMIT):
			move[:] = self.Minimax(env, limit)
			limit += 1
		return move
	


SQUARESIZE = 100
BLUE = (0, 0, 255)
BLACK = (0, 0, 0)
RED = (255, 0, 0)
YELLOW = (255, 255, 0)

ROW_COUNT = 6
COLUMN_COUNT = 7

pygame.init()

SQUARESIZE = 100

width = COLUMN_COUNT * SQUARESIZE
height = (ROW_COUNT+1) * SQUARESIZE

size = (width, height)

RADIUS = int(SQUARESIZE/2 - 5)

screen = pygame.display.set_mode(size)
