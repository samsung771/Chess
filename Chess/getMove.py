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
        status = open(r"status.txt",'w')

        if (i["status"] != "started"):
            print(i["status"])
            if (i["status"] != "aborted"):
                print(i["winner"])
                if (i["winner"] == "white"):
                    status.writelines('w')
                elif (i["winner"] == "black"):
                    status.writelines('b')
                else:
                    status.writelines('0')
            else:
                status.writelines('0')
        else:
            status.writelines('1')

        status.close()
    except:
        print('')
    try:
        print(i["moves"][len(i["moves"])-4 : len(i["moves"])])
        moves = open(r"moves.txt",'w')
        moves.writelines(i["moves"][len(i["moves"])-4 : len(i["moves"])])
        moves.close()
        break
    except:
        continue