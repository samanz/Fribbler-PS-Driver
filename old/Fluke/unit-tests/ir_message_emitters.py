from myro import *
import sys

init()

setIRPower(145)

print "using low power, so we need a reflection to receive messages"
print

s='Hello Robots'
sendIRMessage(s)
print "should receive ", s, " got - ", getIRMessage()

s='No Receivers'
setCommunicateAll(False)
sendIRMessage(s)
print "should receive ", s, " got - ", getIRMessage()

s='Left Receiver'
setCommunicateAll(False)
setCommunicateLeft(True)
sendIRMessage(s)
print "should receive ", s, " got - ", getIRMessage()

s='Right Receiver'
setCommunicateAll(False)
setCommunicateRight(True)
sendIRMessage(s)
print "should receive ", s, " got - ", getIRMessage()

s='Center Receiver'
setCommunicateAll(False)
setCommunicateCenter(True)
sendIRMessage(s)
print "should receive ", s, " got - ", getIRMessage()
