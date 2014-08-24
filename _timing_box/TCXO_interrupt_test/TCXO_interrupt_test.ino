#include "SoftwareSerial.h"

#define LCD_RX A6
#define LCD_TX 2
#define NUM_LANES 8
#define FALSE 0
#define TRUE 1
#define HUNDREDTH 1
#define TENTH 0

SoftwareSerial lcd(LCD_RX, LCD_TX);

uint8_t LANE_lap_count[NUM_LANES];
uint8_t LANE_accepting_signal[NUM_LANES];
uint32_t LANE_last_touch_time[NUM_LANES];
uint8_t LANE_place[NUM_LANES];

uint8_t LANE_placing_by_lap[40];


uint8_t DIP_config[2] = {FALSE, FALSE}; // DIP switch configuration
/*
	A0 - Start trigger level. 1 HIGH, 0 LOW
	A1 - Touchpad trigger levels. 1 HIGH, 0 LOW
*/
uint8_t oldA5 = 2; 

uint32_t old;
volatile uint32_t KHz_counter = 0;
volatile uint8_t RACE_IN_PROGRESS = FALSE;



void setup() {
	DIP_config_routine();	
	pinMode(3, INPUT);			// 1KHz TCXO interrupt
	pinMode(13, OUTPUT);		// Status LED

	for(uint8_t x = 5; x <= 12; x++) {
		pinMode(x, INPUT);	// Lane
		//if(!DIP_config[1]) digitalWrite(x, HIGH);	// Lane pullup
		digitalWrite(x, HIGH);	// Force lane pullup - LM339 open collector output
	}

	pinMode(4, INPUT);			// Start signal
        if(!DIP_config[0]) digitalWrite(4, HIGH);	// Start signal pullup


	pinMode(A5, INPUT);			// Standby / disable splits
	digitalWrite(A5, HIGH);		// Standby / disable splits


	attachInterrupt(1, KHZ_isr, RISING);


	Serial.begin(19200);
	Serial.println("t0.0");
	Serial.println("r");	
	lcd.begin(9600);
	LCD_clear();
	LCD_fullBacklight();
}

void loop() {
	digitalWrite(13, HIGH);					// Status LED
	while(true) {
		while(!RACE_IN_PROGRESS) {			// Check for start signal
			if(digitalRead(A5) != oldA5 && digitalRead(A5)) {
				LCD_clear();				
				lcd.print("STDBY FOR START");
				oldA5 = digitalRead(A5);
			} else if(digitalRead(A5) != oldA5 && !digitalRead(A5)) {
				LCD_clear();				
				lcd.print("    DISABLED");
				oldA5 = digitalRead(A5);
			}

			if(digitalRead(4) == DIP_config[0] && digitalRead(A5)) {
				RACE_IN_PROGRESS = TRUE;
				for(uint8_t index = 0; index < NUM_LANES; index++) {
					LANE_last_touch_time[index] = 0;			// Initialize everything
					LANE_accepting_signal[index] = FALSE;
					LANE_lap_count[index] = 0;
					LANE_place[index] = 0;
				}
				for(uint8_t index = 0; index < 40; index++) {
					LANE_placing_by_lap[index] = 0;
				}
			}
		}




		while(RACE_IN_PROGRESS) {			// Touchpads, timing, etc
			if(KHz_counter/100 != old) {
				old = KHz_counter/100;
				String time_string = formatKHzToTime(KHz_counter, TENTH);
				Serial.println('t' + time_string);

				LCD_clear();
				lcd.print(time_string);

				LCD_goTo(8);
				for(uint8_t x = 0; x < 8; x++) {
					if(LANE_accepting_signal[x]) lcd.write(0b10100101);
					else lcd.write(0b10100000);
				}
				
				LCD_moveLine(1);
				//lcd.print("SIG OK...");
				if(!digitalRead(A5)) {
					lcd.print("SPLITS - OFF");
				} else {
					lcd.print("SPLITS - ON");
				}				
				//lcd.print(LANE_accepting_signal[0]);
				//lcd.print(' ');
				//lcd.print(LANE_lap_count[0]);
				//lcd.print(' ');
				//lcd.print(LANE_last_touch_time[0]);
				//lcd.print(' ');
				//lcd.print(LANE_place[0]);
			}

			/* All Individual Lane Processing */
			for(uint8_t index = 0; index < NUM_LANES; index++) {
				if(KHz_counter - LANE_last_touch_time[index] > 15000) {
					LANE_accepting_signal[index] = TRUE;
				}
				if(LANE_accepting_signal[index] && digitalRead(index+5) == !DIP_config[1]) {
					LANE_last_touch_time[index] = KHz_counter;
					LANE_accepting_signal[index] = FALSE;
					LANE_lap_count[index] += 2;

					LANE_placing_by_lap[LANE_lap_count[index]] += 1; // Increment at index of lap #

					LANE_place[index] = LANE_placing_by_lap[LANE_lap_count[index]]; // Placing based on when lane is added to placing_by_lap

					if(digitalRead(A5)) {
						Serial.print('s');
						Serial.print(index+1);
						Serial.print(' ');
						Serial.print(LANE_place[index]);
						Serial.print(' ');
						Serial.print(formatKHzToTime(LANE_last_touch_time[index], HUNDREDTH));
						Serial.print(' ');
						Serial.println(LANE_lap_count[index]);
					}
				}
			}

		}

	}
}

void KHZ_isr() {
	if(RACE_IN_PROGRESS) KHz_counter++;
}

/* Precision: 0 - 0.1s, 1 - 0.01s */
String formatKHzToTime(uint32_t KHz, uint8_t precision) {
	uint32_t deciseconds = KHz/100; // deciseconds
	uint8_t minutes = deciseconds/600;
	uint8_t seconds = (deciseconds/10) % 60;
	uint8_t mod_decisec = deciseconds % 10;
	if(precision == 1) {
		mod_decisec = (KHz/10) % 100;
	}

	String ASCII_time = String();
	if(minutes) {
		ASCII_time += minutes;
		ASCII_time += ':';
	}
	if(seconds < 10 && minutes) ASCII_time += '0';
	ASCII_time += seconds;
	ASCII_time += '.';
	if(mod_decisec < 10 && precision) ASCII_time += '0';
	ASCII_time += mod_decisec;

	return ASCII_time;
}

void LCD_clear() {
	lcd.write(0xFE); //command
	lcd.write(0x01); //clear
}

void LCD_moveLine(uint8_t line_number) {
	if(line_number == 0) {
		lcd.write(0xFE);
		lcd.write(128);
	}
	if(line_number == 1) {
		lcd.write(0xFE);
		lcd.write(192);
	}
}

void LCD_fullBacklight() {  //turns on the backlight
	lcd.write(0x7C);   //command flag for backlight stuff
	lcd.write(157);    //light level.
	delay(10);
}

void LCD_goTo(int position) { //position = line 1: 0-15, line 2: 16-31, 31+ defaults back to 0
	if (position<20){ 
		lcd.write(0xFE);   //command flag
		lcd.write((position+128));    //position
	} else if (position<40) {
		lcd.write(0xFE);   //command flag
		lcd.write((position+48+128));    //position 
	} else { 
		LCD_goTo(0); 
	}
		delay(10);
}

void DIP_config_routine() {
	pinMode(A0, INPUT);
	pinMode(A1, INPUT);
	digitalWrite(A0, HIGH);
	digitalWrite(A1, HIGH);
	DIP_config[0] = digitalRead(A0);
	DIP_config[1] = digitalRead(A1);
}
