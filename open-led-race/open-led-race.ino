/*
 * ____                     _      ______ _____    _____
  / __ \                   | |    |  ____|  __ \  |  __ \
 | |  | |_ __   ___ _ __   | |    | |__  | |  | | | |__) |__ _  ___ ___
 | |  | | '_ \ / _ \ '_ \  | |    |  __| | |  | | |  _  // _` |/ __/ _ \
 | |__| | |_) |  __/ | | | | |____| |____| |__| | | | \ \ (_| | (_|  __/
  \____/| .__/ \___|_| |_| |______|______|_____/  |_|  \_\__,_|\___\___|
        | |
        |_|
 Open LED Race
 An minimalist cars race for LED strip

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.

 First public version by:
    Angel Maldonado (https://gitlab.com/angeljmc) 
    Gerardo Barbarov (gbarbarov AT singulardevices DOT com)  

  Basen on original idea and 2 players code by: 
    Gerardo Barbarov  for Arduino day Seville 2019
    https://github.com/gbarbarov/led-race
    
 Public Repository for this code:
   https://gitlab.com/open-led-race/olr-arduino

*/

 
// 2020/12/10 - Ver 0.9.6
//   --see changelog.txt

char const softwareId[] = "A4P0";  // A4P -> A = Open LED Race, 4P0 = Game ID (4P = 4 Players, 0=Type 0)
char const version[] = "0.9.6";



#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>
#include "olr-lib.h"
#include "olr-param.h"
#include "SoftTimer.h"
#include "C:\Users\aavan\OneDrive\Arduino\open-led-race\SerialCommand.h"

#define PIN_LED        10    // R 500 ohms to DI pin for WS2812 and WS2813, for WS2813 BI pin of first LED to GND  ,  CAP 1000 uF to VCC 5v/GND,power supplie 5V 2A
#define PIN_AUDIO      13    // through CAP 2uf to speaker 8 ohms

#define REC_COMMAND_BUFLEN  32  // received command buffer size
#define TX_COMMAND_BUFLEN   80  // send command buffer size
#define EOL            '\n' // End of Command char used in Protocol

#define COLOR1         track.Color(255,0,0)
#define COLOR2         track.Color(0,255,0)
#define COLOR3         track.Color(0,0,255)
#define COLOR4         track.Color(255,255,255)

#define COLOR_RAMP     track.Color(64,0,64)
#define COLOR_COIN     track.Color(0,255,255)
#define COLOR_BOXMARKS track.Color(64,64,0)
#define LED_SEMAPHORE  12 

#define CONTDOWN_PHASE_DURATION  2000 
#define CONTDOWN_STARTSOUND_DURATION  40 

#define NEWRACE_DELAY  5000 

enum{
  MAX_CARS = 4,
};


enum loglevel {  // used in Serial Protocol "!" command (send log/error messageS)
    ECHO = 0,
    DISABLE = 0,
    LOG = 1,
    WARNING = 2,
    ERROR = 3
};


enum resp{
  NOK   = -1,
  NOTHING = 0,
  OK      = 1
};

typedef struct ack{
  enum resp rp;
  char type;
}ack_t;



struct cfgcircuit{
    int outtunnel;
};

enum phases{
    IDLE = 0,
    CONFIG,
    CONFIG_OK,
    READY,
    COUNTDOWN,
    RACING,
    PAUSE,
    RESUME,
    COMPLETE,
    RACE_PHASES
};

struct race{
    struct cfgrace    cfg;
    struct cfgcircuit circ;
    bool              newcfg;
    enum phases       phase;
    byte              numcars;
    int               winner; 
};


byte SMOTOR=0;
int TBEEP=0;
int FBEEP=0;

/*------------------------------------------------------*/
enum loglevel verbose = DISABLE;

static struct race race;
static car_t cars[ MAX_CARS ];
static controller_t switchs[ MAX_CARS ];
static track_t tck;

