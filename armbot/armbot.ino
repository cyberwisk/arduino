// inclui bibilioteca do servomotor
#include <Servo.h> 

// define pinos dos servos
#define pinServ1 2
#define pinServ2 3
#define pinServ3 4
#define pinServ4 5
#define button 8
#define speakerPin 9
#define led2 10
#define ledPin 13

// define as portas dos potenciometros
#define pot1 A0
#define pot2 A1
#define pot3 A2
#define pot4 A3

// nomeia os servos
Servo serv1,serv2,serv3,serv4;

// cria as variavies dos angulos de cada motor
int motor1,motor2,motor3,motor4;

unsigned long mostradorTimer = 1;
const unsigned long intervaloMostrador = 1000;

void phrase1() {
    
    int k = random(1000,2000);
    digitalWrite(ledPin, HIGH);
    for (int i = 0; i <=  random(100,2000); i++){
        
        tone(speakerPin, k+(-i*2));          
        delay(random(.9,2));             
    } 
    digitalWrite(ledPin, LOW);   
    for (int i = 0; i <= random(100,1000); i++){
        
        tone(speakerPin, k + (i * 10));          
        delay(random(.9,2));             
    } 
}
void phrase2() {
    
    int k = random(1000,2000);
    digitalWrite(ledPin, HIGH);  
    for (int i = 0; i <= random(100,2000); i++){
        
        tone(speakerPin, k+(i*2));          
        delay(random(.9,2));             
    } 
    digitalWrite(ledPin, LOW);   
    for (int i = 0; i <= random(100,1000); i++){
        
        tone(speakerPin, k + (-i * 10));          
        delay(random(.9,2));             
    } 
}


void setup() {

  //inicia o monitor serial
  Serial.begin(9600); 

  // atribui pinos dos servos
  serv1.attach(pinServ1);
  serv2.attach(pinServ2);
  serv3.attach(pinServ3);
  serv4.attach(pinServ4);

    int K = 2000;
    switch (random(1,7)) {
        
        case 1:phrase1(); break;
        case 2:phrase2(); break;
        case 3:phrase1(); phrase2(); break;
        case 4:phrase1(); phrase2(); phrase1();break;
        case 5:phrase1(); phrase2(); phrase1(); phrase2(); phrase1();break;
        case 6:phrase2(); phrase1(); phrase2(); break;
    }
    for (int i = 0; i <= random(3, 9); i++){
        
        digitalWrite(ledPin, HIGH);  
        tone(speakerPin, K + random(-1700, 2000));          
        delay(random(70, 170));  
        digitalWrite(ledPin, LOW);           
        noTone(speakerPin);         
        delay(random(0, 30));             
    } 
    noTone(speakerPin);  
  
}

void loop(){

   digitalWrite(led2, HIGH);
  // leitura dos potenciometros
  motor1 = map(analogRead(pot1),1023,0,0,180);
  motor2 = map(analogRead(pot2),1023,0,0,180);
  motor3 = map(analogRead(pot3),0,1023,0,180);
  motor4 = map(analogRead(pot4),0,1023,0,180);

  // posicionamento dos potenciometros 
  serv1.write(motor1);
  serv2.write(motor2);
  serv3.write(motor3);
  serv4.write(motor4);

   if ((millis() - mostradorTimer) >= intervaloMostrador) {

  // envio para o monitor serial do posicionamentos dos motores
  Serial.println("**********************************************");
  
  Serial.print("Pot1:");
  Serial.print(analogRead(pot1));
  Serial.print(" Angulo Motor1:");
  Serial.println(motor1);

  Serial.print("Pot2:");
  Serial.print(analogRead(pot2));
  Serial.print(" Angulo Motor2:");
  Serial.println(motor2);

  Serial.print("Pot3:");
  Serial.print(analogRead(pot3));
  Serial.print(" Angulo Motor3:");
  Serial.println(motor3);

  Serial.print("Pot4:");
  Serial.print(analogRead(pot4));
  Serial.print(" Angulo Motor4:");
  Serial.println(motor4);

  mostradorTimer = millis();
  }

  // tempo de espera para recomeçar
  delay(100);
   digitalWrite(led2, LOW);
}
