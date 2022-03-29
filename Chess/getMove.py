import berserk
import json
import time
import sys

session = berserk.TokenSession("lip_J9yRSzhWmnPUALi1acUB")
client = berserk.Client(session=session)
gameID = open(r"gameID.txt", 'r')
ID = gameID.readline()
gameID.close()

for i in client.board.stream_game_state(ID):
    try:
        print(i["moves"][len(i["moves"])-4 : len(i["moves"])])
        moves = open(r"moves.txt",'w')
        moves.writelines(i["moves"][len(i["moves"])-4 : len(i["moves"])])
        moves.close()
        break
    except:
        continue