static int const eeadrInfo = 0; 






static unsigned long lastmillis = 0;

SoftTimer customDelay = SoftTimer(); // non blocking delay()

// Used to manage countdown phases
int     countdown_phase=1;
bool    countdown_new_phase=true;


int win_music[] = {
  2637, 2637, 0, 2637,
  0, 2093, 2637, 0,
  3136
};

//int TBEEP=3;

char tracksID[ NUM_TRACKS ][2] ={"U","M","B","I","O"};

/*  ----------- Function prototypes ------------------- */

void sendResponse( ack_t *ack);

ack_t manageSerialCommand();

void printdebug( const char * msg, int errlevel );
void print_cars_positions( car_t* cars);
void run_racecycle( void );
void draw_winner( track_t* tck, uint32_t color);


char cmd[REC_COMMAND_BUFLEN]; // Stores command received by ReadSerialComand()
SerialCommand serialCommand = SerialCommand(cmd, REC_COMMAND_BUFLEN, EOL, &Serial); // get complete command from serial

char txbuff[TX_COMMAND_BUFLEN];

Adafruit_NeoPixel track;


/*
 * 
 */
void setup() {

  Serial.begin(115200);
  randomSeed( analogRead(A6) + analogRead(A7) );
  controller_setup( );
  param_load( &tck.cfg );

  track = Adafruit_NeoPixel( tck.cfg.track.nled_total, PIN_LED, NEO_GRB + NEO_KHZ800 );

  controller_init( &switchs[0], DIGITAL_MODE, DIG_CONTROL_1 );
  car_init( &cars[0], &switchs[0], COLOR1 );
  controller_init( &switchs[1], DIGITAL_MODE, DIG_CONTROL_2 );
  car_init( &cars[1], &switchs[1], COLOR2 );

  race.numcars = 2;

  if( controller_isActive( DIG_CONTROL_3 )) {
    controller_init( &switchs[2], DIGITAL_MODE, DIG_CONTROL_3 );
    car_init( &cars[2], &switchs[2], COLOR3 );
    ++race.numcars;
  }

  if( controller_isActive( DIG_CONTROL_4 )) {
    controller_init( &switchs[3], DIGITAL_MODE, DIG_CONTROL_4 );
    car_init( &cars[3], &switchs[3], COLOR4 );
    ++race.numcars;
  }

  track.begin();
  strip_clear( &tck ); 


  // Check Box before Physic/Sound to allow user to have Box and Physics with no sound
  if(digitalRead(DIG_CONTROL_2)==0 || tck.cfg.track.box_alwaysOn ) { //push switch 2 on reset for activate boxes (pit lane)
    box_init( &tck );
    track_configure( &tck, tck.cfg.track.nled_total - tck.cfg.track.box_len );
    draw_box_entrypoint( &tck );
  } else{
    track_configure( &tck, 0 );
  }

  if( digitalRead(DIG_CONTROL_1)==0  || tck.cfg.ramp.alwaysOn  ) { //push switch 1 on reset for activate physics
    ramp_init( &tck );    
    draw_ramp( &tck );
    track.show();
    delay(2000);
    if ( digitalRead( DIG_CONTROL_1 ) == 0 ) { //retain push switch  on reset for activate FX sound
                                              SMOTOR=1;
                                              tone(PIN_AUDIO,100);}
  }

  race.cfg.startline  = tck.cfg.race.startline;// true;
  race.cfg.nlap       = tck.cfg.race.nlap;// NUMLAP; 
  race.cfg.nrepeat    = tck.cfg.race.nrepeat;// 1;
  race.cfg.finishline = tck.cfg.race.finishline;// true;

  customDelay.start(0); // first race starts with no delay
  race.phase = READY;
}

/*
 *  
 */
