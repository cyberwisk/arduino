int pinoSensor = 0; //pino que está ligado o terminal central do LM35 (porta analogica 0)
int valorLido = 0; //valor lido na entrada analogica
float temperatura = 0; //valorLido convertido para temperatura
 
void setup() {
 Serial.begin(9600); //Inicializa comunicação Serial
}
 
void loop() {
 valorLido = analogRead(pinoSensor);
 temperatura = (valorLido * 0.00488); // 5V / 1023 = 0.00488 (precisão do A/D)
 temperatura = temperatura * 100; //Converte milivolts para graus celcius, lembrando que a cada 10mV equivalem a 1 grau celcius
 Serial.print("Temperatura atual: ");
 Serial.println(temperatura);
 delay(1000); //espera 5 segundos para fazer nova leitura
}