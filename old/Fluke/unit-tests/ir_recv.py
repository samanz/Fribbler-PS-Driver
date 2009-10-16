from myro import *

init()

while 1:
    msg = getIRMessage()    
    if len(msg) > 0:
        print msg
        
    
