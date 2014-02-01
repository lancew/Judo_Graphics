#define FALSE 0
#define TRUE 0
#define NUM_LANES 8


const uint8_t PIN_start_signal = 2;

volatile uint8_t WAITING_FOR_START_SIGNAL = TRUE;

uint8_t LANE_lap_count[NUM_LANES];
uint8_t LANE_accepting_signal[NUM_LANES];
uint8_t LANE_last_touch_time[NUM_LANES];

volatile uint32_t KHZ_COUNTER = 0;



void setup() {
  Serial.begin(9600);
  pinMode(PIN_start_signal, INPUT);
  digitalWrite(PIN_start_signal, HIGH); // Enable internal pullup
  
  
  
  attachInterrupt(0, pin2ISR, RISING);
}


void loop() {
  while(digitalRead(PIN_start_signal)) {
     // Wait until start is triggered on a LOW pulse
     Serial.println("LOL");
  }
  while(1){
  }
}



void pin2ISR() {
  if(!WAITING_FOR_START_SIGNAL)
    KHZ_COUNTER++; 
}
