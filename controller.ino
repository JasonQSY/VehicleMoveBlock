#include <PS2X_lib.h> 
#include <Servo.h> 

/* Created by Jason Qian on Apr.13, version 0.0.3
 * Final version for preliminary contest.
 * Achieve complete control of the vehicle except servo4.
 *
 * For servos, there can be an effective range.
 * ---------------------------
 * |  Servo  |  Min  |  Max  |
 * | ------- | ----- | ----- |
 * |    1    |  090  |  070  | small as min, big as max
 * |    2    |  150  |  000  | bottom as min, above as max
 * |    3    |  080  |  000  | bottom as min, above as max
 * |    4    |  000  |  000  |
 * ---------------------------
 * Servo5: mid <=> 1420
 */

/* Objects */
PS2X ps2x; 
Servo servo1, servo2, servo3, servo4, servo5;  //From top to bottom. Servo 1 for the head.

/* For ps2x */
int error = 0; 
byte type = 0;
byte vibrate = 0;
/* For servo1-4 */
int pos1 = 0, pos2 = 0, pos3 = 0, pos4 = 0;
const int servoStep = 5;
const int rangeMin = 5;
const int rangeMax = 175;
/* servoSpeed and servoMid for servo5 */
const int servoSpeed = 200;
const int servoMid = 1420;
/* pin for motor */
const int pinLeftOne = 2, pinLeftTwo = 3, pinRightOne = 4, pinRightTwo = 14;//14=A0

void setup(){
    Serial.begin(57600);

    /* pin 6, 7, 8, 9, 5 for five servos */
	servo1.attach(6);
    servo2.attach(7);
    servo3.attach(8);
    servo4.attach(9);
    servo5.attach(5);

    /* Original postion of servos. Keep balance. */
    pos1 = 90;
    pos2 = 135;
    pos3 = 90;
    pos4 = 45;
    servo1.write(pos1);
    servo2.write(pos2);
    servo3.write(pos3);
    servo4.write(pos4);

	 /* pin 10 11 12 13 for ps2 gamepad controller */
	error = ps2x.config_gamepad(13,11,10,12, true, true);   //setup pins and settings:  GamePad(clock, command, attention, data, Pressures?, Rumble?) check for error
 
 	if(error == 0){
   		Serial.println("Found Controller, configured successful");
   		Serial.println("Try out all the buttons, X will vibrate the controller, faster as you press harder;");
  		Serial.println("holding L1 or R1 will print out the analog stick values.");
 	}
   
  	else if(error == 1)
   		Serial.println("No controller found, check wiring, see readme.txt to enable debug. visit www.billporter.info for troubleshooting tips");
   
  	else if(error == 2)
   		Serial.println("Controller found but not accepting commands. see readme.txt to enable debug. Visit www.billporter.info for troubleshooting tips");
   
  	else if(error == 3)
   		Serial.println("Controller refusing to enter Pressures mode, may not support it. ");

   	type = ps2x.readType(); 
    switch(type) {
       	case 0:
        	Serial.println("Unknown Controller type");
       		break;
       	case 1:
        	Serial.println("DualShock Controller Found");
       		break;
       	case 2:
        	Serial.println("GuitarHero Controller Found");
       		break;
    }
}

