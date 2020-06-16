// PA0 NC
// PA1 SHUNT
// PA2 VOLTAGE
// PA3 PTC

#define baseMux    0b01000000
#define ptcMux     (baseMux | 0b11)
#define shuntMux   (baseMux | 0b01)
#define voltageMux (baseMux | 0b10)

int voltageVal = 0;
int currentVal = 0;
int temperatureVal = 0;

void startAnalog() {
	ADMUX = voltageMux;

	ADCSRA = 0b10100111;

	ADCSRA |= (1 << ADSC);
}

int analogCounter = 0;
void analogFlag() {
	analogCounter++;

	if (lcd_head != NULL) {
		return;
	}

	leds[G1]->nextTime += 5;
	if (leds[G1]->nextTime > 100) {
		leds[G1]->nextTime = 5;
	}

	leds[R1]->nextTime = 110 - leds[G1]->nextTime;

	char result[10];
	double res = 4.68 / ((1 << 10) - 1) * (double)(ADCL | (ADCH << 8)) / 16 * 26;
	dtostrf(res, 6, 4, result);
	LCDInstuction(lcd_returnHome);

	char string[40];
	sprintf(string, "%sVDC\nR:%d G:%d", result, leds[R1]->nextTime, leds[G1]->nextTime);
	LCDString(string);
}
