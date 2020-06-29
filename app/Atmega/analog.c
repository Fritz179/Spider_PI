// PA0 NC
// PA1 SHUNT
// PA2 VOLTAGE
// PA3 PTC

#define baseMux    0b01000000
#define ptcMux     (baseMux | 0b11)
#define currentMux (baseMux | 0b01)
#define voltageMux (baseMux | 0b10)

double voltageVal = 0;
double currentVal = 0;
double temperatureVal = 0;

void startAnalog() {
	ADMUX = currentMux;

	ADCSRA = 0b10000111;

	ADCSRA |= (1 << ADSC);
}

#define VCC   4.68
#define cycle 10 // must be even

// resistance at 0°C to 100°C at 10°C increments
int resistanceTemp[] = { 1630, 1772, 1922, 2080, 2245, 2417, 2597, 2785, 2980, 3182, 3392 };

unsigned int analogCounter = 0;
int _analogCounter = 0;
void analogFlag() {
	if (_analogCounter++ < 50) {
		ADCSRA |= (1 << ADSC);
		return;
	}
	_analogCounter = 0;

	double measuredVoltage = VCC / ((1 << 10) - 1) * (double)(ADCL | (ADCH << 8));
	if (ADMUX == voltageMux) {
		// divider 16kOhm & 10kOhm
		voltageVal = measuredVoltage / 16 * 26;
		ADMUX = currentMux;
	} else if (ADMUX == ptcMux) {
		// series with 2.32kOhm
		int ptcResistance = 2320 / (VCC - measuredVoltage) * measuredVoltage;

		for (int i = 1; i < 11; i++) {
			if (ptcResistance < resistanceTemp[i]) {
				int dif = resistanceTemp[i] - ptcResistance;
				int gap = resistanceTemp[i] - resistanceTemp[i - 1];

				temperatureVal = i * 10 - gap / dif * 10;
				break;
			}
		}

		ADMUX = currentMux;
	} else {
		// 66mV/A
		currentVal = (measuredVoltage - 2.5) / 0.066;

		if (analogCounter % cycle == 0) {
			ADMUX = voltageMux;
			} else if (analogCounter % cycle == cycle / 2) {
			ADMUX = ptcMux;
		}
	}
	analogCounter++;

	ADCSRA |= (1 << ADSC);
}

// char result[10];
// voltageVal = 4.68 / ((1 << 10) - 1) * (double)(ADCL | (ADCH << 8)) / 16 * 26;
// dtostrf(voltageVal, 6, 4, result);
// LCDInstuction(lcd_returnHome);
//
// char string[40];
// sprintf(string, "%sVDC\nR:%d G:%d", result, leds[R1]->nextTime, leds[G1]->nextTime);
// LCDString(string);
