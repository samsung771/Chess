import berserk
import json
import time
import sys

session = berserk.TokenSession("lip_J9yRSzhWmnPUALi1acUB")
client = berserk.Client(session=session)
gameID = open(r"gameID.txt", 'r')
ID = gameID.readline()
gameID.close()

def makeMove(move):
    client.board.make_move(ID,move)

if __name__ == "__main__":
    move = str(sys.argv[1])
    makeMove(move)
