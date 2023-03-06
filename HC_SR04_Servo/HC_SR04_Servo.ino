/*
 HC-SR04 Ping distance sensor]
 VCC to arduino 5v GND to arduino GND
 Echo to Arduino pin 13 Trig to Arduino pin 12
 More info at: http://goo.gl/kJ8Gl
 */
#include <Servo.h>   

#define trigPin 9
#define echoPin 8


//Servo
int servopin = 10;
Servo myservo;     

int leftdist = 0;
int rightdist = 0;
      long duration, distance;

void setup() {
  Serial.begin (9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
      //Servo
  myservo.attach(servopin);
  myservo.write(90);

}

void loop() {
if (Serial.available() > 0) {

  // ping();  
   findroute();
}
}

void findroute() {
  lookleft();                  // Olha para esquerda e retorna distancia do objeto
  lookright();                 // Olha para direita e retorna distancia do objeto
  
 if ( leftdist > rightdist )  // decide para que lado virar 
  {
     Serial.print("I turnleft ");
     Serial.print(leftdist);
     Serial.print(" - ");
     Serial.println(rightdist);    
  }
 else
 {
     Serial.print("I turnright ");
     Serial.print(leftdist);
     Serial.print(" - ");
     Serial.println(rightdist);    
 }
}

 //Olha para Esquerda e retorna a distancia
void lookleft() {
  myservo.write(30);
  delay(200); 
  leftdist = ping();
  Serial.print("Qual vai ser? ");  
  Serial.print(rightdist);
  Serial.print(" - ");  
  Serial.println(leftdist);    
  myservo.write(90);
  delay(200);                      
  return;
}
//Olha para Direita e retorna a disntancia 
void lookright () {
  myservo.write(150);
  delay(200);     
  rightdist = ping();
  Serial.print("Qual vai ser? ");  
  Serial.print(rightdist);
  Serial.print(" - ");  
  Serial.println(leftdist);    
  myservo.write(90);                                  
  delay(200);                    
  return;
}

int ping() {
  // HC-SR04 ultrasonic distance sensor (Especifico)
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2) / 29.1;
  if (distance >= 200 || distance <= 0){
    Serial.print("Out of range -> ");
    Serial.print(distance);
    Serial.println(" cm");
  }
  else {
    Serial.print(distance);
    Serial.println(" cm");
  }
  delay(100);
  return distance;
}
