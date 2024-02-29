import random
import time
import pygame
import math
import numpy as np

def countConse(arr, currentPlayer):
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

def countOnes(arr, currentPlayer):
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


weightBoard = [[1, 1, 3, 4, 3, 1, 1],
               [1, 2, 3, 4, 3, 2, 1],
               [1, 3, 4, 5, 4, 3, 1],
               [1, 3, 4, 0, 0, 0, 0],
               [1, 2, 3, 4, 3, 2, 1],
               [1, 1, 3, 4, 3, 1, 1]]
board = np.array(weightBoard)


def CountNs( bd, p):
    n1 = countOnes(bd, p)
    #print(bd)
    n2 = 0
    n3 = 0
    n4 = 0
    width = len(bd)
    height = len(bd[0])
    for row in range(0, width):
        num = countConse(bd[row, :], p)
        numd = countConse(bd.diagonal(row), p)
        numdf = countConse(np.fliplr(bd.copy()).diagonal(row), p)
        n2 += (num[0] + numd[0]+numdf[0])
        n3 += (num[1] + numd[1]+numdf[1])
        n4 += (num[2] + numd[2]+numdf[2])
        print(n2, n3, n4)
    #print("================================")

    for col in range(0, height):
        num = countConse(bd[:, col], p)
        numd = countConse(bd.diagonal(0-col), p)
        numdf = countConse(np.fliplr(bd.copy()).diagonal(0-col),p)
        if(col == 0):
            numd = (0,0,0)
            numdf = (0,0,0)
        n2 += (num[0] + numd[0]+numdf[0])
        n3 += (num[1] + numd[1]+numdf[1])
        n4 += (num[2] + numd[2]+numdf[2])
    return n1, n2, n3, n4







#print(CountNs(board, 1))
# a = np.arange(1, 10).reshape(3, 3)
# print(a)
# print()
# for row in range(0, len(a)):
#     print(a[row, :])
#     print()

#     print(a.diagonal(row))
#     print()
#     print(np.fliplr(a.copy()).diagonal(row))
#     print()

# for col in range(0, len(a[0])):
#     print(a[:, col])
#     print()
#     print(col)
#     if(col == 0):
#         continue
#     print(a.diagonal(0-col))
#     print()
#     print(np.fliplr(a.copy()).diagonal(0-col))
#     print()

# print(a)
# print(np.flip(a))
# print()
# print(a)
# print(np.flipud(a))
# print(a)
# print(np.fliplr(a))
# print()



# print(board.diagonal(a))

