

/*
fht_adc.pde
guest openmusiclabs.com 9.5.12
example sketch for testing the fht library.
it takes in data on ADC0 (Analog0) and processes them
with the fht. the data is sent out over the serial
port at 115.2kb.  there is a pure data patch for
visualizing the data.
*/

/*  This is an implementation based on the same example found
 *  in the original fht_adc.ino to control 3 led matrices as
 *  a simple and very functional spectrum analyser reading 
 *  values directly from the output of a real sound device
 *  like a cellphone or a line out from a computer. Use a 
 *  ceramic capacitor in parallel with the + output (R or L) and 
 *  ground to avoid some noise. The three matrices used in this example came
 *  from www.icstation.com, if you mount them in daisy chain putting
 *  jumpers on the conectors this code will display the bars
 *  as lines instead of columns, and the drawing will be a little
 *  slower. Instead of using jumpers use cables to connect the pins
 *  and rotate the matrices until find the correct orientation.
 *  This code was intended to work with an altered LedControl library
 *  where the setRows was swaped to setColumn vice versa, if you
 *  have the default LedControl just change lc.setColumn to lc.setRow
*/

// Setting some properties of the FHT class
#define LOG_OUT 1 // use the log output function
//#define OCTAVE 1
//#define OCT_NORM 1
#define FHT_N 256 // set to 256 point fht

#include <FHT.h> // include the library
#include <LedControl.h>

// Setting the led matrices
const byte numDevices = 3;      // number of MAX7219s used
// inputs: DIN pin, CLK pin, LOAD pin. number of chips
#define blue A3// data in
#define green A1// clock pin
#define yellow A2//cs pin 

LedControl lc = LedControl(blue, green, yellow, numDevices);
unsigned char dotMode = 0;
/*  This function inverts a bits sequence
    like 00110011 turns to 11001100, we can
    use this in case the bars on your led matrix
    is upside down like in this example
*/
unsigned char bitswap (unsigned char x)
{
 byte result;
   asm("mov __tmp_reg__, %[in] \n\t"
     "lsl __tmp_reg__  \n\t"   /* shift out high bit to carry */
     "ror %[out] \n\t"  /* rotate carry __tmp_reg__to low bit (eventually) */
     "lsl __tmp_reg__  \n\t"   /* 2 */
     "ror %[out] \n\t"
     "lsl __tmp_reg__  \n\t"   /* 3 */
     "ror %[out] \n\t"
     "lsl __tmp_reg__  \n\t"   /* 4 */
     "ror %[out] \n\t"
     
     "lsl __tmp_reg__  \n\t"   /* 5 */
     "ror %[out] \n\t"
     "lsl __tmp_reg__  \n\t"   /* 6 */
     "ror %[out] \n\t"
     "lsl __tmp_reg__  \n\t"   /* 7 */
     "ror %[out] \n\t"
     "lsl __tmp_reg__  \n\t"   /* 8 */
     "ror %[out] \n\t"
     : [out] "=r" (result) : [in] "r" (x));
     return(result);
}

/* We started with values above the noise floor,
   in this case, there are some noise gathered
   by the ADC. The FHT processes the noise with
   values at 64 tops. So we draw the bars starting
   with 64, the lower the noise values the better
   overall result is
*/
int bar (int v){
    if(v == 0)
        return 0;            //--------
    if(v >= 64 && v < 88)
        return bitswap(1);   //*-------
    if(v > 88 && v < 112)
        return bitswap(3 + dotMode);   //**------
    if(v >= 112 && v < 136)
        return bitswap(7 + dotMode);   //***-----
    if(v >= 136 && v < 160)
        return bitswap(15 + dotMode);  //****----
    if(v >= 160 && v < 184)
        return bitswap(31 + dotMode);  //*****---
    if(v >= 184 && v < 208)
        return bitswap(63 + dotMode);  //******--
    if(v >= 208 && v < 232)
        return bitswap(127 + dotMode); //*******-
    if(v >= 232 )
        return bitswap(255 + dotMode); //*******
    return 0;
}



