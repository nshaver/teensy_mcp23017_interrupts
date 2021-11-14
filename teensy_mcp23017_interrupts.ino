/*
	 tests use of multiple CQRobot mcp23017 boards with teensy 4.1
	 board #1 is i2c address 0x27, unsoldered, all pins used as inputs
	 board #2 is i2c address 0x26, soldered across A0, all pins used as outputs
	 board #2 is i2c address 0x25, soldered across A1, all pins used as outputs
 */
#include "Adafruit_MCP23017.h"

Adafruit_MCP23017 mcp1;
Adafruit_MCP23017 mcp2;
Adafruit_MCP23017 mcp3;

#define LED_PIN 13
#define MCP_INT_PIN 2

unsigned long loopcount=0;
unsigned long looptime_ms=0;
unsigned long thisloop_ms=0;
unsigned long nextblinkoff_ms=0;
uint16_t buttonState=0b1111111111111111;
uint16_t tmpButtonState;
volatile bool interruptTriggered=false;

void handleInterrupt(){
	tmpButtonState=mcp1.readGPIOAB();
	interruptTriggered=true;
}

void handleButtons(){
	interruptTriggered=false;
	if (tmpButtonState!=buttonState) {
		digitalWrite(LED_PIN, HIGH);
		nextblinkoff_ms=thisloop_ms+100;
		for (int i=0;i<16;i++){
			bool oldBit=bitRead(buttonState, i);
			bool newBit=bitRead(tmpButtonState, i);
			if (oldBit!=newBit){
				if (newBit==1){
					Serial.println((String)"button " + i + " released");
					mcp2.digitalWrite(i, LOW);
				} else {
					Serial.println((String)"button " + i + " pressed");
					mcp2.digitalWrite(i, HIGH);
				}
			}
		}
		buttonState=tmpButtonState;
	}
}

void setup() {  
	delay(500);
	Serial.println("setup started");

  mcp1.begin(7);
  mcp2.begin(6);
  mcp3.begin(5);

	pinMode(MCP_INT_PIN, INPUT_PULLUP);
	mcp1.setupInterrupts(true,true,LOW);

	for (int i=0;i<16;i++){
  	mcp1.pinMode(i, INPUT); // set all pins on board #1 as inputs
  	mcp1.pullUp(i, HIGH);  // turn on a 100K pullup internally
		mcp1.setupInterruptPin(i,CHANGE);

  	mcp2.pinMode(i, OUTPUT); // set all pins on board #2 as outputs, for LEDs
  	mcp3.pinMode(i, OUTPUT); // set all pins on board #3 as outputs, for LEDs
	}
	// read MCP23017 asap to init for use with interrupt
	mcp1.readGPIOAB();

  pinMode(LED_PIN, OUTPUT);
	digitalWrite(LED_PIN, LOW);

	attachInterrupt(digitalPinToInterrupt(MCP_INT_PIN), handleInterrupt, LOW);
	Serial.println("setup finished");
}

void loop() {
	thisloop_ms=millis();
	loopcount++;
	if (thisloop_ms>=looptime_ms){
		Serial.println((String)"Loops/Sec: " + (loopcount/5));
		loopcount=0;
		looptime_ms=thisloop_ms+5000;
	}

	if (thisloop_ms>nextblinkoff_ms){
		digitalWrite(LED_PIN, LOW);
	}

	if (interruptTriggered){
		handleButtons();
	}
}
