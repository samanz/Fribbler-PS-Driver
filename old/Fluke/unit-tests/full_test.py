from myro import *

initialize()

print getInfo()


print getForwardness()
print "Setting scribbler forward"
setForwardness("scribbler-forward")
print getForwardness()

forward(1,.5)

print "Setting fluke forward"
setForwardness("fluke-forward")
print getForwardness()
forward(1,.5)


print "Back LED 1.0", setLEDBack(1.0)
wait(2)

print "Back LED 0.5", setLEDBack(.5)
wait(2)

print "Front LED on", setLEDFront(1)
wait(2)

print "Battery", getBattery()

print "Front LED off", setLEDFront(0)
print "Back LED 0", setLEDBack(0)

b = currentTime()
p = takePicture()
a = currentTime()
"Color picture", (a-b)*1000.0, "millseconds"
show(p)
wait(2)

b = currentTime()
p = takePicture("gray")
a = currentTime()
"Gray picture", (a-b)*1000.0, "millseconds"
show(p)
wait(2)

b = currentTime()
p = takePicture("blob")
a = currentTime()
"Blob picture", (a-b)*1000.0, "millseconds"
show(p)
wait(2)

for power in range(129,140,2):
    print "setting power to", power
    setIRPower(power)
    for i in range(6):    
        print "l %d\tm %d\tr %d" % (getObstacle("left"), getObstacle("center"), getObstacle("right"))


