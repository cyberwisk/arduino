#include <TimerOne.h>
 
//comment this out to see the demodulated waveform
//it is useful for debugging purpose.
#define MODULATED 1 
 
const int IR_PIN = 3;
const unsigned long DURATION = 180000l;
const int HEADER_DURATION = 2000;
const int HIGH_DURATION = 380;
const int ZERO_LOW_DURATION = 220;
const int ONE_LOW_DURATION = 600;
const byte ROTATION_STATIONARY = 60;
const byte CAL_BYTE = 52; 
 
int Throttle, LeftRight, FwdBack;
 
void sendHeader()
{
  #ifndef MODULATED
    digitalWrite(IR_PIN, HIGH);
  #else
    TCCR2A |= _BV(COM2B1);
  #endif
   
  delayMicroseconds(HEADER_DURATION);
   
  #ifndef MODULATED
    digitalWrite(IR_PIN, LOW);
  #else
    TCCR2A &= ~_BV(COM2B1);
  #endif
   
  delayMicroseconds(HEADER_DURATION);
   
  #ifndef MODULATED
    digitalWrite(IR_PIN, HIGH);
  #else
    TCCR2A |= _BV(COM2B1);
  #endif
   
  delayMicroseconds(HIGH_DURATION);
   
  #ifndef MODULATED
    digitalWrite(IR_PIN, LOW);
  #else
    TCCR2A &= ~_BV(COM2B1);
  #endif
}
 
void sendZero()
{
  delayMicroseconds(ZERO_LOW_DURATION);
 
  #ifndef MODULATED
    digitalWrite(IR_PIN, HIGH);
  #else  
    TCCR2A |= _BV(COM2B1);
  #endif
   
  delayMicroseconds(HIGH_DURATION);
   
  #ifndef MODULATED
    digitalWrite(IR_PIN, LOW);
  #else
    TCCR2A &= ~_BV(COM2B1);
  #endif
}
 
void sendOne()
{
  delayMicroseconds(ONE_LOW_DURATION);
   
  #ifndef MODULATED
    digitalWrite(IR_PIN, HIGH);
  #else
    TCCR2A |= _BV(COM2B1);
  #endif
   
  delayMicroseconds(HIGH_DURATION);
   
  #ifndef MODULATED
    digitalWrite(IR_PIN, LOW);  
  #else
    TCCR2A &= ~_BV(COM2B1);
  #endif
}
 
void sendCommand(int throttle, int leftRight, int forwardBackward)
{
  byte b;
 
  sendHeader();
   
  for (int i = 7; i >=0; i--)
  {
    b = ((ROTATION_STATIONARY + leftRight) & (1 << i)) >> i;    
    if (b > 0) sendOne(); else sendZero();
  }
   
  for (int i = 7; i >=0; i--)
  {
    b = ((63 + forwardBackward) & (1 << i)) >> i;    
    if (b > 0) sendOne(); else sendZero();
  } 
   
  for (int i = 7; i >=0; i--)
  {
    b = (throttle & (1 << i)) >> i;    
    if (b > 0) sendOne(); else sendZero();
  }
   
  for (int i = 7; i >=0; i--)
  {
    b = (CAL_BYTE & (1 << i)) >> i;    
    if (b > 0) sendOne(); else sendZero();
  } 
}
 
void setup()
{
  pinMode(IR_PIN, OUTPUT);
  digitalWrite(IR_PIN, LOW);
 
  //setup interrupt interval: 180ms  
  Timer1.initialize(DURATION);
  Timer1.attachInterrupt(timerISR);
   
  //setup PWM: f=38Khz PWM=0.5  
  byte v = 8000 / 38;
  TCCR2A = _BV(WGM20);
  TCCR2B = _BV(WGM22) | _BV(CS20); 
  OCR2A = v;
  OCR2B = v / 2;
}
 
void loop()
{  
    
}
 
void timerISR()
{
  //read control values from potentiometers
  Throttle = analogRead(0);
  LeftRight = analogRead(1);
  FwdBack = analogRead(2);
   
  Throttle = Throttle / 4; //convert to 0 to 255
  LeftRight = LeftRight / 8 - 64; //convert to -64 to 63
  FwdBack = FwdBack / 4 - 128; //convert to -128 to 127
   
  sendCommand(Throttle, LeftRight, FwdBack);
}
