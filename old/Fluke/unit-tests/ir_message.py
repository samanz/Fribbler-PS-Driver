from myro import *
import sys

init()

setIRPower(250)

print "recv", getIRMessage()

sendIRMessage('Hello Robots')
print "recv", getIRMessage()
