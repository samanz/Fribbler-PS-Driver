from myro import *
import sys

init()

setIRPower(250)

while 1:
    for ch in 'abcdefghijklmnopqrstuvwxyz':
        sendIRMessage(ch)
        wait(.5)
