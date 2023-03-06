/*
 * IRremote: IRrecvDemo - demonstrates receiving IR codes with IRrecv
 * An IR detector/demodulator must be connected to the input RECV_PIN.
 * Version 0.1 July, 2009
 * Copyright 2009 Ken Shirriff
 * http://arcfn.com
 */

#include <IRremote.h>

int motor[] = {2, 3, 4, 7, 5, 6};
// indice: Motor L -, Motor L +, Motor R -, Motor R +, Velocidade motor Direito, Velocidade motor Esquerdo

int RECV_PIN = 11;
IRrecv irrecv(RECV_PIN);
decode_results results;

// LED pin attached to Arduino D13
int LED = 13;
int speed_val = 155;
int autoroute;
int onoff;

void setup()
{
  Serial.begin(9600);
  
  // Start the receiver
  irrecv.enableIRIn(); 
  
  //L298 motor control
  for (int i = 0; i <= 5; i++) pinMode(motor[i],OUTPUT);
  pinMode(LED, OUTPUT);
}

void loop() {
infrared();
}

//Rotina que controla os motores L298 Dual motor (ponte H)
//velocidade X, inteiro de 0 a 255
//Direcao Y 1=Frente, 2=Marcha Re, 3=Esquerda, 4=Direita, 0=Para 
void MOTOR(int X, int Y) {
    if (X >= 255){X = 255;}         //Trava no 255
    if (X <= 0){X = 0;}             //Trava no 0
    analogWrite(motor[4], X);       //Velocidade motor Direito
    analogWrite(motor[5], X );      //Velocidade motor Esquerdo
    digitalWrite(LED, HIGH);
  switch (Y){
      case 1:                       //Rotina Para Frente
      digitalWrite(motor[0],LOW);   //Motor L -
      digitalWrite(motor[1],HIGH);  //Motor L +
      digitalWrite(motor[2],LOW);   //Motor R -
      digitalWrite(motor[3],HIGH);  //Motor R +
      break;

      case 2:                       //Rotina de marcha Re, inverte os dois motores
      digitalWrite(motor[0],HIGH);  //Motor L -
      digitalWrite(motor[1],LOW);   //Motor L +
      digitalWrite(motor[2],HIGH);  //Motor R -
      digitalWrite(motor[3],LOW);   //Motor R +
      break;
  
      case 3:                       //Inverte motor esquerdo virando para esquerda
      digitalWrite(motor[0],HIGH);  //Motor L -
      digitalWrite(motor[1],LOW);   //Motor L +
      digitalWrite(motor[2],LOW);   //Motor R -
      digitalWrite(motor[3],HIGH);  //Motor R +
      break;

      case 4:                       //Inverte o motor direito Virando para direita
      digitalWrite(motor[0],LOW);   //Motor L +
      digitalWrite(motor[1],HIGH);  //Motor L -
      digitalWrite(motor[2],HIGH);  //Motor R +
      digitalWrite(motor[3],LOW);   //Motor R -
      break;
	  
	  case 5:                       //Frea motor esquerdo virando para esquerda com mais velocidade 
      digitalWrite(motor[0],HIGH);  //Motor L -
      digitalWrite(motor[1],LOW);   //Motor L +
      digitalWrite(motor[2],LOW);   //Motor R -
      digitalWrite(motor[3],LOW);   //Motor R +
      break;

      case 6:                       //Frea o motor direito Virando para direita com mais velocidade 
      digitalWrite(motor[0],LOW);   //Motor L +
      digitalWrite(motor[1],LOW);   //Motor L -
      digitalWrite(motor[2],HIGH);  //Motor R +
      digitalWrite(motor[3],LOW);   //Motor R -
      break;

      case 0:                       //Motor Parado
      digitalWrite(motor[0],LOW);   //Motor L +
      digitalWrite(motor[1],LOW);   //Motor L -
      digitalWrite(motor[2],LOW);   //Motor R +
      digitalWrite(motor[3],LOW);   //Motor R -
      break;
  } //EOF switch
  digitalWrite(LED, LOW);
}// EOF motor

// Controle por infra vermelho (IR)
// Pioneer Cxa5857
void infrared(){
  if (irrecv.decode(&results)) {
    Serial.println(results.value);
    if (results.value == 3041526525LL){ // Remote ^
         digitalWrite(LED, HIGH);
		 Serial.println("IR Para Frente");
         MOTOR(speed_val,1); //Frente
         delay (25);
    }
     else if (results.value == 3041575485LL){ // Remote >
         digitalWrite(LED, LOW); 
		 Serial.println("IR Virando para Direita");
         MOTOR(speed_val,4); //Direita
         delay (25);
     }
     else if (results.value == 3041542845LL){ // Remote <
         digitalWrite(LED, LOW);
		 Serial.println("IR Virando para Esquerda");
         MOTOR(speed_val,3); //Esquerda
         delay (25);
     }
     else if (results.value == 3041559165LL){ // Remote v
         digitalWrite(LED, LOW);
		 Serial.println("IR Marcha Re");
         MOTOR(speed_val,2); //Macha Ré
         delay (25);
     }
     else if (results.value == 3041546415LL){ // Remote +
         digitalWrite(LED, LOW);
	     speed_val = speed_val + 5;
	     Serial.print("IR Speed + ");
         Serial.println(speed_val);
         delay (25);
     }
     else if (results.value == 3041579055LL){ // Remote -
         digitalWrite(LED, LOW);
	     speed_val = speed_val - 5;
	     Serial.print("IR Speed - ");
         Serial.println(speed_val);
         delay (25);
     }
	 else if (results.value == 3041536215LL){ // Remote BMS
        Serial.println("IR Pare!");
		MOTOR(0,0);
        //digitalWrite(LED, LOW);
     }
     else if (results.value == 3041556615LL ){ // Remote CD
        if (onoff == 1) onoff = 0; else onoff = 1;
	   Serial.print("IR onoff");
	   Serial.println(onoff);
       delay(25);
     } //onoff
     else if (results.value == 3041540295LL){ // Remote TUNER
        if (autoroute == 1) autoroute = 0; else autoroute = 1;
	   Serial.print("IR Autoroute");
	   Serial.println(autoroute);
       delay(25);
     } //autoroute
    irrecv.resume(); // Receive the next value
  } 
}//EOF IR