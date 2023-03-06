// Controle do Motor
int motor[] = {2, 3, 4, 7, 5, 6};
// indice: Motor L -, Motor L +, Motor R -, Motor R +, Velocidade motor Direito, Velocidade motor Esquerdo


void MOTOR_forward(int X) {
 if (onoff == 1) {
    analogWrite(motor[4], X);    //Velocidade motor Direito
    analogWrite(motor[5], X );   //Velocidade motor Esquerdo
    digitalWrite(LED, HIGH);
    digitalWrite(motor[0],LOW);   //Motor L -
    digitalWrite(motor[1],HIGH);  //Motor L +
    digitalWrite(motor[2],LOW);   //Motor R -
    digitalWrite(motor[3],HIGH);  //Motor R +
    digitalWrite(LED, LOW);
   return;
  }
 }

// Rotina de marcha Re, inverte os dois motores
void MOTOR_backward(int X) {
   analogWrite(motor[4], X);     //Velocidade motor Direito
   analogWrite(motor[5], X);     //Velocidade motor Esquerdo
   digitalWrite(motor[0],HIGH);  //Motor L -
   digitalWrite(motor[1],LOW);   //Motor L +
   digitalWrite(motor[2],HIGH);  //Motor R -
   digitalWrite(motor[3],LOW);   //Motor R +
  return;
}

void MOTOR_turnleft (int X) {   //inverte motor esquerdo virando para esquerda
   analogWrite(motor[4], X);     //Velocidade motor Direito
   analogWrite(motor[5], X);     //Velocidade motor Esquerdo
   digitalWrite(motor[0],HIGH);  //Motor L -
   digitalWrite(motor[1],LOW);   //Motor L +
   digitalWrite(motor[2],LOW);   //Motor R -
   digitalWrite(motor[3],HIGH);  //Motor R +
  return;
}

void MOTOR_turnright (int X) {  //inverte o motor direito Virando para direita
   analogWrite(motor[4], X);     //Velocidade motor Direito
   analogWrite(motor[5], X);     //Velocidade motor Esquerdo
   digitalWrite(motor[0],LOW);   //Motor L +
   digitalWrite(motor[1],HIGH);  //Motor L -
   digitalWrite(motor[2],HIGH);  //Motor R +
   digitalWrite(motor[3],LOW);   //Motor R -
  return;
}

void MOTOR_halt () {        //Parado
   analogWrite(motor[4], 0); //Velocidade motor Direito
   analogWrite(motor[5], 0); //Velocidade motor Esquerdo
   digitalWrite(motor[0],LOW);
   digitalWrite(motor[1],LOW);
   digitalWrite(motor[2],LOW);
   digitalWrite(motor[3],LOW);
   delay(250);
  return;
}
