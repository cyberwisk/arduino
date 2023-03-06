#include <Ultrasonic.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#define TRIGGER_PIN  A2
#define ECHO_PIN     A3
Ultrasonic ultrasonic(TRIGGER_PIN, ECHO_PIN);
float dist;
float tot;
int bot_mede = 8;
int sbot_mede;
int bot_tot = 9;
int sbot_tot;
int bot_modo = 10;
int sbot_modo;
int modo = 1;
int laser = 11;
int pot = A4;
int contraste = 30;

// 7-clk 6-Din 5-DC 4-CE 3-Rst 2-Bl
Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);

void setup() {
  pinMode(bot_mede, INPUT);
  pinMode(bot_tot,  INPUT);
  pinMode(bot_modo, INPUT);
  pinMode(laser,    OUTPUT);
  pinMode(pot, INPUT);
  display.begin();
  display.setContrast(30); //Ajusta o contraste do display
  display.clearDisplay();
  display.setTextColor(BLACK);
  display.setTextSize(2);  //Seta o tamanho do texto
  display.setCursor(12,2);  
  display.print("TRENA");
  display.setTextColor(WHITE, BLACK);
  display.setTextSize(1);  //Seta o tamanho do texto
  display.setCursor(0,20);  
  display.print("    Digital   ");
  display.drawRoundRect(0,30,83,18,1, BLACK);  //box tot
  display.setTextColor(BLACK);
  display.setCursor(3,35);  
  display.print("v a s s i l i");
  display.display();
  delay(1500);
  
}

void loop() {
  contraste = analogRead(pot);
  contraste = map(contraste, 0,255,20,53);
  display.setContrast(contraste); //Ajusta o contraste do display
  tela();
  delay(100);

  sbot_mede = digitalRead(bot_mede);
  if (sbot_mede == HIGH){
    digitalWrite(laser, HIGH);    
    distancia();
    tela();
    delay(800);
    digitalWrite(laser, LOW);
  }
  sbot_tot = digitalRead(bot_tot);
  if (sbot_tot == HIGH){
    tot = tot + dist;
    tela();
    delay(400);
  }

  sbot_modo = digitalRead(bot_modo);
  if (sbot_modo == HIGH){
    modo = -modo;
    distancia();
    tela();
    delay(400);
  }

}

void tela(){
  display.clearDisplay();
  display.fillRoundRect(0,0,83,18,1, BLACK);  //box medida
  display.drawRoundRect(0,20,41,28,1, BLACK);  //box tot
  display.drawRoundRect(43,20,41,28,1, BLACK);  //box modo
  display.setTextSize(2);  //Seta o tamanho do texto
  display.setTextColor(WHITE, BLACK);
  display.setCursor(2,2);  
  display.print(dist);
  display.setCursor(5,22);  
  display.setTextColor(BLACK); 
  display.setTextSize(1);  //Seta o tamanho do texto
  display.print("Total");
  display.setCursor(2,35);  
  if (tot >= 999) {display.print(tot/100); display.print("m");}
  else display.print(tot);
  display.drawRect(45,35,15,8,BLACK);
  display.fillRect(53,36,5,6,BLACK);
  display.drawCircle(48,37,1,BLACK);
  display.drawCircle(48,40,1,BLACK);
  display.drawLine(61,37,61,40,BLACK);
  display.drawLine(63,37,63,40,BLACK);
  display.drawLine(65,37,65,40,BLACK);
  if (modo == 1) display.fillRect(59,25,20,4,BLACK);   //modo1
  if (modo == -1) display.fillRect(45,25,35,4,BLACK);   //modo2
  display.display();
}

void distancia(){
  long microsec = ultrasonic.timing();    //distância
  dist = ultrasonic.convert(microsec, Ultrasonic::CM);   //converte para cm
  if (modo == -1) dist = dist + 13;   // adicionar o tamanho do dispositivo
}
