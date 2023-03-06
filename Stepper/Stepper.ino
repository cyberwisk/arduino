#include <Stepper.h>
 
const int stepsPerRevolution = 300;  // define os passos por volta
 
// Definindo os pinos de comunicação
// (8 e 9 de um lado, 10 e 11 do outro)
Stepper myStepper(stepsPerRevolution, 8,9,10,11);
 
void setup() {
  //definindo velocidade (80 RPM):
  myStepper.setSpeed(80);
  // Inicializa a serial port
  Serial.begin(9600);
}
//inicia o loop de repetição girando vamos definir para girar em sentido horário e anti horário
void loop() {
  // rotação em sentido horario clockwise
   Serial.println("clockwise");
  myStepper.step(stepsPerRevolution);
  delay(500);
 
   // anti horário counterclockwise
  Serial.println("counterclockwise");
  myStepper.step(-stepsPerRevolution);
  delay(500);
 
}