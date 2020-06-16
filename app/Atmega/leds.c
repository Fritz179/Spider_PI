typedef struct node_t {
	int time;
	int nextTime;
	int bit;
	volatile uint8_t *port;
	struct node_t* next;
} node_t;

node_t *head = NULL;
node_t *baseHead = NULL;

void addNode(node_t *node) {
	node->next = NULL;

	// set head if NULL
	if (baseHead == NULL) {
		baseHead = node;
		return;
	}

	// replace head if in first place
	if (baseHead->time > node->time) {
		baseHead->time -= node->time;
		node->next = baseHead;
		baseHead = node;
		return;
	}

	// traverse linked list to find insertion
	node_t *curr = baseHead;
	node_t *prev = NULL;

	while (curr != NULL && curr->time <= node->time) {
		node->time -= curr->time;
		prev = curr;
		curr = curr->next;
	}

	// end
	if (curr == NULL) {
		prev->next = node;
		return;
	}

	// middle
	prev->next = node;
	node->next = curr;
	curr->time -= node->time;
}

node_t *newNode(int time, volatile uint8_t *port, int bit) {
	node_t *node = malloc(sizeof(node_t));
	node->time = time;
	node->nextTime = time;
	node->port = port;
	node->bit = bit;

	addNode(node);
	return node;
}

enum Leds {R1, G1, B1, R2, G2, B2};
node_t *leds[6];

void initNodes() {
	leds[R1] = newNode(0, &PORTB, 0); //R1
	leds[G1] = newNode(5, &PORTB, 1); //G1
	leds[B1] = newNode(0, &PORTB, 2); //B1
	leds[R2] = newNode(0, &PORTA, 4); //R2
	leds[G2] = newNode(0, &PORTA, 5); //G2
	leds[B2] = newNode(0, &PORTA, 6); //B2
	newNode(255, NULL, 0);
}

void startTimer() {
	initNodes();

	head = baseHead;
	OCR0 = head->time;

	TCCR0 = 0b00001011;
	OCR0 = 0b10000001;
	TIMSK = 0b00000010;
}

void flipNext() {
	if (head->port != NULL) {
		*(head->port) &= ~(1 << head->bit);
	}

	head = head->next;
}

ISR(TIMER0_COMP_vect) {
	flipNext();

	while(head->time == 0 && head != NULL) {
		flipNext();
	}

	if (head == NULL) {
		node_t *curr = baseHead;
		node_t *prev = NULL;
		int acc = 0;

		// reorder
		while (curr != NULL) {
			node_t *next = curr->next;

			if (curr->time + acc != curr->nextTime) {
				if (prev != NULL) {
					prev->next = curr->next;
				} else {
					baseHead = curr->next;
				}

				curr->next->time += curr->time;
				curr->time = curr->nextTime;
				addNode(curr);

				// restart loop
				curr = baseHead;
				prev = NULL;
				acc = 0;
			} else {
				acc += curr->time;
				prev = curr;
				curr = next;
			}
		}

		// turn on only non 0 timers
		head = baseHead;
		int ports = 0xff;

		while(head != NULL && head->time == 0) {
			ports &= ~(1 << head->bit);
			head = head->next;
		}

		PORTB |= ports & 0b00000111;
		PORTA |= ports & 0b01110000;

		while(head != NULL && head->time <= TCNT0) {
			flipNext();
			TCNT0 -= head->time;
		}

	}

	OCR0 = head->time;
}