void loop() {

    // look for commands received on serial 
    ack_t ack = manageSerialCommand();
    if(ack.rp != NOTHING){
      sendResponse(&ack);
    }

    // PLEASE NOTE:
    //  DO NOT call  "track.show()" in the loop() while in configuration mode !!!
    //  It would mess up with Serial communication (receives only 2 bytes - if the 
    //  string sent by the host is longer, it gets lost)
    //  In other phases (READY, RACING, etc) ONLY 2 bytes are guaranteed to be 
    //  succesfully received - So "Enter Configuration Mode" command is just one byte (@)

  switch(race.phase) {
    
    case CONFIG:
       {
        if( race.newcfg ) {
          race.newcfg = false;
          countdownReset();
          customDelay.start(0); 
          race.phase = READY;
          send_phase( race.phase );
        }
       }
      break;

    case READY:      
      {
        if(customDelay.elapsed()) {
          for( int i = 0; i < race.numcars; ++i) {
            car_resetPosition( &cars[i] );  
            cars[i].repeats = 0;
          }
          tck.ledcoin  = COIN_RESET;
          race.phase = COUNTDOWN;
          send_phase( race.phase );
        }
      }
      break;

    case COUNTDOWN:
      {
        if( race.cfg.startline ){
          // Countdown: semaphore and tones
          if(start_race_done()) { 
            // Countdown done 
            for( int i = 0; i < race.numcars; ++i ) {
              cars[i].st = CAR_ENTER;
            }
            race.phase = RACING;
            send_phase( race.phase );
          }
        }
      }
      break;
      
    case RACING:
       {
        strip_clear( &tck ); 
  
        if( box_isactive( &tck ) ) {
          if( tck.ledcoin == COIN_RESET ) {
            tck.ledcoin = COIN_WAIT;
            tck.ledtime = millis() + random(2000,7000);
          }
          if( tck.ledcoin > 0 )
            draw_coin( &tck );
          else if( millis() > tck.ledtime )
            tck.ledcoin = random( 20, tck.cfg.track.nled_aux - 20 );
        }
  
        if( ramp_isactive( &tck ) )
          draw_ramp( &tck );
        if( box_isactive( &tck ) )
              draw_box_entrypoint( &tck );
  
        for( int i = 0; i < race.numcars; ++i ) {
          run_racecycle( &cars[i], i );
          if( cars[i].st == CAR_FINISH ) {
            race.phase = COMPLETE;
            race.winner = i;
            send_phase( race.phase );
            break;
          }
        }
  
        track.show();
        if (SMOTOR==1) tone(PIN_AUDIO,FBEEP+int(cars[0].speed*440*1)+int(cars[1].speed*440*2)+int(cars[2].speed*440*3)+int(cars[3].speed*440*4));
        if (TBEEP>0) {TBEEP--;} else {FBEEP=0;};
  
      // Print p command!!! 
        unsigned long nowmillis = millis();
        if( abs( nowmillis - lastmillis ) > 100 ){
          lastmillis = nowmillis;
          print_cars_positions( cars );
        }
      // ---------------- 
       }
      break;

    case COMPLETE :    
    {
        strip_clear( &tck );
        track.show();
        if ( race.cfg.finishline ){
          draw_winner( &tck, cars[race.winner].color );
          sound_winner( &tck, race.winner );
          strip_clear( &tck );
        }
        track.show();
        customDelay.start(NEWRACE_DELAY);
        race.phase = READY;
    }  
      break;
      
    default:
    {
      sprintf( txbuff, "Software Error in main loop switch()");
      printdebug( txbuff, WARNING );
      break;
    }
      
  } // switch
  
}

void send_phase( int phase ) {
  sprintf(txbuff, "R%d%c",phase,EOL);
  serialCommand.sendCommand(txbuff);
}


