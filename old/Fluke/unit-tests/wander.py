from myro import *
import random
from time import *

init()

speed=0.7

def brain():

    setIRPower(130)
    
    for i in timer(60):
        r=getObstacle("right")
        l=getObstacle("left")
        c=getObstacle("center")
        print "%d %d %d %d\n" % (i, l, c, r),
        if r > 100:
            backward(speed, .1)
            rotate(speed)
        elif l > 100 or c > 100:
            backward(speed, .1)
            rotate(-speed)
            sleep(.1)
        else:
            forward(speed)
            sleep(.1)

    stop()

    # end of brain

print "battery:", getBattery()
brain()
