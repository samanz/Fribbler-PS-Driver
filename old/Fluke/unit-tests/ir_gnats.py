from myro import *
import sys

init()

c = 0
while 1:
    msg = getIRMessage()    
    if len(msg) > 0:
        for i in msg:
            print "%.02x" %ord(i),
            
        c += len(msg)
        if c >= 12:
            print         
            c = 0
            c = 0
