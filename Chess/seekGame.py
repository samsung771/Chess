import berserk
import json
import time

session = berserk.TokenSession("lip_J9yRSzhWmnPUALi1acUB")
client = berserk.Client(session=session)
looping = 1

print("searching...")
client.board.seek(10,0)

while looping:
    print(client.board.stream_incoming_events()[0])
    """
    for i in client.board.stream_incoming_events():
        if not looping:
            break
        try:
            print("you are playing as " + i["game"]["color"])
            print("against " + i["game"]["opponent"]["username"])
            
            gameId = i["game"]["gameId"]
            iD = open(r"gameID.txt",'w')
            iD.writelines(gameId)
            iD.close()
            print('\n' + gameId)

            colour = open(r"colour.txt",'w')
            if (i["game"]["color"] == "white"):
                colour.writelines('0')
            else:
                colour.writelines('1')                
            colour.close()
            looping = 0
        except:
            continue"""

