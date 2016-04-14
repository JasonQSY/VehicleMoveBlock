from pyfirmata import Arduino
from pyfirmata import SERVO
import pygame
import time

# Global variable
step = 5  # the position change of servos when pressing once
interval = 0.05  # interval between reading two events
servoPins = [8, 9, 10, 11, 12]  # To control five servos
motorPins = [2, 3, 4, 5]  # To control two motors 2,3 => motor left; 4,5 => motor right
buttonDict = {'UP': 0,
              'DOWN': 1,
              'LEFT': 2,
              'RIGHT': 3,
              'START': 4,
              'BACK': 5,
              'LS': 6,
              'RS': 7,
              'LB': 8,
              'RB': 9,
              'GUIDE': 10,
              'A': 11,
              'B': 12,
              'X': 13,
              'Y': 14}

axisDict = {'LX': 0,  # TODO. Test the axis.
            'LY': 1,
            'RX': 2,
            'RY': 3,
            'LT': 4,
            'RT': 5}

# Initial
board = Arduino('/dev/tty.usbmodemFD121')
#board = Arduino('/dev/tty.HC-06-DevB')
board.digital[13].write(1)

for i in range(len(servoPins)):
    board.digital[servoPins[i]].mode = SERVO

servo1 = board.digital[servoPins[0]]  # 8
servo2 = board.digital[servoPins[1]]  # 9
servo3 = board.digital[servoPins[2]]  # 10
servo4 = board.digital[servoPins[3]]  # 11
servo5 = board.digital[servoPins[4]]  # 12

motorL1 = board.digital[motorPins[0]]  # 2
motorL2 = board.digital[motorPins[1]]  # 3
motorR1 = board.digital[motorPins[2]]  # 4
motorR2 = board.digital[motorPins[3]]  # 5

pygame.init()
joystick = pygame.joystick.Joystick(0)
joystick.init()

# Game
print("Game Start!")
while True:
    pygame.event.pump()

    # Servo
    pos1 = servo1.read()
    pos2 = servo2.read()
    pos3 = servo3.read()
    pos4 = servo4.read()
    pos5 = servo5.read()

    if joystick.get_button(buttonDict['X']):
        if pos2 <= (180 - step):
            servo2.write(pos2 + step)

        print(servo2.read())

    if joystick.get_button(buttonDict['Y']):
        if pos2 >= (step + 0):
            servo2.write(pos2 - step)

        print(servo2.read())

    # Motor
    if joystick.get_axis(axisDict['LT']) != -1.0:  # maybe == 1.0. Test it.
        # TODO. It depends.
        motorL1.write(0)
        motorL2.write(1)
        print("Left motor forward.")

    if joystick.get_button(buttonDict['LB']):
        # TODO. It depends.
        motorL1.write(1)
        motorL2.write(0)
        print("Left motor reverse.")

    if joystick.get_axis(axisDict['RT']) != -1.0:  # maybe == 1.0. Test it.
        # TODO. It depends.
        motorR1.write(0)
        motorR2.write(1)
        print("Right motor forward.")

    if joystick.get_button(buttonDict['RB']):
        # TODO. It depends.
        motorR1.write(1)
        motorR2.write(0)
        print("Right motor reverse.")

    if joystick.get_button(buttonDict['START']):
        # STOP
        motorL1.write(1)
        motorL2.write(1)
        motorR1.write(1)
        motorR2.write(1)
        print("Stop!!!")

    # Just for convenience
    if joystick.get_button(buttonDict['BACK']):
        break

    time.sleep(interval)

print("Game Over!")