void run_racecycle( car_t *car, int i ) {
    struct cfgtrack const* cfg = &tck.cfg.track;
    
    if( car->st == CAR_ENTER ) {
        car_resetPosition( car );
        if( car->repeats < race.cfg.nrepeat )
          car->st = CAR_RACING;
        else
          car->st = CAR_GO_OUT;
    }
    
    if( car->st == CAR_RACING ) {
        update_track( &tck, car );
        car_updateController( car );
        draw_car( &tck, car );

        if( car->nlap == race.cfg.nlap 
              && !car->leaving
              && car->dist > ( cfg->nled_main*car->nlap - race.circ.outtunnel ) ) {
            car->leaving = true;
            car->st = CAR_LEAVING;
        }

        if( car->nlap > race.cfg.nlap ) {
            ++car->repeats;
            car->st = CAR_GO_OUT;
        }

        if( car->repeats >= race.cfg.nrepeat 
              && race.cfg.finishline ) {
            car->st = CAR_FINISH;  
        }
    }

    if ( car->st == CAR_FINISH ){
        car->trackID = NOT_TRACK;
        sprintf( txbuff, "w%d%c", i + 1, EOL );
        serialCommand.sendCommand(txbuff);
        //sendCommand(txbuff);

        car_resetPosition( car );
    }
}


int get_relative_position( car_t* car ) {
    enum{
      MIN_RPOS = 0,
      MAX_RPOS = 99,
    };
    struct cfgtrack const* cfg = &tck.cfg.track;
    int trackdist = 0;
    int pos = 0;

    switch ( car->trackID ){
      case TRACK_MAIN:
        trackdist = (int)car->dist % cfg->nled_main;
        pos = map(trackdist, 0, cfg->nled_main -1, MIN_RPOS, MAX_RPOS);
      break;
      case TRACK_AUX:
        trackdist = (int)car->dist_aux;
        pos = map(trackdist, 0, cfg->nled_aux -1, MIN_RPOS, MAX_RPOS);
      break;
    }
    return pos;
}


void print_cars_positions( car_t* cars ) {
    
    bool outallcar = true;
    for( int i = 0; i < race.numcars; ++i)
      outallcar &= cars[i].st == CAR_WAITING;
    
    if ( outallcar ) return;
    
    for( int i = 0; i < race.numcars; ++i ) {
      int const rpos = get_relative_position( &cars[i] );
      sprintf( txbuff, "p%d%s%d,%d%c", i + 1, tracksID[cars[i].trackID], cars[i].nlap, rpos, EOL );
      serialCommand.sendCommand(txbuff);
      //sendCommand(txbuff);
    }
}




/* 
 *  non-blocking version 
 */
boolean start_race_done( ) { 
  if(countdown_new_phase){
    countdown_new_phase=false;
    //customDelay.start(CONTDOWN_PHASE_DURATION);
    customDelay.start(CONTDOWN_PHASE_DURATION);
    strip_clear( &tck );
    if(ramp_isactive( &tck ))  draw_ramp( &tck );
    if(box_isactive( &tck ))   draw_box_entrypoint( &tck );
    switch(countdown_phase) {
      case 1:
        tone(PIN_AUDIO,400);      
        track.setPixelColor(LED_SEMAPHORE, track.Color(255,0,0));   
        break;  
      case 2:
        tone(PIN_AUDIO,600);      
        track.setPixelColor(LED_SEMAPHORE, track.Color(0,0,0));  
        track.setPixelColor(LED_SEMAPHORE-1, track.Color(255,255,0));    
        break;
      case 3:
        tone(PIN_AUDIO,1200);      
        track.setPixelColor(LED_SEMAPHORE-1, track.Color(0,0,0)); 
        track.setPixelColor(LED_SEMAPHORE-2,  track.Color(0,255,0));   
        break;
      case 4:
        //customDelay.start(CONTDOWN_STARTSOUND_DURATION);
        customDelay.start(CONTDOWN_STARTSOUND_DURATION);
        tone(PIN_AUDIO,880);      
        track.setPixelColor(LED_SEMAPHORE-2, track.Color(0,0,0)); 
        track.setPixelColor(0,  track.Color(255,255,255));   
        break;
      case 5:
        noTone(PIN_AUDIO); 
        countdownReset();  // reset for next countdown
        return(true);
    }
    track.show();    
  }
  if(customDelay.elapsed()) {
    noTone(PIN_AUDIO);    
    countdown_new_phase=true;
    countdown_phase++;
  }
  return(false);
}

