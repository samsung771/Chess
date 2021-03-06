import berserk
import json
import time

session = berserk.TokenSession("lip_J9yRSzhWmnPUALi1acUB")
client = berserk.Client(session=session)
looping = 1

while(looping):
    gameId = ''
    for i in client.board.stream_incoming_events():
        if i["type"] == "challenge":
            print ("\nchallenged")
            print ("by " + i["challenge"]["challenger"]["name"] + "\n")
            print (i["challenge"]["id"])
            gameId = i["challenge"]["id"]
            iD = open(r"gameID.txt",'w')
            iD.writelines(gameId)
            iD.close()
            client.challenges.accept(i["challenge"]["id"])  
            print ("opponent is playing: " + i["challenge"]["finalColor"])
            colour = open(r"colour.txt",'w')
            if (i["challenge"]["finalColor"] == "black"):
                colour.writelines('0')
            else:
                colour.writelines('1')                
            colour.close()
            looping = 0
            break