void loop(){
	if(error == 1) //skip loop if no controller found
  		return; 
  	if(type == 2) //Guitar Hero Controller(useless)
  		return;
 	else { //DualShock Controller(should be)
      	ps2x.read_gamepad(false, vibrate);
      	vibrate = ps2x.Analog(PSAB_BLUE);
    	/* Omit START and SELECT 
    	if(ps2x.Button(PSB_START))
         	Serial.println("Start is being held");
    	if(ps2x.Button(PSB_SELECT))
        	Serial.println("Select is being held");*/
        
      	/* Left axis for servo 2 and servo 4 */
      	if(ps2x.Analog(PSS_LY) < 63){
        	Serial.println("LY Small.");
        	if(pos2 <= rangeMax){
          		pos2 += servoStep;
          		servo2.write(pos2);
        	}
      	}
      	if(ps2x.Analog(PSS_LY) > 191){
        	Serial.println("LY Large.");
        	if(pos2 >= rangeMin){
          		pos2 -= servoStep;
          		servo2.write(pos2);
        	}
      	}
      	if(ps2x.Analog(PSS_LX) < 63){
        	Serial.println("LX Small.");
          	if(pos4 <= rangeMax){
              	pos4 += servoStep;
              	servo4.write(pos4);
          	}
      	}
      	if(ps2x.Analog(PSS_LX) > 191){
        	Serial.println("LX Large.");
        	if(pos4 >= rangeMin){
              	pos4 -= servoStep;
              	servo4.write(pos4);
        	}
      	}
      
      	/* Right axis for servo 1, 3 */ 
      	if(ps2x.Analog(PSS_RY) < 63){
        	Serial.println("RY Small.");
        	if(pos3 >= rangeMin){
              	pos3 -= servoStep;
              	servo3.write(pos3);
        	}
      	}
      	if(ps2x.Analog(PSS_RY) > 191){
        	Serial.println("RY Large.");
        	if(pos3 <= rangeMax){
              	pos3 += servoStep;
              	servo3.write(pos3);
        	}
      	}
      	if(ps2x.Analog(PSS_RX) < 63){
        	Serial.println("RX Small.");
        	if(pos1 <= rangeMax){
              	pos1 += servoStep;
              	servo1.write(pos1);
        	}
      	}
      	if(ps2x.Analog(PSS_RX) > 191){
        	Serial.println("RX Large.");
        	if(pos1 >= rangeMin){
              	pos1 -= servoStep;
              	servo1.write(pos1);
        	}
      	}
      
      	/* Additional control for servo 5 since the 360 servo is blocking. */
      	if(ps2x.Button(PSB_RED)){
        	Serial.println("Red pressed.");
        	servo5.writeMicroseconds(servoMid + servoSpeed);
        	delay(25);
        	servo5.writeMicroseconds(servoMid);
      	}
      	if(ps2x.Button(PSB_PINK)){
        	Serial.println("Pink pressed.");
        	servo5.writeMicroseconds(servoMid - servoSpeed);
        	delay(25);
        	servo5.writeMicroseconds(servoMid);
      	}

      	/* L1 L2 for the left motor 
      	 * L1 - forward
      	 * L2 - Back
      	 * Both is released - stop
      	 */
      	if(ps2x.Button(PSB_L1)){
          	Serial.println("L1 pressed.");
          	digitalWrite(pinLeftOne, 1);
          	digitalWrite(pinLeftTwo, 0);
      	}
      	if(ps2x.Button(PSB_L2)){
          	Serial.println("L2 pressed.");
          	digitalWrite(pinLeftOne, 0);
          	digitalWrite(pinLeftTwo, 1);
      	} 
      	if(ps2x.ButtonReleased(PSB_L1) || ps2x.ButtonReleased(PSB_L2)){
      	  	Serial.println("Left motor stop.");
      	  	digitalWrite(pinLeftOne, 0);
      	  	digitalWrite(pinLeftTwo, 0);
      	}

      	/* R1 R2 for the right motor */
      	if(ps2x.Button(PSB_R1)){
          	Serial.println("R1 pressed.");
          	digitalWrite(pinRightOne, 1);
          	digitalWrite(pinRightTwo, 0);
      	}
      	if(ps2x.Button(PSB_R2)){
          	Serial.println("R2 pressed.");
          	digitalWrite(pinRightOne, 0);
          	digitalWrite(pinRightTwo, 1023);//A0 should use 1023 instead of 1
      	}
      	if(ps2x.ButtonReleased(PSB_R1) || ps2x.ButtonReleased(PSB_R2)){
      	  	Serial.println("Right motor stop.");
      	  	digitalWrite(pinRightOne, 0);
      	  	digitalWrite(pinRightTwo, 0);
      	}
    }
 	delay(50);
}