/*
 * 
 */
void  countdownReset() {
  countdown_phase=1;
  countdown_new_phase=true;
}


void sound_winner( track_t* tck, int winner ) {
  int const msize = sizeof(win_music) / sizeof(int);
  for (int note = 0; note < msize; note++) {
    tone(PIN_AUDIO, win_music[note],200);
    delay(230);
    noTone(PIN_AUDIO);
  }
}


void strip_clear( track_t* tck ) {
    struct cfgtrack const* cfg = &tck->cfg.track;
    for( int i=0; i < cfg->nled_main; i++)
        track.setPixelColor( i, track.Color(0,0,0) );

    for( int i=0; i < cfg->nled_aux; i++)
        track.setPixelColor( cfg->nled_main+i, track.Color(0,0,0) );
}


void draw_coin( track_t* tck ) {
    struct cfgtrack const* cfg = &tck->cfg.track;
    track.setPixelColor( 1 + cfg->nled_main + cfg->nled_aux - tck->ledcoin,COLOR_COIN );
}

void draw_winner( track_t* tck, uint32_t color) {
  struct cfgtrack const* cfg = &tck->cfg.track;
  for(int i=16; i < cfg->nled_main; i=i+(8 * cfg->nled_main / 300 )){
      track.setPixelColor( i , color );
      track.setPixelColor( i-16 ,0 );
      track.show();
  }
}

void draw_car( track_t* tck, car_t* car ) {
    struct cfgtrack const* cfg = &tck->cfg.track;
    
    switch ( car->trackID ){
      case TRACK_MAIN:
        for(int i=0; i<= car->nlap; ++i )
          track.setPixelColor( ((word)car->dist % cfg->nled_main) + i, car->color );
      break;
      case TRACK_AUX:
        for(int i=0; i<= car->nlap; ++i )     
          track.setPixelColor( (word)(cfg->nled_main + cfg->nled_aux - car->dist_aux) + i, car->color);         
      break;
    }
}


/*
 *  Display on LED Strip current values for Slope and Pitlane
 *  
 */
void show_cfgpars_onstrip(){
  strip_clear( &tck ); 
  if( ramp_isactive( &tck ) )
        draw_ramp( &tck );
  if( box_isactive( &tck ) )
        draw_box_entrypoint( &tck );
  track.show();  
}


/*
 * 
 */
void draw_ramp( track_t* _tck ) {
    struct cfgramp const* r = &_tck->cfg.ramp;
    byte dist = 0;
    byte intensity = 0;
    for( int i = r->init; i <= r->center; ++i ) {
      dist = r->center - r->init;
      intensity = ( 32 * (i - r->init) ) / dist;
      track.setPixelColor( i, track.Color( intensity,0,intensity ) );
    }
    for( int i = r->center; i <= r->end; ++i ) {
      dist = r->end - r->center;
      intensity = ( 32 * ( r->end - i ) ) / dist;
      track.setPixelColor( i, track.Color( intensity,0,intensity ) );
    }
}


/*
 * 
 */
void draw_box_entrypoint( track_t* _tck ) {
    struct cfgtrack const* cfg = &_tck->cfg.track;
    int out = cfg->nled_total - cfg->box_len; // Pit lane exit (race start)
    int in = out - cfg->box_len;              // Pit lane Entrance
    track.setPixelColor(in ,COLOR_BOXMARKS ); 
    track.setPixelColor(out  ,COLOR_BOXMARKS ); 
}



/* 
 *  Check Serial to see if there is a command ready to be processed
 *  
 */
