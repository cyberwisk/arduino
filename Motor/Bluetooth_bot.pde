// Bluetooth-bot v1
// Arduino Robotics unofficial chapter 14
// use Bluetooth Mate serial adapter to receive commands from PC
// Arduino decodes commands into motor movements
// Creates high-speed wireless serial link for robot control using keyboard
// Uses keys "i" = forward, "j" = left, "k" = reverse, and "l" = right
// speed control is also implemented using "," = speed down, "." = speed up, and "/" = max speed.

// notes in the melody:
int melody[] = {196, 262};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = { 4, 8, 8, 4,4,4,4,4 };

// L298 motor control variables
int M1_A = 12;
int M1_PWM = 11;
int M1_B = 10;

int M2_A = 4;
int M2_PWM = 3;
int M2_B = 2;

//Ping
int pingPin = 6;
int inPin = 5;

// LED pin attached to Arduino D13
int LED = 13;

// variable to store serial data
int incomingByte = 0;

// variable to store speed value
int speed_val = 255;

//////////////////////////////


void setup(){
 
TCCR2B = TCCR2B & 0b11111000 | 0x01; // change PWM frequency for pins 3 and 11 to 32kHz so there will be no motor whining

// Start serial monitor at 115,200 bps
Serial.begin(9600);

// declare outputs
pinMode(LED, OUTPUT);

pinMode(M1_A, OUTPUT);
pinMode(M1_PWM, OUTPUT);
pinMode(M1_B, OUTPUT);

pinMode(M2_A, OUTPUT);
pinMode(M2_PWM, OUTPUT);
pinMode(M2_B, OUTPUT);

// turn motors Off by default
M1_stop();
M2_stop();

delay(500);

  // iterate over the notes of the melody:
  for (int thisNote = 0; thisNote < 8; thisNote++) {

    // to calculate the note duration, take one second
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000/noteDurations[thisNote];
    tone(8, melody[thisNote],noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(8);
  }


}

////////////////////////////////////

void loop(){
  // Ping
  // establish variables for duration of the ping,
  // and the distance result in inches and centimeters:
  long duration, cm;

  // The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  pinMode(pingPin, OUTPUT);
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(pingPin, LOW);

  // The same pin is used to read the signal from the PING))): a HIGH
  // pulse whose duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(inPin, INPUT);
  duration = pulseIn(inPin, HIGH);

  // convert the time into a distance
  cm = microsecondsToCentimeters(duration);
  Serial.println(cm);
   
  //delay(100);
  //END Ping

// check for serial data
if (Serial.available() > 0) {
// read the incoming byte:
incomingByte = Serial.read();
// if available, blink LED and print serial data received.
digitalWrite(LED, HIGH);
// say what you got:
Serial.print("I received: ");
Serial.println(incomingByte);
// delay 10 milliseconds to allow serial update time
delay(10);

// check incoming byte for direction
// if byte is equal to "46" or "," - raise speed
if (incomingByte == 46){
speed_val = speed_val + 5;
test_speed();
Serial.println(speed_val);
}
// if byte is equal to "44" or "." - lower speed
else if (incomingByte == 44){
speed_val = speed_val - 5;
test_speed();
Serial.println(speed_val);
}
// if byte is equal to "47" or "/" - max speed
else if (incomingByte == 47){
speed_val = 255;
test_speed();
}

// if byte is equal to "105" or "i", go forward
else if (incomingByte == 105){
M1_forward(speed_val);
M2_forward(speed_val);
delay(25);
}
// if byte is equal to "106" or "j", go left
else if (incomingByte == 106){
M2_reverse(speed_val);
M1_forward(speed_val);
delay(25);
}
// if byte is equal to "108" or "l", go right
else if (incomingByte == 108){
M2_forward(speed_val);
M1_reverse(speed_val);
delay(25);
}
// if byte is equal to "107" or "k", go reverse
else if (incomingByte == 107){
M1_reverse(speed_val);
M2_reverse(speed_val);
delay(25);
}
// otherwise, stop both motors
else {
M1_stop();
M2_stop();
}


}

else {
M1_stop();
M2_stop();
digitalWrite(LED, LOW);
}
}

void test_speed(){
// constrain speed value to between 0-255
if (speed_val > 250){
speed_val = 255;
Serial.println(" MAX ");
}
if (speed_val < 0){
speed_val = 0;
Serial.println(" MIN ");
}

}

/////////// motor functions ////////////////

void M1_reverse(int x){
digitalWrite(M1_B, LOW);
digitalWrite(M1_A, HIGH);
analogWrite(M1_PWM, x);
}

void M1_forward(int x){
digitalWrite(M1_A, LOW);
digitalWrite(M1_B, HIGH);
analogWrite(M1_PWM, x);
}

void M1_stop(){
digitalWrite(M1_B, LOW);
digitalWrite(M1_A, LOW);
digitalWrite(M1_PWM, LOW);
}

void M2_forward(int y){
digitalWrite(M2_B, LOW);
digitalWrite(M2_A, HIGH);
analogWrite(M2_PWM, y);
}

void M2_reverse(int y){
digitalWrite(M2_A, LOW);
digitalWrite(M2_B, HIGH);
analogWrite(M2_PWM, y);
}

void M2_stop(){
digitalWrite(M2_B, LOW);
digitalWrite(M2_A, LOW);
digitalWrite(M2_PWM, LOW);
}

long microsecondsToCentimeters(long microseconds)
{
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29 / 2;
}








