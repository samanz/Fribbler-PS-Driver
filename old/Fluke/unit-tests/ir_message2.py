from myro import *
import sys

init()

setIRPower(255)

print "recv", getIRMessage()

for i in range(0, 127):

    sendIRMessage(chr(i)*4)
    msg = getIRMessage()
    if msg:
        print i, map(ord, msg)
    else:
        print i, "***"


