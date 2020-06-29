void startSlave() {
	SPCR = 0b01000001; // MSB first
	SPSR = 0;
}

#define updateRele 0b00000010 // bit0   => on or off
#define getValues  0b00000100 // bit1:0 => current, voltage, ptc
#define updateLeds 0b00001000 // bit2:0 => 6 colors
#define writeLCD   0b10000000 // bit6   => inst/write, bit5:0 => len

void (*slave_nextFun)(int) = NULL;

int sendingByte = 0;
char sendingVal[10];
void sendValue(int value) {
	SPDR = sendingVal[sendingByte++];

	//if (sendingByte > strnlen(sendingVal, 10)) {
	if (sendingByte >= 8) {
		sendingByte = 0;
		slave_nextFun = NULL;
	}
}

int getLeds_led = 0;
int getLeds_repeat = 0;

void getLeds_fun(int value) {
	leds[getLeds_led]->nextTime = value;

	if (--getLeds_repeat > 0) {
		getLeds_led++;
		} else {
		slave_nextFun = NULL;
	}
}

void getLeds(int led, int repeat) {
	getLeds_led = led;
	getLeds_repeat = repeat;
	slave_nextFun = getLeds_fun;
}

int getLCD_isInstruction = 0;
int getLCD_repeat = 0;
void getLCD(int value) {
	LCDPush(value, getLCD_isInstruction);

	if (--getLCD_repeat <= 0) {
		slave_nextFun = NULL;
	}
}

void setRele(int);
void slaveInterrupt() {
	int value = SPDR;
	if (slave_nextFun != NULL) {
		slave_nextFun(value);
		return;
	}

	if (!value) {
		return;
	}

	if (value < updateRele << 1) {
		setRele(value & 1);
		return;
	}

	if (value < getValues << 1) {
		double temp = 0;
		switch (value & 0b11) {
			case 0: temp = voltageVal; break;
			case 1: temp = currentVal; break;
			case 2: temp = temperatureVal; break;
			case 3: temp = voltageVal; break;
		}

		dtostrf(temp, 8, 5, sendingVal);
		LCDString(sendingVal);

		// sendValue(0);
		slave_nextFun = sendValue;

		return;
	}

	if (value < updateLeds << 1) {
		switch (value & 0b111) {
			case 0b000: getLeds(R1, 1); break;
			case 0b001: getLeds(G1, 1); break;
			case 0b010: getLeds(B1, 1); break;
			case 0b011: getLeds(R1, 3); break;

			case 0b100: getLeds(R2, 1); break;
			case 0b101: getLeds(G2, 1); break;
			case 0b110: getLeds(B2, 1); break;
			case 0b111: getLeds(R1, 3); break;
		}
		return;
	}

	/*if (value < writeLCD << 1) {*/
		if (1) {
			getLCD_isInstruction = value & 0b01000000;
			getLCD_repeat = value & 0b00111111;

			slave_nextFun = getLCD;
		}
	}
