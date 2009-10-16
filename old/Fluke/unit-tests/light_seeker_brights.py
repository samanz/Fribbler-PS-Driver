from myro import *

init()

darkenCamera()
    
def brain():

    GAIN=0.25

    for i in range(90):

        b = currentTime()
        left = getBright("left")
        right = getBright("right")
        a = currentTime()
        
        print i, (a - b) * 1000.0, left, right
        
        if (left - right) > 0:            
            robot.move(GAIN, GAIN)
            print "Turning left"            
        elif (right - left) > 0:
            robot.move(GAIN, -GAIN)            
            print "Turning right"            
        else:
            if left >= 220000: #0.9: 
                print "Found the light"
                robot.move(GAIN, 0.0)
            else:
                print "search"
                robot.move(GAIN, GAIN)


brain()
