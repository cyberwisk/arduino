#include "Arduino.h"
#include "olr-lib.h"



void car_init( car_t* car, controller_t* ct, uint32_t color ) {
  car->ct = ct;
  car->color = color;
  car->trackID = TRACK_MAIN;
  car->speed=0;
  car->dist=0;
  car->dist_aux=0;
}

void car_updateController( car_t* car ) {
    car->speed += controller_getSpeed( car->ct ); 
}

void update_track( track_t* tck, car_t* car ) {
  controller_t* ct = car->ct;
  struct cfgtrack const* cfg = &tck->cfg.track;


  if ( car->trackID == TRACK_MAIN    
       &&  (int)car->dist % cfg->nled_main == (cfg->init_aux-(cfg->nled_aux)) 
    //  &&  controller_getStatus( ct ) == 0 ) {              //change track by switch
      &&  (car->speed <= SPD_MIN_TRACK_AUX )) {              //change track by low speed
        
        car->trackID = TRACK_AUX;
        car->dist_aux = 0;
  }
  else if( car->trackID == TRACK_AUX 
      && car->dist_aux > cfg->nled_aux ) {

        car->trackID = TRACK_MAIN;
        car->dist += cfg->nled_aux;         
  }

  /* Update car position in the current track */
  if ( car->trackID == TRACK_AUX )   process_aux_track( tck, car );
  else if ( car->trackID == TRACK_MAIN )  process_main_track( tck, car );

  /* Update car lap */          
  if ( car->dist > ( cfg->nled_main*car->nlap -1) )  car->nlap++;
}

void process_aux_track( track_t* tck, car_t* car ){
    struct cfgtrack const* cfg = &tck->cfg.track;

    if (  (int)car->dist_aux == tck->ledcoin 
          && car->speed <= controller_getAccel() ) {                      
        car->speed = controller_getAccel ()*50;
        tck->ledcoin = COIN_RESET;
    };

    car->speed -= car->speed * cfg->kf;
    car->dist_aux += car->speed;
}


void process_main_track( track_t* tck, car_t* car ) {
    struct cfgtrack const* cfg = &tck->cfg.track;

    if ( tck->rampactive ) { 
        struct cfgramp const* r = &tck->cfg.ramp;
        int const pos = (int)car->dist % cfg->nled_main;
        if ( pos >= r->init && pos < r->center )
        //    car->speed -= cfg->kg * r->high * ( pos - r->init );
        car->speed -= cfg->kg * r->high ;
        
        if ( pos <= r->end && pos > r->center )
            //car->speed += cfg->kg * r->high * ( pos - r->center ); 
            car->speed += cfg->kg * r->high ; 
    } 
    
    car->speed -= car->speed * cfg->kf;
    car->dist += car->speed;

}

void ramp_init( track_t* tck ) {
  tck->rampactive = true;
}


bool ramp_isactive( track_t* tck ) {
  return tck->rampactive;
}


void car_resetPosition( car_t* car) {

  car->trackID = TRACK_MAIN;
  car->speed = 0;
  car->dist = 0;
  car->dist_aux = 0;
  car->nlap = 1;
  car->leaving = false;
}

void box_init( track_t* tck ) {
  tck->boxactive = true;  
}

bool box_isactive( track_t* tck ) {
  return tck->boxactive;
}



int tracklen_configure( track_t* tck, int nled ) {
  struct cfgtrack* cfg = &tck->cfg.track;
  if(  nled <= 0 ) return -1;
  cfg->nled_total = nled;
  return 0;
}

int boxlen_configure( track_t* tck, int box_len, int boxalwaysOn ) {
  struct cfgtrack* cfg = &tck->cfg.track;
  
  if ( boxalwaysOn != 0 && boxalwaysOn != 1 ) return -1;
  if(  box_len <= 0 || box_len >= cfg->nled_total ) return -1;
  cfg->box_len = box_len;
  cfg->box_alwaysOn = boxalwaysOn;

  // Update  track->boxactive 
  tck->boxactive = boxalwaysOn; 
  return 0;
}

int physic_configure( track_t* tck,  float kgp,  float kfp ){
  struct cfgtrack* cfg = &tck->cfg.track;

  if(  kgp <= 0.0 || kgp >= 2.0  ) return -1;
  if(  kfp <= 0.0 || kfp >= 2.0  ) return -1;
  cfg->kf  = kfp; 
  cfg->kg  = kgp; 
  return(0);  
}



int track_configure( track_t* tck, int init_box ) {
  struct cfgtrack* cfg = &tck->cfg.track;
  
  if(init_box >= cfg->nled_total ) return -1;
  cfg->nled_main = ( init_box == 0 ) ? cfg->nled_total : init_box;
  cfg->nled_aux = ( init_box == 0 ) ? 0 : cfg->nled_total - init_box;
  cfg->init_aux = init_box - 1;
  return 0;
}


int ramp_configure( track_t* tck, int init, int center, int end, int high, int alwaysOn  ) {
  struct cfgramp* ramp = &tck->cfg.ramp;

  if ( init >= tck->cfg.track.nled_main || init <= 0 ) return -1;
  if ( center >= tck->cfg.track.nled_main || center <= 0 ) return -2;
  if ( end >= tck->cfg.track.nled_main || end <= 0 ) return -3;
  if ( ! (center > init && center < end) ) return -4;
  if ( alwaysOn != 0 && alwaysOn != 1 ) return -5;
  
  ramp->init = init;
  ramp->center = center;
  ramp->end = end;
  ramp->high = high;
  ramp->alwaysOn = alwaysOn;

  // Update  track->rampactive 
  /**
  boolean rampactive = &tck->rampactive;
  rampactive = alwaysOn;
  **/
  tck->rampactive = alwaysOn; 
  
  return 0;
}

int race_configure( track_t* tck, int startline, int nlap, int nrepeat, int finishline ) {
  struct cfgrace* race = &tck->cfg.race;

  if ( startline != 0 && startline != 1 ) return -1;
  if ( finishline != 0 && finishline != 1 ) return -1;
  race->startline = startline;
  race->finishline = finishline;
  race->nlap = nlap;
  race->nrepeat = nrepeat;
  return 0;
}
