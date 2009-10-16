from myro import *

init()

darkenCamera()

for i in range(60):
    left, center, right = robot.get("bright")    
    print left, center, right
    
