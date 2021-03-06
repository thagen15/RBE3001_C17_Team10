/* main.c
 *
 *  Created on: August 20, 2016
 *      Author: Joest
 */

#include <RBELib/RBELib.h>
//For use of abs()
#include <stdlib.h>
#include <avr/io.h>
#include <RBELib/ADC.h>

#define LOWLINK 2
#define HIGHLINK 3
#define CURRSENSOR 0

int timestamp = 0;
volatile int count = 0;

volatile int printFlag = 0;

//value to get 100Hz
int maxCount = 45;
int armState = 0;

//next line for triangle
int nextLine = 0;

int linkAngle(int angle) {
	return (angle + 85) / .26;
}

void printHighPotVal(int potVal) {
	//5000/1023
	unsigned int mV = potVal * 4.88758;		//in millivolts

	//300/1023
	unsigned int angle = ((potVal * .26) - 85) + 90;		//in degrees
////293255132
	printf("%d %d %d \n\r", potVal, mV, angle);
}

void printLowPotVal(int potVal) {
	//5000/1023
	unsigned int mV = potVal * 4.88758;		//in millivolts

	//300/1023
	unsigned int angle = (potVal * .26) - 85;		//in degrees
//293255132
	printf("%d %d %d \n\r", potVal, mV, angle);
}

void readPotVal() {
	int val = getADC(LOWLINK);
	clearADC(LOWLINK);
	printLowPotVal(val);
}

void sampleADC(int seconds) {

	while (timestamp < seconds) {
		readPotVal(LOWLINK);
		_delay_ms(1000);
		timestamp++;
	}
}

void outputTriangle(int peak) {
	int i = 0;
	for (i = 0; i <= peak; i++) {
		setDAC(0, i);
		setDAC(1, peak - i);
	}
	for (i = peak; i > 0; i--) {
		setDAC(0, i);
		setDAC(1, peak - i);

	}
}
void goToLowLink(int setPos) {
	int potVal = linkAngle(setPos);

	changeADC(LOWLINK);

	int actPos = getADC(LOWLINK);

	int value = calcLowPID(potVal, actPos);

	driveLink('L', value);
}

void goToHighLink(int setPos) {
	int potVal = linkAngle(setPos);

	changeADC(HIGHLINK);

	int actPos = getADC(HIGHLINK);

	int value = calcHighPID(potVal, actPos);

	driveLink('H', value);
}
void buttonGoTo() {

	if (!PINCbits._P7) {
		goToLowLink(0);
		armState = 0;
	} else if (!PINCbits._P6) {
		goToLowLink(30);
		armState = 30;

	} else if (!PINCbits._P5) {
		goToLowLink(60);
		armState = 60;
	} else if (!PINCbits._P4) {
		goToLowLink(90);
		armState = 90;
	} else {
		goToLowLink(0);
		armState = 0;
	}
//	}
//	if (!PINCbits._P6) {
//		goTo(30);
//	}
//	if (!PINCbits._P5) {
//		goTo(60);
//	}
//	if (!PINCbits._P4) {
//		goTo(90);
//
//	}

}
//void part7() {
//	goTo(400);
//	changeADC(0);
//	calcCurrent(getADC(0));
//}

void printArmData() {
	changeADC(2);
	int potVal = getADC(2);
	changeADC(0);
	int currVal = getADC(0);
	float curr = calcCurrent(currVal);
	unsigned int mV = potVal * 4.88758;		//in millivolts

	//300/1023
	unsigned int angle = (potVal * .23) - 75;		//in degrees)

	printf("%d %d %f \n\r", armState, angle, mV, curr);
}

