#ifndef _OLR_PARAM_LIB_h
#define _OLR_PARAM_LIB_h                   

#ifdef __cplusplus

extern "C"{
#endif

#include "Arduino.h"
#include <stdint.h>
#include <stdbool.h>

#define MAXLED       300 
#define BOXLEN        60 
#define NUMLAP         5 



enum{
  LEN_UID = 16,
  CFG_VER = 5,  // "5" in V0.9.6 (manage "permanent" param for Box and Slope)
};

struct cfgrace{
    bool startline;   // Used only in OLRNetwork 
    int  nlap;
    int  nrepeat;     // Used only in OLRNetwork 
    bool finishline;  // Used only in OLRNetwork 
};

// 
struct cfgtrack  {
  int nled_total;
  int nled_main;
  int nled_aux;
  int init_aux;
  int box_len;  // used to hold the Box Length if the default get changed.
                // it's not possible to implicitly store it in nled_main,nled_aux
                // because, if these are different to the default, box gets always activated
                // (the software does not chek "box_isactive" to draw car position)
  bool box_alwaysOn;  // added in ver 0.9.6

  
  float kf;
  float kg;
};

// ramp centred in LED 100 with 10 led fordward and 10 backguard
struct cfgramp  {
  int init;
  int center;
  int end;
  int high;
  bool alwaysOn;  // added in ver 0.9.6
};

struct brdinfo {
  char uid[LEN_UID + 1];
};

struct cfgparam {
    bool setted;
    struct cfgrace  race;  // added in ver 0.9.d
    struct cfgtrack track;
    struct cfgramp  ramp;
    struct brdinfo  info;
};


void param_setdefault( struct cfgparam* cfg );

#ifdef __cplusplus
} // extern "C"
#endif

#endif 
 
