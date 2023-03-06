/* --------------------------------------------
 * Author: Rui Santos - randomnerdtutorials.com
 * --------------------------------------------
 * ============================================
 * PROJETO: BARRA DE LED RGB WS2812 E ARDUINO *
 * ============================================
 * Modificado e Atualizado por: Rodrigo Costa.*
 * --------------------------------------------
 * Detalhes: https://eletronicaparatodos.com   *
 *           http://eletronicaparatodos.net   *
 * --------------------------------------------
 * REV.01 - 14/04/2017.                       *
 * ============================================
 */

#include <FastLED.h>

#define LED_PIN 12   //DEFINIMOS O PINO DIGITAL "5" DO ARDUINO PARA SER O NOSSO "DIN" DO MÓDULO WS2812

// Atenção! Altere a variável abaixo "NUM_LEDS" para o número de LEDs RGB que a sua régua de LEDs realmente tem
#define NUM_LEDS 1   // O NUMERO DE LEDs QUE TEM NA NOSSA "REGUA/TIRA" DE LEDs.

#define BRIGHTNESS 255   // AUMENTA OU DIMINUI A "VELOCIDADE" QUE OS LEDs MUDAM DE CORES (1 ~ 255).
#define LED_TYPE WS2812   // O MODELO DA NOSSA REGUA DE LEDs (PODERIA SER A WS2812B).
#define COLOR_ORDER GRB   // A ORDEM DAS CORES DA TIRA DE LEDs.

CRGB leds[NUM_LEDS];

#define UPDATES_PER_SECOND 60   //ATUALIZACAO DO DISPLAY DE LEDs EM 1 SEGUNDO.

CRGBPalette16 currentPalette;
TBlendType    currentBlending;

void setup() {
    delay(3000);   //Atraso de segurança na inicialização
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness( BRIGHTNESS );
    
    currentPalette = RainbowColors_p;   //PALHETA DE CORES SENDO UTILIZADA ATUALMENTE!
    //currentPallete = RainbowStripeColors_p;   //OUTRA PALHETA DE CORES QUE PODE SER UTILIZADA.
    //currentPalette = RainbowColors_p;   //OUTRA PALHETA DE CORES QUE PODE SER UTILIZADA.
    currentBlending = LINEARBLEND;
}

void loop(){
    ChangePalettePeriodically();
    
    static uint8_t startIndex = 0;
    startIndex = startIndex + 1;
    
    FillLEDsFromPaletteColors(startIndex);
    
    FastLED.show();
    FastLED.delay(2000 / UPDATES_PER_SECOND);
}

void FillLEDsFromPaletteColors(uint8_t colorIndex){
    //uint8_t brightness = 13;   //AUMENTA OU DIMINUI A "INTENSIDADE DE LUMINOSIDADE" QUE OS LEDs EMITEM (1 ~ 255) R.COSTA.
    uint8_t brightness = 200;   //AUMENTA OU DIMINUI A "INTENSIDADE DE LUMINOSIDADE" QUE OS LEDs EMITEM (1 ~ 255) R.COSTA.
    
    for( int i = 0; i < NUM_LEDS; i++){
        leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
        colorIndex += 3;
    }
}

// Existem várias palhetas de efeitos luminosos e diferentes cores demonstradas aqui.
//
// a biblioteca FastLED varias palhetas já configuradas como opcao de uso, como por exemplo:
//
// RaibowColors_p, RainbowColors_p, OceanColors_p, CloudColors_p, LavaColors_p, ForestColors_p and PartyColors_p
//
// Além disso, você pode digitar ou definir manualmente suas próprias palhetas de cores.

void ChangePalettePeriodically(){
    uint8_t secondHand = (millis() / 1000) % 60;
    static uint8_t lastSecond = 99;
    
    if(lastSecond != secondHand){
        lastSecond = secondHand;
        if(secondHand ==  0){ currentPalette = RainbowColors_p; currentBlending = LINEARBLEND;}
    }
}
