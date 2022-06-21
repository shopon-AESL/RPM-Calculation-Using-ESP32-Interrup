#include <Arduino.h>
#include <main.h>

int pin_proxi = 36;
float count_motor_rotation=0;
float count_motor_rpm=0;
volatile int interruptCounter;
int totalInterruptCounter;
bool flag_sense_metal = true;

hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;    //typedef spinlock_t     portMUX_TYPE; Read this article to understand what does this line mean https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/freertos-smp.html
 
void IRAM_ATTR onTimer() {                                //Interrupt handler function
  portENTER_CRITICAL_ISR(&timerMux);
  interruptCounter++;
  count_motor_rpm = count_motor_rotation/SEC_60;
  count_motor_rotation=RESET_VALUE;
  portEXIT_CRITICAL_ISR(&timerMux);
 
}

void setup() {
    // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(pin_proxi,INPUT);
/*
The ESP32 has two timer groups, each one with two general purpose hardware timers. 
All the timers are based on 64 bits counters and 16 bit prescalers. that means presacller can take value upto 65536

// amra je timer ta set korsi shei timer tar setting kora hoise.
timerBegin(uint8_t num, uint16_t divider, bool countUp) 
true hoile count up and false hoile count down.
divider ta hocche prescaller.
80Mhz clock. timer can count 80*10^6 per second.
if we divide this value by 80 (using 80 as the prescaler value), 
we will get a signal with a 1 MHz frequency that will increment the timer counter 1 000 000 times per second.
*/
  timer = timerBegin(TIMER_NUM, VALUE_PRE_SCALE, COUNT_UP);            

/*

*/
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, VALUE_AUTO_RELOAD, AUTO_RELOAD);
  timerAlarmEnable(timer);                                  //enable the Timer.
}

void loop() {
   if(digitalRead(pin_proxi)==LOW){
    if(flag_sense_metal){
      flag_sense_metal = false;
      count_motor_rotation++;
      Serial.println(count_motor_rotation);
    }
  }else{
    flag_sense_metal = true;
  }
  // put your main code here, to run repeatedly:
  if (interruptCounter > RESET_VALUE) {
 
    portENTER_CRITICAL(&timerMux);
    interruptCounter--;
    portEXIT_CRITICAL(&timerMux);
 
    totalInterruptCounter++;
  
    Serial.print("An interrupt as occurred. Total number: ");
    Serial.println(totalInterruptCounter);
    Serial.print("RPM: ");
    Serial.println(count_motor_rpm);
    count_motor_rpm = RESET_VALUE;
  }
}