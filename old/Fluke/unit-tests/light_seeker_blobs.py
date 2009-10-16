from myro import *

init()

GO=True
TOO_LEFT = 90.0
TOO_RIGHT = 154.0
GAIN = 0.4

darkenCamera()
configureBlob(y_low=100, y_high = 255, u_low=0, u_high = 255, v_low=0, v_high = 255)

p = takePicture('blob')
show(p)

print "starting loop"
lastdir = 'r'

for i in range(60):

    print i

    b = currentTime()
    px_cnt, avg_x, avg_y = getBlob()
    a = currentTime()
    print "Time = ", (a-b)*1000.0
    
    print px_cnt, avg_x
    
    if (px_cnt < 1):
        if GO:
            if lastdir == 'l':
                turnLeft(GAIN)
            else:
                turnRight(GAIN)
            print "searching"
    elif (avg_x < TOO_LEFT):
        lastdir = 'l'
        if GO:
            turnLeft(GAIN)
        print "turning left"
    elif (avg_x > TOO_RIGHT):
        lastdir = 'r'
        if GO:
            turnRight(GAIN)
        print "turning right"
    else:
        print "headed toward the object"
        if GO:
            forward(GAIN)

stop()
