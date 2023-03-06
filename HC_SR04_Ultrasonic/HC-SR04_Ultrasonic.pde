#include <LiquidCrystal.h>
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

//Ping  HC-SR04 ultrasonic distance sensor
int trigPin = 11;
int echoPin = 12;
long duration, distance;

//LED pin attached to Arduino D13
int LED = 13;

void setup() {
  //Ping HC-SR04
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  lcd.begin(16, 2);
  lcd.print("Iniciando...");
  
  Serial.begin (9600);
  Serial.println("Iniciando...");
}

void loop()
{
   obstaculo = ping();
  //LCD   
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(obstaculo);
  lcd.print("cm");
  
  //Serial
  Serial.print(obstaculo);
  Serial.println(" cm");
  
  delay(100);
}


int ping() {
// HC-SR04 ultrasonic distance sensor (Especifico)
// A velocidade do som e de 340 m/s ou 29 microssegundos por centimetro.
// O ping e enviado para frente e reflete no objeto para encontrar a distancia
// A distancia do objeto fica na metade da distancia percorrida.

  digitalWrite(LED, LOW);
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2) / 29.1;
  digitalWrite(LED, HIGH);
  delay(100);
  return distance;
} // END Ping