ack_t manageSerialCommand() {

  ack_t ack = { .rp = NOTHING, .type = '\0' };
  
  int clen = serialCommand.checkSerial();
  if(clen == 0) return ack;       // No commands received
  if(clen  < 0) {                 // Error receiving command  
    sprintf( txbuff, "Error reading serial command:[%d]",clen);
    printdebug( txbuff, WARNING );
  }
  
  // clen > 0 ---> Command with length=clen ready in  cmd[]
  ack.rp=NOK;

  switch (cmd[0]) {
    case '#':                         // Handshake
      {
        ack.type = cmd[0];
        sprintf( txbuff, "#%c", EOL );
        serialCommand.sendCommand(txbuff);    
        ack.rp = NOTHING;
      }
      break;

  case  '@' :                         // Enter "Configuration Mode" status
    {
      ack.type = cmd[0];    
      if(race.phase != CONFIG) {// Ignore command if Board already in "Configure Mode"
         race.phase = CONFIG;
        enter_configuration_mode();
      }
      ack.rp = OK;
    }
    break;
  
  case '~' :                          // Exit "Configure Mode"
    {
      ack.type = cmd[0];
      if(race.phase == CONFIG) { // Ignore command if Board is not in "Configure Mode"
        race.newcfg = true;
      } 
      ack.rp = OK;
    }
    break;    
    
  case 'R' :                          // Set Race Phase
    {
      ack.type = cmd[0];
      int const phase = atoi( cmd + 1);
      if( 0 > phase || RACE_PHASES <= phase) return ack;
      race.phase = (enum phases) phase;
      ack.rp = OK;
      if ( race.phase == CONFIG ) { // accept R1 as a EnterConfigurationMode command - DEPRECATED
        enter_configuration_mode();
      }
    }
    break;

  case 'C' :                          //Parse race configuration -> C1,2,3,0
    {
      ack.type = cmd[0];
  
      char * pch = strtok (cmd,"C");
      if( !pch ) return ack;
  
      pch = strtok (pch, "," );
      if( !pch ) return ack;
      int startline = atoi( pch );
  
      pch = strtok (NULL, ",");
      if( !pch ) return ack;
      int nlap = atoi( pch );
  
      pch = strtok (NULL, ",");
      if( !pch ) return ack;
      int nrepeat = atoi( pch );
  
      pch = strtok (NULL, ",");
      if( !pch ) return ack;
      int finishline = atoi( pch );
  
      int err = race_configure( &tck, startline, nlap, nrepeat, finishline);
      if( err ) return ack;
  
      race.cfg.startline  = tck.cfg.race.startline;
      race.cfg.nlap       = tck.cfg.race.nlap;
      race.cfg.nrepeat    = tck.cfg.race.nrepeat;
      race.cfg.finishline = tck.cfg.race.finishline;
      
      ack.rp = OK;
    }
    break;
    

  case 'T' :                          //Parse Track configuration -> Track length
    {
      ack.type = cmd[0];
  
      char * pch = strtok (cmd,"T");
      if( !pch ) return ack;
  
      int nled = atoi( cmd + 1 );
      int err = tracklen_configure( &tck, nled);
      if( err ) return ack;
      track_configure( &tck, 0);
      if( err ) return ack;
      
      ack.rp = OK;
      }
    break;  

  case 'B' :                          //Parse BoxLenght Configuration -> Blen,perm
    {
      ack.type = cmd[0];
  
      char * pch = strtok (cmd,"B");
      if( !pch ) return ack;
  
      pch = strtok (pch, "," );
      if( !pch ) return ack;
      int boxlen = atoi( pch );
  
      pch = strtok (NULL, "," );
      if( !pch ) return ack;
      int boxperm = atoi( pch );
  
      int err = boxlen_configure( &tck, boxlen, boxperm );
      if( err ) return ack;
     
      ack.rp = OK;
  
      // Force Pitlane ON, so "show_cfgpars_onstrip()" 
      // will show the new values, even if AlwaysON=false
      box_init(&tck); 
      show_cfgpars_onstrip();
      }
    break;

  case 'A' :                          // Parse Ramp configuration -> Astart,center,end,high,perm
    {
      ack.type = cmd[0];
  
      char * pch = strtok (cmd,"A");
      if( !pch ) return ack;
  
      pch = strtok (pch, "," );
      if( !pch ) return ack;
      int init = atoi( pch );
  
      pch = strtok (NULL, "," );
      if( !pch ) return ack;
      int center = atoi( pch );
  
      pch = strtok (NULL, "," );
      if( !pch ) return ack;
      int end = atoi( pch );
  
      pch = strtok (NULL, ",");
      if( !pch ) return ack;
      int high = atoi( pch );
  
      pch = strtok (NULL, ",");
      if( !pch ) return ack;
      int slopeperm = atoi( pch );
      
      int err = ramp_configure( &tck, init, center, end, high, slopeperm );
      if( err ) return ack;
      ack.rp = OK;
  
      // Force Ramp ON, so "show_cfgpars_onstrip()" 
      // will show the new values, even if AlwaysON=false
      ramp_init(&tck); 
      
      show_cfgpars_onstrip();
      }
    break;
    
  case 'K':                           // Parse Physic simulation parameters
    {
      ack.type = cmd[0];
  
      char * pch = strtok (cmd,"K");
      if( !pch ) return ack;
  
      pch = strtok (pch, "," );
      if( !pch ) return ack;
      float kgp = atof( pch );
  
      pch = strtok (NULL, "," );
      if( !pch ) return ack;
      float kfp = atof( pch );
  
      int err = physic_configure( &tck, kgp, kfp );
      if( err ) return ack;
      
      ack.rp = OK;
    }
    break;

    
  case 'D' :                          // Load Default Parameters and store them in from EEPROM
    {
      ack.type = cmd[0]; 
      param_setdefault( &tck.cfg );
      EEPROM.put( eeadrInfo, tck.cfg ); // Save immediately 
      
      ack.rp = OK;
      
      // Update box/slope active in current Track Struct with values 
      // just loaded (for show_cfgpars_onstrip())
      struct cfgparam const* cfg = &tck.cfg;
      tck.boxactive  = cfg->track.box_alwaysOn;
      tck.rampactive = cfg->ramp.alwaysOn;
  
      show_cfgpars_onstrip();
    }
    break;
         
  case ':' :                          // Set board Unique Id
    {
      struct brdinfo* info = &tck.cfg.info;
      ack.type = cmd[0];
      if( strlen(cmd + 1) > LEN_UID ) return ack;
      strcpy( info->uid, cmd + 1 );
      EEPROM.put( eeadrInfo, tck.cfg ); // Save immediately 
      ack.rp = OK;
    }
    break;

  case '$':                           // Get Board UID
    {
      sprintf( txbuff, "%s%s%c", "$", tck.cfg.info.uid, EOL );
      serialCommand.sendCommand(txbuff);
      ack.rp = NOTHING;
    }
    break;

  case '?' :                          // Get Software Id
    {
      sprintf( txbuff, "%s%s%c", "?", softwareId, EOL );
      serialCommand.sendCommand(txbuff);
      ack.rp = NOTHING;
    }
    break;
  
  case '%' :                          // Get Software Version
    {
      sprintf( txbuff, "%s%s%c", "%", version, EOL );
      serialCommand.sendCommand(txbuff);    
      ack.rp = NOTHING;
    }
    break;

  case  'Q':                          // Get current configuration Info
    {
      struct cfgparam const* cfg = &tck.cfg;
      sprintf( txbuff, "%s:%d,%d,%d,%d,%d,%d,%d.%03d,%d.%03d%c", "QTRACK",
                                      cfg->track.nled_total,
                                      cfg->track.nled_main,
                                      cfg->track.nled_aux,
                                      cfg->track.init_aux,
                                      cfg->track.box_len,
                                      cfg->track.box_alwaysOn,
                                      (int)cfg->track.kg, (int)(cfg->track.kg*1000)%1000, // std arduino sprintf() missing %f
                                      (int)cfg->track.kf, (int)(cfg->track.kf*1000)%1000, // std arduino sprintf() missing %f
                                      EOL );
      serialCommand.sendCommand(txbuff);
  
      sprintf( txbuff, "%s:%d,%d,%d,%d,%d%c", "QRAMP",
                                      cfg->ramp.init,
                                      cfg->ramp.center,
                                      cfg->ramp.end,
                                      cfg->ramp.high,
                                      cfg->ramp.alwaysOn,
                                      EOL );
      serialCommand.sendCommand(txbuff);
  
      sprintf( txbuff, "%s:%d,%d,%d,%d%c", "QRACE",
                                      cfg->race.startline,
                                      cfg->race.nlap,
                                      cfg->race.nrepeat,
                                      cfg->race.finishline,
                                      EOL );
      serialCommand.sendCommand(txbuff);
  
      ack.rp = NOTHING;
    }
    break;

  case 'W':                           // Write configuration to EEPROM
    {
      ack.type = cmd[0];
      EEPROM.put( eeadrInfo, tck.cfg );
      ack.rp = OK;
    }
    break;
    
  } // switch
  
  return(ack);
 
}

