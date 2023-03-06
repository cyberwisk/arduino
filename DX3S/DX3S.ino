#include "Joystick.h"


// Create Joystick
Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID,JOYSTICK_TYPE_GAMEPAD,
  10, 1,                 // Button Count, Hat Switch Count
  true, true, true,  // X, Y, Z
  false, false, false,  // Rx, Ry, Rz
  false, false,          // Rudder, Throttle
  false, false, false);    // Accelerator, Brake, Steering


void setup() {
 pinMode(9, INPUT_PULLUP);
 pinMode(10, INPUT_PULLUP);
 pinMode(11, INPUT_PULLUP);
 pinMode(12, INPUT_PULLUP);
 pinMode(8, INPUT_PULLUP);
 pinMode(7, INPUT_PULLUP);
 pinMode(6, INPUT_PULLUP);
 pinMode(5, INPUT_PULLUP);
 
 Joystick.begin();
 Joystick.setXAxisRange(0, 1023);
 pinMode(A0, INPUT_PULLUP);
 Joystick.setYAxisRange(0, 1023);
 pinMode(A1, INPUT_PULLUP);
 }
const int pinToButtonMap = 5;

// Last state of the button
int lastButtonState[8] = {0,0,0,0,0,0,0,0};
void loop() {

 int pot1 = analogRead(A0);
 Joystick.setXAxis(pot1);
 int pot2 = analogRead(A1);
 Joystick.setYAxis(pot2);
 int pot3 = analogRead(A2);
 Joystick.setZAxis(pot3);
 
for (int index = 0; index < 8; index++)
 {
   int currentButtonState = !digitalRead(index + pinToButtonMap);
   if (currentButtonState != lastButtonState[index])
   {
     Joystick.setButton(index, currentButtonState);
     lastButtonState[index] = currentButtonState;
   }
 }

 delay(3);
}
