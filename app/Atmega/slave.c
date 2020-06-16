void startSlave() {
  SPCR = 0b01000001; // MSB first
  SPSR = 0;
}

#define updateRele 0b00000010 // bit0   => on or off
#define getValues  0b00000100 // bit1:0 => current, voltage, ptc
#define updateLeds 0b00001000 // bit2:0 => 6 colors
#define writeLCD   0b10000000 // bit6   => inst/write, bit5:0 => len

void (*slave_nextFun)(int) = NULL;
void (*no_fun)(int) { }

int sendingByte = 0;
double sendingVal = 0;
void sendValue(double value) {
  if (sendingByte = 0) {
    sendingVal = value;
    slave_nextFun = sendValue;
  }

  char *ptr = &sendingVal + sendingByte++;
  SPCR = *ptr;

  if (sendingByte >= sizeof(double)) {
    sendingByte = 0;
    slave_nextFun = NULL;
  }
}

node_t *getLeds_led = NULL;
int getLeds_repeat = 0;

void getLeds_fun(int value) {
  getLeds_led->nextTime = value;

  if (--getLeds_repeat > 0) {
    getLeds_led++;
  } else {
    slave_nextFun = NULL;
  }
}

void getLeds(node_t *led, int repeat) {
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

void slaveInterrupt() {
  int value = SPCR;
  if (slave_nextFun != NULL) {
    slave_nextFun(value);
    return;
  }

  if (!value) {
    return;
  }

  if (value < updateRele << 1) {
    setRele(updateRele & 1);
    return;
  }

  if (value < getValues << 1) {
    switch (value & 0b11) {
      case 0: sendValue(voltageVal); break;
      case 1: sendValue(currentVal); break;
      case 2: sendValue(temperatureVal); break;
      case 3: sendValue(voltageVal); break;
    }
    return;
  }

  if (value < updateLeds << 1) {
    switch (value & 0b111) {
      case 0b000: getLeds(leds[R1], 1); break;
      case 0b001: getLeds(leds[G1], 1); break;
      case 0b010: getLeds(leds[B1], 1); break;
      case 0b011: getLeds(leds[R1], 3); break;

      case 0b100: getLeds(leds[R2], 1); break;
      case 0b101: getLeds(leds[G2], 1); break;
      case 0b110: getLeds(leds[B2], 1); break;
      case 0b111: getLeds(leds[R1], 3); break;
    }
  }

  if (value < writeLCD << 1) {
    getLCD_isInstruction = value & 0b01000000;
    getLCD_repeat = value & 0b00111111;

    slave_nextFun = getLCD;
  }
}