/*
 * 
 */
void sendResponse( ack_t *ack) {
  if(ack->type=='\0'){
    sprintf(txbuff, "%s%c", ack->rp==OK? "OK":"NOK" , EOL );
  } else {
    sprintf(txbuff, "%c%s%c", ack->type, ack->rp==OK? "OK":"NOK" , EOL );
  }
  serialCommand.sendCommand(txbuff);
}

/*
 *  Send Log/Warning/Error messages to host 
 */
void printdebug( const char * msg, int errlevel ) {
  char header [5];
  sprintf(header, "!%d,",errlevel);
  Serial.print(header);
  Serial.print(msg);
  Serial.print(EOL);
}


/*
 *  reset race parameters
 *  stop sound
 */
void enter_configuration_mode(){
    noTone(PIN_AUDIO); 
    strip_clear( &tck );
    track.show(); 
}


 
void param_load( struct cfgparam* cfg ) {
    int cfgversion;
    int eeAdress = eeadrInfo;
    EEPROM.get( eeAdress, tck.cfg );
    eeAdress += sizeof( cfgparam );
    EEPROM.get( eeAdress, cfgversion );

    sprintf( txbuff, "%s:%d%c", "Parameters Loaded from EEPROM - Cfg ver", cfgversion, EOL );
    serialCommand.sendCommand(txbuff);

    if ( cfgversion != CFG_VER ) {
      param_setdefault( &tck.cfg );
      eeAdress = 0;
      EEPROM.put( eeAdress, tck.cfg );
      eeAdress += sizeof( cfgparam );
      EEPROM.put( eeAdress, CFG_VER );
      sprintf( txbuff, "%s%c", "DEFAULT PAREMETRS LOADED (and Stored in EEPROM)", EOL );
      serialCommand.sendCommand(txbuff);
     
    }
   
}



/*
 *  Custon Non-Blocking Delay() functions
 *    customDelayStart(unsigned long timeout)
 *    customDelayElapsed
 */
/**
unsigned long   customDelay_startTime=0;
unsigned long   customDelay_timeout=0;
void customDelayStart(unsigned long tout) {
  customDelay_timeout=tout;
  customDelay_startTime=millis();
}
boolean customDelayElapsed(){
  if((millis() - customDelay_startTime) > customDelay_timeout) {
    return(true);
  }
  return(false);
}
**/
