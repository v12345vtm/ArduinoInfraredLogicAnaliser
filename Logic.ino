/*
 * code
 * based on http://arduino.cc/playground/Code/InfraredReceivers
 *
 * modified by v12345vtm
 * see https://www.youtube.com/user/v12345vtm
 * code : https://github.com/v12345vtm/ArduinoInfraredLogicAnaliser/blob/master/Logic.ino
 *
 *1)log every buttun
 *2)paste in exel and sort each line 
 *3)calculate the difference to highlight the changes in the datastream between each button
 *4) normally  a binary pattern will show up
 *
 * 
 */
 
#include <avr/interrupt.h>
#include <avr/io.h>
 
#define READ_IR        ((PIND & _BV(2)) >> 2)  // IR pin
#define TIMER_RESET    TCNT1 = 0
#define TIMER_OVF      (TIFR1 & _BV(TOV1))
#define TIMER_CLR_OVF  TIFR1 |= _BV(TOV1)
#define SAMPLE_SIZE    128
 
unsigned int TimerValue[SAMPLE_SIZE];
 
void setup() {
  Serial.begin(115200);
  Serial.println("Analyze IR Remote : connect IR to D2");
  TCCR1A = 0x00;
  TCCR1B = 0x03;
  TIMSK1 = 0x00;
 
  pinMode(2, INPUT);  // IR pin here
}
 
void loop() {
  int i;
 
  // reset
  for (i = 0; i < SAMPLE_SIZE; i++)
    TimerValue[i] = 0;
 
  Serial.println("Waiting...");
  while(READ_IR == HIGH);
 
  // reset timer, clear overflow flag
  TIMER_RESET;
  TIMER_CLR_OVF;
 
  // first value
  i = 0;
  TimerValue[i++] = 0;
 
  char prev_dir = 0;
  while (i < SAMPLE_SIZE) {
    while (READ_IR == prev_dir) {
      // if the pin has not changed, check for overflow
      if (TIMER_OVF)
        break;
    }
    TimerValue[i++] = TCNT1;
    prev_dir = !prev_dir;
  }
 
  Serial.println("Bit stream detected dalendeflank gezien!");
  for (i = 1; i < SAMPLE_SIZE; i++) {
    long time = (long) (TimerValue[i] - TimerValue[i-1]) * 4;
    if (time == 0 || TimerValue[i] == 0)
      break;
 
    Serial.print(time);
  Serial.print("\t");
 
    // newline every 4 values
    //if ((i % 4) == 0)
     // Serial.print("\n");
  }
  Serial.println("\nBit stream end!");
}
