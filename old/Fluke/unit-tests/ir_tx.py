#! /usr/bin/env python

from myro import *
import sys

init()

setIRPower(250)
sendIRMessage("".join(map(lambda x: chr(int(x)), sys.argv[1:])))
wait(.1)
print "received:", map(ord, getIRMessage())