void buttonGoToXY() {

	if (!PINCbits._P7) {
		goToBothLinks(0, 90);
	} else if (!PINCbits._P6) {
		goToBothLinks(45, 0);
	} else if (!PINCbits._P5) {
		goToBothLinks(45, 90);
	} else if (!PINCbits._P4) {
		goToLowLink(20);
		goToHighLink(45);
	} else {
		goToLowLink(45);
		goToHighLink(10);

	}
}
void goToBothLinks(int theta1, int theta2) {
	if (theta1 > 90) {
		theta1 = 90;
	} else if (theta1 < 0) {
		theta1 = 0;
	}
	if (theta2 > 180) {
		theta2 = 180;
	} else if (theta2 < 0) {
		theta2 = 0;
	}

	goToLowLink(theta1);
	goToHighLink(theta2);

}
void makeTriangle() {
	changeADC(2);

	int angle1Deg = (getADC(2) * .26) - 85;

	changeADC(3);

	int angle2Deg = ((getADC(3) * .26) - 85) - 90;

	if (nextLine == 0) {
		goToBothLinks(0, 90);
		if ((angle1Deg <= 5 && angle1Deg >= -5)
				&& (angle2Deg <= 5 && angle2Deg >= -5))
			nextLine = 1;
	} else if (nextLine == 1) {
		goToBothLinks(45, 0);
		if ((angle1Deg <= 50 && angle1Deg >= 40) && (angle2Deg <= 5))
			nextLine = 2;
	} else if (nextLine == 2) {
		goToBothLinks(45, 90);
		if ((angle1Deg <= 50 && angle1Deg >= 40)
				&& (angle2Deg <= 5 && angle2Deg >= -5))
			nextLine = 3;
	} else if (nextLine == 3) {
		goToBothLinks(0, 90);
		if ((angle1Deg <= 5 && angle1Deg >= -5)
				&& (angle2Deg <= 95 && angle2Deg >= 85))
			nextLine = 4;
	} else if (nextLine == 4) {
		stopMotors();
	}
	//printf("%d %d \n\r", angle1Deg, angle2Deg);
}
int main(void) {
//Enable printf() and setServo()
	initRBELib();
	printf("A");
// Write the USARTDebug.c file using the function prototypes in the H file to enable the usart
//Set the baud rate of the UART
	debugUSARTInit(115200);
	//initTimer(0, NORMAL, 0);

//	printf("ADC-COUNT VOLTAGE ARM-ANGLE \n\r");
//DDRC |= (1 >> PIN7) | (1 >> PIN6) | (1 >> PIN5) | (1 >> PIN4);
//Initialize all the ADC channels we need
//initADC(CURRSENSOR);
//initADC(LOWLINK);
	initADC(HIGHLINK);
//	initADC(7);
//	initADC(6);
//initADC(KDPOT);
//changeADC(0);

//set up buttons for use
	DDRC &= 0x0F;

	initSPI();

//printf("Low Theta", "High Theta", "X", "Y");
//.33,0,0.00001
//setConst('L', 70, 0, 2);
//setConst('L', .33, 0, 0);
	setConst('L', 40, .5, .005);
	setConst('H', 40, 0, 0);

	while (1) {
		//	calcCurrent(getADC(0));

		//PART 1
		//sampleADC(60);

		//Part 2
		//outputTriangle(4095);

		//lab part 7
		//part7();
		//	driveLink('L', -3000);
		//changeADC(LOWLINK);
		//printf("%f \n\r", pidConsts.Kp_L);
		//goTo(45);
		//goToLowLink(45);
//		buttonGoTo();
		//	goToBothLinks(30, 90);
//		if (printFlag) {
//			//printf("a");
//			printArmData();
//			printFlag = 0;
//
//		}
		//printArmData();
		//	printf("%d \n\r", PINCbits._P6);
		//readPotVal();
		//printf("aa");

		//goToLowLink(85);
		//	printLowPotVal(getADC(LOWLINK));
		//LAB 2B
		//Part 4
		//printf("%d \n\r", getADC(HIGHLINK));
		//goToHighLink(160);
		//printHighPotVal(getADC(HIGHLINK));
		//printf("%d \n\r", getADC(LOWLINK));

		//Lab 2B Part 5
		//buttonGoToXY();
		//printXY();

		//Lab 2B Part 6
		//goToLowLink(90);
		makeTriangle();

		//goToBothLinks(30, 160);
//		goToHighLink(45);

	}
	return 0;
}
//POT VALUES
//720 = 90 deg
//340 = 0 deg

//}

ISR(TIMER0_OVF_vect) {
	count++;
	if (count > maxCount) {
		printFlag = 1;
		count = 0;
	}
}
