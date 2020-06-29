#include <string.h>

typedef struct lcd_t {
	int value;
	int isInstruction;
	struct lcd_t *next;
} lcd_t;

lcd_t *lcd_head = NULL;
lcd_t *lcd_tail = NULL;

void LCDPush(int value, int isInstruction) {
	lcd_t *node = malloc(sizeof(lcd_t));
	node->value = value;
	node->isInstruction = isInstruction;
	node->next = NULL;

	if (lcd_head == NULL) {
		lcd_head = node;
		lcd_tail = node;
		return;
	}

	lcd_tail->next = node;
	lcd_tail = node;
}

int convertDisplay(int value) {
	int port = value & 0b00001111;
	port |= (value & 0b10000000) >> 3;
	port |= (value & 0b01000000) >> 1;
	port |= (value & 0b00100000) << 1;
	port |= (value & 0b00010000) << 3;
	return port;
}

void LCDOverflow() {
	if (lcd_head == NULL) {
		return;
	}

	lcd_t *node = lcd_head;
	if (PORTC & (1 << 3)) {
		PORTC &= ~(1 << 3);
		lcd_head = lcd_head->next;
		free(node);
		return;
	}

	if (node->isInstruction) {
		PORTC &= ~(1 << 2);
		} else {
		PORTC |= 1 << 2;
	}

	PORTD = convertDisplay(node->value);
	PORTC |= 1 << 3;
}

#define lcd_funSet     0b00111000
#define lcd_DisplayOff 0b00001000
#define lcd_Clear      0b00000001
#define lcd_EntryMode  0b00000110
#define lcd_displayOn  0b00001111
#define lcd_returnHome 0b00000010

void LCDInstuction(int instruction) {
	LCDPush(instruction, 1);
}

void LCDString(char str[]) {
	for (int i = 0; i < strlen(str); i++) {
		if (str[i] == '\n') {
			for (int j = i; j < 40; j++) {
				LCDPush(32, 0);
			}
			continue;
		}
		LCDPush(str[i], 0);
	}
}

void startLCD() {
	TCCR2 = 0b00000011;

	LCDInstuction(lcd_funSet);
	LCDInstuction(lcd_DisplayOff);
	LCDInstuction(lcd_Clear);
	LCDInstuction(lcd_EntryMode);
	LCDInstuction(lcd_displayOn);
	//LCDString("  SILVIO LARDI\n   MAMA DA ÜZ");
}
