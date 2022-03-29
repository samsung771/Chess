import berserk
import json
import time
import sys

session = berserk.TokenSession("lip_J9yRSzhWmnPUALi1acUB")
client = berserk.Client(session=session)
gameID = open(r"gameID.txt", 'r')
ID = gameID.readline()
gameID.close()

try:
    client.board.abort_game(ID)
except:
    client.board.resign_game(ID)

