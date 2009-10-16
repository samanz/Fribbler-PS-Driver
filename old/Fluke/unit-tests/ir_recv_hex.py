from myro import *
import sys

init()

while 1:
    msg = getIRMessage()    
    if len(msg) > 0:
        for i in msg:
            print "%.02x" %ord(i),
        print        
    
