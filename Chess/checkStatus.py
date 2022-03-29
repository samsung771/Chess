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
                else:
                    status.writelines('b')
        else:
            status.writelines('0')

        status.close()
                