void setup() {
    Serial.begin(115200); // use the serial port
    // Turn off the led built in
    pinMode(LED_BUILTIN, OUTPUT);
	pinMode(A0, INPUT);
    digitalWrite(LED_BUILTIN, LOW);
    // inicializa as matrizes
    for(byte i = 0; i < numDevices; i++){
        lc.shutdown(i, false);  // turns on display
        lc.setIntensity(i, 1); // 15 = brightest
        lc.clearDisplay(i);
        // This will show a moving column just to test the devices
        for(int j = 0; j < 8; j++){
            lc.setColumn(i, j, (byte)255);
            delay(30);
            lc.setColumn(i, j, (byte)0);
        }
    }
    TIMSK0 = 0; // turn off timer0 for lower jitter
    ADCSRA = 0xe5; // set the adc to free running mode
    ADMUX = 0x40; // use adc0
    DIDR0 = 0x01; // turn off the digital input for adc0*/
}

/*
    This is used to calculate the average between an interval
    of frequencies and to smooth the values on the spectrum 
 */
int av(unsigned char *dArray, int x, int y){
    int sum = 0;
    int cont = 0;
    while(x <= y){
        sum += dArray[x];
        cont++;
        x++;
    }
    return sum/cont;
}

void loop() {
    while(1) { // reduces jitter
        cli();  // UDRE interrupt slows this way down on arduino1.0
        for (int i = 0 ; i < FHT_N ; i++) { // save 256 samples
            while(!(ADCSRA & 0x10)); // wait for adc to be ready
            ADCSRA = 0xf5; // restart adc
            byte m = ADCL; // fetch adc data
            byte j = ADCH;
            int k = (j << 8) | m; // form into an int
            k -= 0x0200; // form into a signed int
            k <<= 6; // form into a 16b signed int
            fht_input[i] = k;
        }
			
		

        fht_window(); // window the data for better frequency response
        fht_reorder(); // reorder the data before doing the fht
        fht_run(); // process the data in the fht
        fht_mag_log(); // take the output of the fht
        //fht_mag_octave();
        sei();
        Serial.write(255); // send a start byte
        Serial.write(fht_log_out, FHT_N/2); // send out the data
        
        // Eliminate some noise floor
        for (int i = 0 ; i < FHT_N / 2 ; i++) {
        	if(fht_log_out[i] > 20)
				fht_log_out[i] = fht_log_out[i] - 24;
		}
        
		// First matrix
		
        lc.setColumn(0, 0, bar((fht_log_out[0] ) * 1.2));
        lc.setColumn(0, 1, bar((fht_log_out[1] ) * 1.2));
        lc.setColumn(0, 2, bar((fht_log_out[2]) * 1.4));  
        lc.setColumn(0, 3, bar(av(fht_log_out, 3, 8) * 1.6));
        lc.setColumn(0, 4, bar(av(fht_log_out, 9, 14) * 1.8));
        lc.setColumn(0, 5, bar(av(fht_log_out, 15, 20) * 1.8));
        lc.setColumn(0, 6, bar(av(fht_log_out, 21, 26) * 1.8));
        lc.setColumn(0, 7, bar(av(fht_log_out, 27, 32) * 1.8));
        // Second matrix
        lc.setColumn(1, 0, bar(av(fht_log_out, 33, 40) * 2));
        lc.setColumn(1, 1, bar(av(fht_log_out, 41, 46) * 2));
        lc.setColumn(1, 2, bar(av(fht_log_out, 47, 52) * 2));
        lc.setColumn(1, 3, bar(av(fht_log_out, 53, 58) * 2));
        lc.setColumn(1, 4, bar(av(fht_log_out, 59, 64) * 2));
        lc.setColumn(1, 5, bar(av(fht_log_out, 65, 70) * 2));
        lc.setColumn(1, 6, bar(av(fht_log_out, 71, 76) * 2));
        lc.setColumn(1, 7, bar(av(fht_log_out, 77, 82) * 2));
        // Third matrix
        lc.setColumn(2, 0, bar(av(fht_log_out, 83, 88) * 2.1));
        lc.setColumn(2, 1, bar(av(fht_log_out, 89, 94) * 2.1));
        lc.setColumn(2, 2, bar(av(fht_log_out, 95, 100) * 2.1));
        lc.setColumn(2, 3, bar(av(fht_log_out, 101, 106) * 2.2));
        lc.setColumn(2, 4, bar(av(fht_log_out, 107, 112) * 2.2));
        lc.setColumn(2, 5, bar(av(fht_log_out, 113, 118) * 2.2));
        lc.setColumn(2, 6, bar(av(fht_log_out, 119, 124) * 2.2));
        lc.setColumn(2, 7, bar(av(fht_log_out, 125, 127) * 2.2));
        
  } 
}
