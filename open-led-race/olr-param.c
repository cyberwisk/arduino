#include "olr-param.h"

void param_setdefault( struct cfgparam* cfg ) {
    cfg->setted      = true;

    cfg->race.startline = true;   
    cfg->race.nlap = NUMLAP;  
    cfg->race.nrepeat = 1;
    cfg->race.finishline = true;

    cfg->ramp.init   = 80;
    cfg->ramp.center = 90;
    cfg->ramp.end    = 100;
    cfg->ramp.high   = 6;
    cfg->ramp.alwaysOn = false;

    cfg->track.nled_total = MAXLED;
    cfg->track.nled_main  = MAXLED; // 240 when boxes length = 60
    cfg->track.nled_aux   = 0; // 60 
    cfg->track.init_aux   = -1;  // 239   
    cfg->track.box_len    = BOXLEN;
    cfg->track.box_alwaysOn = false;
    
    cfg->track.kf         = 0.015; // friction constant
    cfg->track.kg         = 0.006; // gravity constant  - Used in Slope
}
