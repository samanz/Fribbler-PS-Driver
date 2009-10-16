from myro import *
import sys

init()

if (len(sys.argv) > 2):
    start = int(sys.argv[1])
    stop = int(sys.argv[2])
else:
    start = 125
    stop = 145


if (len(sys.argv) > 3):
    readings = int(sys.argv[3])
else:
    readings = 10

for power in range(130, 140, 1):
    setIRPower(power)
    for j in range(0, readings):
        print "power = ", power, "\tleft = ", getObstacle("left"), "\tcenter = ", getObstacle("center"), "\tright = ", getObstacle("right")
        wait(.1)
        
